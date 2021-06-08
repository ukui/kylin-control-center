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
#ifndef NETCONNECT_H
#define NETCONNECT_H


#include <QObject>
#include <QtPlugin>

#include <QNetworkInterface>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QtNetwork>

#include <QTimer>
#include <QStringList>
#include <QString>
#include <QGSettings>
#include <QListWidget>
#include <QListWidgetItem>
#include <HoverWidget/hoverwidget.h>
#include <QMap>

#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusInterface>
#include <QDBusReply>

#include "netconnectwork.h"
#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"
#include "netdetail.h"

enum {
    DISCONNECTED,
    NOINTERNET,
    CONNECTED
};

typedef enum {
    LOOPBACK,
    ETHERNET,
    WIFI
}NetType;

typedef struct _CardInfo{
    QString name;
    NetType type;
    bool status;
}CardInfo;

namespace Ui {
class NetConnect;
}

typedef struct ActiveConInfo_s {
    QString strConName;
    QString strConUUID;
    QString strConType;
    QString strSecType;
    QString strChan;
    QString strSpeed;
    QString strMac;
    QString strHz;

    QString strIPV4Address;
    QString strIPV4Prefix;
    QString strIPV4Dns;
    QString strIPV4GateWay;

    QString strBandWidth;
    QString strIPV6Address;
    QString strIPV6GateWay;
    QString strIPV6Prefix;
}ActiveConInfo;

class NetConnect : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    NetConnect();
    ~NetConnect();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initTitleLabel();
    void initSearchText();
    void initComponent();
    void rebuildNetStatusComponent(QString iconPath, QString netName);
    void rebuildAvailComponent(QString iconpath, QString netName, QString type);

    void runExternalApp();
    void runKylinmApp(QString netName, QString type);

    bool getwifiisEnable();

    int getActiveConInfo(QList<ActiveConInfo>& qlActiveConInfo);

private:
    Ui::NetConnect     *ui;

    QString            pluginName;
    int                pluginType;
    QWidget            *pluginWidget;


    QDBusInterface     *m_interface = nullptr;
    SwitchButton       *wifiBtn;

    QMap<QString, int> connectedWifi;
    QMap<QString,int>  wifiList;

    QThread            *pThread;
    NetconnectWork     *pNetWorker;

    QString            connectedLan;
    QString            actLanName;

    QStringList        TwifiList;
    QStringList        TlanList;
    QStringList        lanList;
    QStringList        wifilist;

    bool               mFirstLoad;
    bool               mIsLanVisible = false;
    bool               mIsWlanVisible = false;

    NetDetail          *mWlanDetail;
    NetDetail          *mLanDetail;

    QList<ActiveConInfo> mActiveInfo;
    QTimer             *refreshTimer;
    QString             prefreChan;

    QString             mPreWifiConnectedName;
    QString             mPreLanConnectedName;
    int                 runCount = 0;
private:
    int         setSignal(QString lv);
    QStringList execGetLanList();
    int         getWifiListDone(QVector<QStringList> wifislist, QStringList lanList, bool getWifiListDone);
    QString     geiWifiChan();
    QString     getWifiSpeed();
    bool        getInitStatus();
    bool        getWifiStatus();

    void        clearContent();

    void        deleteNetworkDone(QString);
    void        addNetworkDone(QString);
    void        _buildWidgetForItem(QString);
    void        initNetworkMap();
    void        setWifiBtnDisable();
    void        setNetDetailVisible();                              // 设置网络刷新状态
    QString     wifiIcon(bool isLock, int strength,int category);
    QString     wifiIcon(bool isLock, int strength);
    QList<QVariantMap> getDbusMap(const QDBusMessage &dbusMessage);
private slots:
    void wifiSwitchSlot(bool status);
    void getNetList();
    void netPropertiesChangeSlot(QMap<QString, QVariant> property);
    void netDetailSlot(QString netName);
    void refreshNetInfoTimerSlot();
    void refreshNetInfoSlot();
signals:
    void refresh();
};

Q_DECLARE_METATYPE(QList<QDBusObjectPath>);

#endif // NETCONNECT_H
