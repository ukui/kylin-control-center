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
#include "netconnect.h"
#include "ui_netconnect.h"

#include "commonComponent/HoverBtn/hoverbtn.h"

#include <QGSettings>
#include <QProcess>
#include <QTimer>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDir>
#include <QDebug>
#include <QtAlgorithms>

#define ITEMHEIGH           50
#define CONTROL_CENTER_WIFI "org.ukui.control-center.wifi.switch"

const QString KWifiSymbolic     = "network-wireless-signal-excellent";
const QString KWifiLockSymbolic = "network-wireless-secure-signal-excellent";
const QString KWifiGood         = "network-wireless-signal-good";
const QString KWifiLockGood     = "network-wireless-secure-signal-good";
const QString KWifiOK           = "network-wireless-signal-ok";
const QString KWifiLockOK       = "network-wireless-secure-signal-ok";
const QString KWifiLow          = "network-wireless-signal-low";
const QString KWifiLockLow      = "network-wireless-secure-signal-low";

bool sortByVal(const QPair<QString, int> &l, const QPair<QString, int> &r) {
    return (l.second < r.second);
}

NetConnect::NetConnect() :  mFirstLoad(true)
{
    pluginName = tr("Connect");
    pluginType = NETWORK;
}

NetConnect::~NetConnect()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString NetConnect::get_plugin_name() {
    return pluginName;
}

int NetConnect::get_plugin_type() {
    return pluginType;
}

QWidget *NetConnect::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::NetConnect;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        ui->detailBtn->setText(tr("Network settings"));

        wifiBtn = new SwitchButton(pluginWidget);

        ui->openWIifLayout->addWidget(wifiBtn);

        initTitleLabel();
        initSearchText();
        initComponent();
    }
    return pluginWidget;
}

void NetConnect::plugin_delay_control() {

}

void NetConnect::refreshed_signal_changed() {
    // 仅接收属性更改发出的第一个信号并更改is_refreshed状态
    if(is_refreshed){
        emit refresh();
        this->is_refreshed = false;
    } else {
        return;
    }
}

// 把判断列表是否已刷新的bool值is_refreshed重置为true
void NetConnect::reset_bool_is_refreshed() {
    this->is_refreshed = true;
}

void NetConnect::properties_changed_refresh() {
    getNetList();
    /*等待一段时间后把is_refreshed重置，等待是为了避免在dbus接收属性更改时收到
    多条信号并连续执行槽函数refreshed_signal_changed()并更改is_refreshed导致冲突*/
    QTimer::singleShot(1000, this, SLOT(reset_bool_is_refreshed()));
}

const QString NetConnect::name() const {

    return QStringLiteral("netconnect");
}

void NetConnect::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
}

void NetConnect::initSearchText() {
    //~ contents_path /netconnect/Netconnect Status
    ui->titleLabel->setText(tr("Netconnect Status"));
    //~ contents_path /netconnect/open wifi
    ui->openLabel->setText(tr("open wifi"));
}

