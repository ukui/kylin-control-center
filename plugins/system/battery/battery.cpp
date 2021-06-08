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
#include "battery.h"
#include "../power/powermacrodata.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QSettings>
#include <QButtonGroup>
#include <QPushButton>
#include <QWidget>
#include <QFormLayout>


#include "../../../shell/utils/utils.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

#include "libupower-glib/upower.h"




// 低电量操作
const QStringList kBattery    { QObject::tr("nothing"), QObject::tr("blank"), QObject::tr("suspend"), QObject::tr("hibernate"), QObject::tr("shutdown") };
const QStringList kEnBattery  { "nothing", "blank", "suspend", "hibernate", "shutdown" };



Battery::Battery() : mFirstLoad(true)
{
    pluginName = tr("Battery");
    pluginType = SYSTEM;
}

Battery::~Battery()
{

}



QString Battery::get_plugin_name() {
    return pluginName;
}

int Battery::get_plugin_type() {
    return pluginType;
}

QWidget * Battery::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new QWidget();
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);

        const QByteArray styleID(STYLE_FONT_SCHEMA);
        const QByteArray id(POWERMANAGER_SCHEMA);
        const QByteArray iid(SESSION_SCHEMA);


        if (QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(styleID) && QGSettings::isSchemaInstalled(iid)) {
            settings = new QGSettings(id, QByteArray(), this);
            stylesettings = new QGSettings(styleID, QByteArray(), this);
            sessionsettings = new QGSettings(iid, QByteArray(), this);
            idletime = sessionsettings->get(IDLE_DELAY_KEY).toInt();
            connect(sessionsettings,&QGSettings::changed,[=](QString key)
            {
                if("idle-delay" == key)
                {
                    idletime = sessionsettings->get(IDLE_DELAY_KEY).toInt();
                    retranslateUi();

                }
            });
            connect(stylesettings,&QGSettings::changed,[=](QString key)
            {
                if("systemFont" == key || "systemFontSize" == key)
                {
                    retranslateUi();

                }
            });
            InitUI(pluginWidget);
            initSearText();
            isHibernateSupply();
            initTitleLabel();

            setupComponent();
            initStatus();
            setupConnect();
        } else {
            qCritical() << POWERMANAGER_SCHEMA << "not installed!\n";
        }


    }
    return pluginWidget;
}

void Battery::plugin_delay_control() {

}

const QString Battery::name() const {

    return QStringLiteral("battery");
}

