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
#include "../shell/utils/utils.h"

#include <QGSettings>
#include <QProcess>
#include <QTimer>
#include <QtDBus>
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
const QString KWifiNone         = "network-wireless-signal-none";
const QString KWifiLockNone     = "network-wireless-secure-signal-none";
const QString KLanSymbolic      = ":/img/plugins/netconnect/eth.svg";
const QString NoNetSymbolic     = ":/img/plugins/netconnect/nonet.svg";

const QString KWifi6Symbolic     = ":/img/plugins/netconnect/wifi6-full.svg";
const QString KWifi6LockSymbolic = ":/img/plugins/netconnect/wifi6-full-pwd.svg";
const QString KWifi6Good         = ":/img/plugins/netconnect/wifi6-high.svg";
const QString KWifi6LockGood     = ":/img/plugins/netconnect/wifi6-high-pwd.svg";
const QString KWifi6OK           = ":/img/plugins/netconnect/wifi6-medium.svg";
const QString KWifi6LockOK       = ":/img/plugins/netconnect/wifi6-medium-pwd.svg";
const QString KWifi6Low          = ":/img/plugins/netconnect/wifi6-low.svg";
const QString KWifi6LockLow      = ":/img/plugins/netconnect/wifi6-low-pwd.svg";
const QString KWifi6None         = ":/img/plugins/netconnect/wifi6-none.svg";
const QString KWifi6LockNone     = ":/img/plugins/netconnect/wifi6-none-pwd.svg";

const QString KWifi6ProSymbolic     = ":/img/plugins/netconnect/wifi6+-full.svg";
const QString KWifi6ProLockSymbolic = ":/img/plugins/netconnect/wifi6+-full-pwd.svg";
const QString KWifi6ProGood         = ":/img/plugins/netconnect/wifi6+-high.svg";
const QString KWifi6ProLockGood     = ":/img/plugins/netconnect/wifi6+-high-pwd.svg";
const QString KWifi6ProOK           = ":/img/plugins/netconnect/wifi6+-medium.svg";
const QString KWifi6ProLockOK       = ":/img/plugins/netconnect/wifi6+-medium-pwd.svg";
const QString KWifi6ProLow          = ":/img/plugins/netconnect/wifi6+-low.svg";
const QString KWifi6ProLockLow      = ":/img/plugins/netconnect/wifi6+-low-pwd.svg";
const QString KWifi6ProNone         = ":/img/plugins/netconnect/wifi6+-none.svg";
const QString KWifi6ProLockNone     = ":/img/plugins/netconnect/wifi6+-none-pwd.svg";

bool sortByVal(const QPair<QString, int> &l, const QPair<QString, int> &r) {
    return (l.second < r.second);
}
NetConnect::NetConnect() :  mFirstLoad(true) {
    pluginName = tr("Connect");
    pluginType = NETWORK;
}

NetConnect::~NetConnect() {
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
    delete m_interface;
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
        refreshTimer = new QTimer();
        qDBusRegisterMetaType<QVector<QStringList>>();
        m_interface = new QDBusInterface("com.kylin.network", "/com/kylin/network",
                                         "com.kylin.network",
                                         QDBusConnection::sessionBus());
        if(!m_interface->isValid()) {
            qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        }
        initTitleLabel();
        initSearchText();
        initComponent();
    }
    return pluginWidget;
}

void NetConnect::plugin_delay_control() {

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
    ui->detailBtn->setText(tr("Network settings"));
    //~ contents_path /netconnect/Netconnect Status
    ui->titleLabel->setText(tr("Netconnect Status"));
    //~ contents_path /netconnect/open WLAN
    ui->openLabel->setText(tr("open WLAN"));
}

