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
#include "networkaccount.h"

networkaccount::networkaccount()
{
    pluginWidget = new config_list_widget();
    pluginName = tr("NetworkAccount");
    pluginType = ACCOUNT;
}


QString networkaccount::get_plugin_name(){
    return pluginName;
}

int networkaccount::get_plugin_type(){
    return pluginType;
}

QWidget * networkaccount::get_plugin_ui(){
    return pluginWidget;
}

void networkaccount::plugin_delay_control(){

}


