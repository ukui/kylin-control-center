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

#include "kylin_network_interface.h"


#include <QGSettings/QGSettings>
#include <QProcess>
#include <QTimer>


#define CONTROL_CENTER_WIFI "org.ukui.control-center.wifi.switch"
NetConnect::NetConnect():m_wifiList(new Wifi)
{
    ui = new Ui::NetConnect;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Netconnect");
    pluginType = NETWORK;

    ui->detailBtn->setText(tr("Network settings"));

    wifiBtn = new SwitchButton();

    pluginWidget->setStyleSheet("background: #ffffff;");


    ui->statusListWidget->setStyleSheet("QListWidget#statusListWidget{border: none;}");
    ui->availableListWidget->setStyleSheet("QListWidget#availableListWidget{border: none;}");

    ui->statusListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->statusListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->availableListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->availableListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->statusListWidget->setSpacing(0);
    ui->availableListWidget->setSpacing(0);
    ui->statusListWidget->setFocusPolicy(Qt::NoFocus);
    ui->availableListWidget->setFocusPolicy(Qt::NoFocus);
    ui->statusListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->availableListWidget->setSelectionMode(QAbstractItemView::NoSelection);

//    ui->detailBtn->setStyleSheet("QPushButton{border: none;}");

    ui->openWifiWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
    ui->openWIifLayout->addWidget(wifiBtn);

    ui->openWifiWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
    ui->openWIifLayout->addWidget(wifiBtn);

    //构建网络配置对象
    nmg  = new QNetworkConfigurationManager();
    initComponent();

    getNetList();
}

NetConnect::~NetConnect()
{
    delete ui;
    delete nmg;
//    delete wifiBtn;
    delete m_gsettings;
}

QString NetConnect::get_plugin_name(){
    return pluginName;
}

int NetConnect::get_plugin_type(){
    return pluginType;
}

QWidget *NetConnect::get_plugin_ui(){
    return pluginWidget;
}

void NetConnect::plugin_delay_control(){

}

void NetConnect::initComponent(){
    const QByteArray id(CONTROL_CENTER_WIFI);
    if(QGSettings::isSchemaInstalled(id)) {
//        qDebug()<<"isSchemaInstalled"<<endl;
        m_gsettings = new QGSettings(id);

//        监听key的value是否发生了变化
        connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
//            qDebug()<<"status changed ------------>"<<endl;
            if (key == "switchor") {
                bool judge = getSwitchStatus(key);
                wifiBtn->setChecked(judge);
            }
        });
    }

    //构建网络状态组件
//    rebuildNetStatusComponent();

    //网络配置变化回调
//    connect(nmg, &QNetworkConfigurationManager::configurationChanged, this, [=](const QNetworkConfiguration &config){
//        Q_UNUSED(config)
//        rebuildNetStatusComponent();

//    });

    //详细设置按钮connect
    connect(ui->detailBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        runExternalApp();
    });


    wifiBtn->setChecked(getSwitchStatus("switch"));
    connect(wifiBtn,SIGNAL(checkedChanged(bool)), this, SLOT(wifiSwitchSlot(bool)));
}

void NetConnect::rebuildNetStatusComponent(QString iconPath, QString netName){
    ////构建Widget
    QWidget * baseWidget = new QWidget();
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setContentsMargins(0, 0, 0, 2);

    QWidget * devWidget = new QWidget(baseWidget);
    devWidget->setMinimumWidth(550);
    devWidget->setMaximumWidth(960);
    devWidget->setMinimumHeight(50);
    devWidget->setMaximumHeight(50);

//        devWidget->setFixedHeight(50);
    devWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
    QHBoxLayout * devHorLayout = new QHBoxLayout(devWidget);
    devHorLayout->setSpacing(8);
    devHorLayout->setContentsMargins(16, 0, 0, 0);

    QLabel * iconLabel = new QLabel(devWidget);
    QSizePolicy iconSizePolicy = iconLabel->sizePolicy();
    iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    iconLabel->setSizePolicy(iconSizePolicy);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(iconPath));

    QLabel * nameLabel = new QLabel(devWidget);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setScaledContents(true);
    if ("No Net" != netName) {
        nameLabel->setText(netName);
    }

    QLabel * statusLabel = new QLabel(devWidget);
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

    devWidget->setLayout(devHorLayout);

    baseVerLayout->addWidget(devWidget);
    baseVerLayout->addStretch();

    baseWidget->setLayout(baseVerLayout);

    QListWidgetItem * item = new QListWidgetItem(ui->statusListWidget);
    item->setSizeHint(QSize(502, 52));

    ui->statusListWidget->setItemWidget(item, baseWidget);
}

