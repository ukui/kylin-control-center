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
#include "dataformat.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QGSettings>
#include <KMessageBox>

#define PANEL_GSCHEMAL   "org.ukui.control-center.panel.plugins"
#define CALENDAR_KEY     "calendar"
#define DAY_KEY          "firstday"
#define DATE_FORMATE_KEY "date"
#define TIME_KEY         "hoursystem"

const QVector<QString> CFormats{"zh_SG.UTF-8", "zh_CN.UTF-8", "lt_LT.UTF-8", "en_ZW.UTF-8", "en_ZM.UTF-8",
                               "en_ZA.UTF-8", "en_US.UTF-8", "en_SG.UTF-8", "en_PH.UTF-8", "en_NZ.UTF-8",
                               "en_NG.UTF-8", "en_IN.UTF-8", "en_IL.UTF-8", "en_IE.UTF-8", "en_HK.UTF-8",
                               "en_GB.UTF-8", "en_DK.UTF-8", "en_CA.UTF-8", "en_BW.UTF-8", "en_AU.UTF-8",
                               "en_AG.UTF-8", "af_ZA.UTF-8"};

Area::Area()
{
    ui = new Ui::Area;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
//    pluginWidget->setStyleSheet("background: #ffffff;");
    ui->setupUi(pluginWidget);

    pluginName = tr("Area");
    pluginType = DATETIME;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title3Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");


    const QByteArray id(PANEL_GSCHEMAL);

    if(QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id);
        //  监听key的value是否发生了变化
        /*
        connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key) {
            initFormatData();
        });
        */
    }

    unsigned int uid = getuid();
    objpath = objpath +"/org/freedesktop/Accounts/User"+QString::number(uid);


    m_areaInterface = new QDBusInterface("org.freedesktop.Accounts",
                                         objpath,
                                         "org.freedesktop.Accounts.User",
                                         QDBusConnection::systemBus());

    m_itimer = new QTimer();
    m_itimer->start(1000);
    connect(m_itimer,SIGNAL(timeout()), this, SLOT(datetime_update_slot()));

    initUI();
    initComponent();
    connect(ui->langcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(change_language_slot(int)));
    connect(ui->countrycomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(change_area_slot(int)));
//    connect(ui->addlanBtn, SIGNAL(clicked()), this, SLOT(add_lan_btn_slot()));
    connect(ui->chgformButton,SIGNAL(clicked()),this,SLOT(changeform_slot()));

//    connect(ui->langcomboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
//            [=]{
//        KMessageBox::information(ui->languageframe, tr("Need to cancel to take effect"));
//    });

    connect(ui->countrycomboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]{
        KMessageBox::information(ui->languageframe_2, tr("Need to log off to take effect"));
    });

}

Area::~Area()
{
    delete ui;
    delete m_itimer;
}

QString Area::get_plugin_name(){
    return pluginName;
}

int Area::get_plugin_type(){
    return pluginType;
}

QWidget *Area::get_plugin_ui(){
    return pluginWidget;
}

void Area::plugin_delay_control(){

}

void Area::run_external_app_slot(){
    QString cmd = "gnome-language-selector";

    QProcess process(this);
    process.startDetached(cmd);
}

void Area::initUI(){
    ui->titleLabel->setText(tr("current area"));
    ui->countrylabel->setText(tr("display format area"));
    ui->title2Label->setText(tr("format of area"));
    ui->calendarlabel->setText(tr("calendar"));
    ui->weeklabel->setText(tr("first day of week"));
    ui->datelabel->setText(tr("date"));
    ui->timelabel->setText(tr("time"));
    ui->chgformButton->setText(tr("change format of data"));
    ui->title3Label->setText(tr("first language"));
    ui->languagelabel->setText(tr("system language"));

    ui->countrycomboBox->addItem(tr("US"));
    ui->countrycomboBox->addItem(tr("CN"));
    ui->langcomboBox->addItem(tr("English"));
    ui->langcomboBox->addItem(tr("Chinese"));

    addWgt = new HoverWidget("");
    addWgt->setObjectName(tr("addwgt"));
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add main language"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });
    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        add_lan_btn_slot();
    });

    ui->addLyt->addWidget(addWgt);
}