void NetConnect::initComponent() {
    wifiBtn = new SwitchButton(pluginWidget);
    ui->openWIifLayout->addWidget(wifiBtn);

    mWlanDetail = new NetDetail(true, pluginWidget);
    mLanDetail  = new NetDetail(false, pluginWidget);

    ui->detailLayOut->addWidget(mWlanDetail);
    ui->detailLayOut->addWidget(mLanDetail);

    mLanDetail->setVisible(false);
    mWlanDetail->setVisible(false);

    // 接收到系统创建网络连接的信号时刷新可用网络列表
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager/Settings"), "org.freedesktop.NetworkManager.Settings", "NewConnection", this, SLOT(getNetList(void)));
    // 接收到系统删除网络连接的信号时刷新可用网络列表
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager/Settings"), "org.freedesktop.NetworkManager.Settings", "ConnectionRemoved", this, SLOT(getNetList(void)));
    // 接收到系统更改网络连接属性时把判断是否已刷新的bool值置为false
    QDBusConnection::systemBus().connect(QString(), QString("/org/freedesktop/NetworkManager"), "org.freedesktop.NetworkManager", "PropertiesChanged", this, SLOT(netPropertiesChangeSlot(QMap<QString,QVariant>)));
    // 无线网络断开或连接时刷新可用网络列表
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(getNetList()));
    connect(m_interface, SIGNAL(getWifiListFinished()), this, SLOT(refreshNetInfoTimerSlot()));
    // 有线网络断开或连接时刷新可用网络列表
    connect(m_interface,SIGNAL(actWiredConnectionChanged()), this, SLOT(getNetList()));
    // 网络配置信息发生变化时刷新可用网络列表
    connect(m_interface,SIGNAL(configurationChanged()), this, SLOT(refreshNetInfoSlot()));

    connect(ui->RefreshBtn, &QPushButton::clicked, this, [=](bool checked) {
        Q_UNUSED(checked)
        setWifiBtnDisable();
        if (m_interface) {
            m_interface->call("requestRefreshWifiList");
        }
        if (!getWifiStatus()) {
            getNetList();
        }
    });

    connect(ui->detailBtn, &QPushButton::clicked, this, [=](bool checked) {
        Q_UNUSED(checked)
        runExternalApp();
    });

    if (getwifiisEnable()) {
        wifiBtn->setChecked(getInitStatus());
    }
    connect(wifiBtn, &SwitchButton::checkedChanged, this,[=](bool checked) {
        wifiBtn->blockSignals(true);
        wifiSwitchSlot(checked);
        wifiBtn->blockSignals(false);
        if (m_interface) {
            m_interface->call("requestRefreshWifiList");
        }
    });

    ui->RefreshBtn->setEnabled(false);
    wifiBtn->setEnabled(false);
    ui->openWifiFrame->setVisible(false);
    emit ui->RefreshBtn->clicked(true);
    ui->verticalLayout_2->setContentsMargins(0, 0, 32, 0);
}

void NetConnect::refreshNetInfoTimerSlot() {
    refreshTimer->start(400);
}

void NetConnect::refreshNetInfoSlot() {
    emit ui->RefreshBtn->clicked(true);
    if (mLanDetail->isVisible()) {
        mLanDetail->setVisible(false);
        mIsLanVisible = !mIsLanVisible;
    }
    if (mWlanDetail->isVisible()) {
        mWlanDetail->setVisible(false);
        mIsWlanVisible = !mIsWlanVisible;
    }
}
void NetConnect::rebuildNetStatusComponent(QString iconPath, QString netName) {
    bool hasNet = false;
    if (netName == "无连接" || netName == "No net") {
        hasNet = true;
    }
    HoverBtn * deviceItem;
    if (hasNet || Utils::isWayland()) {
        deviceItem = new HoverBtn(netName, false, pluginWidget);
    } else {
        deviceItem = new HoverBtn(netName, true, pluginWidget);
    }
    deviceItem->mPitLabel->setText(netName);

    if (!hasNet) {
        deviceItem->mDetailLabel->setText(tr("Connected"));
    } else {
        deviceItem->mDetailLabel->setText("");
    }
    QIcon searchIcon = QIcon::fromTheme(iconPath);
    deviceItem->mPitIcon->setProperty("useIconHighlightEffect", 0x10);
    deviceItem->mPitIcon->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(24, 24))));

    deviceItem->mAbtBtn->setMinimumWidth(100);
    deviceItem->mAbtBtn->setText(tr("Detail"));

    connect(deviceItem->mAbtBtn, &QPushButton::clicked, this, [=] {
        netDetailSlot(deviceItem->mName);
    });

    ui->statusLayout->addWidget(deviceItem);
}

