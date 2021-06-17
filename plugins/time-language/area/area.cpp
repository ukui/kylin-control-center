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
#include "area.h"
#include "ui_area.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QGSettings>
#include <QMessageBox>
#include "languageFrame.h"

#define PANEL_GSCHEMAL   "org.ukui.control-center.panel.plugins"
#define CALENDAR_KEY     "calendar"
#define DAY_KEY          "firstday"
#define DATE_FORMATE_KEY "date"
#define TIME_KEY         "hoursystem"


Area::Area() : mFirstLoad(true)
{
    pluginName = tr("Area");
    pluginType = DATETIME;
}

Area::~Area()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

void Area::cloudChangedSlot(const QString &key) {
    if(key == "area") {
        initComponent();
    }
}

void Area::connectToServer(){
    cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!cloudInterface->isValid())
    {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
//    QDBusConnection::sessionBus().connect(cloudInterface, SIGNAL(shortcutChanged()), this, SLOT(shortcutChangedSlot()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(cloudChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

QString Area::get_plugin_name() {
    return pluginName;
}

int Area::get_plugin_type() {
    return pluginType;
}

QWidget *Area::get_plugin_ui() {
    if (mFirstLoad) {

        mFirstLoad = false;

        ui = new Ui::Area;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        ui->countrylabel->adjustSize();
        ui->formframe->adjustSize();
        ui->formframe->setObjectName("formframe");
        ui->formframe->setStyleSheet("QFrame#formframe{background-color: palette(base);}");

        const QByteArray id(PANEL_GSCHEMAL);

        if(QGSettings::isSchemaInstalled(id)) {
            m_gsettings = new QGSettings(id, QByteArray(), pluginWidget);
            mDateFormat = m_gsettings->get(DATE_FORMATE_KEY).toString();
        }

        unsigned int uid = getuid();
        objpath = objpath +"/org/freedesktop/Accounts/User"+QString::number(uid);


        m_areaInterface = new QDBusInterface("org.freedesktop.Accounts",
                                             objpath,
                                             "org.freedesktop.Accounts.User",
                                             QDBusConnection::systemBus());

        initUI();
        initFormFrame();
        initComponent();
        connectToServer();
        initConnect();
    }
    return pluginWidget;
}

void Area::plugin_delay_control() {

}

const QString Area::name() const {
    return QStringLiteral("area");

}

void Area::run_external_app_slot() {
    QString cmd = "gnome-language-selector";

    QProcess process(this);
    process.startDetached(cmd);
}

void Area::initUI() {
    //~ contents_path /area/Regional Format
    ui->titleLabel->setText(tr("Regional Format"));
    //~ contents_path /area/first language
    ui->title3Label->setText(tr("first language"));

    ui->summaryLabel->setText(tr("Language for system windows,menus and web pages"));
    ui->summaryLabel->setVisible(true);

    initLanguage();

    addWgt = new HoverWidget("");
    addWgt->setObjectName(tr("addwgt"));
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    QPalette pal;
    QBrush brush = pal.highlight();  //获取window的色值
    QColor highLightColor = brush.color();
    QString stringColor = QString("rgba(%1,%2,%3)") //叠加20%白色
           .arg(highLightColor.red()*0.8 + 255*0.2)
           .arg(highLightColor.green()*0.8 + 255*0.2)
           .arg(highLightColor.blue()*0.8 + 255*0.2);

    addWgt->setStyleSheet(QString("HoverWidget#addwgt{background: palette(button);\
                                   border-radius: 4px;}\
                                   HoverWidget:hover:!pressed#addwgt{background: %1;  \
                                   border-radius: 4px;}").arg(stringColor));

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add main language"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    iconLabel->setProperty("useIconHighlightEffect", true);
    iconLabel->setProperty("iconHighlightEffectMode", 1);

    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname) {
        Q_UNUSED(mname);
        add_lan_btn_slot();
    });

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](){

        iconLabel->setProperty("useIconHighlightEffect", false);
        iconLabel->setProperty("iconHighlightEffectMode", 0);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: white;");
    });

    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](){

        iconLabel->setProperty("useIconHighlightEffect", true);
        iconLabel->setProperty("iconHighlightEffectMode", 1);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    ui->addBtnLayout->addWidget(addWgt);
}