void Area::initComponent() {
    QStringList res = getUserDefaultLanguage();
    QString lang = res.at(1);
    int langIndex = lang.split(':').at(0) == "en_US" ? 0 : 1;;
    int formatIndex = res.at(0) == "en_US.UTF-8" ? 0 : 1;
    ui->langcomboBox->setCurrentIndex(langIndex);
    ui->countrycomboBox->setCurrentIndex(formatIndex);

    initFormatData();
}


void Area::initFormatData() {

    QString locale = QLocale::system().name();
    if (!m_gsettings) {
        return ;
    }
    const QStringList list = m_gsettings->keys();

    if (!list.contains("calendar") || !list.contains("firstday")){
        return ;
    }

    if ( "zh_CN" != locale) {
        m_gsettings->set(CALENDAR_KEY, "solarlunar");
    }
    QString clac = m_gsettings->get(CALENDAR_KEY).toString();
    if ("lunar" == clac) {
        ui->Lunarcalendar->setText(tr("lunar"));
    } else {
        ui->Lunarcalendar->setText(tr("solar calendar"));
    }

    QString day = m_gsettings->get(DAY_KEY).toString();
    if ("monday" == day) {
        ui->firstDayLabel->setText(tr("monday"));
    } else {
        ui->firstDayLabel->setText(tr("sunday"));
    }

    QDateTime current = QDateTime::currentDateTime();
    QString currentsecStr  ;
    QString dateFormat = m_gsettings->get(DATE_FORMATE_KEY).toString();
    if ("cn" == dateFormat) {
       currentsecStr = current.toString("yyyy/MM/dd ");;
    } else {
       currentsecStr = current.toString("yyyy-MM-dd ");
    }
    ui->datelabelshow->setText(currentsecStr);

    this->hourformat = m_gsettings->get(TIME_KEY).toString();
}

void Area::change_language_slot(int index){
    QDBusReply<bool> res;
    switch (index) {
    case 0:
        res = m_areaInterface->call("SetLanguage","en_US");
        break;
    case 1:
        res = m_areaInterface->call("SetLanguage","zh_CN");
        break;
    }
    ui->countrycomboBox->setCurrentIndex(index);
}

void Area::change_area_slot(int index){
    qDebug()<<"area----------->"<<endl;
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

void Area::datetime_update_slot() {

    QDateTime current = QDateTime::currentDateTime();
    QString timeStr;
    if ("24" == this->hourformat) {
        timeStr = current.toString("hh: mm : ss");
    } else {
        timeStr = current.toString("AP hh: mm : ss");
    }
    ui->timelabelshow->setText(timeStr);
}

void Area::add_lan_btn_slot() {
    QString cmd = "gnome-language-selector";

    QProcess process(this);
    process.startDetached(cmd);
}

void Area::changeform_slot() {
//    DataFormat *dialog = new DataFormat();
//    connect(dialog, SIGNAL(dataChangedSignal()),this,SLOT(initFormatData()));
//    dialog->setWindowTitle(tr("change data format"));
//    dialog->setAttribute(Qt::WA_DeleteOnClose);
//    dialog->exec();
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
    int pos = 0;
    QString formats;
    QString language;
    QStringList filestr;
    QStringList result;
    QString fname = getenv("HOME");
    fname += "/.pam_environment";

    filestr = this->readFile(fname);
//    qDebug()<<"result is------>"<<filestr<<endl;
    QRegExp re("LANGUAGE(\t+DEFAULT)?=(.*)$");
    for(int i = 0; i < filestr.length(); i++) {
        while((pos = re.indexIn(filestr.at(i), pos)) != -1) {
            language = re.cap(2);
            pos += re.matchedLength();
        }
    }

    QDBusInterface * iproperty = new QDBusInterface("org.freedesktop.Accounts",
                                            objpath,
                                            "org.freedesktop.DBus.Properties",
                                            QDBusConnection::systemBus());
    QDBusReply<QMap<QString, QVariant> > reply = iproperty->call("GetAll", "org.freedesktop.Accounts.User");
    if (reply.isValid()){
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        formats = propertyMap.find("FormatsLocale").value().toString();
        if(language.isEmpty()) {
            language = propertyMap.find("Language").value().toString();
        }
    } else {
        qDebug() << "reply failed";
    }
    result.append(formats);
    result.append(language);
//    qDebug()<<"result is---------->"<<result<<endl;
    return result;
}
