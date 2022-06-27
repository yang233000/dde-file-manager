/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liqiang<liqianga@uniontech.com>
 *
 * Maintainer: liqiang<liqianga@uniontech.com>
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
#ifndef MENU_H
#define MENU_H

#include "dfmplugin_menu_global.h"

#include "services/common/dfm_common_service_global.h"

#include <dfm-framework/framework.h>
#include <dfm-framework/dpf.h>

namespace dfm_service_common {
class MenuService;
}

namespace dfmplugin_menu {

class Menu : public dpf::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.plugin.common" FILE "menu.json")
    DPF_EVENT_NAMESPACE(DPMENU_NAMESPACE)

    DPF_EVENT_REG_SLOT(slot_PerfectMenuParams)

public:
    virtual void initialize() override;
    virtual bool start() override;
    virtual ShutdownFlag stop() override;

private:
    void regDefaultScene();

private slots:
    QVariantHash perfectMenuParams(const QVariantHash &params);

private:
    dfm_service_common::MenuService *menuServer = nullptr;
};

}

#endif   // MENU_H
