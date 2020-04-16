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


#define FORMAT_SCHEMA "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY "hoursystem"
#define DATE_KEY "date"

DateTime::DateTime()
{
    ui = new Ui::DateTime;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
//    pluginWidget->setStyleSheet("background: #ffffff;");
    ui->setupUi(pluginWidget);

    pluginName = tr("Datetime");
    pluginType = DATETIME;
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->timeClockLable->setStyleSheet("QLabel{font-size: 24px;QFont:Bold; color: palette(windowText);}");

//    qDebug()<<"进入时间日期UI------------------》"<<endl;


    m_zoneinfo = new ZoneInfo;
    m_timezone = new TimeZoneChooser;
    m_itimer = new QTimer();
    m_itimer->start(1000);
    connect(m_itimer,SIGNAL(timeout()), this, SLOT(datetime_update_slot()));

    m_formTimeBtn = new SwitchButton;
    m_formTimeLabel = new QLabel(tr("24-hour clock"));


    //初始化gsettings
    const QByteArray id(FORMAT_SCHEMA);
    if(QGSettings::isSchemaInstalled(id)) {
        const QByteArray id(FORMAT_SCHEMA);
        m_formatsettings = new QGSettings(id);
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
    connect(m_formTimeBtn, SIGNAL(checkedChanged(bool)),this,SLOT(time_format_clicked_slot(bool)));
    connect(m_timezone, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        qDebug()<<"timezone is---------->"<<timezone<<endl;
        changezone_slot(timezone);
        m_timezone->hide();
        ui->timezoneLabel->setText(timezone);
    });
    connect(ui->synsystimeBtn,SIGNAL(clicked()),this,SLOT(rsync_with_network_slot()));
}

DateTime::~DateTime()
{
    delete ui;
    delete m_formatsettings;
    delete m_datetimeiface;
    delete m_datetimeiproperties;
}

QString DateTime::get_plugin_name(){
    return pluginName;
}

int DateTime::get_plugin_type(){
    return pluginType;
}

QWidget *DateTime::get_plugin_ui(){
    return pluginWidget;
}

void DateTime::plugin_delay_control(){

}

void DateTime::component_init(){
    ui->titleLabel->setContentsMargins(0,0,0,16);
    ui->timeClockLable->setContentsMargins(0,0,0,16);

//    ui->synsystimeBtn->setStyleSheet("QPushButton{background-color:#E9E9E9;border-radius:4px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white;};border-radius:4px");

    ui->synsystimeBtn->setText(tr("Sync system time"));

//    ui->chgtimebtn->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white;};border-radius:4px");

    ui->chgtimebtn->setText(tr("Change time"));

//    ui->chgzonebtn->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white;};border-radius:4px");
    ui->chgzonebtn->setText(tr("Change time zone"));


    ui->chgLayout->setSpacing(16);

//    ui->hourWidget->setStyleSheet("background-color:#E5E7E9;border-radius:6px");

//    ui->syslabel->setStyleSheet("QLabel#syslabel{background: #3D6BE5;border-radius:4px;}");
    ui->syslabel->setVisible(false);

//    ui->endlabel->setStyleSheet("QLabel#endlabel{background: #3D6BE5;border-radius:4px;}");
    ui->endlabel->setVisible(false);

//    m_formTimeBtn->setChecked(false);

    QHBoxLayout *hourLayout = new QHBoxLayout(ui->hourFrame);

    hourLayout->addWidget(m_formTimeLabel);
    hourLayout->addWidget(m_formTimeBtn);


//    ui->hourwidget->addWidget(formTimeLabel);


    QDateTime currentime = QDateTime::currentDateTime();
    QString timeAndWeek = currentime.toString("yyyy/MM/dd ddd");
    ui->dateLabel->setText(timeAndWeek);


    //因为ntpd和systemd的网络时间同步会有冲突，所以安装了ntp的话，禁止使用控制面板设置网络时间同步
    QFileInfo fileinfo("/usr/sbin/ntpd");
    if (fileinfo.exists()){
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

void DateTime::status_init(){

    //时区
    QDBusReply<QVariant> tz = m_datetimeiface->call("Get", "org.freedesktop.timedate1", "Timezone");
    QMap<QString, int>::iterator it = tzindexMapEn.find(tz.value().toString());
//    qDebug()<<"TODO-------》"<<tz.value().toString()<<endl;
    if(it != tzindexMapEn.end()){
        for(QMap<QString,int>::iterator itc = tzindexMapCN.begin();itc!=tzindexMapCN.end();itc++)
        {
            if(itc.value() == it.value()){
                ui->timezoneLabel->setText(itc.key());
                break;
            }
        }
    } else {
        QMap<QString, int>::iterator defaultit =  tzindexMapEn.find(DEFAULT_TZ);
        ui->timezoneLabel->setText(defaultit.key());
    }


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

void DateTime::datetime_update_slot(){
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

void DateTime::changetime_slot(){
    ChangtimeDialog *dialog = new ChangtimeDialog(m_formTimeBtn->isChecked());
    dialog->setWindowTitle(tr("change time"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_itimer->stop();
    m_itimer->start();
//    dialog->exec();
    dialog->show();
}


void DateTime::changezone_slot(){
//    qDebug()<<"changezone_slot------->"<<endl;

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

void DateTime::changezone_slot(QString zone){
    m_datetimeiface->call("SetTimezone", zone, true);
}

void DateTime::time_format_clicked_slot(bool flag){    
    if (!m_formatsettings){
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }
    QStringList keys = m_formatsettings->keys();
    if (keys.contains("hoursystem")) {
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

void DateTime::showendLabel(){
    ui->syslabel->setVisible(false);
    if(ui->syslabel->isVisible()){
        ui->endlabel->setVisible(false);
    }else {
        ui->endlabel->setVisible(true);
    }
    QTimer::singleShot(2*1000,this,SLOT(hidendLabel()));
}

void DateTime::hidendLabel(){
    ui->endlabel->setVisible(false);
}

void DateTime::rsync_with_network_slot(){
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