void NetConnect::getNetList() {
    refreshTimer->stop();
    wifiBtn->blockSignals(true);
    wifiBtn->setChecked(getInitStatus());
    wifiBtn->blockSignals(false);

    this->TlanList.clear();
    this->wifilist.clear();

    bool isWayland = false;
    if (Utils::isWayland()) {
        isWayland = true;
    }
    QDBusReply<QVector<QStringList>> reply = m_interface->call("getWifiList");
    if (!reply.isValid()) {
        qWarning() << "value method called failed!";
    }
    this->TlanList  = execGetLanList();
    if (getWifiStatus() && reply.value().length() == 1) {
        QElapsedTimer time;
        time.start();
        while (time.elapsed() < 300) {
            QCoreApplication::processEvents();
        }
        if (m_interface) {
            m_interface->call("requestRefreshWifiList");
        }
    } else {
        if (getWifiListDone(reply, this->TlanList, isWayland) == -1) {
            getNetList();
        } else {
            // 拿到的wifi列表当无线网络已连接时0位信息为已连接wifi信息，未连接时为"--"，过滤掉即可
            for (int i = 1; i < reply.value().length(); i++) {
                QString wifiName;
                if (isWayland) {
                    wifiName = reply.value().at(i).at(0) + reply.value().at(i).at(5);
                } else {
                    wifiName = reply.value().at(i).at(0);
                }
                if (reply.value().at(i).at(2) != NULL && reply.value().at(i).at(2) != "--") {
                    wifiName += "lock";
                }
                QString signal = reply.value().at(i).at(1);
                int sign = this->setSignal(signal);
                wifilist.append(wifiName + QString::number(sign));
            }
            QString iconamePath;
            for (int i = 0; i < wifilist.size(); i++) {
                if (!wifiBtn->isChecked()) {
                    break;
                }
                QString wifiInfo = wifilist.at(i);
                bool isLock = wifiInfo.contains("lock");
                QString wifiName = wifiInfo.left(wifiInfo.size() - 1);
                int wifiStrength = wifiInfo.right(1).toInt();
                wifiName = isLock ? wifiName.remove("lock") : wifiName;
                if (isWayland) {
                    int category = wifiName.right(1).toInt();
                    wifiName = wifiName.left(wifiName.size() - 1);
                    iconamePath = wifiIcon(isLock, wifiStrength, category);
                } else {
                    iconamePath = wifiIcon(isLock, wifiStrength);
                }
                rebuildAvailComponent(iconamePath, wifiName, "wifi");
            }
            for (int i = 0; i < this->lanList.length(); i++) {
                rebuildAvailComponent(KLanSymbolic , lanList.at(i), "ethernet");
            }
            setNetDetailVisible();
        }
    }
}

void NetConnect::netPropertiesChangeSlot(QMap<QString, QVariant> property) {
    if (property.keys().contains("WirelessEnabled")) {
        setWifiBtnDisable();
        if (m_interface) {
            m_interface->call("requestRefreshWifiList");
        }
    }
}

