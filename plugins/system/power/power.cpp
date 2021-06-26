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
#include "ui_power.h"
#include "powermacrodata.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QSettings>
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
    CUSTDOM
}MODE;

typedef enum {
    PRESENT,
    ALWAYS
}ICONDISPLAY;

Power::Power() : mFirstLoad(true)
{
    pluginName = tr("Power");
    pluginType = SYSTEM;
}

Power::~Power() {
    if (!mFirstLoad) {

    }
}

QString Power::get_plugin_name() {
    return pluginName;
}

int Power::get_plugin_type() {
    return pluginType;
}

QWidget * Power::get_plugin_ui() {
    if (mFirstLoad) {
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);

        const QByteArray styleID(STYLE_FONT_SCHEMA);
        const QByteArray id(POWERMANAGER_SCHEMA);
        const QByteArray iid(SESSION_SCHEMA);


        if (QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(styleID) && QGSettings::isSchemaInstalled(iid)) {
            settings = new QGSettings(id, QByteArray(), this);
            stylesettings = new QGSettings(styleID, QByteArray(), this);
            sessionsettings = new QGSettings(iid, QByteArray(), this);
//            connect(sessionsettings,&QGSettings::changed,[=](QString key)
//            {
//                if("idleDelay" == key)
//                {
//                    idletime = sessionsettings->get(IDLE_DELAY_KEY).toInt();
//                    retranslateUi();

//                }
//            });
            connect(stylesettings,&QGSettings::changed,[=](QString key)
            {
                if("systemFont" == key || "systemFontSize" == key)
                {
                    retranslateUi();

                }
            });
        }

        InitUI(pluginWidget);
        isLidPresent();
        isHibernateSupply();
        setupComponent();
    }

    return pluginWidget;
}

void Power::plugin_delay_control() {

}

const QString Power::name() const {

    return QStringLiteral("power");
}

