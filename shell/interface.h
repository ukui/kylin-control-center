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
#ifndef INTERFACE_H
#define INTERFACE_H

class QString;
class QWidget;

enum FunType{
    SYSTEM,
    DEVICES,
    PERSONALIZED,
    NETWORK,
    ACCOUNT,
    DATETIME,
    UPDATE,
    NOTICEANDTASKS,
    TOTALMODULES,
};

enum SystemIndex{
    DISPLAY,
    DEFAULTAPP,
    POWER,
    AUTOBOOT,
    TOTALSYSFUNC,
};

enum DevicesIndex{
    PRINTER,
    MOUSE,
    TOUCHPAD,
    KEYBOARD,
    SHORTCUT,
    AUDIO,
    TOTALDEVICESFUNC,
};

enum PersonalizedIndex{
    BACKGROUND,
    THEME,
    SCREENLOCK,
    FONTS,
    SCREENSAVER,
    DESKTOP,
    TOTALPERSFUNC,
};

enum NetworkIndex{
    NETCONNECT,
    VPN,
    PROXY,
    TOTALNETFUNC,
};

enum AccountIndex{
    USERINFO,
    NETWORKACCOUNT,
    TOTALACCOUNTFUNC,
};

enum DatetimeIndex{
    DAT,
    AREA,
    TOTALDTFUNC,
};

enum SeUpdatesIndex{
    UPDATES,
    BACKUP,
    TOTALSUFUNC,
};

enum NoticeAndTasksIndex{
    NOTICE,
    ABOUT,
    EXPERIENCEPLAN,
    TOTALNATFUNC,
};

class CommonInterface{
public:
    virtual ~CommonInterface(){}
    virtual QString get_plugin_name() = 0;
    virtual int get_plugin_type() = 0;
    virtual QWidget * get_plugin_ui() = 0;
    virtual void plugin_delay_control() = 0;
};

#define CommonInterface_iid "org.kycc.CommonInterface"

Q_DECLARE_INTERFACE(CommonInterface, CommonInterface_iid)

#endif // INTERFACE_H