void NetConnect::netDetailSlot(QString netName) {
    foreach (ActiveConInfo netInfo, mActiveInfo) {
        if (!netInfo.strConName.compare(netName, Qt::CaseInsensitive)) {
            if (!netInfo.strConType.compare("802-3-ethernet", Qt::CaseInsensitive)) {
                mIsLanVisible = !mIsLanVisible;
                mLanDetail->setSSID(netInfo.strConName);
                mLanDetail->setProtocol(netInfo.strConType);
                mLanDetail->setIPV4(netInfo.strIPV4Address);
                mLanDetail->setIPV4Dns(netInfo.strIPV4Dns);
                mLanDetail->setIPV4Gateway(netInfo.strIPV4GateWay);
                mLanDetail->setIPV4Mask(netInfo.strIPV4Prefix);
                mLanDetail->setIPV6(netInfo.strIPV6Address);
                mLanDetail->setIPV6Prefix(netInfo.strIPV6Prefix);
                mLanDetail->setIPV6Gt(netInfo.strIPV6GateWay);
                mLanDetail->setMac(netInfo.strMac);
                mLanDetail->setBandWidth(netInfo.strBandWidth);
                mLanDetail->setVisible(mIsLanVisible);
            } else {
                mIsWlanVisible = !mIsWlanVisible;

                mWlanDetail->setSSID(netInfo.strConName);
                mWlanDetail->setProtocol(netInfo.strConType);
                mWlanDetail->setSecType(netInfo.strSecType);
                mWlanDetail->setHz(netInfo.strHz);
                mWlanDetail->setChan(netInfo.strChan);
                mWlanDetail->setSpeed(netInfo.strSpeed);
                mWlanDetail->setIPV4(netInfo.strIPV4Address);
                mWlanDetail->setIPV4Mask(netInfo.strIPV4Prefix);
                mWlanDetail->setIPV4Dns(netInfo.strIPV4Dns);
                mWlanDetail->setIPV4Gateway(netInfo.strIPV4GateWay);
                mWlanDetail->setIPV6(netInfo.strIPV6Address);
                mWlanDetail->setIPV6Prefix(netInfo.strIPV6Prefix);
                mWlanDetail->setIPV6Gt(netInfo.strIPV6GateWay);
                mWlanDetail->setMac(netInfo.strMac);
                mWlanDetail->setBandWidth(netInfo.strBandWidth);

                mWlanDetail->setVisible(mIsWlanVisible);
            }
        }
    }
}

void NetConnect::rebuildAvailComponent(QString iconPath, QString netName, QString type) {
    HoverBtn * wifiItem = new HoverBtn(netName, false, pluginWidget);
    wifiItem->mPitLabel->setText(netName);

    QIcon searchIcon = QIcon::fromTheme(iconPath);
    if (iconPath != KLanSymbolic && iconPath != NoNetSymbolic) {
        wifiItem->mPitIcon->setProperty("useIconHighlightEffect", 0x10);
    }
    wifiItem->mPitIcon->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(24, 24))));
    wifiItem->mAbtBtn->setMinimumWidth(100);
    wifiItem->mAbtBtn->setText(tr("Connect"));

    connect(wifiItem->mAbtBtn, &QPushButton::clicked, this, [=] {
        runKylinmApp(netName,type);
    });

    ui->availableLayout->addWidget(wifiItem);
}

void NetConnect::runExternalApp() {
    QString cmd = "nm-connection-editor";
    QProcess process(this);
    process.startDetached(cmd);
}

void NetConnect::runKylinmApp(QString netName, QString type) {
    m_interface->call("showPb",type,netName);
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

bool NetConnect::getWifiStatus() {

    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );
    // 获取当前wifi是否打开
    QDBusReply<QVariant> m_result = interface.call("Get", "org.freedesktop.NetworkManager", "WirelessEnabled");

    if (m_result.isValid()) {
        bool status = m_result.value().toBool();
        return status;
    } else {
        qDebug()<<"org.freedesktop.NetworkManager get invalid"<<endl;
        return false;
    }
}

