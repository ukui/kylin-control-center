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

#include <KFormat>
#include <unistd.h>
#include <QFile>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#elif defined(Q_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QStorageInfo>
#include <QtMath>

#define THEME_STYLE_SCHEMA "org.ukui.style"
#define STYLE_NAME_KEY "style-name"
#define CONTAIN_STYLE_NAME_KEY "styleName"
#define UKUI_DEFAULT "ukui-default"
#define UKUI_DARK "ukui-dark"

const QString vTen        = "v10";
const QString vTenEnhance = "v10.1";
const QString vFour = "v4";

About::About() : mFirstLoad(true)
{
    pluginName = tr("About");
    pluginType = NOTICEANDTASKS;
}

About::~About()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString About::get_plugin_name()
{
    return pluginName;
}

int About::get_plugin_type()
{
    return pluginType;
}

QWidget *About::get_plugin_ui()
{
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::About;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        initTitleLabel();
        initSearchText();
        initActiveDbus();
        setupDesktopComponent();
        setupVersionCompenent();
        setupSerialComponent();
        setupKernelCompenent();
    }

    return pluginWidget;
}

void About::plugin_delay_control()
{
}

const QString About::name() const
{
    return QStringLiteral("about");
}

void About::setupDesktopComponent()
{
    // 获取当前桌面环境
    QString dEnv;
    foreach (dEnv, QProcess::systemEnvironment()) {
        if (dEnv.startsWith("XDG_CURRENT_DESKTOP"))
            break;
    }

    // 设置当前桌面环境信息
    if (!dEnv.isEmpty()) {
        QString desktop = dEnv.section("=", -1, -1);
        if (desktop.contains("UKUI", Qt::CaseInsensitive)) {
            ui->desktopContent->setText("UKUI");
        } else {
            ui->desktopContent->setText(desktop);
        }
    }

    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }

    ui->userContent->setText(name);
}

