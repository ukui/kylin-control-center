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

typedef enum {
    BALANCE,
    SAVING,
    CUSTDOM
}MODE;

typedef enum {
    PRESENT,
    ALWAYS
}ICONDISPLAY;

// 平衡模式
const int DISPLAY_BAT_BALANCE   =  10 * 60;
const int DISPLAY_AC_BALANCE    =  30 * 60;
const int COMPUTER_BALANCE      =  0;

// 节能模式
const int DISPLAY_SAVING        =  5 * 60;
const int COMPUTER_SAVING       =  10 * 60;

const QStringList kHibernate { QObject::tr("Never"),QObject::tr("10min"), QObject::tr("20min"),
                               QObject::tr("40min"), QObject::tr("80min")};

// 电源按钮操作
const QStringList kLid       { QObject::tr("interactive"), QObject::tr("suspend"), QObject::tr("hibernate"), QObject::tr("shutdown") };
const QStringList kEnkLid    { "interactive", "suspend", "hibernate", "shutdown"};

// 低电量操作
const QStringList kBattery    { QObject::tr("nothing"), QObject::tr("blank"), QObject::tr("suspend"), QObject::tr("hibernate"), QObject::tr("shutdown") };
const QStringList kEnBattery  { "nothing", "blank", "suspend", "hibernate", "shutdown" };

Power::Power() : mFirstLoad(true)
{
    pluginName = tr("Power");
    pluginType = SYSTEM;
}

