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
#include "datetime.h"
#include "ui_datetime.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMovie>
#include <QDir>
#include <QDesktopWidget>

#include <locale.h>
#include <libintl.h>
#include <QProcess>
#include <sys/timex.h>

const char kTimezoneDomain[] = "installer-timezones";
const char kDefaultLocale[]  = "en_US.UTF-8";

const QString kcnBj =           "北京";
const QString kenBj =           "Asia/Beijing";

#define FORMAT_SCHEMA           "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY         "hoursystem"
#define DATE_KEY                "date"
#define SYNC_TIME_KEY           "synctime"

volatile bool syncThreadFlag =  false;

DateTime::DateTime() : mFirstLoad(true)
{
    ui = new Ui::DateTime;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);
    ui->infoFrame->setFrameShape(QFrame::Shape::Box);
    pluginName = tr("Date");
    pluginType = DATETIME;
}

DateTime::~DateTime()
{
    if (!mFirstLoad) {
        delete ui;
        delete m_zoneinfo;
        ui         = nullptr;
        m_zoneinfo = nullptr;
    }
}

QString DateTime::get_plugin_name()
{
    return pluginName;
}

int DateTime::get_plugin_type()
{
    return pluginType;
}

QWidget *DateTime::get_plugin_ui()
{

    if (mFirstLoad) {

        mFirstLoad = false;

        initUI();
        initTitleLabel();
        initStatus();
        initComponent();
        initConnect();
        connectToServer();
    }
    return pluginWidget;
}

void DateTime::plugin_delay_control()
{

}

const QString DateTime::name() const
{

    return QStringLiteral("date");
}

void DateTime::initTitleLabel()
{
    QGSettings *m_fontSetting = new QGSettings("org.ukui.style");
    QFont font;
    font.setFamily(m_fontSetting->get("systemFont").toString());
    font.setPointSize(m_fontSetting->get("systemFontSize").toInt());
    ui->titleLabel->setFont(font);
    ui->timeClockLable->setObjectName("timeClockLable");
    font.setPointSize(font.pointSize() + 8);
    font.setBold(true);
    ui->timeClockLable->setFont(font);
}

void DateTime::initUI()
{
    m_formTimeBtn       = new SwitchButton(pluginWidget);
    //~ contents_path /date/24-hour clock
    m_formTimeLabel     = new QLabel(tr("24-hour clock"), pluginWidget);
    syncTimeBtn         = new SwitchButton(pluginWidget);
    syncNetworkLabel    = new QLabel(tr("Sync from network"), pluginWidget);
    syncNetworkRetLabel = new QLabel(pluginWidget);
    syncNetworkRetLabel->setStyleSheet("QLabel{font-size: 15px; color: #D9F82929;}");

    m_zoneinfo          = new ZoneInfo;
    m_timezone          = new TimeZoneChooser(pluginWidget);
    m_itimer            = new QTimer(this);
    m_itimer->start(1000);

    const QByteArray id(FORMAT_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)) {
        const QByteArray id(FORMAT_SCHEMA);
        m_formatsettings = new QGSettings(id, QByteArray(), this);
        connect(m_formatsettings, &QGSettings::changed, this, [=](QString key) {
            QString hourFormat = m_formatsettings->get(TIME_FORMAT_KEY).toString();
            bool status = ("24" == hourFormat ? false : true);
            timeFormatClickedSlot(status, true);
        });
    }

    // 初始化dbus
    m_datetimeiface       = new QDBusInterface("org.freedesktop.timedate1",
                                       "/org/freedesktop/timedate1",
                                       "org.freedesktop.timedate1",
                                       QDBusConnection::systemBus(), this);

    m_datetimeiproperties = new QDBusInterface("org.freedesktop.timedate1",
                                             "/org/freedesktop/timedate1",
                                             "org.freedesktop.DBus.Properties",
                                             QDBusConnection::systemBus(), this);

}

