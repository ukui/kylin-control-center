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

#include "wifi.h"
#include "netconnectwork.h"
#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"

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
    void initSearchText();
    void initComponent();    
    void rebuildNetStatusComponent(QString iconPath, QString netName);
    void rebuildAvailComponent(QString iconpath, QString netName);

    void runExternalApp();
    void runKylinmApp();

    bool getwifiisEnable();

private:
    Ui::NetConnect *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    SwitchButton *wifiBtn;

    QMap<QString, int> connectedWifi;
    QMap<QString,int> wifiList;     //wifi list<name,Signal strength>
    QStringList lanList;            // list of wired network
    QString connectedLan;
    QGSettings *m_gsettings = nullptr;

    Wifi *m_wifiList;
    QThread *pThread;
    NetconnectWork *pNetWorker;

    QStringList TwifiList;
    QStringList TlanList;

    QString actLanName;

    bool is_refreshed;
    bool mFirstLoad;

private:

    QMap<QString, QListWidgetItem *> AvailableNetworkMap;

    int setSignal(QString lv);      //get wifi's strength
    QStringList execGetLanList();
    void getWifiListDone(QStringList wifislist, QStringList lanList);
    bool getSwitchStatus(QString key);

    /*
     * the wifi's origin status
     */
    bool getInitStatus();

    // clear the lan and wifi list
    void clearContent();

    void deleteNetworkDone(QString);
    void addNetworkDone(QString);
    void _buildWidgetForItem(QString);
    void initNetworkMap();

private slots:
    void wifiSwitchSlot(bool signal);
    void getNetList();

    void refreshed_signal_changed();
    void properties_changed_refresh();
    void reset_bool_is_refreshed();

signals:
    void refresh();
};

#endif // NETCONNECT_H