Power::~Power() {
    if (!mFirstLoad) {
        delete ui;
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
        ui = new Ui::Power;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
        ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

        const QByteArray id(POWERMANAGER_SCHEMA);
        const QByteArray sessionId(SESSION_SCHEMA);

        initDbus();
        setupComponent();
        isPowerSupply();
        if (QGSettings::isSchemaInstalled(id)) {
            settings = new QGSettings(id, QByteArray(), this);
            sessionSetting = new QGSettings(sessionId, QByteArray(), this);

            mPowerKeys = settings->keys();

            initGeneralSet();
            initModeStatus();
            setupConnect();
            initPowerOtherStatus();
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

void Power::initSearText() {
    //~ contents_path /power/Balance (suggest)
    ui->balanceLabel->setText(tr("Balance (suggest)"));
    //~ contents_path /power/Saving
    ui->saveLabel->setText(tr("Saving"));
    //~ contents_path /power/Custom
    ui->customLabel->setText(tr("Custom"));
}

void Power::isPowerSupply() {
    QDBusInterface *brightnessInterface = new QDBusInterface("org.freedesktop.UPower",
                                     "/org/freedesktop/UPower/devices/DisplayDevice",
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus());
    if (!brightnessInterface->isValid()) {
        qDebug() << "Create UPower Interface Failed : " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> briginfo;
    briginfo  = brightnessInterface ->call("Get", "org.freedesktop.UPower.Device", "PowerSupply");

    if (!briginfo.value().toBool()) {
        isExitsPower = false ;
        ui->batteryBtn->setVisible(false);
        ui->closeLidFrame->setVisible(false);
        ui->verticalSpacer_2->changeSize(0, 0);
    } else {
        isExitsPower = true ;
        bool status = briginfo.value().toBool();
        ui->batteryBtn->setVisible(status);
    }
}

void Power::setIdleTime(int idleTime) {

    int sleeptime = ui->sleepComboBox->currentData(Qt::UserRole).toInt();
    int closetime = ui->closeComboBox->currentData(Qt::UserRole).toInt();
    if (ui->sleepComboBox->currentIndex()) {
        ui->sleepLabel->setText(QString(tr("Enter idle state %1 min and sleep after %2 min :")).arg(idleTime).arg(idleTime + sleeptime));
    }

    if (ui->closeComboBox->currentIndex()) {
        ui->closeLabel->setText(QString(tr("Enter idle state %1 min and close after %2 min :")).arg(idleTime).arg(idleTime + closetime));
    }
}

void Power::setHibernateTime(QString hibernate) {
    mUkccInterface->call("setSuspendThenHibernate", hibernate);
}

void Power::setupComponent() {

    ui->powerModeBtnGroup->setId(ui->balanceRadioBtn, BALANCE);
    ui->powerModeBtnGroup->setId(ui->savingRadioBtn, SAVING);
    ui->powerModeBtnGroup->setId(ui->custdomRadioBtn, CUSTDOM);

    // 电脑睡眠延迟
    sleepStringList  << tr("never") << tr("10 min") << tr("20 min") << tr("30 min") << tr("60 min") << tr("120 min") << tr("300 min");
    ui->sleepComboBox->insertItem(0, sleepStringList.at(0), QVariant::fromValue(0));
    ui->sleepComboBox->insertItem(1, sleepStringList.at(1), QVariant::fromValue(10));
    ui->sleepComboBox->insertItem(2, sleepStringList.at(2), QVariant::fromValue(20));
    ui->sleepComboBox->insertItem(3, sleepStringList.at(3), QVariant::fromValue(30));
    ui->sleepComboBox->insertItem(4, sleepStringList.at(4), QVariant::fromValue(60));
    ui->sleepComboBox->insertItem(5, sleepStringList.at(5), QVariant::fromValue(120));
    ui->sleepComboBox->insertItem(6, sleepStringList.at(6), QVariant::fromValue(300));

    // 显示器关闭延迟
    closeStringList  << tr("never") << tr("1 min") << tr("5 min") << tr("10 min") << tr("20 min") << tr("30 min") << tr("60 min") << tr("120 min");
    ui->closeComboBox->insertItem(0, closeStringList.at(0), QVariant::fromValue(0));
    ui->closeComboBox->insertItem(1, closeStringList.at(1), QVariant::fromValue(1));
    ui->closeComboBox->insertItem(2, closeStringList.at(2), QVariant::fromValue(5));
    ui->closeComboBox->insertItem(3, closeStringList.at(3), QVariant::fromValue(10));
    ui->closeComboBox->insertItem(4, closeStringList.at(4), QVariant::fromValue(20));
    ui->closeComboBox->insertItem(5, closeStringList.at(5), QVariant::fromValue(30));
    ui->closeComboBox->insertItem(6, closeStringList.at(6), QVariant::fromValue(60));
    ui->closeComboBox->insertItem(7, closeStringList.at(7), QVariant::fromValue(120));

    // 合盖
    closeLidStringList << tr("nothing") << tr("blank") << tr("suspend") << tr("hibernate") << tr("shutdown");
    ui->closeLidCombo->insertItem(0, closeLidStringList.at(0), "nothing");
    ui->closeLidCombo->insertItem(1, closeLidStringList.at(1), "blank");
    ui->closeLidCombo->insertItem(2, closeLidStringList.at(2), "suspend");
    ui->closeLidCombo->insertItem(3, closeLidStringList.at(3), "hibernate");
    ui->closeLidCombo->insertItem(4, closeLidStringList.at(4), "shutdown");

    // 使用电池时屏幕变暗
    darkenStringList << tr("never") << tr("1 min") << tr("5 min") << tr("10 min") << tr("20 min");
    ui->darkenCombo->insertItem(0, darkenStringList.at(0), QVariant::fromValue(0));
    ui->darkenCombo->insertItem(1, darkenStringList.at(1), QVariant::fromValue(1));
    ui->darkenCombo->insertItem(2, darkenStringList.at(2), QVariant::fromValue(5));
    ui->darkenCombo->insertItem(3, darkenStringList.at(3), QVariant::fromValue(10));
    ui->darkenCombo->insertItem(4, darkenStringList.at(4), QVariant::fromValue(20));

    // 默认电源
    ui->acBtn->setChecked(true);
    // 电源不显示变暗功能
    ui->darkenFrame->hide();

    // 电源图标
    iconShowList << tr("always") << tr("present") << tr("charge");
    ui->iconComboBox->insertItem(0, iconShowList.at(0), "always");
    ui->iconComboBox->insertItem(1, iconShowList.at(1), "present");
    ui->iconComboBox->insertItem(2, iconShowList.at(2), "charge");
    refreshUI();
}

void Power::setupConnect() {

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->powerModeBtnGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](int id){
#else
    connect(ui->powerModeBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {
#endif
        refreshUI();

        // 平衡模式
        if (id == BALANCE) {
            // 设置显示器关闭
            settings->set(SLEEP_DISPLAY_AC_KEY, DISPLAY_AC_BALANCE);
            settings->set(SLEEP_DISPLAY_BATT_KEY, DISPLAY_BAT_BALANCE);
            // 设置计算机睡眠
            settings->set(SLEEP_COMPUTER_AC_KEY, COMPUTER_BALANCE);
            settings->set(SLEEP_COMPUTER_BATT_KEY, COMPUTER_BALANCE);
        } else if (id == SAVING) {
            // 设置显示器关闭
            settings->set(SLEEP_DISPLAY_AC_KEY, DISPLAY_SAVING);
            settings->set(SLEEP_DISPLAY_BATT_KEY, DISPLAY_SAVING);
            // 设置计算机睡眠
            settings->set(SLEEP_COMPUTER_AC_KEY, COMPUTER_SAVING);
            settings->set(SLEEP_COMPUTER_BATT_KEY, COMPUTER_SAVING);
        } else {
            initCustomPlanStatus();
        }
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->powerTypeBtnGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [=]{
#else
    connect(ui->powerTypeBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=] {
#endif
        initCustomPlanStatus();
    });


#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->sleepComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(ui->sleepComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif
        Q_UNUSED(index)
        int value = ui->sleepComboBox->currentData(Qt::UserRole).toInt() * 60;
        if (ui->acBtn->isChecked()) {
            settings->set(SLEEP_COMPUTER_AC_KEY, QVariant(value));
        }
        if (ui->batteryBtn->isChecked()) {
            settings->set(SLEEP_COMPUTER_BATT_KEY, QVariant(value));
        }

        ui->sleepLabel->setText(tr("Change PC sleep time:"));
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->closeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(ui->closeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif
        Q_UNUSED(index)
        int value = ui->closeComboBox->currentData(Qt::UserRole).toInt() * 60;
        if (ui->acBtn->isChecked()) {
            settings->set(SLEEP_DISPLAY_AC_KEY, QVariant(value));
        }
        if (ui->batteryBtn->isChecked()) {
            settings->set(SLEEP_DISPLAY_BATT_KEY, QVariant(value));
        }

        ui->closeLabel->setText(tr("Change DP close time:"));;
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->iconComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(ui->iconComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif

        Q_UNUSED(index)
        QString value = ui->iconComboBox->currentData(Qt::UserRole).toString();
        settings->set(ICONPOLICY, value);
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->closeLidCombo,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(ui->closeLidCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif

        Q_UNUSED(index)
        QString value = ui->closeLidCombo->currentData(Qt::UserRole).toString();
        if (ui->acBtn->isChecked()) {
            settings->set(BUTTON_LID_AC_KEY, value);
        }
        if (ui->batteryBtn->isChecked()) {
            settings->set(BUTTON_LID_BATT_KET, value);
        }
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->darkenCombo,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
#else
    connect(ui->darkenCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
#endif

        Q_UNUSED(index)
        int idleDarken = ui->darkenCombo->currentData(Qt::UserRole).toInt() * 60;
        settings->set(IDLE_DIM_TIME_KEY, idleDarken);
    });
}

void Power::initModeStatus() {
    int acsleep = settings->get(SLEEP_COMPUTER_AC_KEY).toInt();
    int acclose = settings->get(SLEEP_DISPLAY_AC_KEY).toInt();

    int batsleep = settings->get(SLEEP_COMPUTER_BATT_KEY).toInt();
    int batclose = settings->get(SLEEP_DISPLAY_BATT_KEY).toInt();

    if (acsleep == COMPUTER_BALANCE && batsleep == COMPUTER_BALANCE &&
<<<<<<< HEAD
            acclose == DISPLAY_AC_BALANCE && batclose == DISPLAY_BAT_BALANCE){
        ui->balanceRadioBtn->setChecked(true);
=======

>>>>>>> 22462b58edfccc28e225f69e87d18d13734b548a
    } else if (acsleep == COMPUTER_SAVING && batsleep == COMPUTER_SAVING &&
               acclose == DISPLAY_SAVING && batclose == DISPLAY_SAVING){
        ui->savingRadioBtn->setChecked(true);
    } else {
<<<<<<< HEAD
        ui->custdomRadioBtn->setChecked(true);
=======

>>>>>>> 22462b58edfccc28e225f69e87d18d13734b548a
        ui->acBtn->setChecked(true);
        initCustomPlanStatus();
    }
    refreshUI();
}

void Power::initPowerOtherStatus() {
    QString value = settings->get(ICONPOLICY).toString();
    ui->iconComboBox->blockSignals(true);
    ui->iconComboBox->setCurrentIndex(ui->iconComboBox->findData(value));
    ui->iconComboBox->blockSignals(false);
}

void Power::resetCustomPlanStatus() {
    // 当其他电源计划切换至自定义时，默认状态为从不
    // 设置显示器关闭
    settings->set(SLEEP_DISPLAY_AC_KEY, 0);
    settings->set(SLEEP_DISPLAY_BATT_KEY, 0);
    // 设置计算机睡眠
    settings->set(SLEEP_COMPUTER_AC_KEY, 0);
    settings->set(SLEEP_COMPUTER_BATT_KEY, 0);

    settings->set(BUTTON_LID_AC_KEY, "nothing");
    settings->set(BUTTON_LID_BATT_KET, "nothing");

    ui->acBtn->setChecked(true);
    initCustomPlanStatus();
}

void Power::initCustomPlanStatus() {
    // 信号阻塞
    ui->sleepComboBox->blockSignals(true);
    ui->closeComboBox->blockSignals(true);
    ui->darkenCombo->blockSignals(true);

    if (ui->acBtn->isChecked()) {
        // 计算机睡眠延迟
        int acsleep = settings->get(SLEEP_COMPUTER_AC_KEY).toInt() / FIXES;
        ui->sleepComboBox->setCurrentIndex(ui->sleepComboBox->findData(acsleep));

        // 显示器关闭延迟
        int acclose = settings->get(SLEEP_DISPLAY_AC_KEY).toInt() / FIXES;
        ui->closeComboBox->setCurrentIndex(ui->closeComboBox->findData(acclose));

        // 合盖
        QString aclid = settings->get(BUTTON_LID_AC_KEY).toString();
        ui->closeLidCombo->setCurrentIndex(ui->closeLidCombo->findData(aclid));

        // 变暗
        ui->darkenFrame->hide();
    }

    if (ui->batteryBtn->isChecked()) {
        // 计算机睡眠延迟
        int batsleep = settings->get(SLEEP_COMPUTER_BATT_KEY).toInt() / FIXES;
        ui->sleepComboBox->setCurrentIndex(ui->sleepComboBox->findData(batsleep));

        // 显示器关闭延迟
        int batclose = settings->get(SLEEP_DISPLAY_BATT_KEY).toInt() / FIXES;
        ui->closeComboBox->setCurrentIndex(ui->closeComboBox->findData(batclose));

        // 合盖
        QString batlid = settings->get(BUTTON_LID_BATT_KET).toString();
        ui->closeLidCombo->setCurrentIndex(ui->closeLidCombo->findData(batlid));

        // 变暗
        int darkentime = settings->get(IDLE_DIM_TIME_KEY).toInt() / FIXES;
        ui->darkenCombo->setCurrentIndex(ui->darkenCombo->findData(darkentime));
        ui->darkenFrame->show();
    }

    ui->sleepLabel->setText(tr("Change PC sleep time:"));
    ui->closeLabel->setText(tr("Change DP close time:"));;

    // 信号阻塞解除
    ui->sleepComboBox->blockSignals(false);
    ui->closeComboBox->blockSignals(false);
    ui->darkenCombo->blockSignals(false);
}

void Power::refreshUI() {
    if (ui->powerModeBtnGroup->checkedId() != CUSTDOM) {
        ui->custom1Frame->hide();
        ui->custom2Frame->hide();
        ui->closeLidFrame->hide();
        if (ui->batteryBtn->isChecked()) {
            ui->darkenFrame->hide();
        }
    } else {
        ui->custom1Frame->show();
        ui->custom2Frame->show();
        ui->closeLidFrame->setVisible(isExitsPower);
    }
}

// 空闲时间
int Power::getIdleTime() {
    return sessionSetting->get(IDLE_DELAY_KEY).toInt();
}

void Power::initGeneralSet() {

    if (isExitsPower) {
        // 电源按钮操作
        mPowerBtn = new ComboxFrame(tr("When the power button is pressed:"), pluginWidget);

        mPowerBtn->mHLayout->setSpacing(48);
        mPowerBtn->mHLayout->setContentsMargins(16, 0, 16, 0);

        mPowerBtn->mTitleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        mPowerBtn->mTitleLabel->setMinimumWidth(300);
        ui->powerLayout->addWidget(mPowerBtn);

        for(int i = 0; i < kLid.length(); i++) {
            mPowerBtn->mCombox->insertItem(i, kLid.at(i), kEnkLid.at(i));
        }

        QString btnStaus = settings->get(BUTTON_POWER_KEY).toString();
        mPowerBtn->mCombox->setCurrentIndex(mPowerBtn->mCombox->findData(btnStaus));

        connect(mPowerBtn->mCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            settings->set(BUTTON_POWER_KEY, mPowerBtn->mCombox->itemData(index));
        });


        // 低电量操作
        mBatteryAct = new ComboxFrame(true, tr("Perform operations when battery is low:"), pluginWidget);
        mBatteryAct->mTitleLabel->setMinimumWidth(300);
        mBatteryAct->mHLayout->setContentsMargins(16, 0, 16, 0);

        mBatteryAct->mNumCombox->setMaximumWidth(230);

        ui->powerLayout->addWidget(mBatteryAct);

        int batteryRemain = settings->get(PER_ACTION_CRI).toInt();
        for(int i = 1; i < batteryRemain; i++) {
            mBatteryAct->mNumCombox->insertItem(i - 1, QString("%1%").arg(i));
        }

        for(int i = 0; i < kBattery.length(); i++) {
            mBatteryAct->mCombox->insertItem(i, kBattery.at(i), kEnBattery.at(i));
        }

        int actionBattery = settings->get(PER_ACTION_KEY).toInt();
        mBatteryAct->mNumCombox->setCurrentIndex(actionBattery - 1);

        QString actionCriBty = settings->get(ACTION_CRI_BTY).toString();
        mBatteryAct->mCombox->setCurrentIndex(mBatteryAct->mCombox->findData(actionCriBty));

        connect(mBatteryAct->mNumCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            settings->set(PER_ACTION_KEY, index + 1);
        });

        connect(mBatteryAct->mCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            Q_UNUSED(index)
            settings->set(ACTION_CRI_BTY, mBatteryAct->mCombox->itemData(index));
        });

    }

    /* 休眠接口后续开放
    if (getHibernateStatus() && mPowerKeys.contains("afterIdleAction")) {
        mHibernate = new ComboxFrame(tr("After suspending this time, the system will go to sleep:"), pluginWidget);


        ui->powerLayout->addWidget(mHibernate);

        ui->powerLayout->addStretch();

        for(int i = 0; i < kHibernate.length(); i++) {
            mHibernate->mCombox->addItem(kHibernate.at(i));
        }

        if (getHibernateTime().isEmpty()) {
            mHibernate->mCombox->setCurrentIndex(0);
        } else {
            mHibernate->mCombox->setCurrentText(getHibernateTime());
        }

        connect(mHibernate->mCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            setHibernateTime(mHibernate->mCombox->currentText());
            if (index) {
                settings->set(HIBERNATE_KEY, "suspend-then-hibernate");
            } else {
                settings->set(HIBERNATE_KEY, "suspend");
            }
        });
    }
    */
}

bool Power::getHibernateStatus() {

    QDBusInterface loginInterface("org.freedesktop.login1",
                                  "/org/freedesktop/login1",
                                  "org.freedesktop.login1.Manager",
                                  QDBusConnection::systemBus());

    if (loginInterface.isValid()) {
        QDBusReply<QString> reply = loginInterface.call("CanSuspendThenHibernate");
        return reply.value() == "yes" ? true : false;
    }
    return true;
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
                                        QDBusConnection::systemBus());
}