void DateTime::initComponent()
{
    ui->titleLabel->setContentsMargins(0,0,0,16);
    ui->timeClockLable->setContentsMargins(0,0,0,16);

    //~ contents_path /date/Change time
    ui->chgtimebtn->setText(tr("Change time"));
    //~ contents_path /date/Change time zone
    ui->chgzonebtn->setText(tr("Change time zone"));

    QHBoxLayout *hourLayout = new QHBoxLayout(ui->hourFrame);

    hourLayout->addWidget(m_formTimeLabel);
    hourLayout->addWidget(m_formTimeBtn);

    QHBoxLayout *syncLayout = new QHBoxLayout(ui->syncFrame);

    syncLayout->addWidget(syncNetworkLabel);
    syncLayout->addStretch();
    syncLayout->addWidget(syncNetworkRetLabel);
    syncLayout->addWidget(syncTimeBtn);

    QDateTime currentime = QDateTime::currentDateTime();
    QString timeAndWeek = currentime.toString("yyyy-MM-dd  ddd").replace("周","星期");
    ui->dateLabel->setText(timeAndWeek + "     " + localizedTimezone);

    QFile tzfile("://zoneUtc");
    if (!tzfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qDebug("TZ File Open Failed");
    } else {
        QTextStream txt(&tzfile);
        int index = 0;
        while (!tzfile.atEnd()) {
            QStringList lineList = txt.readLine().split("\t");
            tzindexMapEn.insert(lineList.at(0),index);
            tzindexMapCN.insert(lineList.at(1),index);
            index++;
        }
    }
    tzfile.close();
}

void DateTime::initStatus()
{
    // 时区
    const QString locale = QLocale::system().name();
    QDBusReply<QVariant> tz = m_datetimeiproperties->call("Get", "org.freedesktop.timedate1", "Timezone");
    localizedTimezone = getLocalTimezoneName(tz.value().toString(), locale);
    loadHour();
}

