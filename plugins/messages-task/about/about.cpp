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
#include <QWidget>

#include "about.h"
#include "ui_about.h"

#include <QProcess>
#include <QFont>
#include <QFile>

#include <QDebug>

const QString TYPEVERSION = "Kylin V10";

About::About()
{
    ui = new Ui::About;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("About");
    pluginType = NOTICEANDTASKS;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
//    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->systemWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->deviceWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");


//    qDebug() << "--------start------->";
//    qDebug() << QSysInfo::kernelType();
//    qDebug() << QSysInfo::kernelVersion();
//    qDebug() << "---------1---------->";
//    qDebug() << QSysInfo::machineHostName();
//    qDebug() << QSysInfo::currentCpuArchitecture();
//    qDebug() << "----------2----------->";
//    qDebug() << QSysInfo::prettyProductName();
//    qDebug() << QSysInfo::productType();
//    qDebug() << QSysInfo::productVersion();
//    qDebug() << "-------end---------->";
//    _call_dbus_get_computer_info();
    _data_init();

//    setupComponent();
    setupDesktopComponent();
    setupKernelCompenent();
    setupVersionCompenent();
    setupSerialComponent();
}

About::~About()
{
    delete ui;
//    delete interface;
}

QString About::get_plugin_name(){
    return pluginName;
}

int About::get_plugin_type(){
    return pluginType;
}

QWidget *About::get_plugin_ui(){
    return pluginWidget;
}

void About::plugin_delay_control(){

}

void About::_data_init(){
    QStringList infoList = computerinfo.split("\n\n");
    QString available;
    if (infoList.length() > 1){
        available = infoList.at(1);
    }
    else {
        available = "";
    }
    if (available != ""){
        for (QString line : available.split("\n")){
            if (!line.contains(":"))
                continue;
            QStringList lineList = line.split(":");
            infoMap.insert(lineList.at(0).simplified(), lineList.at(1).simplified());
        }
    }
}

void About::setupDesktopComponent() {
    //获取当前桌面环境
    QString dEnv;
    foreach (dEnv, QProcess::systemEnvironment()){
        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
            break;
    }

    //设置当前桌面环境信息
    if (!dEnv.isEmpty()) {
        QString desktop = dEnv.section("=", -1, -1);
        ui->desktopContent->setText(desktop);
    }

//    QProcess *userPro = new QProcess();
//    userPro->start("whoami");
//    userPro->waitForFinished();

//    std::string output = userPro->readAll().toStdString();

    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }


    ui->userContent->setText(name);
}

void About::setupKernelCompenent() {
    QString kernal = QSysInfo::kernelType() + " " + QSysInfo::kernelVersion();
    QString diskSize;
    QString memorySize;
    QString cpuType;

    //ubuntukylin youker DBus interface
    QDBusInterface *youkerInterface = new QDBusInterface("com.kylin.assistant.systemdaemon",
                                     "/com/kylin/assistant/systemdaemon",
                                     "com.kylin.assistant.systemdaemon",
                                     QDBusConnection::systemBus());
    if (!youkerInterface->isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
//        return;
    }

    QDBusInterface *sessionInterface = new QDBusInterface("com.kylin.assistant.sessiondaemon",
                                     "/com/kylin/assistant/sessiondaemon",
                                     "com.kylin.assistant.sessiondaemon",
                                     QDBusConnection::sessionBus());
    if (!sessionInterface->isValid()) {
        qCritical() << "Create sessionInterface  Failed When Get Computer info: " << QDBusConnection::sessionBus().lastError();
//        return;
    }

    QDBusReply<QMap<QString, QVariant>> diskinfo;
    diskinfo  = youkerInterface ->call("get_harddisk_info");
    if (!diskinfo.isValid()) {
        qDebug()<<"diskinfo is invalid"<<endl;
    } else {
        QMap<QString, QVariant> res = diskinfo.value();
        diskSize = res["DiskCapacity"].toString();
    }


    QDBusReply<QMap<QString, QVariant>> cpuinfo;
    cpuinfo  = youkerInterface ->call("get_cpu_info");
    if (!diskinfo.isValid()) {
        qDebug()<<"cpuinfo is invalid"<<endl;
    } else {
        QMap<QString, QVariant> res = cpuinfo.value();
        cpuType = res["CpuVersion"].toString();
    }


    QDBusReply<QMap<QString, QVariant>> memoryinfo;
    memoryinfo  = sessionInterface ->call("get_system_message");
    if (!diskinfo.isValid()) {
        qDebug()<<"memoryinfo is invalid"<<endl;
    } else {
        QMap<QString, QVariant> res = memoryinfo.value();
        memorySize = res["ram"].toString();
    }

    ui->cpuContent->setText(cpuType);
    ui->diskContent->setText(diskSize);
    ui->kernalContent->setText(kernal);
    ui->memoryContent->setText(memorySize);

    qDebug()<<"cpuType and "<<cpuType<<" "<<diskSize<<" "<<kernal<<" "<<memorySize<<endl;
}