int NetConnect::getWifiListDone(QVector<QStringList> getwifislist, QStringList getlanList, bool isWayland) {
    clearContent();
    mActiveInfo.clear();
    QString speed = getWifiSpeed();
    if (!speed.contains("/") && runCount < 1) {
        QElapsedTimer time;
        time.start();
        while (time.elapsed() < 1000) {
            QCoreApplication::processEvents();
        }
        runCount ++;
        return -1;
    } else {
        if (getActiveConInfo(mActiveInfo) == -1) {
            QElapsedTimer time;
            time.start();
            while (time.elapsed() < 500) {
                QCoreApplication::processEvents();
            }
            return -1;
        } else {
            runCount = 0;
            bool isNullSpeed = false;
            if (!speed.contains("/")) {
                speed = "null/" + speed;
            } else if (speed == "/") {
                isNullSpeed = true;
            }
            if (!getwifislist.isEmpty() && getwifislist.length() != 1) {
                connectedWifi.clear();
                wifiList.clear();

                QString actWifiName;

                int index = 0;
                while (index < mActiveInfo.size()) {
                    if (mActiveInfo[index].strConType == "wifi"
                            || mActiveInfo[index].strConType == "802-11-wireless") {
                        actWifiName = QString(mActiveInfo[index].strConName);
                        break;
                    }
                    index++;
                }
                QString wname;

                QString lockType;
                QString chan = geiWifiChan();
                QString freq;
                for (int i = 0; i < getwifislist.size(); ++i) {
                    if (getwifislist.at(i).at(0) == actWifiName) {
                        wname = getwifislist.at(i).at(0);
                        lockType = getwifislist.at(i).at(2);
                        freq = getwifislist.at(i).at(3) + " MHz";
                        if (isWayland) {
                            QString category = getwifislist.at(i).at(5);
                            wname = wname + category;
                        } else {
                            wname = wname;
                        }
                        //若切换为新wifi则关闭网络详情
                        if (mPreWifiConnectedName != wname) {
                            mPreWifiConnectedName = wname;
                            if (mWlanDetail->isVisible()) {
                                mWlanDetail->setVisible(false);
                                mIsWlanVisible = !mIsWlanVisible;
                            }
                        }
                        mActiveInfo[index].strSecType = (lockType == "--" ? tr("None") : lockType);
                        mActiveInfo[index].strChan = chan;
                        mActiveInfo[index].strHz = freq;
                        if (isNullSpeed) {
                            mActiveInfo[index].strSpeed = "null";
                        } else {
                            mActiveInfo[index].strSpeed = speed + " (Mbps)";
                        }
                        if (getwifislist.at(i).at(2) != NULL && getwifislist.at(i).at(2) != "--") {
                            wname += "lock";
                        }
                        connectedWifi.insert(wname, this->setSignal(getwifislist.at(i).at(1)));
                    }
                }
            }
            if (!getlanList.isEmpty()) {
                lanList.clear();
                connectedLan.clear();

                int indexLan = 0;
                while (indexLan < mActiveInfo.size()) {
                    if (mActiveInfo[indexLan].strConType == "ethernet"
                            || mActiveInfo[indexLan].strConType == "802-3-ethernet"){
                        actLanName = mActiveInfo[indexLan].strConName;
                        break;
                    }
                    indexLan ++;
                }
                if (mPreLanConnectedName != actLanName) {
                    mPreLanConnectedName = actLanName;
                    if (mLanDetail->isVisible()) {
                        mLanDetail->setVisible(false);
                        mIsLanVisible = !mIsLanVisible;
                    }
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
                    if (ltype != "wifi" && ltype != "" && ltype != "--") {
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
                QString iconamePah;
                if (isWayland) {
                    int category = connectedWifiName.right(1).toInt();
                    connectedWifiName = connectedWifiName.left(connectedWifiName.size() - 1);
                    iconamePah = wifiIcon(isLock, strength, category);
                } else {
                    iconamePah = wifiIcon(isLock, strength);
                }
                rebuildNetStatusComponent(iconamePah, connectedWifiName);
            }
            if (!this->actLanName.isEmpty()) {
                QString lanIconamePah = KLanSymbolic;
                rebuildNetStatusComponent(lanIconamePah, this->actLanName);
            }

            if (this->connectedWifi.isEmpty() && this->actLanName.isEmpty()) {
                rebuildNetStatusComponent(NoNetSymbolic , tr("No net"));
            }
            return 1;
        }
    }
}

QString NetConnect::getWifiSpeed() {
    QProcess *lanPro = new QProcess(this);
    QString rxSpeed;
    QString txSpeed;
    QString output;
    QStringList slist;

    lanPro->start("iw dev wlan0 link");
    lanPro->waitForFinished();
    output = lanPro->readAll();
    foreach (QString line, output.split("\n")) {
        line.replace(QRegExp("[\\s]+"), "");
        slist.append(line);
    }

    for (int i = 0; i < slist.length(); i++) {
        QString str = slist.at(i);
        if (str.contains("rxbitrate:")) {
            str.remove("rxbitrate:");
            rxSpeed = str;
        } else if (str.contains("txbitrate:")) {
            str.remove("txbitrate:");
            txSpeed = str;
        }
    }
    QString uSpeed;
    QString dSpeed;
    for (int i = 0; i < rxSpeed.length(); i++) {
        if (rxSpeed.at(i) == ".") {
            break;
        } else if (rxSpeed.at(i).toLatin1() >= '0' && rxSpeed.at(i).toLatin1() <= '9'){
            uSpeed.append(rxSpeed.at(i));
        }
    }
    for (int i = 0; i < txSpeed.length(); i++) {
        if (txSpeed.at(i) == ".") {
            break;
        } else if (txSpeed.at(i).toLatin1() >= '0' && txSpeed.at(i).toLatin1() <= '9'){
            dSpeed.append(txSpeed.at(i));
        }
    }
    if (uSpeed == "" && dSpeed == "") {
        return "/";
    } else if (uSpeed == "" && dSpeed != "") {
        return dSpeed;
    }
    return uSpeed + "/" + dSpeed;
}

QString NetConnect::geiWifiChan() {
    QProcess *lanPro = new QProcess(this);
    bool isHas = false;
    QStringList slist;
    lanPro->start("nmcli -f in-use,chan device wifi");
    lanPro->waitForFinished();
    QString output = lanPro->readAll();
    foreach (QString line, output.split("\n")) {
        line.replace(QRegExp("[\\s]+"), "");
        slist.append(line);
    }
    for (int i = 0; i < slist.length(); i++) {
        QString str = slist.at(i);
        if (str.contains("*")) {
            isHas = true;
        }
    }
    if (isHas) {
        for (int i = 0; i < slist.length(); i++) {
            QString str = slist.at(i);
            if (str.contains("*")) {
                str.remove("*");
                prefreChan = str;
                return str;
            }
        }
    } else {
        return prefreChan;
    }
}
bool NetConnect::getInitStatus() {

    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );
    //　获取当前wifi是否打开
    QDBusReply<QVariant> m_result = interface.call("Get", "org.freedesktop.NetworkManager", "WirelessEnabled");

    if (m_result.isValid()) {
        bool status = m_result.value().toBool();
        return status;
    } else {
        qDebug()<<"org.freedesktop.NetworkManager get invalid"<<endl;
        return false;
    }
}

void NetConnect::clearContent() {
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

void NetConnect::setWifiBtnDisable() {
    ui->RefreshBtn->setText(tr("Refreshing..."));
    ui->RefreshBtn->setEnabled(false);
    wifiBtn->setEnabled(false);
    ui->openWifiFrame->setVisible(false);
    this->clearContent();
}

void NetConnect::setNetDetailVisible() {
    bool wifiSt = getwifiisEnable();
    wifiBtn->setEnabled(wifiSt);
    ui->openWifiFrame->setVisible(wifiSt);
    ui->RefreshBtn->setEnabled(true);
    ui->RefreshBtn->setText(tr("Refresh"));

    if (!mActiveInfo.count()) {
        this->mWlanDetail->setVisible(false);
        this->mLanDetail->setVisible(false);
    } else if (1 == mActiveInfo.count()){
        if (mActiveInfo.at(0).strConType.contains("802-11-wireless", Qt::CaseSensitive)) {
            this->mLanDetail->setVisible(false);
        } else {
            this->mWlanDetail->setVisible(false);
        }
    }
}

QList<QVariantMap> NetConnect::getDbusMap(const QDBusMessage &dbusMessage) {
    QList<QVariant> outArgsIpv4 = dbusMessage.arguments();
    if (!outArgsIpv4.isEmpty()) {
        QVariant firstIpv4 = outArgsIpv4.at(0);
        QDBusVariant dbvFirstIpv4 = firstIpv4.value<QDBusVariant>();
        QVariant vFirstIpv4 = dbvFirstIpv4.variant();

        const QDBusArgument &dbusArgIpv4 = vFirstIpv4.value<QDBusArgument>();
        QList<QVariantMap> mDatasIpv4;
        dbusArgIpv4 >> mDatasIpv4;
        return mDatasIpv4;
    } else {
        QList<QVariantMap> emptyList;
        return emptyList;
    }
}

QString NetConnect::wifiIcon(bool isLock, int strength, int category) {
    switch (category) {
    case 0:
        switch (strength) {
        case 1:
            return isLock ? KWifiLockSymbolic : KWifiSymbolic;
        case 2:
            return isLock ? KWifiLockGood : KWifiGood;
        case 3:
            return isLock ? KWifiLockOK : KWifiOK;
        case 4:
            return isLock ? KWifiLockLow : KWifiLow;
        case 5:
            return isLock ? KWifiLockNone : KWifiNone;
        default:
            return "";
        }
    case 1:
        switch (strength) {
        case 1:
            return isLock ? KWifi6LockSymbolic : KWifi6Symbolic;
        case 2:
            return isLock ? KWifi6LockGood : KWifi6Good;
        case 3:
            return isLock ? KWifi6LockOK : KWifiOK;
        case 4:
            return isLock ? KWifi6LockLow : KWifi6Low;
        case 5:
            return isLock ? KWifiLockNone : KWifiNone;
        default:
            return "";
        }
    case 2:
        switch (strength) {
        case 1:
            return isLock ? KWifi6ProLockSymbolic : KWifi6ProSymbolic;
        case 2:
            return isLock ? KWifi6ProLockGood : KWifi6ProGood;
        case 3:
            return isLock ? KWifi6ProLockOK : KWifi6ProOK;
        case 4:
            return isLock ? KWifi6ProLockLow : KWifi6ProLow;
        case 5:
            return isLock ? KWifiLockNone : KWifiNone;
        default:
            return "";
        }
    default:
        return "";
    }
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
    if (signal > 75) {
        signalLv = 1;
    } else if (signal > 55 && signal <= 75) {
        signalLv = 2;
    } else if (signal > 35 && signal <= 55) {
        signalLv = 3;
    } else if (signal > 15 && signal  <= 35) {
        signalLv = 4;
    } else if (signal <= 15) {
        signalLv = 5;
    }
    return signalLv;
}

void NetConnect::wifiSwitchSlot(bool status) {

    QString wifiStatus = status ? "on" : "off";
    QString program = "nmcli";
    QStringList arg;
    arg << "radio" << "wifi" << wifiStatus;
    QProcess *nmcliCmd = new QProcess(this);
    nmcliCmd->start(program, arg);
    nmcliCmd->waitForStarted();
}

int NetConnect::getActiveConInfo(QList<ActiveConInfo>& qlActiveConInfo) {
    ActiveConInfo activeNet;
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
        QDBusInterface interfacePro("org.freedesktop.NetworkManager",
                                    objPath.path(),
                                    "org.freedesktop.NetworkManager.Connection.Active",
                                    QDBusConnection::systemBus());
        QVariant replyType = interfacePro.property("Type");
        QVariant replyUuid = interfacePro.property("Uuid");
        QVariant replyId   = interfacePro.property("Id");

        activeNet.strConName = replyId.toString();
        activeNet.strConType = replyType.toString();
        activeNet.strConUUID = replyUuid.toString();

        QString replyIPV4Path = interfacePro.property("Ip4Config")
                .value<QDBusObjectPath>()
                .path();
         //如果此时获取的path为"/",说明出现异常，则需要进行异常处理
        // IPV4信息
        if (replyIPV4Path == "/") {
            return -1;
        } else {
            QDBusInterface IPV4ifc("org.freedesktop.NetworkManager",
                                   replyIPV4Path,
                                   "org.freedesktop.DBus.Properties",
                                   QDBusConnection::systemBus());

            QDBusMessage replyIpv4 = IPV4ifc.call("Get", "org.freedesktop.NetworkManager.IP4Config", "AddressData");
            if (!IPV4ifc.isValid()) {
                qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
            }
            QList<QVariantMap> datasIpv4 = getDbusMap(replyIpv4);
            if (!datasIpv4.isEmpty()) {
                activeNet.strIPV4Address = datasIpv4.at(0).value("address").toString();
                activeNet.strIPV4Prefix = datasIpv4.at(0).value("prefix").toString();
            } else {
                qWarning()<<"Ipv4 data reply empty!";
                return -1;
            }

            QDBusMessage replyIPV4Dns = IPV4ifc.call("Get", "org.freedesktop.NetworkManager.IP4Config", "NameserverData");
            QList<QVariantMap> datasIpv4Dns = getDbusMap(replyIPV4Dns);
            if (!datasIpv4Dns.isEmpty()) {
                activeNet.strIPV4Dns = datasIpv4Dns.at(0).value("address").toString();
            } else {
                qWarning()<<"Ipv4 Dns data reply empty!";
            }

            QDBusMessage replyIPV4Gt = IPV4ifc.call("Get", "org.freedesktop.NetworkManager.IP4Config", "Gateway");
            if (!replyIPV4Gt.arguments().isEmpty()) {
                QVariant ipv4Gt  = replyIPV4Gt.arguments().at(0)
                        .value<QDBusVariant>()
                        .variant();
                activeNet.strIPV4GateWay = ipv4Gt.toString();
            } else {
                qWarning()<<"Ipv4 reply empty!";
                return -1;
            }

            // IPV6信息
            QString replyIpv6Path = interfacePro.property("Ip6Config")
                    .value<QDBusObjectPath>()
                    .path();

            QDBusInterface IPV6ifc("org.freedesktop.NetworkManager",
                                   replyIpv6Path,
                                   "org.freedesktop.DBus.Properties",
                                   QDBusConnection::systemBus());
            if (!IPV6ifc.isValid()) {
                qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
            }
            QDBusMessage replyIPV6 = IPV6ifc.call("Get", "org.freedesktop.NetworkManager.IP6Config", "AddressData");
            QList<QVariantMap> dataIPV6 = getDbusMap(replyIPV6);
            if (!dataIPV6.isEmpty()) {
                activeNet.strIPV6Address = dataIPV6.at(0).value("address").toString();
                activeNet.strIPV6Prefix = dataIPV6.at(0).value("prefix").toString();
            } else {
                qWarning()<<"Ipv6 data reply empty!";
                return -1;
            }

            QDBusMessage replyIPV6Gt = IPV6ifc.call("Get", "org.freedesktop.NetworkManager.IP6Config", "GateWay");
            if (!replyIPV6Gt.arguments().isEmpty()) {
                QVariant IPV6Gt  = replyIPV6Gt.arguments().at(0)
                        .value<QDBusVariant>()
                        .variant();
                activeNet.strIPV6GateWay = IPV6Gt.toString().isEmpty() ? "" : IPV6Gt.toString();
            } else {
                qWarning()<<"Ipv6 info reply empty!";
                return -1;
            }
            // 设备信息
            auto replyDevicesPaths = interfacePro.property("Devices")
                    .value<QList<QDBusObjectPath>>();
            if (!replyDevicesPaths.isEmpty()) {
                if (!activeNet.strConType.compare("802-3-ethernet", Qt::CaseInsensitive)) {
                    QDBusInterface netDeviceifc("org.freedesktop.NetworkManager",
                                                replyDevicesPaths.at(0).path(),
                                                "org.freedesktop.NetworkManager.Device.Wired",
                                                QDBusConnection::systemBus());
                    activeNet.strBandWidth = netDeviceifc.property("Speed").toString() + "Mb/s";
                    activeNet.strMac = netDeviceifc.property("HwAddress").toString().toLower();
                } else {
                    QDBusInterface netDeviceifc("org.freedesktop.NetworkManager",
                                                replyDevicesPaths.at(0).path(),
                                                "org.freedesktop.NetworkManager.Device.Wireless",
                                                QDBusConnection::systemBus());
                    activeNet.strBandWidth = netDeviceifc.property("Bitrate").toString() + "Mb/s";
                    activeNet.strMac = netDeviceifc.property("HwAddress").toString().toLower();
                }
            } else {
                qWarning()<<"Reply for Devices Paths empty!";
                return -1;
            }
            qlActiveConInfo.append(activeNet);
        }

    }
    dbusArgs.endArray();
    return 1;
}
