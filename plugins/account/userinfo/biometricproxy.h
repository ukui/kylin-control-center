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
#ifndef BIOMETRICPROXY_H
#define BIOMETRICPROXY_H

#include <QDBusAbstractInterface>
#include <QStringList>
#include "biometricdeviceinfo.h"

#define SERVICE "biometric-authentication.service"
#define DBUS_SERVICE "org.ukui.Biometric"
#define DBUS_PATH "/org/ukui/Biometric"
#define DBUS_INTERFACE "org.ukui.Biometric"

/**
 * @brief UpdateStauts调用返回的结果
 */
struct StatusReslut
{
    int result;
    int enable;
    int devNum;
    int devStatus;
    int opsStatus;
    int notifyMessageId;
};

void setDefaultDevice(const QString &deviceName);
QString getDefaultDevice();

/**
 * @brief USB设备插拔动作
 */
enum USBDeviceAction
{
    ACTION_ATTACHED = 1,
    ACTION_DETACHED = -1
};

/**
 * @brief DBus代理类，负责调用对应的DBus接口
 */
class BiometricProxy : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    explicit BiometricProxy(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 使用指定id的设备进行用户认证
     * @param drvid         驱动（设备）id
     * @param uid           用户id
     * @param indexStart    用于认证的特征索引范围
     * @param indexEnd
     * @return  结果：<int result, int uid> （结果，用户id)
     */
    QDBusPendingCall Identify(int drvid, int uid, int indexStart = 0, int indexEnd = -1);
    /**
     * @brief 终止设备上正在进行的操作
     * @param drvid     设备id
     * @param waiting   等待时间（秒）
     * @return
     */
    int StopOps(int drvid, int waiting = 5);
    /**
     * @brief 删除生物特征
     * @return
     */
    bool deleteFeature(int drvid, int uid, int indexStart = 0, int indexEnd = -1);
    /**
     * @brief 重命名生物特征
     * @return
     */
    bool renameFeature(int drvid, int uid, int index ,QString newname);
    /**
     * @brief 获取当前用户已连接设备对应特征数目
     * @param uid              用户id
     * @param indexStart       用于认证的特征索引范围
     * @param indexEnd
     * @return
     */
    int GetFeatureCount(int uid, int indexStart = 0, int indexEnd = -1);
    QStringList getFeaturelist(int drvid,int uid,int indexStart = 0,int indexEnd = -1);

    /**
     * @brief 获取已连接的设备列表
     * @return
     */
    DeviceList GetDevList();
    /**
     * @brief 获取驱动列表
     * @return
     */
    DeviceList GetDrvList();
    /**
     * @brief 获取设备数量
     * @return
     */
    int GetDevCount();
    /**
     * @brief 获取设备消息
     * @param drvid 驱动id
     * @return
     */
    QString GetDevMesg(int drvid);
    /**
     * @brief GetNotifyMesg 获取通知消息
     * @param drvid 驱动id
     * @return
     */
    QString GetNotifyMesg(int drvid);
    /**
     * @brief GetOpsMesg    获取操作消息
     * @param drvid 驱动id
     * @return
     */
    QString GetOpsMesg(int drvid);
    /**
     * @brief UpdateStatus  获取更新的设备状态
     * @param drvid 驱动id
     * @return  结果：<int result, int enable, int devNum,
     *                int devStatus, int opsStatus, notifyMessageId, ...>
     */
    StatusReslut UpdateStatus(int drvid);

    QString getDefaultDevice();
    void setDefaultDevice(const QString &deviceName);

private:
    QString configFile;


Q_SIGNALS:
    /**
     * @brief 设备状态发生变化
     * @param drvid     设备id
     * @param status    设备状态
     */
    void StatusChanged(int drvid, int status);
    /**
     * @brief USB设备热插拔
     * @param drvid     设备id
     * @param action    插拔动作（1：插入，-1：拔出）
     * @param deviceNum 插拔动作后该驱动拥有的设备数量
     */
    void USBDeviceHotPlug(int drvid, int action, int deviceNum);

};

#endif // BIOMETRICPROXY_H
