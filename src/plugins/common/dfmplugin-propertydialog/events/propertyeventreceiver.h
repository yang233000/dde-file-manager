/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
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
#ifndef PROPERTYDIALOGUNICASTRECEIVER_H
#define PROPERTYDIALOGUNICASTRECEIVER_H

#include "dfmplugin_propertydialog_global.h"
#include "services/common/propertydialog/property_defines.h"

#include <QObject>

namespace dfmplugin_propertydialog {
class PropertyEventReceiver : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PropertyEventReceiver)
private:
    explicit PropertyEventReceiver(QObject *parent = nullptr);

public:
    static PropertyEventReceiver *instance();
    void connectService();

signals:

public slots:
    void showPropertyControl(const QList<QUrl> &url);
};
}
#endif   // PROPERTYDIALOGEVENTRECEIVER_H