void Area::initLanguage()
{
    LanguageFrame *chineseFrame = new LanguageFrame(tr("Simplified Chinese"));
    LanguageFrame *englishFrame = new LanguageFrame(tr("English"));

    QStringList res = getUserDefaultLanguage();
    QString lang = res.at(1);
    int langIndex = lang.split(':').at(0) == "zh_CN" ? 0 : 1;

    if (0 == langIndex) {
        chineseFrame->showSelectedIcon(true);
        englishFrame->showSelectedIcon(false);
    } else {
        chineseFrame->showSelectedIcon(false);
        englishFrame->showSelectedIcon(true);
    }

    ui->addLyt->addWidget(chineseFrame);
    ui->addLyt->addWidget(englishFrame);

    connect(chineseFrame, &LanguageFrame::clicked, this, [=](){
        englishFrame->showSelectedIcon(false);
        m_areaInterface->call("SetLanguage","zh_CN");
        QMessageBox::information(pluginWidget, tr("Message"),tr("Need to log off to take effect"));
    });

    connect(englishFrame, &LanguageFrame::clicked, this, [=](){
        chineseFrame->showSelectedIcon(false);
        m_areaInterface->call("SetLanguage","en_US");
        QMessageBox::information(pluginWidget, tr("Message"),tr("Need to log off to take effect"));
    });
}

void Area::initComponent() {
    ui->countrycomboBox->addItem(tr("US"));
    ui->countrycomboBox->addItem(tr("CN"));

    QStringList res = getUserDefaultLanguage();
    QString lang = res.at(1);
    int langIndex = lang.split(':').at(0) == "zh_CN" ? 1 : 0;
    int formatIndex = res.at(0) == "zh_CN.UTF-8" ? 1 : 0;
   // ui->langcomboBox->setCurrentIndex(langIndex);
    ui->countrycomboBox->setCurrentIndex(formatIndex);

    initFormComponent(0b1111);
}

void Area::initFormComponent(int8_t value)
{
    const QStringList list = m_gsettings->keys();

    if (!list.contains(CALENDAR_KEY) || !list.contains(DAY_KEY)
            || !list.contains(DATE_FORMATE_KEY) || !list.contains(TIME_KEY)){
        return ;
    }

    if (value >> 0 & 1) {
        QString clac = m_gsettings->get(CALENDAR_KEY).toString();
        if ("solarlunar" == clac) {
            ui->calendarBox->setCurrentIndex(0);
        } else {
            ui->calendarBox->setCurrentIndex(1);
        }
    }

    if (value >> 1 & 1) {
        QString day = m_gsettings->get(DAY_KEY).toString();
        if ("monday" == day) {
            ui->dayBox->setCurrentIndex(0);
        } else {
            ui->dayBox->setCurrentIndex(1);
        }
    }

    if (value >> 2 & 1) {
        QString dateFormat = m_gsettings->get(DATE_FORMATE_KEY).toString();
        if ("cn" == dateFormat) {
           ui->dateBox->setCurrentIndex(0);
        } else {
           ui->dateBox->setCurrentIndex(1);
        }
    }

    if (value >> 3 & 1) {
        QString hourFormat = m_gsettings->get(TIME_KEY).toString();
        if ("24" == hourFormat) {
            ui->timeBox->setCurrentIndex(1);
        } else {
            ui->timeBox->setCurrentIndex(0);
        }
    }
}

void Area::change_area_slot(int index) {
    QDBusReply<bool> res;
    switch (index) {
    case 0:
        res = m_areaInterface->call("SetFormatsLocale","en_US.UTF-8");
        break;
    case 1:
        res = m_areaInterface->call("SetFormatsLocale","zh_CN.UTF-8");
        break;
    }
}

void Area::add_lan_btn_slot() {
    QString cmd = "gnome-language-selector";

    QProcess process(this);
    process.startDetached(cmd);
}

QStringList Area::readFile(const QString& filepath) {
    QStringList res;
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
            res<<line;
        }
        file.close();
        return res;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return QStringList();
    }
}