void NetConnect::_acquireCardInfo(){
    QList<QNetworkInterface> network = QNetworkInterface::allInterfaces();
    for (QList<QNetworkInterface>::const_iterator it = network.constBegin(); it != network.constEnd(); it++){
        CardInfo ci;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        if ((*it).type() == QNetworkInterface::Loopback)
            continue;

        if ((*it).type() == QNetworkInterface::Ethernet)
            ci.type = ETHERNET;
        else if ((*it).type() == QNetworkInterface::Wifi)
            ci.type = WIFI;
#else
        if ((*it).flags().testFlag(QNetworkInterface::IsLoopBack))
            continue;
       ci.type =ETHERNET;
#endif
        ci.name = (*it).humanReadableName();

        QList<QNetworkAddressEntry> addressList = (*it).addressEntries();
        if (addressList.count() == 0)
            ci.status = false;
        else
            ci.status = true;

        cardinfoQList.append(ci);
    }
}


void NetConnect::getNetList() {
    ui->availableListWidget->clear();
    ui->statusListWidget->clear();

    this->TlanList =  execGetLanList();
    pThread = new QThread;
    pNetWorker = new NetconnectWork;

    connect(pNetWorker, &NetconnectWork::wifiGerneral,this,[&](QStringList list){

        this->TwifiList = list;
        getWifiListDone(this->TwifiList, this->TlanList);
        QMap<QString, int>::iterator iter = this->wifiList.begin();
        QString iconamePah;
        while(iter != this->wifiList.end()) {
            if (!wifiBtn->isChecked()){
                break;
            }
            iconamePah= ":/img/plugins/netconnect/wifi" + QString::number(iter.value())+".png";
            rebuildAvailComponent(iconamePah , iter.key());
            iter++;
        }

        for(int i = 0; i < this->lanList.length(); i++) {        ;
            iconamePah= ":/img/plugins/netconnect/eth.png";
            rebuildAvailComponent(iconamePah , lanList.at(i));
        }

    });
    connect(pNetWorker, &NetconnectWork::workerComplete, [=]{
       pThread->quit();
       pThread->wait();
    });
    pNetWorker->moveToThread(pThread);
    connect(pThread, &QThread::started, pNetWorker, &NetconnectWork::run);
    connect(pThread, &QThread::finished, this, [=]{

    });
    pThread->start();

//    QMap<QString, int>::iterator iter = this->wifiList.begin();
//    while(iter != this->wifiList.end()) {
//        iconamePah= ":/img/plugins/netconnect/wifi" + QString::number(iter.value())+".png";
//        rebuildAvailComponent(iconamePah , iter.key());
//        iter++;
//    }

//    for(int i = 0; i < lanList.length(); i++) {        ;
//        iconamePah= ":/img/plugins/netconnect/eth.png";
//        rebuildAvailComponent(iconamePah , lanList.at(i));
//    }
}

void NetConnect::rebuildAvailComponent(QString iconPath, QString netName){

    ////构建Widget
    QWidget * baseWidget = new QWidget();
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setContentsMargins(0, 0, 0, 2);

    QWidget * devWidget = new QWidget(baseWidget);
    devWidget->setMinimumWidth(550);
    devWidget->setMaximumWidth(960);
    devWidget->setMinimumHeight(50);
    devWidget->setMaximumHeight(50);

//        devWidget->setFixedHeight(50);
    devWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
    QHBoxLayout * devHorLayout = new QHBoxLayout(devWidget);
    devHorLayout->setSpacing(8);
    devHorLayout->setContentsMargins(16, 0, 0, 0);

    QLabel * iconLabel = new QLabel(devWidget);
    QSizePolicy iconSizePolicy = iconLabel->sizePolicy();
    iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    iconLabel->setSizePolicy(iconSizePolicy);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(iconPath));

    QLabel * nameLabel = new QLabel(devWidget);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setScaledContents(true);
    nameLabel->setText(netName);

//    QLabel * statusLabel = new QLabel(devWidget);
//    QSizePolicy statusSizePolicy = statusLabel->sizePolicy();
//    statusSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
//    statusSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
//    statusLabel->setSizePolicy(statusSizePolicy);
//    statusLabel->setScaledContents(true);
//    statusLabel->setText(netName);

    devHorLayout->addWidget(iconLabel);
    devHorLayout->addWidget(nameLabel);
//    devHorLayout->addWidget(statusLabel);
    devHorLayout->addStretch();

    devWidget->setLayout(devHorLayout);

    baseVerLayout->addWidget(devWidget);
    baseVerLayout->addStretch();

    baseWidget->setLayout(baseVerLayout);

    QListWidgetItem * item = new QListWidgetItem(ui->availableListWidget);
    item->setSizeHint(QSize(502, 52));

    ui->availableListWidget->setItemWidget(item, baseWidget);
}


void NetConnect::runExternalApp(){
    QString cmd = "nm-connection-editor";
    QProcess process(this);
    process.startDetached(cmd);
}


QStringList NetConnect::execGetLanList(){
    QProcess *lanPro = new QProcess();
    QString shellOutput = "";
    lanPro->start("nmcli -f type,device,name connection show");
    lanPro->waitForFinished();
    QString output = lanPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

//    qDebug()<<"lanslist--------------->"<<slist<<endl;
    return slist;

}

