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
#include "power.h"
#include "powermacrodata.h"

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

typedef enum {
    BALANCE,
    SAVING,
}MODE;



// 电源按钮操作
const QStringList kLid       { QObject::tr("interactive"), QObject::tr("suspend"), QObject::tr("hibernate"), QObject::tr("shutdown") };
const QStringList kEnkLid    { "interactive", "suspend", "hibernate", "shutdown"};


Power::Power() : mFirstLoad(true)
{
    pluginName = tr("Power");
    pluginType = SYSTEM;
}

Power::~Power() {


}

QString Power::get_plugin_name() {
    return pluginName;
}

int Power::get_plugin_type() {
    return pluginType;
}

QWidget * Power::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new QWidget();
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);

        const QByteArray id(POWERMANAGER_SCHEMA);
        const QByteArray iid(SESSION_SCHEMA);
        const QByteArray styleID(STYLE_FONT_SCHEMA);
        
        
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

            mPowerKeys = settings->keys();
            InitUI(pluginWidget);
            initDbus();
            isPowerSupply();
            isLidPresent();
            isHibernateSupply();
            isSlptoHbtSupply();
            initTitleLabel();
            setupComponent();

            initGeneralSet();

            initCustomPlanStatus();

            initModeStatus();

            setupConnect();
        } else {
            qCritical() << POWERMANAGER_SCHEMA << "not installed!\n";
        }


    }
    return pluginWidget;
}

void Power::plugin_delay_control() {

}

const QString Power::name() const {

    return QStringLiteral("power");
}