void DateTime::connectToServer()
{
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void DateTime::keyChangedSlot(const QString &key)
{
    if (key == "datetime") {
        initStatus();
    }
}

bool DateTime::fileIsExits(const QString &filepath)
{
    QFile file(filepath);
    if(file.exists()) {
        return true;
    } else {
        return false;
    }
}

void DateTime::datetimeUpdateSlot()
{
    QString dateformat;
    if (m_formatsettings) {
        QStringList keys = m_formatsettings->keys();
        if(keys.contains("date")) {
            dateformat = m_formatsettings->get(DATE_KEY).toString();
        }
    }

    setCurrentTime();

    QString timeAndWeek;
    if ("cn" == dateformat) {
       timeAndWeek = current.toString("yyyy-MM-dd  ddd").replace("周","星期");
    } else {
       timeAndWeek = current.toString("yyyy-MM-dd  ddd");
    }

    ui->dateLabel->setText(timeAndWeek + "     " + localizedTimezone);
}

void DateTime::changetimeSlot()
{
    ChangtimeDialog *dialog = new ChangtimeDialog(m_formTimeBtn->isChecked());
    dialog->setWindowTitle(tr("change time"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_itimer->stop();
    m_itimer->start();
    dialog->exec();
}

void DateTime::changezoneSlot()
{
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = m_timezone->width();
    int y = m_timezone->height();
    m_timezone->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());

    m_timezone->setWindowModality(Qt::ApplicationModal);
    m_timezone->show();

    m_timezone->setMarkedTimeZoneSlot(m_zoneinfo->getCurrentTimzone());
}

void DateTime::changezoneSlot(QString zone)
{
    m_datetimeiface->call("SetTimezone", zone, true);
}

void DateTime::timeFormatClickedSlot(bool flag, bool outChange)
{
    if (!m_formatsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }
    QStringList keys = m_formatsettings->keys();
    if (keys.contains("hoursystem") && !outChange) {
        if (flag == true) {
            m_formatsettings->set(TIME_FORMAT_KEY, "24");
        } else {
            m_formatsettings->set(TIME_FORMAT_KEY, "12");
        }
    }
    //重置时间格式
    m_itimer->stop();
    m_itimer->start(1000);
}

QDBusMessage DateTime::rsyncWithNetworkSlot(bool status)
{
    return m_datetimeiface->call("SetNTP", status, true);
}

void DateTime::loadHour()
{
    if (!m_formatsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed";
        return;
    }
    QStringList keys = m_formatsettings->keys();
    QString format;
    bool formatB;
    if (keys.contains("hoursystem")) {
        format = m_formatsettings->get(TIME_FORMAT_KEY).toString();
    }

    if (format == "24") {
        m_formTimeBtn->setChecked(true);
    } else {
        m_formTimeBtn->setChecked(false);
    }

    setCurrentTime();

    if (keys.contains(SYNC_TIME_KEY)) {
        formatB = m_formatsettings->get(SYNC_TIME_KEY).toBool();
        syncTimeBtn->setChecked(formatB);
        if (formatB != false) {
            ui->chgtimebtn->setEnabled(false);
        }
    }
}

void DateTime::setCurrentTime()
{

    current = QDateTime::currentDateTime();
    QString currentsecStr ;
    if (m_formTimeBtn->isChecked()) {
        currentsecStr = current.toString("hh : mm : ss");
    } else {
        currentsecStr = current.toString("AP hh: mm : ss");
    }
    ui->timeClockLable->setText(currentsecStr);
}

void DateTime::initConnect()
{
    connect(ui->chgtimebtn,SIGNAL(clicked()),this,SLOT(changetimeSlot()));
    connect(ui->chgzonebtn,SIGNAL(clicked()),this,SLOT(changezoneSlot()));

    connect(m_formTimeBtn, &SwitchButton::checkedChanged, this, [=](bool status) {
        timeFormatClickedSlot(status, false);
    });

    connect(syncTimeBtn, &SwitchButton::checkedChanged, this,[=](bool status) {
        synctimeFormatSlot(status,true); //按钮被改变，需要修改
    });

    connect(m_timezone, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        changezoneSlot(timezone);
        m_timezone->hide();
        const QString locale = QLocale::system().name();
        localizedTimezone = m_zoneinfo->getLocalTimezoneName(timezone, locale);
    });

    connect(m_itimer,SIGNAL(timeout()), this, SLOT(datetimeUpdateSlot()));

    connect(m_formatsettings, &QGSettings::changed, this, [=] (const QString &key) {
        if (key == "hoursystem") {
            QString value = m_formatsettings->get(TIME_FORMAT_KEY).toString();
            bool checked = (value == "24" ? true : false);
            m_formTimeBtn->setChecked(checked);
        }

        if (key == SYNC_TIME_KEY){
            bool valueB = m_formatsettings->get(SYNC_TIME_KEY).toBool();
            syncTimeBtn->setChecked(valueB);
        }
    });
}

QString DateTime::getLocalTimezoneName(QString timezone, QString locale)
{
    (void) setlocale(LC_ALL, QString(locale + ".UTF-8").toStdString().c_str());
    const QString local_name(dgettext(kTimezoneDomain,
                                      timezone.toStdString().c_str()));
    int index = local_name.lastIndexOf('/');
    if (index == -1) {
      // Some translations of locale name contains non-standard char.
      index = local_name.lastIndexOf("∕");
    }

    // Reset locale.
    (void) setlocale(LC_ALL, kDefaultLocale);

    return (index > -1) ? local_name.mid(index + 1) : local_name;
}

void DateTime::synctimeFormatSlot(bool status,bool outChange)
{
    if (!m_formatsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }
    QStringList keys = m_formatsettings->keys();
    if (keys.contains(SYNC_TIME_KEY) && outChange) {
        if (status != false) {
            m_formatsettings->set(SYNC_TIME_KEY, true);
        } else {
            m_formatsettings->set(SYNC_TIME_KEY, false);
        }
    }
    QDBusMessage retDBus =  rsyncWithNetworkSlot(status);
    if (status != false) {
        ui->chgtimebtn->setEnabled(false);

        if (retDBus.type() == QDBusMessage::ReplyMessage) {
            QString successMSG = tr("  ");
            QString failMSG = tr("Sync from network failed");
            CGetSyncRes *syncThread = new CGetSyncRes(this,successMSG,failMSG);
            connect(syncThread,SIGNAL(finished()),syncThread,SLOT(deleteLater()));
            syncThread->start();
        } else {
            syncNetworkRetLabel->setText(tr("Sync from network failed"));
        }
    } else {
        ui->chgtimebtn->setEnabled(true);
    }
}
/*同步硬件时钟*/
void DateTime::syncRTC()
{
    QDBusInterface * changeRTCinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                                             "/",
                                                             "com.control.center.interface",
                                                             QDBusConnection::systemBus());

    if (!changeRTCinterface->isValid()) {
        qCritical() << "Create Client Interface Failed When execute gpasswd: " << QDBusConnection::systemBus().lastError();
        return;
    }
    changeRTCinterface->call("changeRTC");
    delete changeRTCinterface;
    changeRTCinterface = nullptr;
}

