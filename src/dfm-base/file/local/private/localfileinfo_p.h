/*
 * Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#ifndef LOCALFILEINFO_P_H
#define LOCALFILEINFO_P_H

#include "interfaces/private/abstractfileinfo_p.h"
#include "file/local/localfileinfo.h"
#include "mimetype/mimedatabase.h"

#include <QFileInfo>
#include <QIcon>
#include <QTimer>
#include <QFuture>
#include <QQueue>
#include <QMimeType>
#include <QReadWriteLock>

namespace dfmbase {
class LocalFileInfoPrivate : public AbstractFileInfoPrivate
{
    friend class LocalFileInfo;
    LocalFileInfo *const q;
    QSharedPointer<DFileInfo> dfmFileInfo { nullptr };   // dfm文件的信息

    QAtomicInteger<quint64> inode { 0 };   // 文件的inode，唯一表示符
    QVariantHash extraProperties;   // 扩展属性列表
    MimeDatabase::FileType fileType { MimeDatabase::FileType::kUnknown };   // 缓存文件的FileType
    QMap<DFileInfo::AttributeID, QVariant> attributes;   // 缓存的fileinfo信息
    QMap<DFileInfo::AttributeExtendID, QVariant> attributesExtend;   // 缓存的fileinfo 扩展信息
    QList<DFileInfo::AttributeExtendID> extendIDs;

    DFMIO::DFileInfo::AttributeExtendFuncCallback attributesExtendCallbackFunc = nullptr;

    QMimeType mimeType;
    QMimeDatabase::MatchMode mimeTypeMode;

    QReadWriteLock lock;
    QMutex mutex;

    enum IconType {
        kDefaultIcon,
        kThumbIcon,
    };
    QReadWriteLock iconLock;
    QMap<IconType, QIcon> icons;
    std::atomic_bool loadingThumbnail = { false };
    std::atomic_int enableThumbnail = { -1 };   // 小于0时表示此值未初始化，0表示不支持，1表示支持
    QPointer<QTimer> getIconTimer = nullptr;

    std::atomic_int enableEmblems = { -1 };
    QMap<int, QIcon> gioEmblemsMap;   // 缓存的角标数据

    // permission
    std::atomic_bool permissionsInited { false };
    QFileDevice::Permissions permissions;

public:
    explicit LocalFileInfoPrivate(LocalFileInfo *qq);
    virtual ~LocalFileInfoPrivate();
    QString sizeString(const QString &str) const
    {
        int begin_pos = str.indexOf('.');

        if (begin_pos < 0)
            return str;

        QString size = str;

        while (size.count() - 1 > begin_pos) {
            if (!size.endsWith('0'))
                return size;

            size = size.left(size.count() - 1);
        }

        return size.left(size.count() - 1);
    }
    virtual QMimeType readMimeType(QMimeDatabase::MatchMode mode = QMimeDatabase::MatchDefault) const;

    void clearIcon()
    {
        icons.clear();
        loadingThumbnail = false;
        enableThumbnail = -1;
        getIconTimer = nullptr;
    }

    QIcon thumbIcon();
    QIcon defaultIcon();
    void onRequestThumbFinished(const QString &path);

    void attributesExtendCallback(bool ok, QMap<DFMIO::DFileInfo::AttributeExtendID, QVariant> values)
    {
        if (ok) {
            auto it = values.constBegin();
            while (it != values.constEnd()) {
                const QVariant &value = it.value();
                if (value.isValid())
                    attributesExtend.insert(it.key(), value);
                ++it;
            }

            QMap<DFMIO::DFileInfo::AttributeExtendID, QVariant> ret;
            for (const DFileInfo::AttributeExtendID &id : extendIDs) {
                if (attributesExtend.count(id) > 0) {
                    ret.insert(id, attributesExtend.value(id));
                }
            }

            if (attributesExtendCallbackFunc) {
                attributesExtendCallbackFunc(true, ret);
            }
        } else {
            if (attributesExtendCallbackFunc)
                attributesExtendCallbackFunc(false, {});
        }
    }
};

LocalFileInfoPrivate::LocalFileInfoPrivate(LocalFileInfo *qq)
    : AbstractFileInfoPrivate(qq), q(qq)
{
}

LocalFileInfoPrivate::~LocalFileInfoPrivate()
{
}

QMimeType LocalFileInfoPrivate::readMimeType(QMimeDatabase::MatchMode mode) const
{
    QUrl url = q->url();
    if (url.isLocalFile())
        return MimeDatabase::mimeTypeForUrl(url);
    else
        return MimeDatabase::mimeTypeForFile(UrlRoute::urlToPath(url),
                                             mode);
}

}
Q_DECLARE_METATYPE(DFMBASE_NAMESPACE::LocalFileInfoPrivate *)

#endif   // LOCALFILEINFO_P_H