QStringList NetConnect::execGetWifiList(){
    QProcess *wifiPro = new QProcess();
    QString shellOutput = "";
    wifiPro->start("nmcli -f signal,ssid device wifi");
    wifiPro->waitForFinished();
    QString output = wifiPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

//    emit getWifiListFinished(slist);
//    qDebug()<<"wifilist--------------->"<<slist<<endl;
    return slist;
}

void NetConnect::getWifiListDone(QStringList getwifislist, QStringList getlanList) {

    //if is wifi list
    if(!getwifislist.isEmpty()){
        connectedWifi.clear();
        wifiList.clear();

        QString actWifiName = "--";
        activecon *act = kylin_network_get_activecon_info();

        int index = 0;
        while(act[index].con_name != NULL){
            if(QString(act[index].type) == "wifi"){
                actWifiName = QString(act[index].con_name);
                break;
            }
            index ++;
        }
//        qDebug()<<"now wifi is----->"<<actWifiName<<endl;

        // 填充可用网络列表
        QString headLine = getwifislist.at(0);
        headLine = headLine.trimmed();
        int indexRate = headLine.indexOf("SIGNAL");
        int indexName = headLine.indexOf("SSID");

        QStringList wnames;
        int count = 0;
        for(int i = 1; i < getwifislist.size(); i ++) {
            QString line = getwifislist.at(i);
            QString wsignal = line.mid(0, indexName).trimmed();
            QString wname = line.mid(indexName).trimmed();

            bool isContinue = false;
            foreach (QString addName, wnames) {
                // 重复的网络名称，跳过不处理
                if(addName == wname){ isContinue = true; }
            }
            if(isContinue){ continue; }

            if(wname != "" && wname != "--"){
                int strength = this->setSignal(wsignal);
                wifiList.insert(wname, strength);

                if(wname == actWifiName) {
                    connectedWifi.insert(wname, strength);
                }
                wnames.append(wname);
            }
        }
    }

    if(!getlanList.isEmpty()){
        lanList.clear();
        connectedLan.clear();

        // 获取当前连接的lan name
        activecon *actLan = kylin_network_get_activecon_info();
        int indexLan = 0;
        while(actLan[indexLan].con_name != NULL){
            if(QString(actLan[indexLan].type) == "ethernet"){
                actLanName = QString(actLan[indexLan].con_name);
                break;
            }
            indexLan ++;
        }
//        qDebug()<<"actLanName is-------->"<<this->actLanName<<endl;

        // 填充可用网络列表
        QString lanheadLine = getlanList.at(0);
        lanheadLine = lanheadLine.trimmed();
        int lanindexDevice = lanheadLine.indexOf("DEVICE");
        int lanindexName = lanheadLine.indexOf("NAME");

    //    qDebug()<<"getlanList-------------->"<<getlanList<<endl;
        for(int i =1 ;i < getlanList.length(); i++)
        {
            QString line = getlanList.at(i);
            QString ltype = line.mid(0, lanindexDevice).trimmed();
            QString nname = line.mid(lanindexName).trimmed();
            if(ltype  != "wifi" && ltype != "" && ltype != "--"){
                this->lanList << nname;
            }
        }
//        qDebug()<<"lanList is-------------->"<<lanList<<endl;
    }

    if (!this->connectedWifi.isEmpty()){
        QMap<QString, int>::iterator iter = this->connectedWifi.begin();
        QString iconamePah = ":/img/plugins/netconnect/wifi" + QString::number(iter.value())+".png";
//        qDebug()<<"name is=------------>"<<iter.key();
        rebuildNetStatusComponent(iconamePah , iter.key());
    }
    if (!this->actLanName.isEmpty()){
        QString lanIconamePah= ":/img/plugins/netconnect/eth.png";
        rebuildNetStatusComponent(lanIconamePah, this->actLanName);
//        qDebug()<<"name is=------------>"<<this->actLanName;
    }

    if (this->connectedWifi.isEmpty() && this->actLanName.isEmpty())  {
        rebuildNetStatusComponent(":/img/plugins/netconnect/nonet.png" , "No Net");
    }
}

bool NetConnect::getSwitchStatus(QString key){
//    qDebug()<<"key is------------->"<<key<<endl;
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

//get wifi's strength
int NetConnect::setSignal(QString lv) {
    int signal = lv.toInt();
//    qDebug()<<"signal is---------->"<<lv<<endl;
    int signalLv;

    if(signal > 75){
        signalLv = 1;
    }
    if(signal > 55 && signal <= 75){
        signalLv = 2;
    }
    if(signal > 35 && signal <= 55){
        signalLv = 3;
    }
    if(signal > 15 && signal <= 35){
        signalLv = 4;
    }
    if(signal <= 15){
        signalLv = 4;
    }

    return signalLv;
}

void NetConnect::wifiSwitchSlot(bool signal){
    getNetList();
//    qDebug()<<"wifiSwitchSlot--------------->"<<endl;
    if(!m_gsettings) {
        return ;
    }
    const QStringList list = m_gsettings->keys();
    if (!list.contains("switch")) {
        return ;
    }
    m_gsettings->set("switch",signal);
}