void Power::InitUI(QWidget *widget)
{
    QVBoxLayout *mverticalLayout = new QVBoxLayout(widget);
    mverticalLayout->setSpacing(0);
    mverticalLayout->setContentsMargins(0, 0, 32, 40);

    QWidget *Powerwidget = new QWidget(widget);
    Powerwidget->setMinimumSize(QSize(550, 0));
    Powerwidget->setMaximumSize(QSize(960, 16777215));

    QVBoxLayout *PowerLayout = new QVBoxLayout(Powerwidget);
    PowerLayout->setContentsMargins(0, 0, 0, 0);
    PowerLayout->setSpacing(0);

    CustomTitleLabel = new TitleLabel(Powerwidget);

    PowerLayout->addWidget(CustomTitleLabel);
    PowerLayout->addSpacing(8);

    mSleepPwdFrame = new QFrame(Powerwidget);
    mSleepPwdFrame->setMinimumSize(QSize(550, 60));
    mSleepPwdFrame->setMaximumSize(QSize(960, 60));
    mSleepPwdFrame->setFrameShape(QFrame::NoFrame);
    mSleepPwdFrame->setStyleSheet("background: palette(window);border-top-left-radius: 6px;border-top-right-radius: 6px");

    QHBoxLayout *mSleepPwdLayout = new QHBoxLayout(mSleepPwdFrame);
    mSleepPwdLayout->setContentsMargins(16, 0, 16, 0);

    mSleepPwdLabel = new QLabel(mSleepPwdFrame);
    mSleepPwdLabel->setMinimumSize(550,60);

    mSleepPwdBtn = new SwitchButton(mSleepPwdFrame);

    mSleepPwdLayout->addWidget(mSleepPwdLabel);
    mSleepPwdLayout->addStretch();
    mSleepPwdLayout->addWidget(mSleepPwdBtn);

    PowerLayout->addWidget(mSleepPwdFrame);
    PowerLayout->addSpacing(1);

    mWakenPwdFrame = new QFrame(Powerwidget);
    mWakenPwdFrame->setMinimumSize(QSize(550, 49));
    mWakenPwdFrame->setMaximumSize(QSize(960, 49));
    mWakenPwdFrame->setFrameShape(QFrame::NoFrame);
    mWakenPwdFrame->setStyleSheet("background: palette(window);border-radius: 0px");

    QHBoxLayout *mWakenPwdLayout = new QHBoxLayout(mWakenPwdFrame);
    mWakenPwdLayout->setContentsMargins(16, 0, 16, 0);

    mWakenPwdLabel = new QLabel(mWakenPwdFrame);
    mWakenPwdLabel->setMinimumSize(550,49);

    mWakenPwdBtn = new SwitchButton(mWakenPwdFrame);

    mWakenPwdLayout->addWidget(mWakenPwdLabel);
    mWakenPwdLayout->addStretch();
    mWakenPwdLayout->addWidget(mWakenPwdBtn);

    PowerLayout->addWidget(mWakenPwdFrame);
    PowerLayout->addSpacing(1);

    mPowerKeyFrame = new QFrame(Powerwidget);
    mPowerKeyFrame->setObjectName("mpowerkeyframe");
    mPowerKeyFrame->setMinimumSize(QSize(550, 69));
    mPowerKeyFrame->setMaximumSize(QSize(960, 69));
    mPowerKeyFrame->setFrameShape(QFrame::NoFrame);
    mPowerKeyFrame->setStyleSheet("QFrame#mpowerkeyframe{background: palette(window);\
                                        border-bottom-left-radius: 6px;\
                                            border-bottom-right-radius: 6px}");

    QHBoxLayout *mPowerKeyLayout = new QHBoxLayout(mPowerKeyFrame);
    mPowerKeyLayout->setContentsMargins(16, 0, 16, 0);

    mPowerKeyLabel = new QLabel(mPowerKeyFrame);
    mPowerKeyLabel->setMinimumSize(550,69);

    mPowerKeyComboBox = new QComboBox(mPowerKeyFrame);
    mPowerKeyComboBox->setFixedSize(250, 40);
    mPowerKeyComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mPowerKeyLayout->addWidget(mPowerKeyLabel);
    mPowerKeyLayout->addStretch();
    mPowerKeyLayout->addWidget(mPowerKeyComboBox);

    PowerLayout->addWidget(mPowerKeyFrame);
    PowerLayout->addSpacing(1);

    mCloseFrame = new QFrame(Powerwidget);
    mCloseFrame->setObjectName("mcloseframe");
    mCloseFrame->setMinimumSize(QSize(550, 60));
    mCloseFrame->setMaximumSize(QSize(960, 60));
    mCloseFrame->setFrameShape(QFrame::NoFrame);
    mCloseFrame->setStyleSheet("QFrame#mcloseframe{background: palette(window);\
                                        border-top-left-radius: 6px;\
                                            border-top-right-radius: 6px}");

    QHBoxLayout *mCloseLayout = new QHBoxLayout(mCloseFrame);
    mCloseLayout->setContentsMargins(16, 0, 16, 0);

    mCloseLabel = new QLabel(mCloseFrame);
    mCloseLabel->setMinimumSize(550,60);

    mCloseComboBox = new QComboBox(mCloseFrame);
    mCloseComboBox->setFixedSize(250, 40);
    mCloseComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mCloseLayout->addWidget(mCloseLabel);
    mCloseLayout->addStretch();
    mCloseLayout->addWidget(mCloseComboBox);

    PowerLayout->addWidget(mCloseFrame);
    PowerLayout->addSpacing(1);

    mSleepFrame = new QFrame(Powerwidget);
    mSleepFrame->setObjectName("msleepframe");
    mSleepFrame->setMinimumSize(QSize(550, 59));
    mSleepFrame->setMaximumSize(QSize(960, 59));
    mSleepFrame->setFrameShape(QFrame::NoFrame);
    mSleepFrame->setStyleSheet("QFrame#msleepframe{background: palette(window);border-radius: 0px}");


    QHBoxLayout *mSleepLayout = new QHBoxLayout(mSleepFrame);
    mSleepLayout->setContentsMargins(16, 0, 16, 0);

    mSleepLabel = new QLabel(mSleepFrame);
    mSleepLabel->setMinimumSize(550,59);

    mSleepComboBox = new QComboBox(mSleepFrame);
    mSleepComboBox->setFixedSize(250, 40);
    mSleepComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mSleepLayout->addWidget(mSleepLabel);
    mSleepLayout->addStretch();
    mSleepLayout->addWidget(mSleepComboBox);

    PowerLayout->addWidget(mSleepFrame);
    PowerLayout->addSpacing(1);

    mCloseLidFrame = new QFrame(Powerwidget);
    mCloseLidFrame->setObjectName("mcloselidframe");
    mCloseLidFrame->setMinimumSize(QSize(550, 59));
    mCloseLidFrame->setMaximumSize(QSize(960, 59));
    mCloseLidFrame->setFrameShape(QFrame::NoFrame);
    mCloseLidFrame->setStyleSheet("QFrame#mcloselidframe{background: palette(window);\
                                        border-bottom-left-radius: 6px;\
                                            border-bottom-right-radius: 6px}");

    QHBoxLayout *mCloseLidLayout = new QHBoxLayout(mCloseLidFrame);
    mCloseLidLayout->setContentsMargins(16, 0, 16, 0);

    mCloseLidLabel = new QLabel(mCloseLidFrame);
    mCloseLidLabel->setMinimumSize(550,59);

    mCloseLidComboBox = new QComboBox(mCloseLidFrame);
    mCloseLidComboBox->setFixedSize(250, 40);
    mCloseLidComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mCloseLidLayout->addWidget(mCloseLidLabel);
    mCloseLidLayout->addStretch();
    mCloseLidLayout->addWidget(mCloseLidComboBox);

    PowerLayout->addWidget(mCloseLidFrame);
    PowerLayout->addSpacing(40);

    PowerPlanTitleLabel = new TitleLabel(Powerwidget);
    PowerLayout->addWidget(PowerPlanTitleLabel);
    PowerLayout->addSpacing(8);

    mPowerFrame = new QFrame(Powerwidget);
    mPowerFrame->setObjectName("mpowerframe");
    mPowerFrame->setMinimumSize(QSize(550, 60));
    mPowerFrame->setMaximumSize(QSize(960, 60));
    mPowerFrame->setFrameShape(QFrame::NoFrame);
    mPowerFrame->setStyleSheet("QFrame#mpowerframe{background: palette(window);\
                                        border-top-left-radius: 6px;\
                                            border-top-right-radius: 6px}");

    QHBoxLayout *mPowerLayout = new QHBoxLayout(mPowerFrame);
    mPowerLayout->setContentsMargins(16, 0, 16, 0);


    mPowerLabel = new QLabel(Powerwidget);
    mPowerLabel->setMinimumSize(550,60);

    mPowerComboBox = new QComboBox(mPowerFrame);
    mPowerComboBox->setFixedSize(250, 40);
    mPowerComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mPowerLayout->addWidget(mPowerLabel);
    mPowerLayout->addStretch();
    mPowerLayout->addWidget(mPowerComboBox);

    PowerLayout->addWidget(mPowerFrame);
    PowerLayout->addSpacing(1);

    mBatteryFrame = new QFrame(Powerwidget);
    mBatteryFrame->setObjectName("mbatteryframe");
    mBatteryFrame->setMinimumSize(QSize(550, 59));
    mBatteryFrame->setMaximumSize(QSize(960, 59));
    mBatteryFrame->setFrameShape(QFrame::NoFrame);
    mBatteryFrame->setStyleSheet("QFrame#mbatteryframe{background: palette(window);\
                                        border-bottom-left-radius: 6px;\
                                            border-bottom-right-radius: 6px}");

    QHBoxLayout *mBatteryLayout = new QHBoxLayout(mBatteryFrame);
    mBatteryLayout->setContentsMargins(16, 0, 16, 0);

    mBatteryLabel = new QLabel(mBatteryFrame);
    mBatteryLabel->setMinimumSize(550,59);

    mBatteryComboBox = new QComboBox(mBatteryFrame);
    mBatteryComboBox->setFixedSize(250, 40);
    mBatteryComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mBatteryLayout->addWidget(mBatteryLabel);
    mBatteryLayout->addStretch();
    mBatteryLayout->addWidget(mBatteryComboBox);

    PowerLayout->addWidget(mBatteryFrame);
    PowerLayout->addSpacing(40);

    BatteryPlanTitleLabel = new TitleLabel(Powerwidget);
    PowerLayout->addWidget(BatteryPlanTitleLabel);
    PowerLayout->addSpacing(8);

    mLowpowerFrame = new QFrame(Powerwidget);
    mLowpowerFrame->setObjectName("mlowpowerframe");
    mLowpowerFrame->setMinimumSize(QSize(550, 60));
    mLowpowerFrame->setMaximumSize(QSize(960, 60));
    mLowpowerFrame->setFrameShape(QFrame::NoFrame);
    mLowpowerFrame->setStyleSheet("QFrame#mlowpowerframe{background: palette(window);\
                                        border-top-left-radius: 6px;\
                                            border-top-right-radius: 6px}");

    mLowpowerLabel1 = new QLabel(mLowpowerFrame);
    mLowpowerLabel1->setFixedSize(130,60);
    mLowpowerLabel2 = new QLabel(mLowpowerFrame);
    mLowpowerLabel2->setFixedSize(72,60);

    QHBoxLayout *mLowpowerLayout = new QHBoxLayout(mLowpowerFrame);
    mLowpowerLayout->setContentsMargins(16, 0, 16, 0);

    mLowpowerComboBox1 = new QComboBox(mLowpowerFrame);
    mLowpowerComboBox1->setFixedSize(70, 40);
    mLowpowerComboBox1->setStyleSheet("QComboBox{background-color: palette(button);}");
    mLowpowerComboBox2 = new QComboBox(mLowpowerFrame);
    mLowpowerComboBox2->setFixedSize(250, 40);
    mLowpowerComboBox2->setStyleSheet("QComboBox{background-color: palette(button);}");

    mLowpowerLayout->setSpacing(16);
    mLowpowerLayout->addWidget(mLowpowerLabel1);
    mLowpowerLayout->addWidget(mLowpowerComboBox1);
    mLowpowerLayout->addWidget(mLowpowerLabel2);
    mLowpowerLayout->addStretch();
    mLowpowerLayout->addWidget(mLowpowerComboBox2);

    PowerLayout->addWidget(mLowpowerFrame);
    PowerLayout->addSpacing(1);

    mNoticeLFrame = new QFrame(Powerwidget);
    mNoticeLFrame->setObjectName("mnoticeframe");
    mNoticeLFrame->setMinimumSize(QSize(550, 60));
    mNoticeLFrame->setMaximumSize(QSize(960, 60));
    mNoticeLFrame->setFrameShape(QFrame::NoFrame);
    mNoticeLFrame->setStyleSheet("QFrame#mnoticeframe{background: palette(window);border-radius: 0px}");


    QHBoxLayout *mNoticeLayout = new QHBoxLayout(mNoticeLFrame);
    mNoticeLayout->setContentsMargins(16, 0, 16, 0);

    mNoticeLabel = new QLabel(mNoticeLFrame);
    mNoticeLabel->setMinimumSize(550,59);

    mNoticeComboBox = new QComboBox(mNoticeLFrame);
    mNoticeComboBox->setFixedSize(250, 40);
    mNoticeComboBox->setStyleSheet("QComboBox{background-color: palette(button);}");

    mNoticeLayout->addWidget(mNoticeLabel);
    mNoticeLayout->addStretch();
    mNoticeLayout->addWidget(mNoticeComboBox);

    PowerLayout->addWidget(mNoticeLFrame);
    PowerLayout->addSpacing(1);

    mLowSaveFrame = new QFrame(Powerwidget);
    mLowSaveFrame->setObjectName("mlowsaveframe");
    mLowSaveFrame->setMinimumSize(QSize(550, 60));
    mLowSaveFrame->setMaximumSize(QSize(960, 60));
    mLowSaveFrame->setFrameShape(QFrame::NoFrame);
    mLowSaveFrame->setStyleSheet("QFrame#mlowsaveframe{background: palette(window);border-radius: 0px}");


    QHBoxLayout *mLowSaveLayout = new QHBoxLayout(mLowSaveFrame);
    mLowSaveLayout->setContentsMargins(16, 0, 16, 0);

    mLowSaveLabel = new QLabel(mLowSaveFrame);
    mLowSaveLabel->setMinimumSize(550,59);

    mLowSaveBtn = new SwitchButton(mLowSaveFrame);

    mLowSaveLayout->addWidget(mLowSaveLabel);
    mLowSaveLayout->addStretch();
    mLowSaveLayout->addWidget(mLowSaveBtn);

    PowerLayout->addWidget(mLowSaveFrame);
    PowerLayout->addSpacing(1);

    mBatterySaveFrame = new QFrame(Powerwidget);
    mBatterySaveFrame->setObjectName("mbatterysaveframe");
    mBatterySaveFrame->setMinimumSize(QSize(550, 60));
    mBatterySaveFrame->setMaximumSize(QSize(960, 60));
    mBatterySaveFrame->setFrameShape(QFrame::NoFrame);
    mBatterySaveFrame->setStyleSheet("QFrame#mbatterysaveframe{background: palette(window);border-radius: 0px}");


    QHBoxLayout *mBatterySaveLayout = new QHBoxLayout(mBatterySaveFrame);
    mBatterySaveLayout->setContentsMargins(16, 0, 16, 0);

    mBatterySaveLabel = new QLabel(mBatterySaveFrame);
    mBatterySaveLabel->setMinimumSize(550,59);

    mBatterySaveBtn = new SwitchButton(mBatterySaveFrame);

    mBatterySaveLayout->addWidget(mBatterySaveLabel);
    mBatterySaveLayout->addStretch();
    mBatterySaveLayout->addWidget(mBatterySaveBtn);

    PowerLayout->addWidget(mBatterySaveFrame);
    PowerLayout->addSpacing(1);

    mDisplayTimeFrame = new QFrame(Powerwidget);
    mDisplayTimeFrame->setObjectName("mdisplaytimeframe");
    mDisplayTimeFrame->setMinimumSize(QSize(550, 60));
    mDisplayTimeFrame->setMaximumSize(QSize(960, 60));
    mDisplayTimeFrame->setFrameShape(QFrame::NoFrame);
    mDisplayTimeFrame->setStyleSheet("QFrame#mdisplaytimeframe{background: palette(window);\
                                     border-bottom-left-radius: 6px;\
                                         border-bottom-right-radius: 6px}");


    QHBoxLayout *mDisplayTimeLayout = new QHBoxLayout(mDisplayTimeFrame);
    mDisplayTimeLayout->setContentsMargins(16, 0, 16, 0);

    mDisplayTimeLabel = new QLabel(mDisplayTimeFrame);
    mDisplayTimeLabel->setMinimumSize(550,59);

    mDisplayTimeBtn = new SwitchButton(mDisplayTimeFrame);

    mDisplayTimeLayout->addWidget(mDisplayTimeLabel);
    mDisplayTimeLayout->addStretch();
    mDisplayTimeLayout->addWidget(mDisplayTimeBtn);

    PowerLayout->addWidget(mDisplayTimeFrame);

    mverticalLayout->addWidget(Powerwidget);
    mverticalLayout->addStretch();

    retranslateUi();
}

