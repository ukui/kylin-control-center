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
#include <QLineEdit>

#include <QTimeZone>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <locale.h>
#include <libintl.h>
#include <QProcess>
#include <sys/timex.h>
#include <qmath.h>
#include <polkit-qt5-1/polkitqt1-authority.h>

const char kTimezoneDomain[] = "installer-timezones";
const char kDefaultLocale[]  = "en_US.UTF-8";

const QString kcnBj =           "北京";
const QString kenBj =           "Asia/Beijing";

#define FORMAT_SCHEMA           "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY         "hoursystem"
#define DATE_KEY                "date"
#define SYNC_TIME_KEY           "synctime"
#define NTP_KEY                 "ntp"
#define TIMEZONES_KEY           "timezones"
#define MAX_TIMES               5

volatile bool syncThreadFlag =  false;

const QStringList ntpAddressList = {
                            "pool.ntp.org",
                            "cn.ntp.org.cn",
                            "cn.pool.ntp.org",
                            "ntp.aliyun.com",

                            "0.debian.pool.ntp.org",
                            "1.debian.pool.ntp.org",

                            "0.arch.pool.ntp.org",
                            "1.arch.pool.ntp.org",

                            "0.fedora.pool.ntp.org",
                            "1.fedora.pool.ntp.org",

                          };

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
    QFont font2;
    font2.setPointSize(font.pointSize() + 3);
    ui->titleLabel->setFont(font2);
    ui->titleLabel_2->setFont(font2);
    ui->titleLabel_2->adjustSize();
    ui->titleLabel_2->setText(tr("Other Timezone"));
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
            Q_UNUSED(key);
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

    initNtp();
    initTimeShow();

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
    QString timeAndWeek = currentime.toString("yyyy/MM/dd  ddd").replace("周","星期");
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


void DateTime::initTimeShow()
{
    QFont font;
    font.setPixelSize(14);
    ui->summaryLabel->setFont(font);
    ui->summaryLabel->setObjectName("summaryText");
    ui->summaryLabel->setText(tr("Add time zones to display the time, up to 5 can be added"));

    addTimeBtn = new QPushButton(ui->addFrame);
    ui->addLayout->addWidget(addTimeBtn);
    addTimeBtn->setText(tr("Add Timezone"));
    addTimeBtn->setFixedHeight(50);

    if (m_formatsettings->keys().contains(TIMEZONES_KEY)) {
        timezonesList = m_formatsettings->get(TIMEZONES_KEY).toStringList();

        int timesNum = timezonesList.size();
        if (timezonesList.size() >= MAX_TIMES) {
            timesNum = MAX_TIMES;
            addTimeBtn->setEnabled(false);
            for (int i = MAX_TIMES; i < timezonesList.size(); ++i) {
                timezonesList.removeLast();
            }
            m_formatsettings->set(TIMEZONES_KEY, timezonesList);
        }
        ui->showFrame->setFixedHeight(timesNum * (50 + 2) + 16 - 2);

        for (int i = 0; i < timesNum; ++i) {
            newTimeshow(timezonesList[i]);
        }
     }

     connect(addTimeBtn, &QPushButton::clicked, this, [=](){
        changeZoneFlag = false;
        changezoneSlot(1);
     });
}

void DateTime::addTimezone(const QString &timezone)
{
    for (int i = 0; i < timezonesList.size(); ++i) {
        if (timezonesList[i] == timezone) {
            return;
        } else if (i == timezonesList.size() - 1) {
            break;
        }
    }
    timezonesList.append(timezone);
    if (timezonesList.size() >= MAX_TIMES) {
        addTimeBtn->setEnabled(false);
    }
     if (m_formatsettings->keys().contains(TIMEZONES_KEY)) {
        m_formatsettings->set(TIMEZONES_KEY, timezonesList);
     }
    ui->showFrame->setFixedHeight(timezonesList.size() * (50 + 2) + 16 - 2);
    newTimeshow(timezone);
}