void NetConnect::initComponent(){

    // 把判断列表是否已刷新的bool值初始化为true
    this->is_refreshed = true;

    // 接收到系统创建网络连接的信号时刷新可用网络列表
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager/Settings"), "org.freedesktop.NetworkManager.Settings", "NewConnection", this, SLOT(getNetList(void)));
    // 接收到系统删除网络连接的信号时刷新可用网络列表
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager/Settings"), "org.freedesktop.NetworkManager.Settings", "ConnectionRemoved", this, SLOT(getNetList(void)));
    // 接收到系统更改网络连接属性的信号时刷新可用网络列表
    // QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager"), "org.freedesktop.NetworkManager", "PropertiesChanged", this, SLOT(getNetList(void)));
    // 接收到系统更改网络连接属性时把判断是否已刷新的bool值置为false
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager"), "org.freedesktop.NetworkManager", "PropertiesChanged", this, SLOT(refreshed_signal_changed(void)));

    // 接收到刷新信号refresh()时执行刷新(仅适用与网络属性或当前连接的网络更改)
    connect(this, SIGNAL(refresh()), this, SLOT(properties_changed_refresh()));

    const QByteArray id(CONTROL_CENTER_WIFI);
    if(QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id, QByteArray(), this);

        connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
            if (key == "switchor") {
                bool judge = getSwitchStatus(key);
                wifiBtn->setChecked(judge);
            }
        });
    }

    // 详细设置按钮connect
    connect(ui->detailBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        runExternalApp();
    });

    connect(ui->RefreshBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        ui->RefreshBtn->setText(tr("Refreshing..."));
        ui->RefreshBtn->setEnabled(false);
        wifiBtn->setEnabled(false);
        QTimer::singleShot(1*1000,this,SLOT(getNetList()));
    });

    if (getwifiisEnable()) {
        wifiBtn->setChecked(getInitStatus());
    }
    connect(wifiBtn, &SwitchButton::checkedChanged, this,[=](bool checked){
        ui->RefreshBtn->setText(tr("Refreshing..."));
        ui->RefreshBtn->setEnabled(false);
        wifiBtn->setEnabled(false);
        ui->openWifiFrame->setVisible(false);

        wifiBtn->blockSignals(true);
        wifiSwitchSlot(checked);
        wifiBtn->blockSignals(false);
    });

    ui->RefreshBtn->setEnabled(false);
    wifiBtn->setEnabled(false);
    ui->openWifiFrame->setVisible(false);

    emit ui->RefreshBtn->clicked(true);
    ui->verticalLayout_2->setContentsMargins(0,0,32,0);
}

void NetConnect::rebuildNetStatusComponent(QString iconPath, QString netName) {

    QWidget * baseWidget = new QWidget();
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setContentsMargins(0, 0, 0, 2);

    QFrame * devFrame = new QFrame(baseWidget);
    devFrame->setFrameShape(QFrame::Shape::Box);
    devFrame->setMinimumWidth(550);
    devFrame->setMaximumWidth(960);
    devFrame->setMinimumHeight(50);
    devFrame->setMaximumHeight(50);

    QHBoxLayout * devHorLayout = new QHBoxLayout(devFrame);
    devHorLayout->setSpacing(8);
    devHorLayout->setContentsMargins(16, 0, 0, 0);

    QLabel * iconLabel = new QLabel(devFrame);
    QIcon searchIcon = QIcon::fromTheme(iconPath);
    iconLabel->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(24, 24))));

    QLabel * nameLabel = new QLabel(devFrame);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setScaledContents(true);
    if ("No Net" != netName) {
        nameLabel->setText(netName);
    }

    QLabel * statusLabel = new QLabel(devFrame);
    QSizePolicy statusSizePolicy = statusLabel->sizePolicy();
    statusSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    statusSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    statusLabel->setSizePolicy(statusSizePolicy);
    statusLabel->setScaledContents(true);
    if ("No Net" != netName) {
        statusLabel->setText(tr("connected"));
    } else {
        statusLabel->setText(tr("No network"));
    }

    devHorLayout->addWidget(iconLabel);
    devHorLayout->addWidget(nameLabel);
    devHorLayout->addWidget(statusLabel);
    devHorLayout->addStretch();

    devFrame->setLayout(devHorLayout);

    baseVerLayout->addWidget(devFrame);
    baseVerLayout->addStretch();

    baseWidget->setLayout(baseVerLayout);

    ui->statusLayout->addWidget(baseWidget);
}

