/**
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#include "biometricproxy.h"
#include <QDebug>
#include <QDir>
#include <QDebug>

void setDefaultDevice(const QString &deviceName)
{
    QString configPath = QDir::homePath() + "/" + UKUI_BIOMETRIC_CONFIG_PATH;
    QSettings settings(configPath, QSettings::IniFormat);

    settings.setValue("DefaultDevice", deviceName);
    settings.sync();

    //由于greeter没有权限访问家目录，所以单独写一个配置文件
    QString configFile1 = QString("/var/lib/lightdm-data/%1/.biometric_auth/ukui_biometric.conf").arg(getenv("USER"));
    QSettings settings1(configFile1, QSettings::IniFormat);

    settings1.setValue("DefaultDevice", deviceName);
    settings1.sync();

}

QString getDefaultDevice()
{
    QString configPath = QDir::homePath() + "/" + UKUI_BIOMETRIC_CONFIG_PATH;
    QSettings settings(configPath, QSettings::IniFormat);

    QString defaultDevice = settings.value("DefaultDevice").toString();

    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }

    return defaultDevice;
}

BiometricProxy::BiometricProxy(QObject *parent)
    : QDBusAbstractInterface(BIOMETRIC_DBUS_SERVICE,
                             BIOMETRIC_DBUS_PATH,
                             BIOMETRIC_DBUS_INTERFACE,
                             QDBusConnection::systemBus(),
                             parent)
{
    registerMetaType();
    setTimeout(2147483647);

    configFile = QDir::homePath() + "/.biometric_auth/ukui_biometric.conf";
}

QDBusPendingCall BiometricProxy::Identify(int drvid, int uid, int indexStart, int indexEnd)
{
    QList<QVariant> argList;
    argList << drvid << uid << indexStart << indexEnd;
    return asyncCallWithArgumentList(QStringLiteral("Identify"), argList);
}

int BiometricProxy::GetFeatureCount(int uid, int indexStart, int indexEnd)
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return 0;
    }
    auto dbusArg =  result.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> variantList;
    dbusArg >> variantList;
    int res = 0;
    for(int i = 0; i < variantList.size(); i++)
    {

        DeviceInfoPtr pDeviceInfo = std::make_shared<DeviceInfo>();

        auto arg = variantList.at(i).value<QDBusArgument>();
        arg >> *pDeviceInfo;

        QDBusMessage FeatureResult = call(QStringLiteral("GetFeatureList"),pDeviceInfo->id,uid,indexStart,indexEnd);
        if(FeatureResult.type() == QDBusMessage::ErrorMessage)
        {
            	qWarning() << "GetFeatureList error:" << FeatureResult.errorMessage();
                return 0;
        }
        res += FeatureResult.arguments().takeFirst().toInt();
    }
    return res;
}

int BiometricProxy::StopOps(int drvid, int waiting)
{
    QDBusReply<int> reply = call(QStringLiteral("StopOps"), drvid, waiting);
    if(!reply.isValid())
    {
        qWarning() << "StopOps error:" << reply.error();
        return -1;
    }
    return reply.value();
}

QStringList BiometricProxy::getFeaturelist(int drvid, int uid, int indexStart, int indexEnd)
{
    QStringList list;
    QList<QDBusVariant> qlist;
    FeatureInfo *featureInfo;
    int listsize;
    QDBusMessage result = call(QStringLiteral("GetFeatureList"),drvid,uid,indexStart,indexEnd);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return list;
    }
    QList<QVariant> variantList = result.arguments();
    listsize = variantList[0].value<int>();
    variantList[1].value<QDBusArgument>() >> qlist;
    for (int i = 0; i < listsize; i++) {
        featureInfo = new FeatureInfo;
        qlist[i].variant().value<QDBusArgument>() >> *featureInfo;
        list.append(featureInfo->index_name);
        delete featureInfo;
    }
    return list;
}

bool BiometricProxy::renameFeature(int drvid, int uid, int index, QString newname)
{
    QDBusMessage result = call(QStringLiteral("Rename"),drvid,uid,index,newname);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return false;
    }
    return result.arguments().first().value<bool>();
}

bool BiometricProxy::deleteFeature(int drvid, int uid, int indexStart, int indexEnd)
{
    QDBusMessage result = call(QStringLiteral("Clean"),drvid,uid,indexStart,indexEnd);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return false;
    }
    return result.arguments().first().value<bool>();
}

DeviceList BiometricProxy::GetDrvList()
{
    QDBusMessage result = call(QStringLiteral("GetDrvList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return DeviceList();
    }
    auto dbusArg =  result.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> variantList;
    DeviceList deviceList;
    dbusArg >> variantList;
    for(int i = 0; i < variantList.size(); i++)
    {
        DeviceInfoPtr pDeviceInfo = std::make_shared<DeviceInfo>();

        auto arg = variantList.at(i).value<QDBusArgument>();
        arg >> *pDeviceInfo;

        deviceList.push_back(pDeviceInfo);
    }

    return deviceList;
}

DeviceList BiometricProxy::GetDevList()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return DeviceList();
    }
    auto dbusArg =  result.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> variantList;
    DeviceList deviceList;
    dbusArg >> variantList;
    for(int i = 0; i < variantList.size(); i++)
    {
        DeviceInfoPtr pDeviceInfo = std::make_shared<DeviceInfo>();

        auto arg = variantList.at(i).value<QDBusArgument>();
        arg >> *pDeviceInfo;

        deviceList.push_back(pDeviceInfo);
    }

    return deviceList;
}

int BiometricProxy::GetDevCount()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return 0;
    }
    int count = result.arguments().at(0).value<int>();
    return count;
}

QString BiometricProxy::GetDevMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetDevMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetNotifyMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetNotifyMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetNotifyMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetOpsMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetOpsMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetOpsMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

StatusReslut BiometricProxy::UpdateStatus(int drvid)
{
    StatusReslut status;
    QDBusMessage result = call(QStringLiteral("UpdateStatus"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "UpdateStatus error:" << result.errorMessage();
        status.result = -1;
        return status;
    }

    status.result           = result.arguments().at(0).toInt();
    status.enable           = result.arguments().at(1).toInt();
    status.devNum           = result.arguments().at(2).toInt();
    status.devStatus        = result.arguments().at(3).toInt();
    status.opsStatus        = result.arguments().at(4).toInt();
    status.notifyMessageId  = result.arguments().at(5).toInt();

    return status;
}

QString BiometricProxy::getDefaultDevice()
{
    QSettings settings(configFile, QSettings::IniFormat);

    return settings.value("DefaultDevice").toString();
}

void BiometricProxy::setDefaultDevice(const QString &deviceName)
{
    QSettings settings(configFile, QSettings::IniFormat);

    settings.setValue("DefaultDevice", deviceName);
    settings.sync();

    //由于greeter没有权限访问家目录，所以单独写一个配置文件
    QString configFile1 = QString("/var/lib/lightdm-data/%1/.biometric_auth/ukui_biometric.conf").arg(getenv("USER"));
    QSettings settings1(configFile1, QSettings::IniFormat);

    settings1.setValue("DefaultDevice", deviceName);
    settings1.sync();

}
