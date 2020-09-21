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
#include "memoryentry.h"

#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QStorageInfo>

const QString TYPEVERSION = "Kylin V10";
const QString UbuntuVesion = "Ubuntu 20.04 LTS";
const QString UbuntuVesionEnhance = "Ubuntu 20.04.1 LTS";

About::About() {
    ui = new Ui::About;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("About");
    pluginType = NOTICEANDTASKS;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    initSearchText();
    setupDesktopComponent();
    setupKernelCompenent();
    setupVersionCompenent();
    setupSerialComponent();
}

About::~About() {
    delete ui;
}

QString About::get_plugin_name() {
    return pluginName;
}

int About::get_plugin_type() {
    return pluginType;
}

QWidget *About::get_plugin_ui() {
    return pluginWidget;
}

void About::plugin_delay_control() {

}

const QString About::name() const {
    return QStringLiteral("about");
}

void About::setupDesktopComponent() {
    //获取当前桌面环境
    QString dEnv;
    foreach (dEnv, QProcess::systemEnvironment()) {
        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
            break;
    }

    //设置当前桌面环境信息
    if (!dEnv.isEmpty()) {
        QString desktop = dEnv.section("=", -1, -1);
        ui->desktopContent->setText(desktop);
    }

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
    QDBusInterface *youkerInterface;
    for (int i = 0; i < 2; i++) {
        youkerInterface = new QDBusInterface("com.kylin.assistant.systemdaemon",
                                             "/com/kylin/assistant/systemdaemon",
                                             "com.kylin.assistant.systemdaemon",
                                             QDBusConnection::systemBus(), this);
    }
    if (!youkerInterface->isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QMap<QString, QVariant>> diskinfo;
    diskinfo  = youkerInterface ->call("get_harddisk_info");
    if (!diskinfo.isValid()) {
        qDebug() << "diskinfo is invalid" << endl;
    } else {
        QMap<QString, QVariant> res = diskinfo.value();
        diskSize = res["DiskCapacity"].toString();
        if (diskSize.contains("<1_1>")) {
            int index = diskSize.indexOf("<1_1>");
            QString disk1 = diskSize.left(index);
            diskSize = tr("Disk:") + disk1;
        }
    }

    QDBusReply<QMap<QString, QVariant>> cpuinfo;
    cpuinfo  = youkerInterface ->call("get_cpu_info");
    if (!diskinfo.isValid()) {
        qDebug() << "cpuinfo is invalid" << endl;
    } else {
        QMap<QString, QVariant> res = cpuinfo.value();
        cpuType = res["CpuVersion"].toString();
    }

    MemoryEntry memoryInfo;
    QStringList memory = memoryInfo.totalMemory();
    memorySize = memorySize + memory.at(0) + "(" + memory.at(1) + tr(" available") + ")";

    ui->cpuContent->setText(cpuType);
    ui->diskContent->setText(diskSize);
    ui->kernalContent->setText(kernal);
    ui->memoryContent->setText(memorySize);
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
    if (UbuntuVesion == version) {
        version = "UbuntuKylin 20.04 LTS";
    } else if (UbuntuVesionEnhance == version) {
        version = "UbuntuKylin 20.04.1 LTS";
    }
    ui->versionContent->setText(version);
    if (version == "Kylin V10" || version == "Kylin V10.1" || "Kylin V4" == version) {
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/galaxyUnicorn.png"));
    } else {
        ui->activeFrame->setVisible(false);
        ui->trialButton->setVisible(false);
        //设置桌面环境LOGO
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logoukui.svg"));
    }
}

void About::setupSerialComponent() {
    ui->trialButton->setFlat(true);
    ui->trialButton->setStyleSheet("text-align: left");
    QDBusInterface *activeInterface = new QDBusInterface("org.freedesktop.activation",
                                     "/org/freedesktop/activation",
                                     "org.freedesktop.activation.interface",
                                     QDBusConnection::systemBus(), this);
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

    qDebug()<<"status and serial is:"<<status<<" "<<serial<<endl;

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

QStringList About::readFile(QString filepath) {
    QStringList fileCont;
    QFile file(filepath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while (!textStream.atEnd()) {
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

void About::initSearchText() {
    //~ contents_path /about/version
    ui->versionLabel->setText(tr("version"));
    //~ contents_path /about/Kernel
    ui->kernalLabel->setText(tr("Kernel"));
    //~ contents_path /about/CPU
    ui->cpuLabel->setText(tr("CPU"));
    //~ contents_path /about/Memory
    ui->memoryLabel->setText(tr("Memory"));
    //~ contents_path /about/Disk
    ui->diskLabel->setText(tr("Disk"));
}

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

