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
#include "vino.h"
#include "ui_vino.h"

Vino::Vino() : ui(new Ui::Vino), mFirstLoad(true) {
    pluginName = tr("Vino");
    pluginType = NETWORK;
}

Vino::~Vino() {
    if (!mFirstLoad) {
        delete ui;
    }
}

QString Vino::get_plugin_name() {
    return pluginName;
}

int Vino::get_plugin_type() {
    return pluginType;
}

QWidget *Vino::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new ShareMain;
        ui->setupUi(pluginWidget);
    }

    return pluginWidget;
}

void Vino::plugin_delay_control() {

}

const QString Vino::name() const {
    return QStringLiteral("vino");
}