void Power::retranslateUi()
{
    CustomTitleLabel->setText(tr("General"));


    if (QLabelSetText(mSleepPwdLabel, tr("Require password when waking from sleep/hibernation"))) {
        mSleepPwdLabel->setToolTip(tr("Require password when waking from sleep/hibernation"));
    }

    if (QLabelSetText(mWakenPwdLabel, tr("Password required when waking up the screen"))) {
        mSleepPwdLabel->setToolTip(tr("Password required when waking up the screen"));
    }

    if (QLabelSetText(mPowerKeyLabel, tr("Press the power button"))) {
        mPowerKeyLabel->setToolTip("Press the power button");
    }

    if (QLabelSetText(mCloseLabel, tr("Time to close display :"))) {
        mCloseLabel->setToolTip(tr("Time to close display"));
    }

    if (QLabelSetText(mSleepLabel, tr("Time to sleep"))) {
        mSleepLabel->setToolTip(tr("Time to sleep"));
    }

    if (QLabelSetText(mCloseLidLabel, tr("Notebook cover"))) {
        mCloseLidLabel->setToolTip(tr("Notebook cover"));
    }

    PowerPlanTitleLabel->setText(tr("Select Powerplan"));

    if (QLabelSetText(mPowerLabel, tr("Using power"))) {
        mPowerLabel->setToolTip(tr("Using power"));
    }

    if (QLabelSetText(mBatteryLabel, tr("Using battery"))) {
        mBatteryLabel->setToolTip(tr("Using power"));
    }

    BatteryPlanTitleLabel->setText((tr("Battery saving plan")));

    if (QLabelSetText(mLowpowerLabel1, tr("Battery level is lower than"))) {
        mLowpowerLabel1->setToolTip(tr("Battery level is lower than"));
    }

    mLowpowerLabel2->setText(tr("Run"));

    if (QLabelSetText(mNoticeLabel, tr("Low battery notification"))) {
        mNoticeLabel->setToolTip(tr("Low battery notification"));
    }

    if (QLabelSetText(mLowSaveLabel, tr("Automatically run saving mode when the battery is low"))) {
        mLowSaveLabel->setToolTip(tr("Automatically run saving mode when the battery is low"));
    }

    if (QLabelSetText(mBatterySaveLabel, tr("Automatically run saving mode when using battery"))) {
        mBatterySaveLabel->setToolTip(tr("Automatically run saving mode when using battery"));
    }

    if (QLabelSetText(mDisplayTimeLabel, tr("Display remaining charging time and usage time"))) {
        mDisplayTimeLabel->setToolTip(tr("Display remaining charging time and usage time"));
    }
}

