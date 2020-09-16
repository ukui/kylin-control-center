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

const char kTimezoneDomain[] = "installer-timezones";
const char kDefaultLocale[] = "en_US.UTF-8";

#define FORMAT_SCHEMA "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY "hoursystem"
#define DATE_KEY "date"

DateTime::DateTime() {
    ui = new Ui::DateTime;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Datetime");
    pluginType = DATETIME;
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->timeClockLable->setStyleSheet("QLabel{font-size: 24px; color: palette(windowText);}");

    m_zoneinfo = new ZoneInfo;
    m_timezone = new TimeZoneChooser;
    m_itimer = new QTimer();
    m_itimer->start(1000);
    connect(m_itimer,SIGNAL(timeout()), this, SLOT(datetime_update_slot()));

    m_formTimeBtn = new SwitchButton;
    //~ contents_path /datetime/24-hour clock
    m_formTimeLabel = new QLabel(tr("24-hour clock"));

    // 初始化gsettings
    const QByteArray id(FORMAT_SCHEMA);
    if(QGSettings::isSchemaInstalled(id)) {
        const QByteArray id(FORMAT_SCHEMA);
        m_formatsettings = new QGSettings(id);
        connect(m_formatsettings, &QGSettings::changed, this, [=](QString key) {
            QString hourFormat = m_formatsettings->get(TIME_FORMAT_KEY).toString();
            bool status = ("24" == hourFormat ? false : true);
            time_format_clicked_slot(status, true);
        });
    }
    connectGSetting();
    //初始化dbus
    m_datetimeiface = new QDBusInterface("org.freedesktop.timedate1",
                                       "/org/freedesktop/timedate1",
                                       "org.freedesktop.timedate1",
                                       QDBusConnection::systemBus());

    m_datetimeiproperties = new QDBusInterface("org.freedesktop.timedate1",
                                             "/org/freedesktop/timedate1",
                                             "org.freedesktop.DBus.Properties",
                                             QDBusConnection::systemBus());
    component_init();
    status_init();

    connect(ui->chgtimebtn,SIGNAL(clicked()),this,SLOT(changetime_slot()));
    connect(ui->chgzonebtn,SIGNAL(clicked()),this,SLOT(changezone_slot()));
    connect(m_formTimeBtn, &SwitchButton::checkedChanged, this, [=](bool status) {
        time_format_clicked_slot(status, false);
    });
    connect(m_timezone, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        changezone_slot(timezone);
        m_timezone->hide();
        const QString locale = QLocale::system().name();
        QString localizedTimezone = m_zoneinfo->getLocalTimezoneName(timezone, locale);
        ui->timezoneLabel->setText(localizedTimezone);
    });
    connect(ui->synsystimeBtn,SIGNAL(clicked()),this,SLOT(rsync_with_network_slot()));
}

DateTime::~DateTime() {
    delete ui;
    delete m_formatsettings;
    delete m_datetimeiface;
    delete m_datetimeiproperties;
}

QString DateTime::get_plugin_name() {
    return pluginName;
}

int DateTime::get_plugin_type() {
    return pluginType;
}

QWidget *DateTime::get_plugin_ui() {
    return pluginWidget;
}

void DateTime::plugin_delay_control(){

}

const QString DateTime::name() const {

    return QStringLiteral("datetime");
}

void DateTime::component_init() {
    ui->titleLabel->setContentsMargins(0,0,0,16);
    ui->timeClockLable->setContentsMargins(0,0,0,16);

    //~ contents_path /datetime/Sync system time
    ui->synsystimeBtn->setText(tr("Sync system time"));
    //~ contents_path /datetime/Change time
    ui->chgtimebtn->setText(tr("Change time"));
    //~ contents_path /datetime/Change time zone
    ui->chgzonebtn->setText(tr("Change time zone"));

    ui->chgLayout->setSpacing(16);
    ui->syslabel->setVisible(false);

    ui->endlabel->setVisible(false);

    QHBoxLayout *hourLayout = new QHBoxLayout(ui->hourFrame);

    hourLayout->addWidget(m_formTimeLabel);
    hourLayout->addWidget(m_formTimeBtn);

    QDateTime currentime = QDateTime::currentDateTime();
    QString timeAndWeek = currentime.toString("yyyy/MM/dd ddd");
    ui->dateLabel->setText(timeAndWeek);

    //因为ntpd和systemd的网络时间同步会有冲突，所以安装了ntp的话，禁止使用控制面板设置网络时间同步
    QFileInfo fileinfo("/usr/sbin/ntpd");
    if (fileinfo.exists()) {
        ui->synsystimeBtn->setVisible(false);
    }

    QFile tzfile("://zoneUtc");
    if(!tzfile.open(QIODevice::ReadOnly | QIODevice::Text)){
         qDebug("TZ File Open Failed");
    } else {
        QTextStream txt(&tzfile);
        int index = 0;
        qDebug()<<"TODO------->language problem"<<endl;
        while (!tzfile.atEnd()) {
            QStringList lineList = txt.readLine().split("\t");
            tzindexMapEn.insert(lineList.at(0),index);
            tzindexMapCN.insert(lineList.at(1),index);
            index++;
        }
    }
    tzfile.close();
}