void NetConnect::getNetList() {

    bool wifiSt = getwifiisEnable();
    if (!wifiSt) {
        wifiBtn->setChecked(wifiSt);
    }
    wifiBtn->setEnabled(wifiSt);
    ui->openWifiFrame->setVisible(wifiSt);

    this->TlanList = execGetLanList();
    pThread = new QThread;
    pNetWorker = new NetconnectWork;

    connect(pNetWorker, &NetconnectWork::wifiGerneral,this,[&](QStringList list){
        this->TwifiList = list;
        wifiBtn->setEnabled(wifiSt);
        getWifiListDone(this->TwifiList, this->TlanList);
        QMap<QString, int>::iterator iter = this->wifiList.begin();
        QVector<QPair<QString, int>> vec;
        QString iconamePah;

        while (iter != this->wifiList.end()) {
            vec.push_back(qMakePair(iter.key(), iter.value()));
            iter++;
        }
        qSort(vec.begin(), vec.end(), sortByVal);
        for (int i = 0; i < vec.size(); i++) {
            if (!wifiBtn->isChecked()){
                break;
            }
            bool isLock = vec[i].first.contains("lock");
            QString wifiName = isLock ? vec[i].first.remove("lock") : vec[i].first;
            iconamePah = wifiIcon(isLock, vec[i].second);
            rebuildAvailComponent(iconamePah, wifiName);
        }

        for (int i = 0; i < this->lanList.length(); i++) {
            iconamePah= ":/img/plugins/netconnect/eth.svg";
            rebuildAvailComponent(iconamePah , lanList.at(i));
        }
    });
    connect(pNetWorker, &NetconnectWork::workerComplete,this, [=] {
       pThread->quit();
       pThread->wait();
    });
    pNetWorker->moveToThread(pThread);
    connect(pThread, &QThread::started, pNetWorker, &NetconnectWork::run);
    connect(pThread, &QThread::finished, this, [=]{
        bool wifiSt = getwifiisEnable();
        wifiBtn->setEnabled(wifiSt);
        ui->openWifiFrame->setVisible(wifiSt);
        ui->RefreshBtn->setEnabled(true);
        ui->RefreshBtn->setText(tr("Refresh"));
    });
    connect(pThread, &QThread::finished, pNetWorker, &NetconnectWork::deleteLater);
    pThread->start();
}

void NetConnect::rebuildAvailComponent(QString iconPath, QString netName) {

    HoverBtn * wifiItem = new HoverBtn(netName, pluginWidget);
    wifiItem->mPitLabel->setText(netName);

    QIcon searchIcon = QIcon::fromTheme(iconPath);
    wifiItem->mPitIcon->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(24, 24))));
    wifiItem->mAbtBtn->setText(tr("Connect"));

    connect(wifiItem->mAbtBtn, &QPushButton::clicked, this, [=] {
        runKylinmApp();
    });

    ui->availableLayout->addWidget(wifiItem);
}

void NetConnect::runExternalApp() {
    QString cmd = "nm-connection-editor";
    QProcess process(this);
    process.startDetached(cmd);
}

void NetConnect::runKylinmApp() {
    QString cmd = "kylin-nm";
    QProcess process(this);
    process.startDetached(cmd);
}

bool NetConnect::getwifiisEnable() {
    QDBusInterface m_interface( "org.freedesktop.NetworkManager",
                                "/org/freedesktop/NetworkManager",
                                "org.freedesktop.NetworkManager",
                                QDBusConnection::systemBus() );

    QDBusReply<QList<QDBusObjectPath>> obj_reply = m_interface.call("GetAllDevices");
    if (!obj_reply.isValid()) {
        qDebug()<<"execute dbus method 'GetAllDevices' is invalid in func getObjectPath()";
    }

    QList<QDBusObjectPath> obj_paths = obj_reply.value();

    foreach (QDBusObjectPath obj_path, obj_paths) {
        QDBusInterface interface( "org.freedesktop.NetworkManager",
                                  obj_path.path(),
                                  "org.freedesktop.DBus.Introspectable",
                                  QDBusConnection::systemBus() );

        QDBusReply<QString> reply = interface.call("Introspect");
        if (!reply.isValid()) {
            qDebug()<<"execute dbus method 'Introspect' is invalid in func getObjectPath()";
        }

        if(reply.value().indexOf("org.freedesktop.NetworkManager.Device.Wired") != -1) {

        } else if (reply.value().indexOf("org.freedesktop.NetworkManager.Device.Wireless") != -1) {
            return true;
        }
    }
    return false ;
}

QStringList NetConnect::execGetLanList() {
    QProcess *lanPro = new QProcess(this);
    QString shellOutput = "";
    lanPro->start("nmcli -f type,device,name connection show");
    lanPro->waitForFinished();
    QString output = lanPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

    return slist;
}

