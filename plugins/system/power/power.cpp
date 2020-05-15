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

typedef enum {
    BALANCE,
    SAVING,
    CUSTDOM
}MODE;

typedef enum {
    PRESENT,
    ALWAYS
}ICONDISPLAY;

/**
 * 平衡：关闭显示器10分钟；计算机进入睡眠30分钟
 * 节能：关闭显示器20分钟；计算机进入睡眠2小时
 * 自定义
 */

#define DISPLAY_BALANCE 10 * 60
#define COMPUTER_BALANCE 30 * 60
#define DISPLAY_SAVING 20 * 60
#define COMPUTER_SAVING 2 * 60 * 60



Power::Power()
{
    ui = new Ui::Power;
    itemDelege = new QStyledItemDelegate();
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Power");
    pluginType = SYSTEM;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    settingsCreate = false;

    const QByteArray id(POWERMANAGER_SCHEMA);

    setupStylesheet();
    setupComponent();

    if (QGSettings::isSchemaInstalled(id)){
        settingsCreate = true;
        settings = new QGSettings(id);
        setupConnect();
        initModeStatus();
        initIconPolicyStatus();
    } else {
        qCritical() << POWERMANAGER_SCHEMA << "not installed!\n";
    }

}

Power::~Power()
{
    delete ui;
    if (settingsCreate)
        delete settings;
}

QString Power::get_plugin_name(){
    return pluginName;
}

int Power::get_plugin_type(){
    return pluginType;
}

QWidget * Power::get_plugin_ui(){
    return pluginWidget;
}

void Power::plugin_delay_control(){

}

void Power::setupStylesheet(){
//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->balanceWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->savingWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->customWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->custom1Widget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->custom2Widget->setStyleSheet("QWidget{background: #F4F4F4;}"
//                                     "QWidget#custom2Widget{border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

//    ui->acBtn->setStyleSheet("QPushButton#acBtn:checked{background: #3D6BE5; border-radius: 4px; color: #ffffff;}"
//                             "QPushButton#acBtn:!checked{background: #ffffff; border-radius: 4px;}");
//    ui->batteryBtn->setStyleSheet("QPushButton#batteryBtn:checked{background: #3D6BE5; border-radius: 4px; color: #ffffff;}"
//                                  "QPushButton#batteryBtn:!checked{background: #ffffff; border-radius: 4px; color: #000000;}");

//    ui->iconWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

}

void Power::setupComponent(){
    //
    ui->powerModeBtnGroup->setId(ui->balanceRadioBtn, BALANCE);
    ui->powerModeBtnGroup->setId(ui->savingRadioBtn, SAVING);
    ui->powerModeBtnGroup->setId(ui->custdomRadioBtn, CUSTDOM);

    //电脑睡眠延迟
    sleepStringList  << tr("never") << tr("10 min") << tr("20 min") << tr("30 min") << tr("60 min") << tr("120 min") << tr("300 min");
//    ui->sleepComboBox->addItems(sleepStringList);
    ui->sleepComboBox->setItemDelegate(itemDelege);
    ui->sleepComboBox->setMaxVisibleItems(7);
    ui->sleepComboBox->insertItem(0, sleepStringList.at(0), QVariant::fromValue(0));
    ui->sleepComboBox->insertItem(1, sleepStringList.at(1), QVariant::fromValue(10));
    ui->sleepComboBox->insertItem(2, sleepStringList.at(2), QVariant::fromValue(20));
    ui->sleepComboBox->insertItem(3, sleepStringList.at(3), QVariant::fromValue(30));
    ui->sleepComboBox->insertItem(4, sleepStringList.at(4), QVariant::fromValue(60));
    ui->sleepComboBox->insertItem(5, sleepStringList.at(5), QVariant::fromValue(120));
    ui->sleepComboBox->insertItem(6, sleepStringList.at(6), QVariant::fromValue(300));


    //显示器关闭延迟
    closeStringList  << tr("never") << tr("1 min") << tr("5 min") << tr("10 min") << tr("20 min") << tr("30 min") << tr("60 min") << tr("120 min");
//    ui->closeComboBox->addItems(closeStringList);
    ui->closeComboBox->setItemDelegate(itemDelege);
    ui->closeComboBox->setMaxVisibleItems(8);
    ui->closeComboBox->insertItem(0, closeStringList.at(0), QVariant::fromValue(0));
    ui->closeComboBox->insertItem(1, closeStringList.at(1), QVariant::fromValue(1));
    ui->closeComboBox->insertItem(2, closeStringList.at(2), QVariant::fromValue(5));
    ui->closeComboBox->insertItem(3, closeStringList.at(3), QVariant::fromValue(10));
    ui->closeComboBox->insertItem(4, closeStringList.at(4), QVariant::fromValue(20));
    ui->closeComboBox->insertItem(5, closeStringList.at(5), QVariant::fromValue(30));
    ui->closeComboBox->insertItem(6, closeStringList.at(6), QVariant::fromValue(60));
    ui->closeComboBox->insertItem(7, closeStringList.at(7), QVariant::fromValue(120));


    //默认电源
    ui->acBtn->setChecked(true);

    //电源图标
    iconShowList << tr("always") << tr("present");
    ui->iconComboBox->setItemDelegate(itemDelege);
    ui->iconComboBox->setMaxVisibleItems(6);
    ui->iconComboBox->insertItem(0, iconShowList.at(0), "always");
    ui->iconComboBox->insertItem(1, iconShowList.at(1), "present");


    //lid
//    lidStringList << tr("nothing") << tr("blank") << tr("suspend") << tr("shutdown");
//    ui->aclidComboBox->addItems(lidStringList);
//    ui->batlidComboBox->addItems(lidStringList);

    //button
//    buttonStringList << tr("interactive") << tr("suspend") << tr("shutdown");
//    ui->powerbtnComboBox->addItems(buttonStringList);
//    ui->suspendComboBox->addItems(buttonStringList);

    //
//    ui->icondisplayBtnGroup->setId(ui->presentRadioBtn, PRESENT);
//    ui->icondisplayBtnGroup->setId(ui->alwaysRadioBtn, ALWAYS);

    refreshUI();
}

