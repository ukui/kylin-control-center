/*
* Copyright (C) 2020 Tianjin KYLIN Information Technology Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/

#include <QtDBus/QDBusConnection>
#include <QCoreApplication>
#include <QDebug>
#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusMessage>
#include "group_manager_server.h"
#include "custom_struct.h"
#include "groupmanager_adaptor.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

    qRegisterMetaType<custom_struct>("custom_struct");
    qDBusRegisterMetaType<custom_struct>();
	QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.registerService("org.ukui.groupmanager")) {
		qDebug() << "error:" << connection.lastError().message();
//		exit(-1);
	}
    group_manager_server *dbus_demo = new group_manager_server();
    new InterfaceAdaptor(dbus_demo);
    connection.registerObject("/org/ukui/groupmanager", dbus_demo);
    QDBusMessage msg = QDBusMessage::createSignal("/org/ukui/groupmanager", "org.ukui.groupmanager.interface", "message");
    QDBusConnection::systemBus().send(msg);
	return a.exec();	
}
