/*
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * 
**/

#include "servicemanager.h"
#include <QDebug>
#include "biometricdeviceinfo.h"

#define SERVICE "biometric-authentication.service"
#define DBUS_SERVICE "org.ukui.Biometric"
#define DBUS_PATH "/org/ukui/Biometric"
#define DBUS_INTERFACE "org.ukui.Biometric"

#define FD_DBUS_SERVICE     "org.freedesktop.DBus"
#define FD_DBUS_PATH        "/org/freedesktop/DBus"
#define FD_DBUS_INTERFACE   "org.freedesktop.DBus"

ServiceManager *ServiceManager::instance_ = nullptr;

ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent),
      dbusService(nullptr),
      bioService(nullptr)
{
    init();
}

void ServiceManager::init()
{
    if(!dbusService)
    {
        dbusService = new QDBusInterface(FD_DBUS_SERVICE,
                                         FD_DBUS_PATH,
                                         FD_DBUS_INTERFACE,
                                         QDBusConnection::systemBus());
        connect(dbusService, SIGNAL(NameOwnerChanged(QString, QString, QString)),
                this, SLOT(onDBusNameOwnerChanged(QString,QString,QString)));
    }
}

ServiceManager *ServiceManager::instance()
{
    if(!instance_)
    {
        instance_ = new ServiceManager;
    }
    return instance_;
}

bool ServiceManager::connectToService()
{
    if(!bioService)
    {
        bioService = new QDBusInterface(DBUS_SERVICE,
                                        DBUS_PATH,
                                        DBUS_INTERFACE,
                                        QDBusConnection::systemBus());
    }
    return bioService->isValid();
}

void ServiceManager::onDBusNameOwnerChanged(const QString &name,
                                            const QString &oldOwner,
                                            const QString &newOwner)
{
    if(name == DBUS_SERVICE)
    {
        qDebug() << "service status changed:"
                 << (newOwner.isEmpty() ? "inactivate" : "activate");
        Q_EMIT serviceStatusChanged(!newOwner.isEmpty());
    }
}

/*!
 * \brief checkServiceExist
 * 检查生物识别后台服务是否已启动
 */
bool ServiceManager::serviceExists()
{
    QDBusReply<bool> reply = dbusService->call("NameHasOwner", DBUS_SERVICE);
    if(!reply.isValid())
    {
        qDebug() << "check service exists error:" << reply.error();
        return false;
    }
    return reply.value();
}

