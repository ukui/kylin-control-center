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
#include "experienceplan.h"
#include "ui_experienceplan.h"

#define E_PLAN_SCHEMA "org.ukui.control-center.experienceplan"
#define JOIN_KEY "join"

ExperiencePlan::ExperiencePlan()
{
    ui = new Ui::ExperiencePlan;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Experienceplan");
    pluginType = NOTICEANDTASKS;

    QByteArray id(E_PLAN_SCHEMA);
    eSettings = new QGSettings(id);

    setupComponent();
    initEpStatus();

}

ExperiencePlan::~ExperiencePlan()
{
    delete ui;
    ui = nullptr;
    delete eSettings;
    eSettings = nullptr;
}

QString ExperiencePlan::get_plugin_name(){
    return pluginName;
}

int ExperiencePlan::get_plugin_type(){
    return pluginType;
}

QWidget * ExperiencePlan::get_plugin_ui(){
    return pluginWidget;
}

void ExperiencePlan::plugin_delay_control(){

}

const QString ExperiencePlan::name() const{

    return QStringLiteral("experienceplan");
}

void ExperiencePlan::setupComponent(){
    joinSwitchBtn = new SwitchButton(pluginWidget);

    ui->joinHorLayout->addWidget(joinSwitchBtn);

    connect(joinSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        eSettings->set(JOIN_KEY, checked);
    });
}

void ExperiencePlan::initEpStatus(){
    joinSwitchBtn->blockSignals(true);

    joinSwitchBtn->setChecked(eSettings->get(JOIN_KEY).toBool());

    joinSwitchBtn->blockSignals(false);
}

