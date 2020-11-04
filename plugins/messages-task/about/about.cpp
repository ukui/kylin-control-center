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

const QString vTen        = "v10";
const QString vTenEnhance = "v10.1";
const QString vFour       = "v4";

About::About() {
    ui = new Ui::About;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("About");
    pluginType = NOTICEANDTASKS;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    initSearchText();
    initActiveDbus();
    setupDesktopComponent();
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
    setupKernelCompenent();
}

const QString About::name() const {
    return QStringLiteral("about");
}

void About::setupDesktopComponent() {
    // 获取当前桌面环境
    QString dEnv;
    foreach (dEnv, QProcess::systemEnvironment()) {
        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
            break;
    }

    // 设置当前桌面环境信息
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

    QDBusInterface youkerInterface("com.kylin.assistant.systemdaemon",
                                   "/com/kylin/assistant/systemdaemon",
                                   "com.kylin.assistant.systemdaemon",
                                   QDBusConnection::systemBus());
    if (!youkerInterface.isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QMap<QString, QVariant>> diskinfo;
    diskinfo  = youkerInterface.call("get_harddisk_info");
    if (!diskinfo.isValid()) {
        qDebug() << "diskinfo is invalid" << endl;
    } else {
        QMap<QString, QVariant> res = diskinfo.value();
        diskSize = res["DiskCapacity"].toString();
        QStringList diskList = diskSize.split("<1_1>");
        diskSize.clear();
        for (int i = 0; i < diskList.length(); i++) {
            diskSize += tr("Disk") + QString::number(i+1) + ":" +diskList.at(i) + " ";
        }
    }

    QDBusReply<QMap<QString, QVariant>> cpuinfo;
    cpuinfo  = youkerInterface.call("get_cpu_info");
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
    QString versionID;
    QString version;

    for (QString str : osRes) {
        if (str.contains("VERSION_ID=")) {
            QRegExp rx("VERSION_ID=\"(.*)\"$");
            int pos = rx.indexIn(str);
            if (pos > -1) {
                versionID = rx.cap(1);
            }
        }
        if (str.contains("VERSION=")) {
            QRegExp rx("VERSION=\"(.*)\"$");
            int pos = rx.indexIn(str);
            if (pos > -1) {
                version = rx.cap(1);
            }
        }
    }

    ui->versionContent->setText(version);
    if (!versionID.compare(vTen, Qt::CaseInsensitive) ||
            !versionID.compare(vTenEnhance, Qt::CaseInsensitive) ||
            !versionID.compare(vFour, Qt::CaseInsensitive)) {

        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/galaxyUnicorn.png"));
    } else {
        ui->activeFrame->setVisible(false);
        ui->trialButton->setVisible(false);
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logoukui.svg"));
    }
}

void About::setupSerialComponent() {
    ui->trialButton->setFlat(true);
    ui->trialButton->setStyleSheet("text-align: left");
    if (!activeInterface.get()->isValid()) {
        qDebug() << "Create active Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return;
    }

    int status;
    QDBusReply<int> activeStatus;
    activeStatus  = activeInterface.get()->call("status");
    if (!activeStatus.isValid()) {
        qDebug()<<"activeStatus is invalid"<<endl;
    } else {
        status = activeStatus.value();
    }

    QString serial;
    QDBusReply<QString> serialReply;
    serialReply  = activeInterface.get()->call("serial_number");
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

void About::initActiveDbus() {
    activeInterface = QSharedPointer<QDBusInterface>(
                new QDBusInterface("org.freedesktop.activation",
                                   "/org/freedesktop/activation",
                                   "org.freedesktop.activation.interface",
                                   QDBusConnection::systemBus()));
    if (activeInterface.get()->isValid()) {
        connect(activeInterface.get(), SIGNAL(activation_result(int)), this, SLOT(activeSlot(int)));
    }
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

void About::activeSlot(int activeSignal) {
    if (!activeSignal) {
        setupSerialComponent();
    }
}

