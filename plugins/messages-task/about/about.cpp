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

#include <QDebug>

About::About()
{
    ui = new Ui::About;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("About");
    pluginType = NOTICEANDTASKS;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

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
    _call_dbus_get_computer_info();
    _data_init();

    setupComponent();
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

void About::setupComponent(){
    //设置桌面环境LOGO
    ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logo.png"));

    //获取当前桌面环境
    QString dEnv;
    foreach (dEnv, QProcess::systemEnvironment()){
        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
            break;
    }
    //设置当前桌面环境信息
    QLabel * dEnvLabel = new QLabel(tr("UNKNOWN"));
    if (!dEnv.isEmpty())
        dEnvLabel->setText(dEnv.section("=", -1, -1));
    ui->systemFormLayout->addRow(tr("Current desktop env:"), dEnvLabel);

    /**
     * 设置当前操作系统信息
     */
    //设置操作系统版本
//    qDebug() << QSysInfo::prettyProductName() << QSysInfo::productType() << QSysInfo::productVersion();
    QLabel * osVersionLabel = new QLabel(tr("UNKNOWN"));
    osVersionLabel->setText(QSysInfo::productVersion());
    ui->systemFormLayout->addRow(tr("OS Version:"), osVersionLabel);
    //设置CPU平台
    QLabel * cpuArch = new QLabel(tr("UNKNOWN"));
    cpuArch->setText(QSysInfo::currentCpuArchitecture());
    ui->systemFormLayout->addRow(tr("CPU Arch:"), cpuArch);
    //设置内核版本号
//    qDebug() << QSysInfo::kernelType() << QSysInfo::kernelVersion();
    QLabel * kernelLabel = new QLabel(tr("UNKNOWN"));
    kernelLabel->setText(QSysInfo::kernelType() + " " + QSysInfo::kernelVersion());
    ui->systemFormLayout->addRow(tr("Kernel Version"), kernelLabel);


    /**
     * 设置硬件详情的信息项目
     */
    QLabel * manufacturers = new QLabel(tr("UNKNOWN"));
    if (infoMap.contains(MANUFACTURER))
        manufacturers->setText(QString(infoMap.find(MANUFACTURER).value()));

    QLabel * productname = new QLabel("UNKNOWN");
    if (infoMap.contains(PRODUCTNAME))
        productname->setText(QString(infoMap.find(PRODUCTNAME).value()));

    QLabel * version = new QLabel("UNKNOWN");
    if (infoMap.contains(VERSION))
        version->setText(QString(infoMap.find(VERSION).value()));

    QLabel * serialnumber = new QLabel("UNKNOWN");
    if (infoMap.contains(SERIALNUMBER))
        serialnumber->setText(QString(infoMap.find(SERIALNUMBER).value()));

    ui->devicesFormLayout->setHorizontalSpacing(70);
    ui->devicesFormLayout->addRow(tr("Manufacturers:"), manufacturers);
    ui->devicesFormLayout->addRow(tr("Product Name:"), productname);
    ui->devicesFormLayout->addRow(tr("Version:"), version);
    ui->devicesFormLayout->addRow(tr("Serial Number:"), serialnumber);
//    ui->devicesFormLayout->addRow(tr("hostname:"), "hostname");
//    ui->devicesFormLayout->addRow(tr("running time:"), "running time");
//    ui->devicesFormLayout->addRow(tr("os type:"), "os type");
//    ui->devicesFormLayout->addRow(tr("os version"), "os version");
//    ui->devicesFormLayout->addRow(tr("system bit"), "system bit");
//    ui->devicesFormLayout->addRow(tr("kernel version"), "kernel version");
//    ui->devicesFormLayout->addRow(tr("architecture"), "architecture");
}

void About::_call_dbus_get_computer_info(){
    interface = new QDBusInterface("com.control.center.qt.systemdbus",
                                     "/",
                                     "com.control.center.interface",
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