void About::setupVersionCompenent() {
    QString versionPath = "/etc/os-release";
    QStringList osRes =  readFile(versionPath);
    QString version;

    for (QString str : osRes) {
        if (str.contains("PRETTY_NAME=")) {
            int index = str.indexOf("PRETTY_NAME=");
            int startIndex = index + 13;
            int length = str.length() - startIndex - 1;
            version = str.mid(startIndex, length);
        }
    }
    ui->versionContent->setText(version);
    if (version == "Kylin V10" || version == "Kylin V10.1") {
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/galaxyUnicorn.png"));
    } else {
//        qDebug()<<"version----->"<<version<<endl;
        ui->activeFrame->setVisible(false);
        ui->trialButton->setVisible(false);
        //设置桌面环境LOGO
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logoukui.png"));
    }
}

void About::setupSerialComponent() {
    ui->trialButton->setFlat(true);
    ui->trialButton->setStyleSheet("text-align: left");
    //ubuntukylin youker DBus interface
    QDBusInterface *activeInterface = new QDBusInterface("org.freedesktop.activation",
                                     "/org/freedesktop/activation",
                                     "org.freedesktop.activation.interface",
                                     QDBusConnection::systemBus());
    if (!activeInterface->isValid()) {
        qDebug() << "Create active Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return;
    }

    int status;
    QDBusReply<int> activeStatus;
    activeStatus  = activeInterface ->call("status");
    if (!activeStatus.isValid()) {
        qDebug()<<"activeStatus is invalid"<<endl;
    } else {
        status = activeStatus.value();
    }


    QString serial;
    QDBusReply<QString> serialReply;
    serialReply  = activeInterface ->call("serial_number");
    if (!serialReply.isValid()) {
        qDebug()<<"serialReply is invalid"<<endl;
    } else {
        serial = serialReply.value();
    }

    qDebug()<<"status and serial is----->"<<status<<" "<<serial<<endl;

    if (status != 1) {
        ui->activeContent->setText(tr("Inactivated"));
    } else {
        ui->activeContent->setText(tr("Activated"));
        ui->activeButton->hide();
    }

    ui->serviceContent->setText(serial);

    connect(ui->activeButton, &QPushButton::clicked, this, &About::runActiveWindow);
    connect(ui->trialButton, &QPushButton::clicked, this, &About::showPdf);
}