void Battery::InitUI(QWidget *battery)
{
    QVBoxLayout *mverticalLayout = new QVBoxLayout(battery);
    mverticalLayout->setSpacing(0);
    mverticalLayout->setObjectName(QString::fromUtf8("mverticalLayout"));
    mverticalLayout->setContentsMargins(0, 0, 32, 200);

    QWidget *batterywidget = new QWidget(battery);
    batterywidget->setMinimumSize(QSize(550, 0));
    batterywidget->setMaximumSize(QSize(960, 16777215));


    QVBoxLayout *BatteryLayout = new QVBoxLayout(batterywidget);
    BatteryLayout->setContentsMargins(0, 0, 0, 0);
    BatteryLayout->addSpacing(8);

    BatterytitleLabel = new QLabel(batterywidget);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(BatterytitleLabel->sizePolicy().hasHeightForWidth());
    BatterytitleLabel->setSizePolicy(sizePolicy);
    BatterytitleLabel->setScaledContents(true);

    BatteryLayout->addWidget(BatterytitleLabel);

    mSleepFrame = new QFrame(batterywidget);
    mSleepFrame->setMinimumSize(QSize(550, 88));
    mSleepFrame->setMaximumSize(QSize(960, 88));
    mSleepFrame->setFrameShape(QFrame::Box);

    QFormLayout *msleepLayout = new QFormLayout(mSleepFrame);
    msleepLayout->setContentsMargins(16, 0, 16, 0);

    msleepLabel = new QLabel(mSleepFrame);
    msleepLabel->setMinimumSize(QSize(182, 88));
    msleepLabel->setMaximumSize(QSize(182, 88));

    QStringList mSleepTime;
    mSleepTime<< tr("10m") << tr("20m") << tr("30m") << tr("1h") << tr("2h")
              <<tr("never");

    sleepuslider  = new Uslider(mSleepTime);
    sleepuslider->setOrientation(Qt::Horizontal);
    sleepuslider->setRange(1, 6);
    sleepuslider->setTickInterval(1);
    sleepuslider->setPageStep(1);


    msleepLayout->addRow(msleepLabel,sleepuslider);
    msleepLayout->setHorizontalSpacing(50);

    BatteryLayout->addWidget(mSleepFrame);

    mCloseFrame = new QFrame(batterywidget);
    mCloseFrame->setMinimumSize(QSize(550, 88));
    mCloseFrame->setMaximumSize(QSize(960, 88));
    mCloseFrame->setFrameShape(QFrame::Box);

    QFormLayout *mCloseLayout = new QFormLayout(mCloseFrame);
    mCloseLayout->setContentsMargins(16, 0, 16, 0);


    mCloseLabel = new QLabel(mCloseFrame);
    mCloseLabel->setMinimumSize(QSize(182, 88));
    mCloseLabel->setMaximumSize(QSize(182, 88));

    QStringList mCloseTime;
    mCloseTime<< tr("5m") << tr("10m") << tr("30m") << tr("1h") << tr("2h")
              <<tr("never");

    CloseUslider  = new Uslider(mCloseTime);
    CloseUslider->setRange(1,6);
    CloseUslider->setTickInterval(1);
    CloseUslider->setPageStep(1);

    mCloseLayout->addRow(mCloseLabel,CloseUslider);
    mCloseLayout->setHorizontalSpacing(50);

    BatteryLayout->addWidget(mCloseFrame);

    mDarkenFrame = new QFrame(batterywidget);
    mDarkenFrame->setMinimumSize(QSize(550, 88));
    mDarkenFrame->setMaximumSize(QSize(960, 88));
    mDarkenFrame->setFrameShape(QFrame::Box);

    QFormLayout *mDarkenLayout = new QFormLayout(mDarkenFrame);
    mDarkenLayout->setContentsMargins(16, 0, 16, 0);


    mDarkenLabel = new QLabel(mDarkenFrame);
    mDarkenLabel->setMinimumSize(QSize(182, 88));
    mDarkenLabel->setMaximumSize(QSize(182, 88));

    QStringList mDarkenTime;
    mDarkenTime<< tr("1m") << tr("3m") << tr("5m") << tr("15m") << tr("30m")
              <<tr("1h");

    DarkenUslider  = new Uslider(mDarkenTime);
    DarkenUslider->setRange(1,6);
    DarkenUslider->setTickInterval(1);
    DarkenUslider->setPageStep(1);

    mDarkenLayout->addRow(mDarkenLabel,DarkenUslider);
    mDarkenLayout->setHorizontalSpacing(50);

    BatteryLayout->addWidget(mDarkenFrame);

    mLowpowerFrame = new QFrame(batterywidget);
    mLowpowerFrame->setMinimumSize(QSize(0, 60));
    mLowpowerFrame->setMaximumSize(QSize(16777215, 60));
    mLowpowerFrame->setFrameShape(QFrame::Box);

    QHBoxLayout *mLowpowerLayout = new QHBoxLayout(mLowpowerFrame);
    mLowpowerLayout->setContentsMargins(16, 0, 16, 0);
    mLowpowerLayout->setSpacing(8);

    mLowpowerLabel1 =new QLabel(mLowpowerFrame);
    mLowpowerLabel1->setMinimumSize(QSize(130, 60));
    mLowpowerLabel1->setMaximumSize(QSize(130, 60));

    mLowpowerLayout->addWidget(mLowpowerLabel1);

    mLowpowerComboBox1 = new QComboBox(mLowpowerFrame);
    mLowpowerComboBox1->setMinimumSize(QSize(70, 36));
    mLowpowerComboBox1->setMaximumSize(QSize(70, 36));
    mLowpowerComboBox1->setStyleSheet("QComboBox{background-color: palette(button);}");

    mLowpowerLayout->addWidget(mLowpowerComboBox1);

    mLowpowerLabel2 =new QLabel(mLowpowerFrame);
    mLowpowerLabel2->setMinimumSize(QSize(72, 60));
    mLowpowerLabel2->setMaximumSize(QSize(72, 60));

    mLowpowerLayout->addWidget(mLowpowerLabel2);

    mLowpowerComboBox2 = new QComboBox(mLowpowerFrame);
    mLowpowerComboBox2->setMinimumSize(QSize(150, 36));
    mLowpowerComboBox2->setMaximumSize(QSize(150, 36));
    mLowpowerComboBox2->setStyleSheet("QComboBox{background-color: palette(button);}");

    mLowpowerLayout->addWidget(mLowpowerComboBox2);

    mLowpowerLayout->addStretch();

    BatteryLayout->addWidget(mSleepFrame);
    BatteryLayout->addWidget(mCloseFrame);
    BatteryLayout->addWidget(mDarkenFrame);
    BatteryLayout->addWidget(mLowpowerFrame);

    mverticalLayout->addWidget(batterywidget);

    mverticalLayout->addStretch();

    retranslateUi();

    QMetaObject::connectSlotsByName(battery);

}

