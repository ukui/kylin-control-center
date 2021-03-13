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
#ifndef BIOMETRICDEVICEINFO_H
#define BIOMETRICDEVICEINFO_H

#include <memory>
#include <QtDBus>

#define BIOMETRIC_DBUS_SERVICE      "org.ukui.Biometric"
#define BIOMETRIC_DBUS_PATH         "/org/ukui/Biometric"
#define BIOMETRIC_DBUS_INTERFACE    "org.ukui.Biometric"

#define UKUI_BIOMETRIC_IMAGES_PATH  "/usr/share/ukui-biometric/images/"
#define UKUI_BIOMETRIC_CONFIG_PATH  ".biometric_auth/ukui_biometric.conf"
#define UKUI_BIOMETRIC_SYS_CONFIG_PATH  "/etc/biometric-auth/ukui-biometric.conf"

#define BIOMETRIC_PAM_DOUBLE    "BIOMETRIC_PAM_DOUBLE"
#define BIOMETRIC_PAM       "BIOMETRIC_PAM"
#define BIOMETRIC_IGNORE    "BIOMETRIC_IGNORE"
#define BIOMETRIC_SUCCESS   "BIOMETRIC_SUCCESS"

/**
 * @brief 设备类型
 */
class DeviceType : public QObject
{
    Q_OBJECT
public:
    DeviceType();
    enum Type {
        FingerPrint,
        FingerVein,
        Iris,
        Face,
        VoicePrint,
        __MAX_NR_TYPES
    };
    Q_ENUM(Type)
    /**
     * @brief 获取设备类型的字符串表现形式
     * @param deviceType 设备类型
     * @return
     */
    static QString getDeviceType(int deviceType);

    /**
     * @brief 获取设备类型的国际化字符串
     * @param deviceType 设备类型
     * @return
     */
    static QString getDeviceType_tr(int deviceType);
};

/**
 * @brief StatusChanged D-Bus 信号触发时的状态变化类型
 */
enum StatusType {
    STATUS_DEVICE,
    STATUS_OPERATION,
    STATUS_NOTIFY
};

/**
 * @brief 识别、终止操作等DBus调用的结果，即返回值里的 result
 */
enum DBusResult {
    DBUS_RESULT_SUCCESS = 0,
    DBUS_RESULT_NOTMATCH = -1,
    DBUS_RESULT_ERROR = -2,
    DBUS_RESULT_DEVICEBUSY = -3,
    DBUS_RESULT_NOSUCHDEVICE = -4,
    DBUS_RESULT_PERMISSIONDENIED = -5
};

/**
 * @brief 识别操作（Identify）的ops状态
 */
enum IdentifyOpsStatus {
    IDENTIFY_MATCH = 400,
    IDENTIFY_NOTMATCH,
    IDENTIFY_ERROR,
    IDENTIFY_STOPBYUSER,
    IDENTIFY_TIMEOUT,
    IDENTIFY_MAX
};

/**
 * @brief 设备的信息
 */
struct DeviceInfo
{
    int         id;
    QString     shortName;
    QString     fullName;
    int         driverEnable;
    int         deviceNum;
    int         deviceType;
    int         storageType;
    int         eigType;
    int         verifyType;
    int         identifyType;
    int         busType;
    int         deviceStatus;
    int         OpsStatus;
};

struct FeatureInfo {
    int uid;
    int biotype;
    QString device_shortname;
    int index;
    QString index_name;
};

enum BioType {
    BIOTYPE_FINGERPRINT,
    BIOTYPE_FINGERVEIN,
    BIOTYPE_IRIS,
    BIOTYPE_FACE,
    BIOTYPE_VOICEPRINT,
    __MAX_NR_BIOTYPES
};


QString transferBioType(int type);

QDBusArgument &operator <<(QDBusArgument &arg, const DeviceInfo &deviceInfo);
const QDBusArgument &operator >>(const QDBusArgument &arg, DeviceInfo &deviceInfo);
QDBusArgument &operator<<(QDBusArgument &argument, const FeatureInfo &featureInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, FeatureInfo &featureInfo);

void registerMetaType();

typedef std::shared_ptr<DeviceInfo> DeviceInfoPtr;
typedef QList<DeviceInfoPtr> DeviceList;
typedef QMap<int, DeviceList> DeviceMap;

QDebug operator <<(QDebug stream, const DeviceInfo &deviceInfo);

Q_DECLARE_METATYPE(DeviceInfo)

/**
 * @brief 获取默认设备
 * @return
 */
QString GetDefaultDevice(const QString &userName);

/**
 * @brief 获取失败后自动重新开始的最大次数
 * @param userName
 * @return
 */
int GetMaxFailedAutoRetry(const QString &userName);

/**
 * @brief 获取超时后自动重新开始的最大次数
 * @param userName
 * @return
 */
int GetMaxTimeoutAutoRetry(const QString &userName);

#endif // BIOMETRICDEVICEINFO_H
