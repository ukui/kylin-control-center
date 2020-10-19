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
#ifndef USERDISPATCHER_H
#define USERDISPATCHER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>

#include <unistd.h>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
}

class UserInfo;

class UserDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit UserDispatcher(QString objpath, QObject *parent = 0);
    ~UserDispatcher();

    QString change_user_pwd(QString pwd, QString hint);
    void change_user_type(int atype);
    void change_user_face(QString facefile);
    void change_user_autologin(QString username);
    bool get_autoLogin_status();

private:
    QDBusInterface * useriface;
    QDBusInterface * userPropert;
    QDBusInterface * iproperty;

    UserInfo * pUserInfo;

    /***加密,来自gtk控制面板***/
    QString make_crypted(const gchar * plain);
    /*****/

private slots:
    void propertyChanged(QString, QMap<QString, QVariant>, QStringList);
};

#endif // USERDISPATCHER_H