void DateTime::status_init() {
    //时区
    const QString locale = QLocale::system().name();
    QDBusReply<QVariant> tz = m_datetimeiproperties->call("Get", "org.freedesktop.timedate1", "Timezone");
    ui->timezoneLabel->setText(getLocalTimezoneName(tz.value().toString(), locale));
    loadHour();
}

bool DateTime::fileIsExits(const QString &filepath) {
    QFile file(filepath);
    if(file.exists()) {
        return true;
    } else {
        return false;
    }
}

void DateTime::datetime_update_slot() {
    QString dateformat;
    if(m_formatsettings) {
        QStringList keys = m_formatsettings->keys();
        if(keys.contains("date")) {
            dateformat =  m_formatsettings->get(DATE_KEY).toString();
        }
    }

    //当前时间    
    current = QDateTime::currentDateTime();
//    qDebug()<<"current time is-------->"<<current<<endl;

    QString currentsecStr ;
    if(m_formTimeBtn->isChecked()){
        currentsecStr = current.toString("hh : mm : ss");
    }else{
        currentsecStr = current.toString("AP hh: mm : ss");
    }
    QString timeAndWeek;
    if ("cn" == dateformat) {
       timeAndWeek = current.toString("yyyy/MM/dd ddd");
    } else {
       timeAndWeek = current.toString("yyyy-MM-dd ddd");
    }

//    qDebug()<<"year is----------->"<<timeAndWeek<<endl;
    ui->dateLabel->setText(timeAndWeek);
    ui->timeClockLable->setText(currentsecStr);

}

void DateTime::changetime_slot() {
    ChangtimeDialog *dialog = new ChangtimeDialog(m_formTimeBtn->isChecked());
    dialog->setWindowTitle(tr("change time"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_itimer->stop();
    m_itimer->start();
    dialog->exec();
}

void DateTime::changezone_slot() {

    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = m_timezone->width();
    int y = m_timezone->height();
    m_timezone->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());

    m_timezone->show();

    m_timezone->setMarkedTimeZoneSlot(m_zoneinfo->getCurrentTimzone());
}

void DateTime::changezone_slot(QString zone) {
    m_datetimeiface->call("SetTimezone", zone, true);
}

void DateTime::time_format_clicked_slot(bool flag, bool outChange) {
    if (!m_formatsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }
    QStringList keys = m_formatsettings->keys();
    if (keys.contains("hoursystem") && !outChange) {
        if(flag == true) {
            m_formatsettings->set(TIME_FORMAT_KEY, "24");
        } else {
            m_formatsettings->set(TIME_FORMAT_KEY, "12");
        }
    }
    //重置时间格式
    m_itimer->stop();
    m_itimer->start(1000);
}

void DateTime::showendLabel() {
    ui->syslabel->setVisible(false);
    if(ui->syslabel->isVisible()){
        ui->endlabel->setVisible(false);
    }else {
        ui->endlabel->setVisible(true);
    }
    QTimer::singleShot(2*1000,this,SLOT(hidendLabel()));
}

void DateTime::hidendLabel() {
    ui->endlabel->setVisible(false);
}

void DateTime::rsync_with_network_slot() {
//    qDebug()<<"TODO------> sleep waies?"<<endl;

    m_datetimeiface->call("SetNTP", true, true);

//    QMovie *loadgif = new QMovie(":/sys.gif");
//    loadgif->start();
//    ui->syslabel->setVisible(true);
//    ui->syslabel->setMovie(loadgif);
//    ui->syslabel->setScaledContents(true);
//    ui->syslabel->setStyleSheet("QLabel#syslabel{border-radius:4px;}");

//    QTimer::singleShot(2*1000,this,SLOT(showendLabel()));
}

void DateTime::loadHour() {
    if (!m_formatsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }    
    QStringList keys = m_formatsettings->keys();
    QString format;
    if (keys.contains("hoursystem")) {
        format = m_formatsettings->get(TIME_FORMAT_KEY).toString();
    }
    if (format == "24") {
        m_formTimeBtn->setChecked(true);
    } else {
        m_formTimeBtn->setChecked(false);
    }
}

void DateTime::connectGSetting() {
    connect(m_formatsettings, &QGSettings::changed, this, [=] (const QString &key) {
//            qDebug()<<"status changed ------------>"<<endl;
        if (key == "hoursystem") {
            QString value = m_formatsettings->get(TIME_FORMAT_KEY).toString();
            bool checked = (value == "24" ? true : false);
            m_formTimeBtn->setChecked(checked);
        }
        if (key == "date") {
            QString value = m_formatsettings->get(DATE_KEY).toString();
        }
    });
}

QString DateTime::getLocalTimezoneName(QString timezone, QString locale) {

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
