/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "modulepagewidget.h"
#include "ui_modulepagewidget.h"

#include <QListWidgetItem>
#include <QDebug>

#include "mainwindow.h"
#include "interface.h"
#include "utils/keyvalueconverter.h"
#include "utils/functionselect.h"
#include "utils/utils.h"
#include "component/leftwidgetitem.h"
#include "component/leftmenulist.h"

ModulePageWidget::ModulePageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulePageWidget)
{
    ui->setupUi(this);

    initUI();
}

ModulePageWidget::~ModulePageWidget()
{
    delete ui;
    ui = nullptr;
}

void ModulePageWidget::initUI() {
    //设置伸缩策略

    QSizePolicy rightSizePolicy = ui->widget->sizePolicy();

    rightSizePolicy.setHorizontalStretch(5);


    ui->widget->setSizePolicy(rightSizePolicy);
}

void ModulePageWidget::switchPage(QObject *plugin, bool recorded){

    CommonInterface * pluginInstance = qobject_cast<CommonInterface *>(plugin);
    refreshPluginWidget(pluginInstance);
}

void ModulePageWidget::refreshPluginWidget(CommonInterface *plu){
    if (plu->pluginBtn) {
        plu->pluginBtn->setChecked(true);
    }

    ui->scrollArea->takeWidget();
    delete(ui->scrollArea->widget());

    ui->scrollArea->setWidget(plu->get_plugin_ui());

    //延迟操作
    plu->plugin_delay_control();

    //记录打开历史
    if (flagBit){
        FunctionSelect::pushRecordValue(plu->get_plugin_type(), plu->get_plugin_name());
    }

    //恢复标志位
    flagBit = true;
}