void DateTime::newTimeshow(const QString &timezone)
{
    HoverWidget *addWgt       = new HoverWidget(timezone);
    QHBoxLayout *addWgtLayout = new QHBoxLayout(addWgt);
    QWidget     *timeWid      = new QWidget(addWgt);
    QHBoxLayout *timeLayout   = new QHBoxLayout(timeWid);
    QPushButton *btn          = new QPushButton(addWgt);
    QLabel      *label_1      = new QLabel(addWgt);    //时间
    QLabel      *label_2      = new QLabel(addWgt);    //日期
    QFont fontTime;
    QGSettings *m_fontSetting = new QGSettings("org.ukui.style");
    fontTime.setFamily(m_fontSetting->get("systemFont").toString());
    fontTime.setPointSize(m_fontSetting->get("systemFontSize").toInt());

    ui->showLayout->addWidget(addWgt);
    addWgt->setParent(ui->showFrame);
    addWgt->setObjectName("addWgt");
    addWgt->setStyleSheet("HoverWidget#addWgt{background: palette(base);}");
    addWgtLayout->setMargin(0);
    addWgtLayout->setSpacing(16);
    addWgt->setMinimumSize(QSize(552, 50));  //552 - 96
    addWgt->setMaximumSize(QSize(960, 50));//960-96
    addWgt->setAttribute(Qt::WA_DeleteOnClose);

    addWgtLayout->addWidget(timeWid);
    timeWid     ->setObjectName("timeWid");
    timeWid     ->setStyleSheet("QWidget#timeWid{background-color: palette(window); border-radius: 4px;}");

    addWgtLayout->addWidget(btn);

    timeLayout->addWidget(label_1);
    timeLayout->addWidget(label_2);
    timeLayout->setSpacing(24);
    label_1->setObjectName("label_1_time");
    label_2->setObjectName("label_2_week");
    timeLayout->addStretch();

    fontTime.setPointSize(fontTime.pointSize() + 2);
    fontTime.setBold(true);
    label_1->setFont(fontTime);

    fontTime.setPointSize(fontTime.pointSize() - 2);
    fontTime.setBold(false);
    label_2->setFont(fontTime);

    QTimeZone thisZone = QTimeZone(timezone.toLatin1().data());
    QDateTime thisZoneTime = QDateTime::currentDateTime().toTimeZone(thisZone);
    QString thisZoneTimeStr ;
    if (m_formTimeBtn->isChecked()) {
        thisZoneTimeStr = thisZoneTime.toString("hh : mm : ss");
    } else {
        thisZoneTimeStr = thisZoneTime.toString("AP hh: mm : ss");
    }

    label_1->setText(thisZoneTimeStr);

    const QString locale = QLocale::system().name();
    QString timeAndWeek = getTimeAndWeek(thisZoneTime);
    label_2->setText(timeAndWeek + "     " + m_zoneinfo->getLocalTimezoneName(timezone, locale));

    btn->setText(tr("Delete"));
    btn->setFixedSize(80,  36);
    btn->hide();

    connect(addWgt, &HoverWidget::enterWidget, this, [=](){
        btn->show();
    });
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](){
        btn->hide();
    });

    connect(btn, &QPushButton::clicked, this, [=, addWgt](){
        timezonesList.removeOne(addWgt->_name);
        if (m_formatsettings->keys().contains(TIMEZONES_KEY)) {
            m_formatsettings->set(TIMEZONES_KEY, timezonesList);
        }
        ui->showFrame->setFixedHeight(timezonesList.size() * (50 + 2) + 16 - 2);
        addWgt->close();
        if (!addTimeBtn->isEnabled() && timezonesList.size() < MAX_TIMES) {
            addTimeBtn->setEnabled(true);
        }
    });

}

