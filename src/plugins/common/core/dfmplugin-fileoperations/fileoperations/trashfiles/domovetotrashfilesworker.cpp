/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liyigang<liyigang@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "domovetotrashfilesworker.h"
#include "fileoperations/copyfiles/storageinfo.h"

#include "dfm-base/base/schemefactory.h"
#include "dfm-base/base/standardpaths.h"
#include "dfm-base/utils/decorator/decoratorfile.h"
#include "dfm-base/utils/decorator/decoratorfileenumerator.h"
#include "dfm-base/utils/universalutils.h"
#include "dfm-base/base/device/deviceutils.h"

#include <dfm-io/dfmio_global.h>
#include <dfm-io/core/diofactory.h>
#include <dfm-io/dfmio_utils.h>

#include <QUrl>
#include <QDebug>
#include <QtGlobal>
#include <QCryptographicHash>
#include <QStorageInfo>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

USING_IO_NAMESPACE
DPFILEOPERATIONS_USE_NAMESPACE
DoMoveToTrashFilesWorker::DoMoveToTrashFilesWorker(QObject *parent)
    : FileOperateBaseWorker(parent)
{
    jobType = AbstractJobHandler::JobType::kMoveToTrashType;
    fstabMap = DeviceUtils::fstabBindInfo();
}

DoMoveToTrashFilesWorker::~DoMoveToTrashFilesWorker()
{
    stop();
}
/*!
 * \brief DoMoveToTrashFilesWorker::doWork the thread move to trash work function
 * \return move to trash is successed
 */
bool DoMoveToTrashFilesWorker::doWork()
{
    if (!AbstractWorker::doWork())
        return false;

    // check progress notify type
    determineCountProcessType();

    doMoveToTrash();

    endWork();

    return true;
}
/*!
 * \brief DoMoveToTrashFilesWorker::statisticsFilesSize init target arguments
 * \return
 */
bool DoMoveToTrashFilesWorker::statisticsFilesSize()
{
    sourceFilesCount = sourceUrls.size();
    targetUrl = FileUtils::trashRootUrl();
    return true;
}

void DoMoveToTrashFilesWorker::onUpdateProgress()
{
    emitProgressChangedNotify(completeFilesCount);
}
/*!
 * \brief DoMoveToTrashFilesWorker::doMoveToTrash do move to trash
 * \return move to trash success
 */
bool DoMoveToTrashFilesWorker::doMoveToTrash()
{
    bool result = false;
    DFMBASE_NAMESPACE::LocalFileHandler fileHandler;
    static QString homeTrashFileDir = dfmbase::StandardPaths::location(StandardPaths::StandardLocation::kTrashLocalFilesPath);
    // 总大小使用源文件个数
    for (const auto &url : sourceUrls) {
        QUrl urlSource = url;
        if (!fstabMap.empty()) {
            for (const auto &device : fstabMap.keys()) {
                if (urlSource.path().startsWith(device)) {
                    urlSource.setPath(urlSource.path().replace(0, device.size(), fstabMap[device]));
                    break;
                }
            }
        }

        if (!stateCheck())
            return false;

        if (FileUtils::isTrashFile(urlSource)) {
            completeFilesCount++;
            completeSourceFiles.append(urlSource);
            continue;
        }

        // url是否可以删除 canrename
        if (!isCanMoveToTrash(urlSource, &result)) {
            if (result) {
                completeFilesCount++;
                completeSourceFiles.append(urlSource);
                continue;
            }
            return false;
        }

        const auto &fileInfo = InfoFactory::create<AbstractFileInfo>(urlSource);
        if (!fileInfo) {
            // pause and emit error msg
            if (AbstractJobHandler::SupportAction::kSkipAction != doHandleErrorAndWait(urlSource, targetUrl, AbstractJobHandler::JobErrorType::kProrogramError)) {
                return false;
            } else {
                completeFilesCount++;
                continue;
            }
        }

        emitCurrentTaskNotify(urlSource, targetUrl);

        AbstractJobHandler::SupportAction action = AbstractJobHandler::SupportAction::kNoAction;
        do {
            QString trashPath = fileHandler.trashFile(urlSource);
            if (!trashPath.isEmpty()) {
                QUrl trashUrl;
                trashUrl.setScheme(dfmbase::Global::Scheme::kTrash);
                if (!trashPath.startsWith(homeTrashFileDir))
                    trashPath = "/" + trashPath.replace("/", "\\");
                trashUrl.setPath(trashPath.replace(homeTrashFileDir, ""));
                completeTargetFiles.append(trashUrl);
                emitProgressChangedNotify(completeFilesCount);
                completeSourceFiles.append(urlSource);
                continue;
            } else {
                // pause and emit error msg
                action = doHandleErrorAndWait(url, QUrl(),
                                              AbstractJobHandler::JobErrorType::kDeleteFileError, false,
                                              fileHandler.errorCode() == DFMIOErrorCode::DFM_IO_ERROR_NONE ?
                                                "Unknown error" : fileHandler.errorString());
            }
        } while (action == AbstractJobHandler::SupportAction::kRetryAction && !isStopped());

        if (action == AbstractJobHandler::SupportAction::kNoAction
                || action == AbstractJobHandler::SupportAction::kSkipAction) {
            completeFilesCount++;
            continue;
        }

        return false;
    }
    return true;
}

/*!
 * \brief DoMoveToTrashFilesWorker::isCanMoveToTrash loop to check the source file can move to trash
 * \param url the source file url
 * \param result Output parameters, is skip this file
 * \return can move to trash
 */
bool DoMoveToTrashFilesWorker::isCanMoveToTrash(const QUrl &url, bool *result)
{
    AbstractJobHandler::SupportAction action = AbstractJobHandler::SupportAction::kNoAction;

    if (Q_UNLIKELY(!stateCheck())) {
        return false;
    }

    do {
        if (!canWriteFile(url))
            // pause and emit error msg
            action = doHandleErrorAndWait(url, targetUrl, AbstractJobHandler::JobErrorType::kPermissionDeniedError);

    } while (action == AbstractJobHandler::SupportAction::kRetryAction && !isStopped());

    if (action != AbstractJobHandler::SupportAction::kNoAction) {
        *result = action == AbstractJobHandler::SupportAction::kSkipAction;
        return false;
    }

    return true;
}