void Battery::retranslateUi()
{
    BatterytitleLabel->setText(tr("General"));
    if (QLabelSetText(msleepLabel, QString(tr("Time to sleep after %1 minute of idle time")).arg(idletime))) {
       msleepLabel->setToolTip(QString(tr("Time to sleep after %1 minute of idle time %2").arg(idletime).arg(QString(tr("(No operation for %1 minute is considered idle)")).arg(idletime))));
    }

    if (QLabelSetText(mCloseLabel, tr("Time to close display :"))) {
        mCloseLabel->setToolTip(tr("Time to close display"));
    }
    if (QLabelSetText(mDarkenLabel, tr("Time to decrease screen brightness"))) {
        mDarkenLabel->setToolTip(tr("Time to decrease screen brightness"));
    }
    if (QLabelSetText(mLowpowerLabel1, tr("Battery level is lower than"))) {
        mLowpowerLabel1->setToolTip(tr("Battery level is lower than"));
    }
    if (QLabelSetText(mLowpowerLabel2, tr("Run"))) {
        mLowpowerLabel2->setToolTip(tr("Run"));
    }

}

void Battery::initTitleLabel()
{
    QFont font ("Microsoft YaHei", 14, 55);
    BatterytitleLabel->setFont(font);

}

void Battery::initSearText()
{
    //~ contents_path /battery/General
    BatterytitleLabel->setText(tr("General"));
}

void Battery::setupComponent()
{
    //电池电量低时执行
    int batteryRemain = settings->get(PER_ACTION_CRI).toInt();
    for(int i = 5; i < batteryRemain; i++) {
        mLowpowerComboBox1->insertItem(i - 5, QString("%1%").arg(i));
    }

    for(int i = 0; i < kBattery.length(); i++) {
        if (kEnBattery.at(i) == "hibernate" && !isExitHibernate){
            continue;
        }
        mLowpowerComboBox2->insertItem(i, kBattery.at(i), kEnBattery.at(i));
    }


}

void Battery::setupConnect()
{
    connect(sleepuslider, &QSlider::valueChanged, [=](int value){

        int batterysleep;
        switch(value){
        case 1:
            batterysleep = 10*60;
            break;
        case 2:
            batterysleep = 20*60;
            break;
        case 3:
            batterysleep = 30*60;
            break;
        case 4:
            batterysleep = 60*60;
            break;
        case 5:
            batterysleep = 120*60;
            break;
        case 6:
            batterysleep = 0;
        }
        settings->set(SLEEP_COMPUTER_BATT_KEY,QVariant(batterysleep));
    });

    connect(CloseUslider, &QSlider::valueChanged, [=](int value){

        int batteryclose;
        switch(value){
        case 1:
            batteryclose = 5*60;
            break;
        case 2:
            batteryclose = 10*60;
            break;
        case 3:
            batteryclose = 30*60;
            break;
        case 4:
            batteryclose = 60*60;
            break;
        case 5:
            batteryclose = 120*60;
            break;
        case 6:
            batteryclose = 0;
        }
        settings->set(SLEEP_DISPLAY_BATT_KEY,QVariant(batteryclose));
    });

    connect(DarkenUslider, &QSlider::valueChanged, [=](int value){

        int batterydarken;
        switch(value){
        case 1:
            batterydarken = 1*60;
            break;
        case 2:
            batterydarken = 3*60;
            break;
        case 3:
            batterydarken = 5*60;
            break;
        case 4:
            batterydarken = 15*60;
            break;
        case 5:
            batterydarken = 30*60;
            break;
        case 6:
            batterydarken = 60*60;
        }
        settings->set(IDLE_DIM_TIME_KEY,QVariant(batterydarken));

    });

    connect(mLowpowerComboBox1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        settings->set(PER_ACTION_KEY, index + 5);
        });

    connect(mLowpowerComboBox2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        Q_UNUSED(index)
        settings->set(ACTION_CRI_BTY, mLowpowerComboBox2->itemData(index));
    });

    connect(stylesettings,&QGSettings::changed,[=](QString key)
    {
        if("systemFont" == key || "systemFontSize" == key)
        {
            retranslateUi();

        }
    });



}