void DateTime::initNtp()
{
    QLabel      *ntpLabel  = new QLabel(ui->ntpFrame);
    QHBoxLayout *ntpLayout = new QHBoxLayout(ui->ntpFrame);
                 ntpCombox = new QComboBox(ui->ntpFrame);
    ntpLabel->setFixedWidth(260);
    ui->ntpFrame->setLayout(ntpLayout);
    ntpLayout->addWidget(ntpLabel);
    ntpLayout->addWidget(ntpCombox);
    ntpLabel->setText("Time Server");
    ntpCombox->setFixedHeight(36);
    ntpCombox->addItem(tr("Default"));
    ntpCombox->addItems(ntpAddressList);
    ntpCombox->addItem(tr("Customize"));

    /*自定义*/
    QLabel *ntpLabel_2 = new QLabel(ui->ntpFrame_2);
    QHBoxLayout *ntpLayout_2 = new QHBoxLayout(ui->ntpFrame_2);
    QLineEdit *ntpLineEdit = new QLineEdit();
    QPushButton *saveBtn = new QPushButton(ui->ntpFrame_2);
    ntpLineEdit->setParent(ui->ntpFrame_2);
    ntpLabel_2->setText(tr("Server Address"));
    ntpLayout_2->addWidget(ntpLabel_2);
    ntpLabel_2->setFixedWidth(260);
    ntpLayout_2->addWidget(ntpLineEdit);
    ntpLayout_2->addWidget(saveBtn);
    ntpLineEdit->setPlaceholderText(tr("Required"));
    saveBtn->setText(tr("Save"));

    if (m_formatsettings->keys().contains(NTP_KEY))
        ntpLineEdit->setText(m_formatsettings->get(NTP_KEY).toString());

    connect(ntpLineEdit, &QLineEdit::textChanged, this, [=](){
        saveBtn->setEnabled(!ntpLineEdit->text().isEmpty());   //为空时不允许保存
    });

    connect(saveBtn, &QPushButton::clicked, this, [=](){
        QString setAddr = ntpLineEdit->text();
        if (!setNtpAddr(setAddr)) {   //失败or不修改
            if (m_formatsettings->keys().contains(NTP_KEY))
                ntpLineEdit->setText(m_formatsettings->get(NTP_KEY).toString());
        } else {
            if (m_formatsettings->keys().contains(NTP_KEY))
                m_formatsettings->set(NTP_KEY, setAddr);
        }
    });


    const QString ntpFileName = "/etc/systemd/timesyncd.conf.d/kylin.conf";
    QFile ntpFile(ntpFileName);
    if (!ntpFile.exists()) {    //默认
        ntpCombox->setCurrentIndex(0);
        ui->ntpFrame_2->setVisible(false);
    } else {
        QSettings readFile(ntpFileName, QSettings::IniFormat);
        QString initAddress = readFile.value("Time/NTP").toString();
        for (int i = 0; i < ntpCombox->count(); ++i) {
            if (initAddress == ntpCombox->itemText(i)) {   //是选中的
                ntpCombox->setCurrentIndex(i);
                ui->ntpFrame_2->setVisible(false);
                break;
            } else if (i == ntpCombox->count() - 1) {     //是自定义的
                ntpCombox->setCurrentIndex(i);
                ntpLineEdit->setText(initAddress);
                ui->ntpFrame_2->setVisible(true);
                break;
            }
        }
    }

    ntpComboxPreId = ntpCombox->currentIndex();

    connect(ntpCombox, &QComboBox::currentTextChanged, this, [=](){
        QString setAddr;
        if (m_formatsettings->keys().contains(NTP_KEY))
            setAddr = m_formatsettings->get(NTP_KEY).toString();
        if (ntpCombox->currentIndex() == (ntpCombox->count() - 1) && setAddr == "") { //自定义且为空
            ui->ntpFrame_2->setVisible(true);  //需要添加地址并点击保存再授权
        } else {
            if (ntpCombox->currentIndex() == 0) {  //默认
                setAddr = "default";
                ui->ntpFrame_2->setVisible(false);
            } else if (ntpCombox->currentIndex() != ntpCombox->count() - 1) { //选择系统
                setAddr = ntpCombox->currentText();
                ui->ntpFrame_2->setVisible(false);
            } else { //自定义且不为空
                ui->ntpFrame_2->setVisible(true);
            }
            if (!setNtpAddr(setAddr)) {   //失败or不修改
                ntpCombox->blockSignals(true);
                ntpCombox->setCurrentIndex(ntpComboxPreId);
                ntpCombox->blockSignals(false);
                if (ntpComboxPreId == ntpCombox->count() - 1) {
                    ui->ntpFrame_2->setVisible(true);
                } else {
                    ui->ntpFrame_2->setVisible(false);
                }
            } else {
                ntpComboxPreId = ntpCombox->currentIndex();
                if (m_formatsettings->keys().contains(NTP_KEY)) //防止未保存的内容一直存在
                    ntpLineEdit->setText(m_formatsettings->get(NTP_KEY).toString());
            }
        }
    });
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
    setCurrentTime();
    QString timeAndWeek = getTimeAndWeek(current);
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

void DateTime::changezoneSlot(int flag)
{
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = m_timezone->width();
    int y = m_timezone->height();
    m_timezone->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
    if (flag == 1) {
        m_timezone->setTitle(tr("Add Timezone"));
    } else {
        m_timezone->setTitle(tr("Change Timezone"));
    }
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

        } else {
            setNtpFrame(false);
        }
    }
}

QString  DateTime::getTimeAndWeek(const QDateTime timeZone)
{
    QString dateformat;
    if (m_formatsettings) {
        QStringList keys = m_formatsettings->keys();
        if(keys.contains("date")) {
            dateformat = m_formatsettings->get(DATE_KEY).toString();
        }
    }
    QString timeAndWeek;
    if ("cn" == dateformat) {
       timeAndWeek = timeZone.toString("yyyy/MM/dd  ddd").replace("周","星期");
    } else {
       timeAndWeek = timeZone.toString("yyyy-MM-dd  ddd");
    }
    return timeAndWeek;
}