void Power::setupConnect(){

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 8)
    connect(ui->powerModeBtnGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](int id){
#else
    connect(ui->powerModeBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id){
#endif
        refreshUI();
        if (id == BALANCE){
            //设置显示器关闭
            settings->set(SLEEP_DISPLAY_AC_KEY, DISPLAY_BALANCE);
            settings->set(SLEEP_DISPLAY_BATT_KEY, DISPLAY_BALANCE);
            //设置计算机睡眠
            settings->set(SLEEP_COMPUTER_AC_KEY, COMPUTER_BALANCE);
            settings->set(SLEEP_COMPUTER_BATT_KEY, COMPUTER_BALANCE);
        } else if (id == SAVING){
            //设置显示器关闭
            settings->set(SLEEP_DISPLAY_AC_KEY, DISPLAY_SAVING);
            settings->set(SLEEP_DISPLAY_BATT_KEY, DISPLAY_SAVING);
            //设置计算机睡眠
            settings->set(SLEEP_COMPUTER_AC_KEY, COMPUTER_SAVING);
            settings->set(SLEEP_COMPUTER_BATT_KEY, COMPUTER_SAVING);

        } else {
            resetCustomPlanStatus();
        }

    });


#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 8)
    connect(ui->powerTypeBtnGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [=]{
#else
    connect(ui->powerTypeBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=]{
#endif
        initCustomPlanStatus();
    });


#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 8)
    connect(ui->sleepComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
#else
    connect(ui->sleepComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
#endif
        Q_UNUSED(index)
        int value = ui->sleepComboBox->currentData(Qt::UserRole).toInt() * 60;
        if (ui->acBtn->isChecked()){
            settings->set(SLEEP_COMPUTER_AC_KEY, QVariant(value));
        }
        if (ui->batteryBtn->isChecked()){
            settings->set(SLEEP_COMPUTER_BATT_KEY, QVariant(value));
        }
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 8)
    connect(ui->closeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
#else
    connect(ui->closeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
#endif
        Q_UNUSED(index)
        int value = ui->closeComboBox->currentData(Qt::UserRole).toInt() * 60;
        if (ui->acBtn->isChecked()){
            settings->set(SLEEP_DISPLAY_AC_KEY, QVariant(value));
        }
        if (ui->batteryBtn->isChecked()){
            settings->set(SLEEP_DISPLAY_BATT_KEY, QVariant(value));
        }
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 8)
    connect(ui->iconComboBox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
#else
    connect(ui->iconComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
#endif

        Q_UNUSED(index)
        QString value = ui->iconComboBox->currentData(Qt::UserRole).toString();
        settings->set(ICONPOLICY, value);
    });
}

void Power::initModeStatus(){
    int acsleep = settings->get(SLEEP_COMPUTER_AC_KEY).toInt();
    int acclose = settings->get(SLEEP_DISPLAY_AC_KEY).toInt();

    int batsleep = settings->get(SLEEP_COMPUTER_BATT_KEY).toInt();
    int batclose = settings->get(SLEEP_DISPLAY_BATT_KEY).toInt();

    if (acsleep == COMPUTER_BALANCE && batsleep == COMPUTER_BALANCE &&
            acclose == DISPLAY_BALANCE && batclose == DISPLAY_BALANCE){
        ui->balanceRadioBtn->setChecked(true);

    } else if (acsleep == COMPUTER_SAVING && batsleep == COMPUTER_SAVING &&
               acclose == DISPLAY_SAVING && batclose == DISPLAY_SAVING){
        ui->savingRadioBtn->setChecked(true);
    } else {
        ui->custdomRadioBtn->setChecked(true);
        //
        ui->acBtn->setChecked(true);

        initCustomPlanStatus();
    }
    refreshUI();
}

void Power::initIconPolicyStatus(){
    QString value = settings->get(ICONPOLICY).toString();
    ui->iconComboBox->blockSignals(true);
    ui->iconComboBox->setCurrentIndex(ui->iconComboBox->findData(value));
    ui->iconComboBox->blockSignals(false);
}

void Power::resetCustomPlanStatus(){
    //当其他电源计划切换至自定义时，默认状态为从不
    //设置显示器关闭
    settings->set(SLEEP_DISPLAY_AC_KEY, 0);
    settings->set(SLEEP_DISPLAY_BATT_KEY, 0);
    //设置计算机睡眠
    settings->set(SLEEP_COMPUTER_AC_KEY, 0);
    settings->set(SLEEP_COMPUTER_BATT_KEY, 0);

    ui->acBtn->setChecked(true);
    initCustomPlanStatus();

}

void Power::initCustomPlanStatus(){
    //信号阻塞
    ui->sleepComboBox->blockSignals(true);
    ui->closeComboBox->blockSignals(true);

    if (ui->acBtn->isChecked()){
        //计算机睡眠延迟
        int acsleep = settings->get(SLEEP_COMPUTER_AC_KEY).toInt() / FIXES;
        ui->sleepComboBox->setCurrentIndex(ui->sleepComboBox->findData(acsleep));

        //显示器关闭延迟
        int acclose = settings->get(SLEEP_DISPLAY_AC_KEY).toInt() / FIXES;
        ui->closeComboBox->setCurrentIndex(ui->closeComboBox->findData(acclose));

    }

    if (ui->batteryBtn->isChecked()){
        //计算机睡眠延迟
        int batsleep = settings->get(SLEEP_COMPUTER_BATT_KEY).toInt() / FIXES;
        ui->sleepComboBox->setCurrentIndex(ui->sleepComboBox->findData(batsleep));

        //显示器关闭延迟
        int batclose = settings->get(SLEEP_DISPLAY_BATT_KEY).toInt() / FIXES;
        ui->closeComboBox->setCurrentIndex(ui->closeComboBox->findData(batclose));
    }

    //信号阻塞解除
    ui->sleepComboBox->blockSignals(false);
    ui->closeComboBox->blockSignals(false);

    //lid 枚举类型但是toint为零只能toString
//    QString aclidString = settings->get(BUTTON_LID_AC_KEY).toString();
//    ui->aclidComboBox->setCurrentText(aclidString);
//    QString batlidString = settings->get(BUTTON_LID_BATT_KET).toString();
//    ui->batlidComboBox->setCurrentText(batlidString);

    //power button
//    QString powerbtn = settings->get(BUTTON_POWER_KEY).toString();
//    ui->powerbtnComboBox->setCurrentText(powerbtn);
//    QString suspentbtn = settings->get(BUTTON_SUSPEND_KEY).toString();
//    ui->suspendComboBox->setCurrentText(suspentbtn);

    //电池图标  枚举类型但是toint为零只能toString
//    QString ipvalue = settings->get(ICONPOLICY).toString();
//    if (ipvalue == PRESENT_VALUE)
//        ui->presentRadioBtn->setChecked(true);
//    else if (ipvalue == ALWAYS_VALUE)
//        ui->alwaysRadioBtn->setChecked(true);
}

void Power::refreshUI(){
    if (ui->powerModeBtnGroup->checkedId() != CUSTDOM){
//        ui->custom1Widget->setEnabled(false);
//        ui->custom2Widget->setEnabled(false);
        ui->custom1Frame->hide();
        ui->custom2Frame->hide();
//        ui->customWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");


    } else {
//        ui->custom1Widget->setEnabled(true);
//        ui->custom2Widget->setEnabled(true);
        ui->custom1Frame->show();
        ui->custom2Frame->show();
//        ui->customWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
    }
}