void About::setupKernelCompenent()
{
    QString memorySize;
    QString cpuType;

    QString kernal = QSysInfo::kernelType() + " " + QSysInfo::kernelVersion();
    memorySize = getTotalMemory();

    ui->kernalContent->setText(kernal);
    ui->memoryContent->setText(memorySize);

    QDBusInterface youkerInterface("com.kylin.assistant.systemdaemon",
                                   "/com/kylin/assistant/systemdaemon",
                                   "com.kylin.assistant.systemdaemon",
                                   QDBusConnection::systemBus());
    if (!youkerInterface.isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " <<
            QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QMap<QString, QVariant> > cpuinfo;
    cpuinfo = youkerInterface.call("get_cpu_info");
    if (!cpuinfo.isValid()) {
        qDebug() << "cpuinfo is invalid" << endl;
    } else {
        QMap<QString, QVariant> res = cpuinfo.value();
        cpuType = res["CpuVersion"].toString();
    }

    ui->cpuContent->setText(cpuType);
    ui->diskContent->setVisible(false);
}

void About::setupVersionCompenent()
{
    QString versionPath = "/etc/os-release";
    QStringList osRes = readFile(versionPath);
    QString versionID;
    QString version;

    if (QGSettings::isSchemaInstalled(THEME_STYLE_SCHEMA)) {
            themeStyleQgsettings = new QGSettings(THEME_STYLE_SCHEMA, QByteArray(), this);
    } else {
        themeStyleQgsettings = nullptr;
        qDebug()<<THEME_STYLE_SCHEMA<<" not installed";
    }

    for (QString str : osRes) {
        if (str.contains("VERSION_ID=")) {
            QRegExp rx("VERSION_ID=\"(.*)\"$");
            int pos = rx.indexIn(str);
            if (pos > -1) {
                versionID = rx.cap(1);
            }
        }

        if (!QLocale::system().name().compare("zh_CN", Qt::CaseInsensitive)) {
            if (str.contains("VERSION=")) {
                QRegExp rx("VERSION=\"(.*)\"$");
                int pos = rx.indexIn(str);
                if (pos > -1) {
                    version = rx.cap(1);
                }
            }
        } else {
            if (str.contains("VERSION_US=")) {
                QRegExp rx("VERSION_US=\"(.*)\"$");
                int pos = rx.indexIn(str);
                if (pos > -1) {
                    version = rx.cap(1);
                }
            }
        }
    }

    if (!version.isEmpty()) {
        ui->versionContent->setText(version);
    }

    if (!versionID.compare(vTen, Qt::CaseInsensitive) ||
            !versionID.compare(vTenEnhance, Qt::CaseInsensitive) ||
            !versionID.compare(vFour, Qt::CaseInsensitive)) {
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logo-light.svg")); //默认设置为light
        if (themeStyleQgsettings != nullptr && themeStyleQgsettings->keys().contains(CONTAIN_STYLE_NAME_KEY)) {
            if (themeStyleQgsettings->get(STYLE_NAME_KEY).toString() == UKUI_DARK) { //深色模式改为dark
                ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logo-dark.svg"));
            }
            connect(themeStyleQgsettings,&QGSettings::changed,this,[=](QString changedKey) {  //监听主题变化
                if (changedKey == CONTAIN_STYLE_NAME_KEY) {
                    if (themeStyleQgsettings->get(STYLE_NAME_KEY).toString() == UKUI_DARK) {
                        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logo-dark.svg"));
                    } else {
                        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logo-light.svg"));
                    }
                }
            });
       }
    } else {
        ui->activeFrame->setVisible(false);
        ui->trialButton->setVisible(false);
        ui->logoLabel->setPixmap(QPixmap("://img/plugins/about/logoukui.svg"));
    }
}

void About::setupSerialComponent()
{
    if (!activeInterface.get()->isValid()) {
        qDebug() << "Create active Interface Failed When Get active info: " <<
            QDBusConnection::systemBus().lastError();
        return;
    }

    int status = 0;
    QDBusMessage activeReply = activeInterface.get()->call("status");
    if (activeReply.type() == QDBusMessage::ReplyMessage) {
        status = activeReply.arguments().at(0).toInt();
    }

    QString serial;
    QDBusReply<QString> serialReply;
    serialReply = activeInterface.get()->call("serial_number");
    if (!serialReply.isValid()) {
        qDebug()<<"serialReply is invalid"<<endl;
    } else {
        serial = serialReply.value();
    }

    if (1 == status) {
        ui->activeContent->setText(tr("Activated"));
        ui->activeButton->hide();
        ui->trialButton->hide();
    } else {
        QDBusMessage dateReply = activeInterface.get()->call("date");
        QString dateRes;
        if (dateReply.type() == QDBusMessage::ReplyMessage) {
            dateRes = dateReply.arguments().at(0).toString();
            if (!dateRes.isEmpty()) {
                ui->activeContent->setText(tr("The system has expired. The expiration time is:")
                                           + dateRes);
                ui->activeButton->setText(tr("Extended"));
            } else {
                ui->activeContent->setText(tr("Inactivated"));
            }
        }
    }
    ui->serviceContent->setText(serial);

    connect(ui->activeButton, &QPushButton::clicked, this, &About::runActiveWindow);
    connect(ui->trialButton, &QPushButton::clicked, this, &About::showPdf);
}

qlonglong About::calculateTotalRam()
{
    qlonglong ret = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        // manpage "sizes are given as multiples of mem_unit bytes"
        ret = qlonglong(info.totalram) * info.mem_unit;
#elif defined(Q_OS_FREEBSD)
    /* Stuff for sysctl */
    size_t len;

    unsigned long memory;
    len = sizeof(memory);
    sysctlbyname("hw.physmem", &memory, &len, NULL, 0);

    ret = memory;
#endif
    return ret;
}

QString About::getTotalMemory()
{
    const QString fileName = "/proc/meminfo";
    QFile meninfoFile(fileName);
    if (!meninfoFile.exists()) {
        printf("/proc/meminfo file not exist \n");
    }
    if (!meninfoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("open /proc/meminfo fail \n");
    }

    QTextStream in(&meninfoFile);
    QString line = in.readLine();
    float memtotal = 0;

    while (!line.isNull()) {
        if (line.contains("MemTotal")) {
            line.replace(QRegExp("[\\s]+"), " ");

            QStringList lineList = line.split(" ");
            QString mem = lineList.at(1);
            memtotal = mem.toFloat();
            break;
        } else {
            line = in.readLine();
        }
    }

    memtotal = ceil(memtotal / 1024 / 1024);
    // 向2的n次方取整
    int nPow = ceil(log(memtotal)/log(2.0));
    memtotal = pow(2.0, nPow);

    return QString::number(memtotal) + " GB";
}

QStringList About::totalMemory()
{
    QStringList res;
    const qlonglong totalRam = calculateTotalRam();

    if (totalRam > 0) {
        QString total = KFormat().formatByteSize(totalRam, 0, KFormat::JEDECBinaryDialect);
        QString available = KFormat().formatByteSize(totalRam, 1, KFormat::JEDECBinaryDialect);
        if (atof(total.toLatin1()) < atof(available.toLatin1())) {
            qSwap(total, available);
        }
        res << total << available;
        return res;
    }
    return res;
}


QStringList About::readFile(QString filepath)
{
    QStringList fileCont;
    QFile file(filepath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while (!textStream.atEnd()) {
            QString line = textStream.readLine();
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

void About::initTitleLabel()
{
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
}

void About::initSearchText()
{
    //~ contents_path /about/version
    ui->versionLabel->setText(tr("version"));
    //~ contents_path /about/Kernel
    ui->kernalLabel->setText(tr("Kernel"));
    //~ contents_path /about/CPU
    ui->cpuLabel->setText(tr("CPU"));
    //~ contents_path /about/Memory
    ui->memoryLabel->setText(tr("Memory"));
    ui->diskLabel->setVisible(false);
}

void About::initActiveDbus()
{
    activeInterface = QSharedPointer<QDBusInterface>(
        new QDBusInterface("org.freedesktop.activation",
                           "/org/freedesktop/activation",
                           "org.freedesktop.activation.interface",
                           QDBusConnection::systemBus()));
    if (activeInterface.get()->isValid()) {
        connect(activeInterface.get(), SIGNAL(activation_result(int)), this, SLOT(activeSlot(int)));
    }
}

void About::runActiveWindow()
{
    QString cmd = "kylin-activation";

    QProcess process(this);
    process.startDetached(cmd);
}

void About::showPdf()
{
    QStringList res = getUserDefaultLanguage();
    QString lang = res.at(1);
    QString cmd;
    QFile pdfFile_zh("/usr/share/kylin-verify-gui/免责协议.pdf");
    QFile pdfFile_en("/usr/share/kylin-verify-gui/disclaimers.pdf");
    if (lang.split(':').at(0) == "zh_CN") {
        if (pdfFile_zh.exists()) {
            cmd = "atril /usr/share/kylin-verify-gui/免责协议.pdf";
        } else {
            cmd = "atril /usr/share/man/statement.pdf.gz";
        }
    } else {
        if (pdfFile_en.exists()) {
            cmd = "atril /usr/share/kylin-verify-gui/disclaimers.pdf";
        }
        else {
            cmd = "atril /usr/share/man/statement_en.pdf.gz";
        }
    }

    QProcess process(this);
    process.startDetached(cmd);
}

void About::activeSlot(int activeSignal)
{
    if (!activeSignal) {
        setupSerialComponent();
    }
}


QStringList About::getUserDefaultLanguage() {
    QString formats;
    QString language;
    QStringList result;

    unsigned int uid = getuid();
    QString objpath = "/org/freedesktop/Accounts/User"+QString::number(uid);

    QDBusInterface iproperty("org.freedesktop.Accounts",
                             objpath,
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusReply<QMap<QString, QVariant> > reply = iproperty.call("GetAll", "org.freedesktop.Accounts.User");
    if (reply.isValid()) {
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        if (propertyMap.keys().contains("FormatsLocale")) {
            formats = propertyMap.find("FormatsLocale").value().toString();
        }
        if(language.isEmpty() && propertyMap.keys().contains("Language")) {
            language = propertyMap.find("Language").value().toString();
        }
    } else {
        //qDebug() << "reply failed";
    }
    result.append(formats);
    result.append(language);
    return result;
}