void Power::InitUI(QWidget *power)
{
    powerModeBtnGroup = new QButtonGroup(power);
    powerModeBtnGroup->setObjectName(QString::fromUtf8("powerModeBtnGroup"));

    QVBoxLayout *mverticalLayout = new QVBoxLayout(power);
    mverticalLayout->setSpacing(0);
    mverticalLayout->setObjectName(QString::fromUtf8("mverticalLayout"));
    mverticalLayout->setContentsMargins(0, 0, 32, 48);

    QWidget *powerwidget = new QWidget(power);
    powerwidget->setObjectName(QString::fromUtf8("powerwidget"));
    powerwidget->setMinimumSize(QSize(550, 0));
    powerwidget->setMaximumSize(QSize(960, 16777215));

    QVBoxLayout *vLayout_1 = new QVBoxLayout(powerwidget);
    vLayout_1->setSpacing(0);
    vLayout_1->setObjectName(QString::fromUtf8("vLayout_1"));
    vLayout_1->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *PowerLayout = new QVBoxLayout();
    PowerLayout->setSpacing(0);
    PowerLayout->setObjectName(QString::fromUtf8("PowerLayout"));
    PowerLayout->setContentsMargins(-1, -1, -1, 20);

    PowertitleLabel = new QLabel(powerwidget);
    PowertitleLabel->setObjectName(QString::fromUtf8("PowertitleLabel"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(PowertitleLabel->sizePolicy().hasHeightForWidth());
    PowertitleLabel->setSizePolicy(sizePolicy);
    PowertitleLabel->setScaledContents(true);

    PowerLayout->addWidget(PowertitleLabel);

    QSpacerItem *verticalSpacer_1 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);

    PowerLayout->addItem(verticalSpacer_1);

    QWidget *PowerModeWidget = new QWidget(powerwidget);
    PowerModeWidget->setObjectName(QString::fromUtf8("PowerModeWidget"));
    PowerModeWidget->setMinimumSize(QSize(550, 0));
    PowerModeWidget->setMaximumSize(QSize(960, 16777215));

    QVBoxLayout *mPowerModeLayout = new QVBoxLayout(PowerModeWidget);
    mPowerModeLayout->setSpacing(0);
    mPowerModeLayout->setObjectName(QString::fromUtf8("mPowerModeLayout"));
    mPowerModeLayout->setContentsMargins(0, 0, 0, 0);

    mBalanceBtn = new QPushButton(PowerModeWidget);

    powerModeBtnGroup->addButton(mBalanceBtn);

    mBalanceBtn->setObjectName(QString::fromUtf8("mBalanceBtn"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(mBalanceBtn->sizePolicy().hasHeightForWidth());
    mBalanceBtn->setSizePolicy(sizePolicy1);
    mBalanceBtn->setMinimumSize(QSize(550, 50));
    mBalanceBtn->setMaximumSize(QSize(960, 50));
    mBalanceBtn->setStyleSheet("QPushButton{background-color:#F4F4F4;border:none;}");



    mPowerModeLayout->addWidget(mBalanceBtn);

    mSaveBtn = new QPushButton(PowerModeWidget);

    powerModeBtnGroup->addButton(mSaveBtn);

    mSaveBtn->setObjectName(QString::fromUtf8("mSaveBtn"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(mSaveBtn->sizePolicy().hasHeightForWidth());
    mSaveBtn->setSizePolicy(sizePolicy2);
    mSaveBtn->setMinimumSize(QSize(550, 50));
    mSaveBtn->setMaximumSize(QSize(960, 50));
    mSaveBtn->setStyleSheet("QPushButton{background-color:#F4F4F4;border:none;}");


    QSpacerItem *verticalSpacer_2 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);

    mPowerModeLayout->addItem(verticalSpacer_2);
    mPowerModeLayout->addWidget(mSaveBtn);

    PowerLayout->addWidget(PowerModeWidget);

    QSpacerItem *verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

    PowerLayout->addItem(verticalSpacer_3);


    mCustomtitleLabel = new QLabel(powerwidget);
    mCustomtitleLabel->setObjectName(QString::fromUtf8("mCustomtitleLabel"));
    sizePolicy.setHeightForWidth(mCustomtitleLabel->sizePolicy().hasHeightForWidth());
    mCustomtitleLabel->setSizePolicy(sizePolicy);
    mCustomtitleLabel->setScaledContents(true);

    PowerLayout->addWidget(mCustomtitleLabel);
    QSpacerItem *verticalSpacer_4 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);

    PowerLayout->addItem(verticalSpacer_4);

    mSleepFrame = new QFrame(powerwidget);
    mSleepFrame->setObjectName(QString::fromUtf8("mSleepFrame"));
    mSleepFrame->setMinimumSize(QSize(550, 88));
    mSleepFrame->setMaximumSize(QSize(960, 88));
    mSleepFrame->setFrameShape(QFrame::Box);

    QFormLayout *msleepLayout = new QFormLayout(mSleepFrame);
    msleepLayout->setContentsMargins(16, 0, 16, 0);

    msleepLabel = new QLabel(mSleepFrame);
    msleepLabel->setObjectName(QString::fromUtf8("msleepLabel"));
    sizePolicy.setHeightForWidth(msleepLabel->sizePolicy().hasHeightForWidth());
    msleepLabel->setSizePolicy(sizePolicy);
    msleepLabel->setMinimumSize(QSize(182, 88));
    msleepLabel->setMaximumSize(QSize(182, 88));
    msleepLabel->setScaledContents(false);


    QStringList mSleepTime;
    mSleepTime<< tr("10m") << tr("20m") << tr("30m") << tr("1h") << tr("2h")
              <<tr("never");

    sleepuslider  = new Uslider(mSleepTime);
    sleepuslider->setRange(1,6);
    sleepuslider->setTickInterval(1);
    sleepuslider->setPageStep(1);

    msleepLayout->addRow(msleepLabel,sleepuslider);
    msleepLayout->setHorizontalSpacing(50);

    PowerLayout->addWidget(mSleepFrame);

    QSpacerItem *verticalSpacer_5 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);

    PowerLayout->addItem(verticalSpacer_5);



    mCloseFrame = new QFrame(powerwidget);
    mCloseFrame->setObjectName(QString::fromUtf8("mCloseFrame"));
    mCloseFrame->setMinimumSize(QSize(550, 88));
    mCloseFrame->setMaximumSize(QSize(960, 88));
    mCloseFrame->setFrameShape(QFrame::Box);

    QFormLayout *mCloseLayout = new QFormLayout(mCloseFrame);
    mCloseLayout->setContentsMargins(16, 0, 16, 0);


    mCloseLabel = new QLabel(mCloseFrame);
    mCloseLabel->setObjectName(QString::fromUtf8("mCloseLabel"));
    sizePolicy.setHeightForWidth(mCloseLabel->sizePolicy().hasHeightForWidth());
    mCloseLabel->setSizePolicy(sizePolicy);
    mCloseLabel->setMinimumSize(QSize(182, 88));
    mCloseLabel->setMaximumSize(QSize(182, 88));
    mCloseLabel->setScaledContents(false);

    QStringList mCloseTime;
    mCloseTime<< tr("5m") << tr("10m") << tr("30m") << tr("1h") << tr("2h")
              <<tr("never");

    CloseUslider  = new Uslider(mCloseTime);
    CloseUslider->setRange(1,6);
    CloseUslider->setTickInterval(1);
    CloseUslider->setPageStep(1);

    mCloseLayout->addRow(mCloseLabel,CloseUslider);
    mCloseLayout->setHorizontalSpacing(50);
    PowerLayout->addWidget(mCloseFrame);

    QSpacerItem *verticalSpacer_6 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);
    PowerLayout->addItem(verticalSpacer_6);

    mslptohbtFrame = new QFrame(powerwidget);
    mslptohbtFrame->setObjectName(QString::fromUtf8("mslptohbtFrame"));
    mslptohbtFrame->setMinimumSize(QSize(550, 88));
    mslptohbtFrame->setMaximumSize(QSize(960, 88));
    mslptohbtFrame->setFrameShape(QFrame::Box);

    QFormLayout *mslptohbtLayout = new QFormLayout(mslptohbtFrame);
    mslptohbtLayout->setContentsMargins(16, 0, 16, 0);

    mslptohbtlabel = new QLabel(mslptohbtFrame);
    mslptohbtlabel->setObjectName(QString::fromUtf8("mslptohbtlabel"));
    sizePolicy.setHeightForWidth(mslptohbtlabel->sizePolicy().hasHeightForWidth());
    mslptohbtlabel->setSizePolicy(sizePolicy);
    mslptohbtlabel->setMinimumSize(QSize(182, 88));
    mslptohbtlabel->setMaximumSize(QSize(182, 88));

    slptohbtslider = new Uslider(mCloseTime);
    slptohbtslider->setRange(1,6);
    slptohbtslider->setTickInterval(1);
    slptohbtslider->setPageStep(1);


    mslptohbtLayout->addRow(mslptohbtlabel,slptohbtslider);
    mslptohbtLayout->setHorizontalSpacing(50);

    PowerLayout->addWidget(mslptohbtFrame);

    verticalSpacer_7 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);
    PowerLayout->addItem(verticalSpacer_7);

    mEnterPowerFrame = new QFrame(powerwidget);
    mEnterPowerFrame->setObjectName(QString::fromUtf8("mEnterPowerFrame"));
    mEnterPowerFrame->setMinimumSize(QSize(0, 50));
    mEnterPowerFrame->setMaximumSize(QSize(16777215, 50));
    mEnterPowerFrame->setFrameShape(QFrame::Box);

    QFormLayout *mEnterPowerLayout = new QFormLayout(mEnterPowerFrame);
    mEnterPowerLayout->setContentsMargins(16, 7, 16, 0);


    mEnterPowerlabel = new QLabel(mEnterPowerFrame);
    mEnterPowerlabel->setObjectName(QString::fromUtf8("mEnterPowerlabel"));
    sizePolicy.setHeightForWidth(mEnterPowerlabel->sizePolicy().hasHeightForWidth());
    mEnterPowerlabel->setSizePolicy(sizePolicy);
    mEnterPowerlabel->setMinimumSize(QSize(220, 0));
    mEnterPowerlabel->setMaximumSize(QSize(220, 16777215));


    mEnterPowerComboBox = new QComboBox(mEnterPowerFrame);
    mEnterPowerComboBox->setObjectName(QString::fromUtf8("mEnterPowerComboBox"));
    sizePolicy2.setHeightForWidth(mEnterPowerComboBox->sizePolicy().hasHeightForWidth());
    mEnterPowerComboBox->setSizePolicy(sizePolicy2);
    mEnterPowerComboBox->setMinimumSize(QSize(0, 36));
    mEnterPowerComboBox->setMaximumSize(QSize(16777215, 36));
    mEnterPowerComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mEnterPowerLayout->addRow(mEnterPowerlabel,mEnterPowerComboBox);
    mEnterPowerLayout->setHorizontalSpacing(50);


    PowerLayout->addWidget(mEnterPowerFrame);

    QSpacerItem *verticalSpacer_8 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed);
    PowerLayout->addItem(verticalSpacer_8);

    mCloselidFrame = new QFrame(powerwidget);
    mCloselidFrame->setObjectName(QString::fromUtf8("mCloselidFrame"));
    mCloselidFrame->setMinimumSize(QSize(0, 50));
    mCloselidFrame->setMaximumSize(QSize(16777215, 50));
    mCloselidFrame->setFrameShape(QFrame::Box);
    QFormLayout *mCloselidLayout = new QFormLayout(mCloselidFrame);
    mCloselidLayout->setContentsMargins(16, 7, 16, 0);

    mCloselidlabel = new QLabel(mCloselidFrame);
    mCloselidlabel->setObjectName(QString::fromUtf8("mCloselidlabel"));
    sizePolicy.setHeightForWidth(mCloselidlabel->sizePolicy().hasHeightForWidth());
    mCloselidlabel->setSizePolicy(sizePolicy);
    mCloselidlabel->setMinimumSize(QSize(220, 0));
    mCloselidlabel->setMaximumSize(QSize(220, 16777215));


    mCloselidComboBox = new QComboBox(mCloselidFrame);
    mCloselidComboBox->setObjectName(QString::fromUtf8("mCloselidComboBox"));
    sizePolicy2.setHeightForWidth(mCloselidComboBox->sizePolicy().hasHeightForWidth());
    mCloselidComboBox->setSizePolicy(sizePolicy2);
    mCloselidComboBox->setMinimumSize(QSize(0, 36));
    mCloselidComboBox->setMaximumSize(QSize(16777215, 36));
    mCloselidComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mCloselidLayout->addRow(mCloselidlabel,mCloselidComboBox);
    mCloselidLayout->setHorizontalSpacing(50);


    PowerLayout->addWidget(mCloselidFrame);

    vLayout_1->addLayout(PowerLayout);

    mverticalLayout->addWidget(powerwidget);

    QSpacerItem *verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    mverticalLayout->addItem(verticalSpacer_9);

    retranslateUi();
    buildPowerModeBtn(mBalanceBtn,tr("Balance"),tr("Autobalance energy and performance with available hardware"));
    buildPowerModeBtn(mSaveBtn,tr("Saving"), tr("Minimize performance"));


    QMetaObject::connectSlotsByName(power);

}