void NetConnect::getWifiListDone(QStringList getwifislist, QStringList getlanList) {

    clearContent();

    QList<ActiveConInfo> lsActiveInfo;
    lsActiveInfo.clear();
    getActiveConInfo(lsActiveInfo);

    if (!getwifislist.isEmpty()){
        connectedWifi.clear();
        wifiList.clear();

        QString actWifiName;

        int index = 0;
        while (index < lsActiveInfo.size()) {
            if (lsActiveInfo[index].strConType == "wifi"
                    || lsActiveInfo[index].strConType == "802-11-wireless") {
                actWifiName = QString(lsActiveInfo[index].strConName);
                break;
            }
            index ++;
        }

        // 填充可用网络列表
        QString headLine = getwifislist.at(0);
        headLine = headLine.trimmed();
        int indexName = headLine.indexOf("SSID");
        int indexLock = headLine.indexOf("SECURITY");

        QStringList wnames;

        for (int i = 1; i < getwifislist.size(); i ++) {
            QString line = getwifislist.at(i);
            QString wsignal  = line.mid(0, indexLock).trimmed();
            QString lockType = line.mid(indexLock, indexName -indexLock).trimmed();
            QString wname    = line.mid(indexName).trimmed();

            // 过滤重复wifi
            if (wnames.contains(wname, Qt::CaseInsensitive)) {
                continue;
            }
            if (!wname.isEmpty() && wname != "--") {
                int strength = this->setSignal(wsignal);
                if (wname == actWifiName) {
                    if ("--" != lockType && !lockType.isEmpty()) {
                        wname += "lock";
                    }
                    connectedWifi.insert(wname, strength);
                }
                if ("--" != lockType && !lockType.isEmpty()) {
                    wname += "lock";
                }
                wifiList.insert(wname, strength);
                wnames.append(wname);
            }
        }
    }

    if (!getlanList.isEmpty()) {
        lanList.clear();
        connectedLan.clear();

        int indexLan = 0;
        while (indexLan < lsActiveInfo.size()) {
            if (lsActiveInfo[indexLan].strConType == "ethernet"
                    || lsActiveInfo[indexLan].strConType == "802-3-ethernet"){
                actLanName = lsActiveInfo[indexLan].strConName;
                break;
            }
            indexLan ++;
        }

        // 填充可用网络列表
        QString headLine = getlanList.at(0);
        int indexDevice, indexName;
        headLine = headLine.trimmed();

        bool isChineseExist = headLine.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
        if (isChineseExist) {
            indexDevice = headLine.indexOf("设备") + 2;
            indexName = headLine.indexOf("名称") + 4;
        } else {
            indexDevice = headLine.indexOf("DEVICE");
            indexName = headLine.indexOf("NAME");
        }

        for (int i =1 ;i < getlanList.length(); i++) {
            QString line = getlanList.at(i);
            QString ltype = line.mid(0, indexDevice).trimmed();
            QString nname = line.mid(indexName).trimmed();
            if (ltype  != "wifi" && ltype != "" && ltype != "--") {
                this->lanList << nname;
            }
        }
    }
    if (!this->connectedWifi.isEmpty()) {
        QMap<QString, int>::iterator iter = this->connectedWifi.begin();

        QString connectedWifiName = iter.key();
        int strength = iter.value();

        bool isLock = connectedWifiName.contains("lock");
        connectedWifiName = isLock ? connectedWifiName.remove("lock") : connectedWifiName;
        QString iconamePah = wifiIcon(isLock, strength);
        rebuildNetStatusComponent(iconamePah, connectedWifiName);
    }
    if (!this->actLanName.isEmpty()) {
        QString lanIconamePah = ":/img/plugins/netconnect/eth.svg";
        rebuildNetStatusComponent(lanIconamePah, this->actLanName);
    }

    if (this->connectedWifi.isEmpty() && this->actLanName.isEmpty()) {
        rebuildNetStatusComponent(":/img/plugins/netconnect/nonet.svg" , "No Net");
    }
}