CGetSyncRes::CGetSyncRes(DateTime *dataTimeUI,QString successMSG,QString failMSG)
{
    this -> dataTimeUI = dataTimeUI;
    this -> successMSG = successMSG;
    this -> failMSG = failMSG;
}

CGetSyncRes::~CGetSyncRes()
{

}
void CGetSyncRes::run()
{
    for(qint8 i = 0;i < 10; ++i) {
        struct timex txc = {};
        if (adjtimex(&txc) < 0 || txc.maxerror >= 16000000) {  //未能同步时间
            QString pixName = QString(":/img/plugins/upgrade/loading%1.svg").arg(i+10);
            QPixmap pix(pixName);
            this->dataTimeUI->syncTimeBtn->setEnabled(false);
            qApp->processEvents();
            this->dataTimeUI->syncNetworkRetLabel->setPixmap(pix);
            msleep(500);
            continue;
        } else {                                               //同步时间成功
            DateTime::syncRTC();
            this->dataTimeUI->syncNetworkRetLabel->setText(successMSG);
            this->dataTimeUI->syncTimeBtn->setEnabled(true);
            return;
        }
    }
    this->dataTimeUI->syncTimeBtn->setEnabled(true);
    this->dataTimeUI->syncNetworkRetLabel->setText(failMSG);
    if (syncThreadFlag == false) { //创建线程一直查时间同步是否成功
        CSyncTime *syncTimeThread = new CSyncTime(this->dataTimeUI,successMSG,failMSG);
        connect(syncTimeThread,SIGNAL(finished()),syncTimeThread,SLOT(deleteLater()));
        syncTimeThread->start();
        syncThreadFlag = true;
    }
    return;
}

CSyncTime::CSyncTime(DateTime *dataTimeUI,QString successMSG,QString failMSG)
{
    this -> dataTimeUI = dataTimeUI;
    this -> successMSG = successMSG;
    this -> failMSG = failMSG;
}

CSyncTime::~CSyncTime()
{

}
void CSyncTime::run()
{
    QDBusInterface *r_datetimeiface = new QDBusInterface("org.freedesktop.timedate1",
                                           "/org/freedesktop/timedate1",
                                           "org.freedesktop.timedate1",
                                           QDBusConnection::systemBus(), this);
    while (true) {
        if (this->dataTimeUI->syncTimeBtn->isChecked() == false) {
            syncThreadFlag = false;
            delete r_datetimeiface;
            return;
        }
        r_datetimeiface->call("SetNTP", true, true);
        struct timex txc = {};
        if (adjtimex(&txc) > 0 && txc.maxerror < 16000000) { //同步时间成功
            DateTime::syncRTC();
            this->dataTimeUI->syncNetworkRetLabel->setText(successMSG);
            syncThreadFlag = false;
            delete r_datetimeiface;
            return;
        }
        sleep(2);
    }
}