void About::setupComponent(){


//    //获取当前桌面环境
//    QString dEnv;
//    foreach (dEnv, QProcess::systemEnvironment()){
//        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
//            break;
//    }
//    //设置当前桌面环境信息
//    QLabel * dEnvLabel = new QLabel(tr("UNKNOWN"));
//    if (!dEnv.isEmpty())
//        dEnvLabel->setText(dEnv.section("=", -1, -1));
//    ui->systemFormLayout->addRow(tr("Current desktop env:"), dEnvLabel);
//    qDebug()<<"dEnvLabel is------->"<<(*dEnvLabel).text()<<endl;

//    /**
//     * 设置当前操作系统信息
//     */
//    //设置操作系统版本
////    qDebug() << QSysInfo::prettyProductName() << QSysInfo::productType() << QSysInfo::productVersion();
//    QLabel * osVersionLabel = new QLabel(tr("UNKNOWN"));
//    osVersionLabel->setText(QSysInfo::productVersion());
//    ui->systemFormLayout->addRow(tr("OS Version:"), osVersionLabel);
//    //设置CPU平台
//    QLabel * cpuArch = new QLabel(tr("UNKNOWN"));
//    cpuArch->setText(QSysInfo::currentCpuArchitecture());
//    ui->systemFormLayout->addRow(tr("CPU Arch:"), cpuArch);
//    //设置内核版本号
////    qDebug() << QSysInfo::kernelType() << QSysInfo::kernelVersion();
//    QLabel * kernelLabel = new QLabel(tr("UNKNOWN"));
//    kernelLabel->setText(QSysInfo::kernelType() + " " + QSysInfo::kernelVersion());
//    ui->systemFormLayout->addRow(tr("Kernel Version"), kernelLabel);


//    /**
//     * 设置硬件详情的信息项目
//     */
//    QLabel * manufacturers = new QLabel(tr("UNKNOWN"));
//    if (infoMap.contains(MANUFACTURER))
//        manufacturers->setText(QString(infoMap.find(MANUFACTURER).value()));

//    QLabel * productname = new QLabel("UNKNOWN");
//    if (infoMap.contains(PRODUCTNAME))
//        productname->setText(QString(infoMap.find(PRODUCTNAME).value()));

//    QLabel * version = new QLabel("UNKNOWN");
//    if (infoMap.contains(VERSION))
//        version->setText(QString(infoMap.find(VERSION).value()));

//    QLabel * serialnumber = new QLabel("UNKNOWN");
//    if (infoMap.contains(SERIALNUMBER))
//        serialnumber->setText(QString(infoMap.find(SERIALNUMBER).value()));

////    ui->devicesFormLayout->setHorizontalSpacing(70);
////    ui->devicesFormLayout->addRow(tr("Manufacturers:"), manufacturers);
////    ui->devicesFormLayout->addRow(tr("Product Name:"), productname);
////    ui->devicesFormLayout->addRow(tr("Version:"), version);
////    ui->devicesFormLayout->addRow(tr("Serial Number:"), serialnumber);
////    ui->devicesFormLayout->addRow(tr("hostname:"), "hostname");
////    ui->devicesFormLayout->addRow(tr("running time:"), "running time");
////    ui->devicesFormLayout->addRow(tr("os type:"), "os type");
////    ui->devicesFormLayout->addRow(tr("os version"), "os version");
////    ui->devicesFormLayout->addRow(tr("system bit"), "system bit");
////    ui->devicesFormLayout->addRow(tr("kernel version"), "kernel version");
////    ui->devicesFormLayout->addRow(tr("architecture"), "architecture");QT
}

QStringList About::readFile(QString filepath) {
    QStringList fileCont;
    QFile file(filepath);
    if(file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while(!textStream.atEnd()) {
            QString line= textStream.readLine();
            line.remove('\n');
            fileCont<<line;
        }
        file.close();
        return fileCont;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return QStringList();
    }
}


void About::_call_dbus_get_computer_info(){
    interface = new QDBusInterface("com.kylin.assistant.qsessiondbus",
                                     "/com/kylin/assistant/qsessiondbus",
                                     "com.kylin.assistant.qsessiondbus",
                                     QDBusConnection::systemBus());

    if (!interface->isValid()){
        qCritical() << "Create Client Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return;
    }

//    QDBusConnection::systemBus().connect("com.control.center.qt.systemdbus",
//                                         "/",
//                                         "com.control.center.interface",
//                                         "computerinfo", this,
//                                         SLOT(get(QString)));

    QDBusReply<QString> reply =  interface->call("GetComputerInfo");
    if (reply.isValid()){
        computerinfo =  reply.value();
    }
    else {
        qCritical() << "Call 'GetComputerInfo' Failed!" << reply.error().message();
    }


    //async
//    QDBusPendingCall async = interface->asyncCall("GetComputerInfo");
//    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(async, this);

//    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(call_finished_slot(QDBusPendingCallWatcher*)));
}

//void About::call_finished_slot(QDBusPendingCallWatcher * call){
//    qDebug() << "----------------start------------>";
//    QDBusPendingReply<QString> reply = *call;
//    if (!reply.isError()){
//        QString info = reply.argumentAt<0>();
//        qDebug() << "-----------0--->" << "\n" << info;
//    }
//    else{
//        qDebug() << reply.error().message();
//    }
//    call->deleteLater();
//}


void About::runActiveWindow() {
    QString cmd = "kylin-activation";

    QProcess process(this);
    process.startDetached(cmd);
}

void About::showPdf() {
    QString cmd = "atril /usr/share/man/statement.pdf.gz";
    QProcess process(this);
    process.startDetached(cmd);
}