void Battery::initStatus()
{
    // 信号阻塞
    sleepuslider->blockSignals(true);
    CloseUslider->blockSignals(true);
    DarkenUslider->blockSignals(true);
    mLowpowerComboBox1->blockSignals(true);
    mLowpowerComboBox2->blockSignals(true);


    int batsleep = settings->get(SLEEP_COMPUTER_BATT_KEY).toInt() / FIXES;
    int batclose = settings->get(SLEEP_DISPLAY_BATT_KEY).toInt() / FIXES;
    int batdarken = settings->get(IDLE_DIM_TIME_KEY) .toInt() / FIXES;


    int value ;
    if (batsleep == 10) {
        value = 1;
    }else if (batsleep == 20) {
        value = 2;
    }else if (batsleep == 30) {
        value = 3;
    }else if (batsleep == 60) {
        value = 4;
    }else if (batsleep == 120) {
        value = 5;
    }else {
        value = 6;
    }
    sleepuslider->setValue(value);

    if (batclose == 5) {
        value = 1;
    }else if (batclose == 10) {
        value = 2;
    }else if (batclose == 30) {
        value = 3;
    }else if (batclose == 60) {
        value = 4;
    }else if (batclose == 120) {
        value = 5;
    }else {
        value = 6;
    }
    CloseUslider->setValue(value);

    if (batdarken == 1) {
        value = 1;
    }else if (batdarken == 3) {
        value = 2;
    }else if (batdarken == 5) {
        value = 3;
    }else if (batdarken == 15) {
        value = 4;
    }else if (batdarken == 30) {
        value = 5;
    }else {
        value = 6;
    }
    DarkenUslider->setValue(value);


    int actionBattery = settings->get(PER_ACTION_KEY).toInt();
    mLowpowerComboBox1->setCurrentIndex(actionBattery - 5);

    QString actionCriBty = settings->get(ACTION_CRI_BTY).toString();
    mLowpowerComboBox2->setCurrentIndex(mLowpowerComboBox2->findData(actionCriBty));


    // 信号阻塞解除
    sleepuslider->blockSignals(false);
    CloseUslider->blockSignals(false);
    DarkenUslider->blockSignals(false);
    mLowpowerComboBox1->blockSignals(false);
    mLowpowerComboBox2->blockSignals(false);
}

void Battery::isHibernateSupply()
{
    QDBusInterface *HibernateInterface = new QDBusInterface("org.freedesktop.login1",
                       "/org/freedesktop/login1",
                       "org.freedesktop.login1.Manager",
                        QDBusConnection::systemBus(),this);
    if (!HibernateInterface->isValid()) {
        qDebug() << "Create login1 Hibernate Interface Failed : " <<
            QDBusConnection::systemBus().lastError();
        return;
    }
    QDBusReply<QString> HibernateInfo;
    HibernateInfo = HibernateInterface->call("CanHibernate");
    isExitHibernate = HibernateInfo == "yes"?true:false;
}

bool Battery::QLabelSetText(QLabel *label, QString string)
{
    bool is_over_length = false;
    QFontMetrics fontMetrics(label->font());
    int fontSize = fontMetrics.width(string);
    QString str = string;
    if (fontSize > (label->width()-5)) {
        str = fontMetrics.elidedText(string, Qt::ElideRight, label->width());
        is_over_length = true;
    }
    label->setText(str);
    return is_over_length;
}
