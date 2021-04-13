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
#include "userdispatcher.h"

#include <QDebug>

UserDispatcher::UserDispatcher(QString objpath, QObject *parent) :
    QObject(parent)
{
    this->setParent(parent);
    useriface = new QDBusInterface("org.freedesktop.Accounts",
                                     objpath,
                                     "org.freedesktop.Accounts.User",
                                     QDBusConnection::systemBus());
    pUserInfo = (UserInfo *)QObject::parent();


    userPropert = new QDBusInterface("org.freedesktop.Accounts",
                                     objpath,
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus());
}

UserDispatcher::~UserDispatcher()
{
    delete useriface;
    useriface = NULL;
}

// 来自gtk控制面板的加密代码
QString UserDispatcher::make_crypted (const gchar *plain){
    GString *salt;
    gchar *result;
    GRand *rand;
    gint i;

    rand = g_rand_new ();
    salt = g_string_sized_new (21);

    gchar salt_char[] = "ABCDEFGHIJKLMNOPQRSTUVXYZ"
                   "abcdefghijklmnopqrstuvxyz"
                   "./0123456789";

//    /* SHA 256 */
    g_string_append (salt, "$6$");
    for (i = 0; i < 16; i++) {
        g_string_append_c (salt, salt_char[g_rand_int_range(rand, 0, G_N_ELEMENTS (salt_char) )]);
    }
    g_string_append_c (salt, '$');

    result = g_strdup ((const gchar *)crypt(plain, salt->str)); //运行后找不到crypt undefined symbol: crypt

    g_string_free (salt, TRUE);
    g_rand_free (rand);

    return QString(result);

}

QString UserDispatcher::change_user_pwd(QString pwd, QString hint){
       QByteArray ba = pwd.toLatin1();
       QString pwdencryption = make_crypted(ba.data());

    QDBusReply<QString> reply = useriface->call("SetPassword", pwdencryption, hint);
//    if (reply.isValid())
//        qDebug() << reply.value();
//    else
//        qDebug() << reply.error().message();
    return "";
}

void UserDispatcher::change_user_type(int atype){
    useriface->call("SetAccountType", QVariant(atype));
}

void UserDispatcher::change_user_face(QString facefile){
    useriface->call("SetIconFile", QVariant(facefile));
}


void UserDispatcher::change_user_name(QString newName){
    useriface->call("SetRealName", QVariant(newName));
}

void UserDispatcher::change_user_autologin(QString username){
    QDBusInterface * tmpSysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                                          "/",
                                                          "com.control.center.interface",
                                                          QDBusConnection::systemBus());

    if (!tmpSysinterface->isValid()){
        qCritical() << "Create Client Interface Failed When : " << QDBusConnection::systemBus().lastError();
        return;
    }
    tmpSysinterface->call("setAutoLoginStatus", username);

    delete tmpSysinterface;
    tmpSysinterface = nullptr;
}

bool UserDispatcher::get_autoLogin_status() {
    QDBusReply<QVariant> reply = userPropert->call("Get", "org.freedesktop.Accounts.User", "AutomaticLogin");
    return reply.value().toBool();
}
