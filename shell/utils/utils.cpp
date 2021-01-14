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
#include "utils.h"

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnection>
#include <QDebug>

#ifdef WITHKYSEC
#include <kysec/libkysec.h>
#include <kysec/status.h>
#endif

void Utils::centerToScreen(QWidget* widget) {
    if (!widget)
      return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}

void Utils::setCLIName(QCommandLineParser &parser) {

    QCommandLineOption monitorRoleOption(QStringList() << "m" << "display", QObject::tr("Go to monitor settings page"));
    QCommandLineOption defaultRoleOption("defaultapp", QObject::tr("Go to defaultapp settings page"));
    QCommandLineOption powerRoleOption(QStringList() << "p" << "power", QObject::tr("Go to power settings page"));
    QCommandLineOption autobootRoleOption("autoboot", QObject::tr("Go to autoboot settings page"));

    QCommandLineOption printerRoleOption("printer", QObject::tr("Go to printer settings page"));
    QCommandLineOption mouseRoleOption("mouse", QObject::tr("Go to mouse settings page"));
    QCommandLineOption touchpadRoleOption("touchpad", QObject::tr("Go to touchpad settings page"));
//    QCommandLineOption keyboardRoleOption("keyboard", QObject::tr("Go to keyboard settings page"));
    QCommandLineOption keyboardRoleOption(QStringList() << "k" << "keyboard", QObject::tr("Go to keyboard settings page"));
    QCommandLineOption shortcutRoleOption("shortcut", QObject::tr("Go to shortcut settings page"));
    QCommandLineOption audioRoleOption(QStringList() << "s" << "audio", QObject::tr("Go to audio settings page"));
    QCommandLineOption blutetoothRoleOption("bluetooth", QObject::tr("Go to bluetooth settings page"));

    QCommandLineOption backgroundRoleOption(QStringList() << "b" << "background", QObject::tr("Go to background settings page"));
    QCommandLineOption themeRoleOption("theme", QObject::tr("Go to theme settings page"));
    QCommandLineOption screenlockRoleOption("screenlock", QObject::tr("Go to screenlock settings page"));
    QCommandLineOption screensaverRoleOption("screensaver", QObject::tr("Go to screensaver settings page"));
    QCommandLineOption fontsRoleOption("fonts", QObject::tr("Go to fonts settings page"));
    QCommandLineOption desktopRoleOption(QStringList() << "d" << "desktop", QObject::tr("Go to desktop settings page"));

    QCommandLineOption netconnectRoleOption("netconnect", QObject::tr("Go to netconnect settings page"));
    QCommandLineOption vpnRoleOption(QStringList() << "g" << "vpn", QObject::tr("Go to vpn settings page"));
    QCommandLineOption proxyRoleOption("proxy", QObject::tr("Go to proxy settings page"));

    QCommandLineOption userinfoRoleOption(QStringList() << "u" << "userinfo", QObject::tr("Go to userinfo settings page"));
    QCommandLineOption cloudaccountRoleOption("cloudaccount", QObject::tr("Go to cloudaccount settings page"));

    QCommandLineOption datetimeRoleOption(QStringList() << "t" << "datetime", QObject::tr("Go to datetime settings page"));
    QCommandLineOption areaRoleOption("area", QObject::tr("Go to area settings page"));

    QCommandLineOption updateRoleOption("update", QObject::tr("Go to update settings page"));
    QCommandLineOption backupRoleOption("backup", QObject::tr("Go to backup settings page"));
    QCommandLineOption upgradeRoleOption("upgrade", QObject::tr("Go to upgrade settings page"));

    QCommandLineOption noticeRoleOption(QStringList() << "n" << "notice", QObject::tr("Go to notice settings page"));
    QCommandLineOption aboutRoleOption(QStringList() << "a" << "about", QObject::tr("Go to about settings page"));

    parser.addHelpOption();
    parser.addVersionOption();

    //三权分立开启
#ifdef WITHKYSEC
    if (!kysec_is_disabled() && kysec_get_3adm_status()){
        if (!getuid() || !geteuid()){
            //时间和日期 | 用户账户 | 电源管理 |网络连接 |网络代理|更新
            parser.addOption(powerRoleOption);
            parser.addOption(netconnectRoleOption);
            parser.addOption(vpnRoleOption);
            parser.addOption(proxyRoleOption);
            parser.addOption(userinfoRoleOption);
            parser.addOption(datetimeRoleOption);
            parser.addOption(updateRoleOption);
            parser.addOption(upgradeRoleOption);
            parser.addOption(backupRoleOption);
        }

    } else {
        parser.addOption(powerRoleOption);
        parser.addOption(netconnectRoleOption);
        parser.addOption(vpnRoleOption);
        parser.addOption(proxyRoleOption);
        parser.addOption(userinfoRoleOption);
        parser.addOption(datetimeRoleOption);
        parser.addOption(updateRoleOption);
        parser.addOption(upgradeRoleOption);
        parser.addOption(backupRoleOption);
    }

#else
    parser.addOption(powerRoleOption);
    parser.addOption(netconnectRoleOption);
    parser.addOption(vpnRoleOption);
    parser.addOption(proxyRoleOption);
    parser.addOption(userinfoRoleOption);
    parser.addOption(datetimeRoleOption);
    parser.addOption(updateRoleOption);
    parser.addOption(upgradeRoleOption);

#endif

    parser.addOption(monitorRoleOption);
    parser.addOption(defaultRoleOption);
    parser.addOption(autobootRoleOption);

    parser.addOption(printerRoleOption);
    parser.addOption(mouseRoleOption);
    parser.addOption(touchpadRoleOption);
    parser.addOption(keyboardRoleOption);
    parser.addOption(shortcutRoleOption);
    parser.addOption(audioRoleOption);
    parser.addOption(blutetoothRoleOption);

    parser.addOption(backgroundRoleOption);
    parser.addOption(themeRoleOption);
    parser.addOption(screenlockRoleOption);
    parser.addOption(screensaverRoleOption);
    parser.addOption(fontsRoleOption);
    parser.addOption(desktopRoleOption);

    parser.addOption(cloudaccountRoleOption);

    parser.addOption(areaRoleOption);

    parser.addOption(noticeRoleOption);
    parser.addOption(aboutRoleOption);
}

QVariantMap Utils::getModuleHideStatus() {
    QDBusInterface m_interface( "org.ukui.ukcc.session",
                                "/",
                                "org.ukui.ukcc.session.interface",
                                QDBusConnection::sessionBus());

    QDBusReply<QVariantMap> obj_reply = m_interface.call("getModuleHideStatus");
    if (!obj_reply.isValid()) {
        qDebug()<<"execute dbus method getModuleHideStatus failed";
    }
    return obj_reply.value();
}