void Power::retranslateUi()
{
    PowertitleLabel->setText(tr("select power plan"));
    mCustomtitleLabel->setText(tr("General Settings"));


    if (QLabelSetText(msleepLabel, QString(tr("Time to sleep after %1 minute of idle time")).arg(idletime))) {
       msleepLabel->setToolTip(QString(tr("Time to sleep after %1 minute of idle time %2").arg(idletime).arg(QString(tr("(No operation for %1 minute is considered idle)")).arg(idletime))));
    }
    if (QLabelSetText(mCloseLabel, tr("Time to close display :"))) {
        mCloseLabel->setToolTip(tr("Time to close display"));
    }
    if (QLabelSetText(mslptohbtlabel, tr("Set sleeping to hibernation"))) {
        mslptohbtlabel->setToolTip(tr("Set sleeping to hibernation"));
    }
    if (QLabelSetText(mEnterPowerlabel, tr("Press the power button"))) {
        mEnterPowerlabel->setToolTip(tr("Press the power button"));
    }
    if (QLabelSetText(mCloselidlabel, tr("Notebook cover"))) {
        mCloselidlabel->setToolTip(tr("Notebook cover"));
    }

}

void Power::initTitleLabel()
{
    QFont font ("Microsoft YaHei", 14, 55);
    PowertitleLabel->setFont(font);
    mCustomtitleLabel->setFont(font);
}