bool NetConnect::getSwitchStatus(QString key) {
    if (!m_gsettings) {
        return true;
    }
    const QStringList list = m_gsettings->keys();
    if (!list.contains(key)) {
        return true;
    }
    bool res = m_gsettings->get(key).toBool();
    return res;
}

bool NetConnect::getInitStatus() {

    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );
    //获取当前wifi是否连接
    QDBusReply<QVariant> m_result = interface.call("Get", "org.freedesktop.NetworkManager", "WirelessEnabled");

    if (m_result.isValid()) {
        bool status = m_result.value().toBool();
        return status;
    } else {
        qDebug()<<"org.freedesktop.NetworkManager get invalid"<<endl;
        return false;
    }
}

void NetConnect::clearContent()
{
    if (ui->availableLayout->layout() != NULL) {
        QLayoutItem* item;
        while ((item = ui->availableLayout->layout()->takeAt(0)) != NULL ) {
            delete item->widget();
            delete item;
            item = nullptr;
        }
    }

    if (ui->statusLayout->layout() != NULL) {
        QLayoutItem* item;
        while ((item = ui->statusLayout->layout()->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
            item = nullptr;
        }
    }

    this->connectedLan.clear();
    this->connectedWifi.clear();
    this->actLanName.clear();
    this->wifiList.clear();
    this->lanList.clear();
    this->TlanList.clear();
    this->TwifiList.clear();
}

QString NetConnect::wifiIcon(bool isLock, int strength) {
    switch (strength) {
    case 1:
        return isLock ? KWifiLockSymbolic : KWifiSymbolic;
    case 2:
        return isLock ? KWifiLockGood : KWifiGood;
    case 3:
        return isLock ? KWifiLockOK : KWifiOK;
    case 4:
        return isLock ? KWifiLockLow : KWifiLow;
    default:
        return "";
    }

}

int NetConnect::setSignal(QString lv) {
    int signal = lv.toInt();
    int signalLv = 0;

    if(signal > 75){
        signalLv = 1;
    } else if(signal > 55 && signal <= 75){
        signalLv = 2;
    } else if(signal > 35 && signal <= 55){
        signalLv = 3;
    } else if(signal  <= 35){
        signalLv = 4;
    }

    return signalLv;
}

void NetConnect::wifiSwitchSlot(bool signal) {
    if(!m_gsettings) {
        return ;
    }
    const QStringList list = m_gsettings->keys();
    if (!list.contains("switch")) {
        return ;
    }
    m_gsettings->set("switch",signal);

    QTimer::singleShot(2*1000, this, SLOT(getNetList()));
}

void NetConnect::getActiveConInfo(QList<ActiveConInfo>& qlActiveConInfo) {
    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );

    QDBusMessage result = interface.call("Get", "org.freedesktop.NetworkManager", "ActiveConnections");
    QList<QVariant> outArgs = result.arguments();
    QVariant first = outArgs.at(0);
    QDBusVariant dbvFirst = first.value<QDBusVariant>();
    QVariant vFirst = dbvFirst.variant();
    const QDBusArgument &dbusArgs = vFirst.value<QDBusArgument>();

    QDBusObjectPath objPath;
    dbusArgs.beginArray();

    while (!dbusArgs.atEnd()) {
        dbusArgs >> objPath;

        QDBusInterface interfacePro( "org.freedesktop.NetworkManager",
                                  objPath.path(),
                                  "org.freedesktop.DBus.Properties",
                                  QDBusConnection::systemBus() );

        QDBusReply<QVariant> replyType = interfacePro.call("Get", "org.freedesktop.NetworkManager.Connection.Active", "Type");
        QDBusReply<QVariant> replyUuid = interfacePro.call("Get", "org.freedesktop.NetworkManager.Connection.Active", "Uuid");
        QDBusReply<QVariant> replyId   = interfacePro.call("Get", "org.freedesktop.NetworkManager.Connection.Active", "Id");

        ActiveConInfo activeNet;
        activeNet.strConName = replyId.value().toString();
        activeNet.strConType = replyType.value().toString();
        activeNet.strConUUID = replyUuid.value().toString();
        qlActiveConInfo.append(activeNet);
    }
    dbusArgs.endArray();
}