void Power::setupComponent()
{
    // 合盖
    closeLidStringList << tr("nothing") << tr("blank") << tr("suspend") << tr("shutdown");
    mCloseLidComboBox->insertItem(0, closeLidStringList.at(0), "nothing");
    mCloseLidComboBox->insertItem(1, closeLidStringList.at(1), "blank");
    mCloseLidComboBox->insertItem(2, closeLidStringList.at(2), "suspend");
    mCloseLidComboBox->insertItem(3, closeLidStringList.at(3), "shutdown");
    if (!Utils::isWayland() && isExitHibernate){
        closeLidStringList << tr("hibernate");
        mCloseLidComboBox->insertItem(4, closeLidStringList.at(4), "hibernate");
     }

    //按下电源键时
    buttonStringList << tr("interactive") << tr("suspend") << tr("shutdown") << tr("hibernate");
    mPowerKeyComboBox->insertItem(0, buttonStringList.at(0), "interactive");
    mPowerKeyComboBox->insertItem(1, buttonStringList.at(1), "suspend");
    mPowerKeyComboBox->insertItem(2, buttonStringList.at(2), "shutdown");
    if (isExitHibernate) {
        mPowerKeyComboBox->insertItem(3, buttonStringList.at(3), "hibernate");
    }

    //关闭显示器
    closeStringList << tr("5min") << tr("10minn") << tr("15min") << tr("30min") << tr("1h") << tr("2h") << tr("never");
    mCloseComboBox->insertItem(0, closeStringList.at(0), "5min");
    mCloseComboBox->insertItem(1, closeStringList.at(1), "10min");
    mCloseComboBox->insertItem(2, closeStringList.at(2), "15min");
    mCloseComboBox->insertItem(3, closeStringList.at(3), "30min");
    mCloseComboBox->insertItem(4, closeStringList.at(4), "1h");
    mCloseComboBox->insertItem(5, closeStringList.at(5), "2h");
    mCloseComboBox->insertItem(6, closeStringList.at(6), "nerver");

    //睡眠
    sleepStringList << tr("10min") << tr("15minn") << tr("30min") << tr("1h") << tr("2h") << tr("3h") << tr("never");
    mSleepComboBox->insertItem(0, sleepStringList.at(0), "10min");
    mSleepComboBox->insertItem(1, sleepStringList.at(1), "15min");
    mSleepComboBox->insertItem(2, sleepStringList.at(2), "30min");
    mSleepComboBox->insertItem(3, sleepStringList.at(3), "1h");
    mSleepComboBox->insertItem(4, sleepStringList.at(4), "2h");
    mSleepComboBox->insertItem(5, sleepStringList.at(5), "3h");
    mSleepComboBox->insertItem(6, sleepStringList.at(6), "nerver");

    //电源计划
    PowerplanStringList << tr("Balance Model") << tr("Save Model");
    mPowerComboBox->insertItem(0, PowerplanStringList.at(0), "Balance Model");
    mPowerComboBox->insertItem(1, PowerplanStringList.at(1), "Save Model");

    BatteryplanStringList << tr("Balance Model") << tr("Save Model");
    mBatteryComboBox->insertItem(0, BatteryplanStringList.at(0), "Balance Model");
    mBatteryComboBox->insertItem(1, BatteryplanStringList.at(1), "Save Model");

    //低电量时执行
    LowpowerStringList << tr("nothing") << tr("blank") << tr("suspend") << tr("shutdown");
    mLowpowerComboBox2->insertItem(0, LowpowerStringList.at(0), "nothing");
    mLowpowerComboBox2->insertItem(1, LowpowerStringList.at(1), "blank");
    mLowpowerComboBox2->insertItem(2, LowpowerStringList.at(2), "suspend");
    mLowpowerComboBox2->insertItem(3, LowpowerStringList.at(3), "shutdown");
    if (isExitHibernate){
        LowpowerStringList << tr("hibernate");
        mLowpowerComboBox2->insertItem(4, LowpowerStringList.at(4), "hibernate");
     }

    //低电量通知

    LowNoticeStringList << "10%" << "20%" << "30%" <<"40%";
    mNoticeComboBox->insertItem(0, LowNoticeStringList.at(0), "10%");
    mNoticeComboBox->insertItem(1, LowNoticeStringList.at(1), "20%");
    mNoticeComboBox->insertItem(2, LowNoticeStringList.at(2), "30%");
    mNoticeComboBox->insertItem(3, LowNoticeStringList.at(3), "40%");

    //电池低电量范围
    int batteryRemain = settings->get(PER_ACTION_CRI).toInt();
    for(int i = 5; i < batteryRemain; i++) {
        mLowpowerComboBox1->insertItem(i - 5, QString("%1%").arg(i));
    }
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

bool Power::QLabelSetText(QLabel *label, QString string)
{
    bool is_over_length = false;
    QFontMetrics fontMetrics(label->font());
    int fontSize = fontMetrics.width(string);
    QString str = string;
   // label->setMinimumWidth(550);
    qDebug()<<label->width();
    if (fontSize > (label->width()-5)) {
        str = fontMetrics.elidedText(string, Qt::ElideRight, label->width());
        is_over_length = true;
    }
    label->setText(str);
    return is_over_length;
}