QStringList Area::getUserDefaultLanguage() {
    QString formats;
    QString language;
    QStringList result;

    QDBusInterface iproperty("org.freedesktop.Accounts",
                             objpath,
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusReply<QMap<QString, QVariant> > reply = iproperty.call("GetAll", "org.freedesktop.Accounts.User");
    if (reply.isValid()){
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        if (propertyMap.keys().contains("FormatsLocale")) {
            formats = propertyMap.find("FormatsLocale").value().toString();
        }
        if(language.isEmpty() && propertyMap.keys().contains("Language")) {
            language = propertyMap.find("Language").value().toString();
        }
    } else {
        qDebug() << "reply failed";
    }
    result.append(formats);
    result.append(language);
    return result;
}


void Area::initFormFrame()
{
    //~ contents_path /area/Current Region
    ui->countrylabel->setText(tr("Current Region"));
    //~ contents_path /area/Calendar
    ui->calendarLabel->setText(tr("Calendar"));
    //~ contents_path /area/First Day Of Week
    ui->dayLabel->setText(tr("First Day Of Week"));
    //~ contents_path /area/Date
    ui->dateLabel->setText(tr("Date"));
    //~ contents_path /area/Time
    ui->timelabel->setText(tr("Time"));

    ui->calendarBox->addItem(tr("solar calendar"));
    QString locale = QLocale::system().name();
    if ("zh_CN" == locale){
        ui->calendarBox->addItem(tr("lunar"));
    }

    ui->dayBox->addItem(tr("monday"));
    ui->dayBox->addItem(tr("sunday"));

    QString currentsecStr;
    QDateTime current = QDateTime::currentDateTime();

    currentsecStr = current.toString("yyyy/MM/dd ");
    ui->dateBox->addItem(currentsecStr);

    currentsecStr = current.toString("yyyy-MM-dd ");
    ui->dateBox->addItem(currentsecStr);

    ui->timeBox->addItem(tr("12 Hours"));
    ui->timeBox->addItem(tr("24 Hours"));
}

void Area::initConnect()
{
    connect(ui->countrycomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(change_area_slot(int)));
    connect(ui->countrycomboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]{
        QMessageBox::information(pluginWidget, tr("Message"),tr("Need to log off to take effect"));
    });
    connect(ui->timeBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]() {
        bool flag_24;
        if (0 == ui->timeBox->currentIndex()) {
            flag_24 = false;
        } else {
            flag_24 = true;
        }
        timeFormatClicked(flag_24);
    });

    connect(ui->dayBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]{
        QString dayValue;
        if (0 == ui->dayBox->currentIndex()) {
            dayValue = "monday";
        } else {
            dayValue = "sunday";
        }
        writeGsettings(DAY_KEY, dayValue);
    });

    connect(ui->calendarBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]{
        QString calendarValue;
        if ( 0 == ui->calendarBox->currentIndex()) {
            calendarValue = "solarlunar";
        } else {
            calendarValue = "lunar";
        }
        writeGsettings(CALENDAR_KEY, calendarValue);
    });

    connect(ui->dateBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]{
        QString dateValue;
        if ( 0 == ui->dateBox->currentIndex()) {
            dateValue = "cn";
        } else {
            dateValue = "en";
        }
        writeGsettings(DATE_FORMATE_KEY, dateValue);
    });

    connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
        if (key == CALENDAR_KEY ) {
            ui->calendarBox->blockSignals(true);
            initFormComponent(0b0001);
            ui->calendarBox->blockSignals(false);
        } else if(key == DAY_KEY ) {
            ui->dayBox->blockSignals(true);
            initFormComponent(0b0010);
            ui->dayBox->blockSignals(false);
        } else if(key == DATE_FORMATE_KEY) {
            ui->dateBox->blockSignals(true);
            initFormComponent(0b0100);
            ui->dateBox->blockSignals(false);
        } else if(key == TIME_KEY) {
            ui->timeBox->blockSignals(true);
            initFormComponent(0b1000);
            ui->timeBox->blockSignals(false);
        }
    });

}

void Area::writeGsettings(const QString &key, const QString &value) {
    if(!m_gsettings) {
        return ;
    }

    const QStringList list = m_gsettings->keys();
    if (!list.contains(key)) {
        return ;
    }
    m_gsettings->set(key,value);
}

void Area::timeFormatClicked(bool flag)
{
    if (!m_gsettings) {
        qDebug()<<"org.ukui.control-center.panel.plugins not installed"<<endl;
        return;
    }
    QStringList keys = m_gsettings->keys();
    if (keys.contains(TIME_KEY)) {
        if (flag == true) {
            m_gsettings->set(TIME_KEY, "24");
        } else {
            m_gsettings->set(TIME_KEY, "12");
        }
    }
}