void Power::initSearText()
{
    //~ contents_path /power/Balance (suggest)
    mBalanceBtn->setText(tr("Balance"));
    //~ contents_path /power/Saving
    mSaveBtn->setText(tr("Saving"));
}

void Power::setupComponent()
{

    powerModeBtnGroup->setId(mBalanceBtn, BALANCE);
    powerModeBtnGroup->setId(mSaveBtn, SAVING);

    // 合盖
    closeLidStringList << tr("nothing") << tr("blank") << tr("suspend") << tr("shutdown");
    mCloselidComboBox->insertItem(0, closeLidStringList.at(0), "nothing");
    mCloselidComboBox->insertItem(1, closeLidStringList.at(1), "blank");
    mCloselidComboBox->insertItem(2, closeLidStringList.at(2), "suspend");
    mCloselidComboBox->insertItem(3, closeLidStringList.at(3), "shutdown");
    if (!Utils::isWayland() && isExitHibernate){
        closeLidStringList << tr("hibernate");
        mCloselidComboBox->insertItem(4, closeLidStringList.at(4), "hibernate");
     }

    //按下电源键时
    for(int i = 0; i < kLid.length(); i++) {
        if (kEnkLid.at(i) == "hibernate" && !isExitHibernate ){
            continue;
        }

        mEnterPowerComboBox->insertItem(i, kLid.at(i), kEnkLid.at(i));
    }

    refreshUI();

}

