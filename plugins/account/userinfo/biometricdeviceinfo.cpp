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
#include "biometricdeviceinfo.h"
#include <QApplication>
#include <QObject>


QString DeviceType::getDeviceType(int deviceType)
{
    if(deviceType >= __MAX_NR_TYPES)
    {
        return "";
    }
    QMetaEnum meta = QMetaEnum::fromType<Type>();
    const char *typeString = meta.valueToKey(deviceType);
    return QString(typeString);
}

QString DeviceType::getDeviceType_tr(int deviceType)
{
    switch(deviceType)
    {
    case FingerPrint:
        return tr("FingerPrint");
    case FingerVein:
        return tr("FingerVein");
    case Iris:
        return tr("Iris");
    case Face:
        return tr("Face");
    case VoicePrint:
        return tr("VoicePrint");
    default:
        return "";
    }
}

QDebug operator <<(QDebug stream, const DeviceInfo &deviceInfo)
{
    stream << "["
           << deviceInfo.id
           << deviceInfo.shortName
           << deviceInfo.fullName
           << deviceInfo.deviceType
           << deviceInfo.driverEnable
           << deviceInfo.deviceNum
           << "]";
    return stream;
}

QDBusArgument &operator <<(QDBusArgument &arg, const DeviceInfo &deviceInfo)
{
    arg.beginStructure();
    arg << deviceInfo.id
        << deviceInfo.shortName
        << deviceInfo.fullName
        << deviceInfo.driverEnable
        << deviceInfo.deviceNum
        << deviceInfo.deviceType
        << deviceInfo.storageType
        << deviceInfo.eigType
        << deviceInfo.verifyType
        << deviceInfo.identifyType
        << deviceInfo.busType
        << deviceInfo.deviceStatus
        << deviceInfo.OpsStatus;
    arg.endStructure();
    return arg;
}
const QDBusArgument &operator >>(const QDBusArgument &arg, DeviceInfo &deviceInfo)
{
    arg.beginStructure();
    arg >> deviceInfo.id
        >> deviceInfo.shortName
        >> deviceInfo.fullName
        >> deviceInfo.driverEnable
        >> deviceInfo.deviceNum
        >> deviceInfo.deviceType
        >> deviceInfo.storageType
        >> deviceInfo.eigType
        >> deviceInfo.verifyType
        >> deviceInfo.identifyType
        >> deviceInfo.busType
        >> deviceInfo.deviceStatus
        >> deviceInfo.OpsStatus;
    arg.endStructure();
    return arg;
}

/* For the type FeatureInfo */
QDBusArgument &operator<<(QDBusArgument &argument, const FeatureInfo &featureInfo)
{
    argument.beginStructure();
    argument << featureInfo.uid << featureInfo.biotype
        << featureInfo.device_shortname << featureInfo.index
        << featureInfo.index_name;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, FeatureInfo &featureInfo)
{
    argument.beginStructure();
    argument >> featureInfo.uid >> featureInfo.biotype
        >> featureInfo.device_shortname >> featureInfo.index
        >> featureInfo.index_name;
    argument.endStructure();
    return argument;
}

void registerMetaType()
{
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qDBusRegisterMetaType<DeviceInfo>();
}


QString GetDefaultDevice(const QString &userName)
{
//    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
//    QSettings settings(configPath, QSettings::IniFormat);
//    qDebug() << "configure path: " << settings.fileName();
	
    QString configPath = QDir::homePath() + "/" + UKUI_BIOMETRIC_CONFIG_PATH; 
    QSettings settings(configPath, QSettings::IniFormat);

    QString defaultDevice = settings.value("DefaultDevice").toString();

    if(defaultDevice.isEmpty())
    {
        QString configPath = QString("/var/lib/lightdm-data/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
	QSettings settings(configPath, QSettings::IniFormat);
	defaultDevice = settings.value("DefaultDevice").toString();
    }

    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }

    return defaultDevice;
}

static int getValueFromSettings(const QString &userName, const QString &key, int defaultValue = 3)
{
    //从家目录下的配置文件中获取
//    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QString configPath = QDir::homePath() + "/" + UKUI_BIOMETRIC_CONFIG_PATH;
    QSettings settings(configPath, QSettings::IniFormat);
    QString valueStr = settings.value(key).toString();

    //如果没有获取到，则从系统配置文件中获取
    if(valueStr.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        valueStr = sysSettings.value(key).toString();
    }

    bool ok;
    int value = valueStr.toInt(&ok);
    if( (value == 0 && !ok) || valueStr.isEmpty() )
    {
        value = defaultValue;
    }
    return value;
}

int GetMaxFailedAutoRetry(const QString &userName)
{
    return getValueFromSettings(userName, "MaxFailedAutoRetry");
}

int GetMaxTimeoutAutoRetry(const QString &userName)
{
    return getValueFromSettings(userName, "MaxTimeoutAutoRetry");
}