void DateTime::setCurrentTimeOthers()
{
    for (QObject *obj : ui->showFrame->children()) {
        if (obj->objectName() == "addWgt") {
            HoverWidget *addWgt = static_cast<HoverWidget*>(obj);
            QTimeZone thisZone     = QTimeZone(addWgt->_name.toLatin1().data());
            QDateTime thisZoneTime = QDateTime::currentDateTime().toTimeZone(thisZone);
            for (QObject *objTime : addWgt->children()) {
                if (objTime->objectName() == "timeWid") {
                    QWidget *timeWid = static_cast<QWidget*>(objTime);
                    for (QObject *objLabel : timeWid->children()){
                        if (objLabel->objectName() == "label_1_time") {
                            QString currentsecStr ;
                            if (m_formTimeBtn->isChecked()) {
                                currentsecStr = thisZoneTime.toString("hh : mm : ss");
                            } else {
                                currentsecStr = thisZoneTime.toString("AP hh: mm : ss");
                            }
                            QLabel *label_1 = static_cast<QLabel*>(objLabel);
                            label_1->setText(currentsecStr);
                        } else if(objLabel->objectName() == "label_2_week") {
                            QLabel *label_2 = static_cast<QLabel*>(objLabel);
                            QString timeAndWeek  = getTimeAndWeek(thisZoneTime);
                            label_2->setText(timeAndWeek + "     " + m_zoneinfo->getLocalTimezoneName(addWgt->_name, QLocale::system().name()));
                        }
                    }
                }
            }
        }
    }
}


void DateTime::setCurrentTime()
{
    setCurrentTimeOthers();
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
    connect(ui->chgzonebtn, &QPushButton::clicked, this, [=](){
        changeZoneFlag = true;
        changezoneSlot();
    });

    connect(m_formTimeBtn, &SwitchButton::checkedChanged, this, [=](bool status) {
        timeFormatClickedSlot(status, false);
    });

    connect(syncTimeBtn, &SwitchButton::checkedChanged, this,[=](bool status) {
        synctimeFormatSlot(status,true); //按钮被改变，需要修改
    });


    connect(m_timezone, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        if (changeZoneFlag) {
            changezoneSlot(timezone);
            const QString locale = QLocale::system().name();
            localizedTimezone = m_zoneinfo->getLocalTimezoneName(timezone, locale);
        } else {
            addTimezone(timezone);
        }
        m_timezone->hide();
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
        setNtpFrame(true);
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
        setNtpFrame(false);
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

bool DateTime::setNtpAddr(QString address)
{
    PolkitQt1::Authority::Result result;
    result = PolkitQt1::Authority::instance()->checkAuthorizationSync(
                "org.control.center.qt.systemdbus.action.ntp",
                PolkitQt1::UnixProcessSubject(QCoreApplication::applicationPid()),
                PolkitQt1::Authority::AllowUserInteraction);

    if (result == PolkitQt1::Authority::Yes) {
        QDBusInterface *setNtpAddrDbus = new QDBusInterface("com.control.center.qt.systemdbus",
                                                                 "/",
                                                                 "com.control.center.interface",
                                                                 QDBusConnection::systemBus());

        QDBusReply<bool> reply = setNtpAddrDbus->call("setNtpSerAddress", address);
        delete setNtpAddrDbus;
        setNtpAddrDbus = nullptr;
        return reply;
    }
    return false;
}

void DateTime::setNtpFrame(bool visiable)
{
    ui->ntpFrame->setVisible(visiable);
    if (visiable && ntpCombox != nullptr) {
        ui->ntpFrame_2->setVisible(ntpCombox->currentIndex() == ntpCombox->count() - 1);
    } else {
        ui->ntpFrame_2->setVisible(visiable);
    }
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
    for(qint8 i = 0; i < 80; ++i) {
        struct timex txc = {};
        if (adjtimex(&txc) < 0 || txc.maxerror >= 16000000) {  //未能同步时间
            int picNum = i - qFloor(i/8)*8; //限制在0~7
            QString pixName = QString(":/img/plugins/upgrade/loading%1.svg").arg(picNum+10);
            QPixmap pix(pixName);
            this->dataTimeUI->syncTimeBtn->setEnabled(false);
            qApp->processEvents();
            this->dataTimeUI->syncNetworkRetLabel->setPixmap(pix);
            msleep(70);
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