void Power::setupConnect()
{
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(powerModeBtnGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](int id){
#else
    connect(powerModeBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {
#endif
        // 平衡模式
        if (id == BALANCE) {
            settings->set(POWER_POLICY_KEY, 1);
        } else  {
            settings->set(POWER_POLICY_KEY, 2);
        }
    });


    connect(sleepuslider, &QSlider::valueChanged, [=](int value){

        int acsleep;
        switch(value){
        case 1:
            acsleep = 10*60;
            break;
        case 2:
            acsleep = 20*60;
            break;
        case 3:
            acsleep = 30*60;
            break;
        case 4:
            acsleep = 60*60;
            break;
        case 5:
            acsleep = 120*60;
            break;
        case 6:
            acsleep = 0;
        }
        settings->set(SLEEP_COMPUTER_AC_KEY,QVariant(acsleep));
    });

    connect(CloseUslider, &QSlider::valueChanged, [=](int value){

        int acclose;
        switch(value){
        case 1:
            acclose = 5*60;
            break;
        case 2:
            acclose = 10*60;
            break;
        case 3:
            acclose = 30*60;
            break;
        case 4:
            acclose = 60*60;
            break;
        case 5:
            acclose = 120*60;
            break;
        case 6:
            acclose = 0;
        }
        settings->set(SLEEP_DISPLAY_AC_KEY,QVariant(acclose));
    });
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(mCloselidComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(mCloselidComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif

        Q_UNUSED(index)
        QString value = mCloselidComboBox->currentData(Qt::UserRole).toString();
        settings->set(BUTTON_LID_AC_KEY, value);
        settings->set(BUTTON_LID_BATT_KEY,value);

    });
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(mEnterPowerComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(mEnterPowerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif

        Q_UNUSED(index)
        QString value = mEnterPowerComboBox->currentData(Qt::UserRole).toString();
        settings->set(BUTTON_POWER_KEY,value);
    });
}

void Power::initModeStatus()
{
    int power_policy = settings->get(POWER_POLICY_KEY).toInt();
    if (power_policy == 1 ) {
        powerModeBtnGroup->buttonClicked(mBalanceBtn);

    } else {
        powerModeBtnGroup->buttonClicked(mSaveBtn);
    }

}

void Power::initCustomPlanStatus()
{
    // 信号阻塞
    sleepuslider->blockSignals(true);
    CloseUslider->blockSignals(true);
    mCloselidComboBox->blockSignals(true);

    // 计算机睡眠延迟
    int acsleep = settings->get(SLEEP_COMPUTER_AC_KEY).toInt() / FIXES;
    int acclose = settings->get(SLEEP_DISPLAY_AC_KEY).toInt() / FIXES;


    int value ;
    if (acsleep == 10) {
        value = 1;
    }else if (acsleep == 20) {
        value = 2;
    }else if (acsleep == 30) {
        value = 3;
    }else if (acsleep == 60) {
        value = 4;
    }else if (acsleep == 120) {
        value = 5;
    }else {
        value = 6;
    }
    sleepuslider->setValue(value);

    if (acclose == 5) {
        value = 1;
    }else if (acclose == 10) {
        value = 2;
    }else if (acclose == 30) {
        value = 3;
    }else if (acclose == 60) {
        value = 4;
    }else if (acclose == 120) {
        value = 5;
    }else {
        value = 6;
    }
    CloseUslider->setValue(value);

    if (isExitsLid) {
        QString aclid = settings->get(BUTTON_LID_AC_KEY).toString();
        mCloselidComboBox->setCurrentIndex(mCloselidComboBox->findData(aclid));
    }

    // 信号阻塞解除
    sleepuslider->blockSignals(false);
    CloseUslider->blockSignals(false);
    mCloselidComboBox->blockSignals(false);
}




void Power::buildPowerModeBtn(QPushButton *button,QString Btn_name,QString BtnFun_name)
{
     QHBoxLayout * baseHerLayout = new QHBoxLayout(button);
     baseHerLayout->setContentsMargins(16,0,8,0);


     QLabel *mBtnLabel = new QLabel(button);
     mBtnLabel->setObjectName(QString::fromUtf8("mBtnLabel"));
     QSizePolicy sizePolicy = mBtnLabel->sizePolicy();
     sizePolicy.setHeightForWidth(mBtnLabel->sizePolicy().hasHeightForWidth());
     mBtnLabel->setSizePolicy(sizePolicy);
     mBtnLabel->setScaledContents(true);
     mBtnLabel->setText(Btn_name);

     baseHerLayout->addWidget(mBtnLabel);

     QLabel *mBtnFun_Label = new QLabel(button);
     mBtnFun_Label->setObjectName(QString::fromUtf8("mBtnFun_Label"));
     QSizePolicy sizePolicy_1 = mBtnFun_Label->sizePolicy();
     sizePolicy_1.setHeightForWidth(mBtnFun_Label->sizePolicy().hasHeightForWidth());
     mBtnFun_Label->setSizePolicy(sizePolicy);
     mBtnFun_Label->setScaledContents(true);
     mBtnFun_Label->setText(BtnFun_name);

     baseHerLayout->addWidget(mBtnFun_Label);

     QLabel * statusLabel = new QLabel(button);
     statusLabel->setFixedSize(QSize(16, 16));
     statusLabel->setScaledContents(true);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(powerModeBtnGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton * eBtn){
#else
    connect(powerModeBtnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [=](QAbstractButton * eBtn){
#endif
        if (eBtn == button) {
            statusLabel->setPixmap(QPixmap("://img/plugins/theme/selected.svg"));
        }
        else {
            statusLabel->clear();
        }
    });

    baseHerLayout->addStretch();
    baseHerLayout->addWidget(statusLabel);

    button->setLayout(baseHerLayout);
}

void Power::isPowerSupply()
{
    QDBusInterface *brightnessInterface = new QDBusInterface("org.freedesktop.UPower",
                                     "/org/freedesktop/UPower/devices/DisplayDevice",
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus(),this);
    if (!brightnessInterface->isValid()) {
        qDebug() << "Create UPower Interface Failed : " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> briginfo;
    briginfo  = brightnessInterface ->call("Get", "org.freedesktop.UPower.Device", "PowerSupply");
    isExitsPower = briginfo.value().toBool();

}

void Power::isLidPresent()
{
    QDBusInterface *LidInterface = new QDBusInterface("org.freedesktop.UPower",
                       "/org/freedesktop/UPower",
                       "org.freedesktop.DBus.Properties",
                        QDBusConnection::systemBus(),this);


    if (!LidInterface->isValid()) {
        qDebug() << "Create UPower Lid Interface Failed : " <<
            QDBusConnection::systemBus().lastError();
        return;
    }
    QDBusReply<QVariant> LidInfo;
    LidInfo = LidInterface->call("Get", "org.freedesktop.UPower", "LidIsPresent");
    isExitsLid = LidInfo.value().toBool();

}
void Power::isHibernateSupply()
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

void Power::isSlptoHbtSupply()
{
    QDBusInterface *loginInterface = new QDBusInterface("org.freedesktop.login1",
                       "/org/freedesktop/login1",
                       "org.freedesktop.login1.Manager",
                        QDBusConnection::systemBus(),this);
    if (!loginInterface->isValid()) {
        qDebug() << "Create login1 Interface Failed : " <<
            QDBusConnection::systemBus().lastError();
        return;
    }
    QDBusReply<QString> SlptohbtInfo;
    SlptohbtInfo = loginInterface->call("CanSuspendThenHibernate");
    isExitslptoHbt = SlptohbtInfo == "yes"?true:false;

}


void Power::refreshUI()
{
    mCloselidFrame->setVisible(isExitsLid);
    //先屏蔽睡眠转休眠
    mslptohbtFrame->setVisible(isExitslptoHbt && !Utils::isWayland());
    if (!isExitslptoHbt || Utils::isWayland()) {
        verticalSpacer_7->changeSize(0,0);
    }

}


bool Power::QLabelSetText(QLabel *label, QString string)
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

void Power::initGeneralSet()
{

    if (isExitsPower || Utils::isWayland()) {
        // 电源按钮操作
        QString btnStaus = settings->get(BUTTON_POWER_KEY).toString();
        mEnterPowerComboBox->setCurrentIndex(mEnterPowerComboBox->findData(btnStaus));

        connect(mEnterPowerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            settings->set(BUTTON_POWER_KEY, mEnterPowerComboBox->itemData(index));
        });
    }else {
       mEnterPowerFrame->hide();
    }

    if (isExitslptoHbt) {
        slptohbtslider->blockSignals(true);
        if (getHibernateTime().isEmpty()) {
            slptohbtslider->setValue(6);
        } else {
            QString mhibernate = getHibernateTime();
            if (mhibernate == "5min") {
                slptohbtslider->setValue(1);
            }else if (mhibernate == "10min"){
                slptohbtslider->setValue(2);
            }else if (mhibernate == "30min"){
                slptohbtslider->setValue(3);
            }else if (mhibernate == "60min"){
                slptohbtslider->setValue(4);
            }else if (mhibernate == "120min"){
                slptohbtslider->setValue(5);
            }

        }
        slptohbtslider->blockSignals(false);
        connect(CloseUslider, &QSlider::valueChanged, [=](int value) {
            QString hibernate;
            switch (value) {
            case 1:
                hibernate = "5min";
                break;
            case 2:
                hibernate = "10min";
                break;
            case 3:
                hibernate = "30min";
                break;
            case 4:
                hibernate = "60min";
                break;
            case 5:
                hibernate = "120min";
                break;
            case 6:
                hibernate = "";
                break;

            }

            mUkccInterface->call("setSuspendThenHibernate", hibernate);
        });
    }
}
QString Power::getHibernateTime() {
    QDBusReply<QString> hibernateTime = mUkccInterface->call("getSuspendThenHibernate");
    if (hibernateTime.isValid()) {
        return hibernateTime.value();
    }
    return "";
}

void Power::initDbus() {
    mUkccInterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                        "/",
                                        "com.control.center.interface",
                                        QDBusConnection::systemBus(),
                                        this);
}





