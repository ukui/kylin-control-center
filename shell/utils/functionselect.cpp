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
#include "functionselect.h"
#include "../interface.h"

#include <QDebug>

QList<QList<FuncInfo>> FunctionSelect::funcinfoList;
QStack<RecordFunc> FunctionSelect::recordFuncStack;

//FuncInfo FunctionSelect::displayStruct;


FunctionSelect::FunctionSelect()
{
}

FunctionSelect::~FunctionSelect()
{
}

void FunctionSelect::initValue(){

    //system
    QList<FuncInfo> systemList;
    for (int i = 0; i < TOTALSYSFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = SYSTEM;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        systemList.append(funcStruct);
    }
    systemList[DISPLAY].nameString = QString("Display");
    systemList[DISPLAY].namei18nString =  QObject::tr("Display");
    systemList[TOUCHSCREEN].nameString = QString("TouchScreen");
    systemList[TOUCHSCREEN].namei18nString =  QObject::tr("TouchScreen");
    systemList[DEFAULTAPP].nameString = QString("Defaultapp");
    systemList[DEFAULTAPP].namei18nString =  QObject::tr("Default App");
    systemList[DEFAULTAPP].mainShow = false;
    systemList[POWER].nameString = QString("Power");
    systemList[POWER].namei18nString = QObject::tr("Power");
    systemList[BATTERY].nameString = QString("Battery");
    systemList[BATTERY].namei18nString = QObject::tr("Battery");
    systemList[AUTOBOOT].nameString = QString("Autoboot");
    systemList[AUTOBOOT].namei18nString = QObject::tr("Auto Boot");

    funcinfoList.append(systemList);

    //devices
    QList<FuncInfo> devicesList;
    for (int i = 0; i < TOTALDEVICESFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = DEVICES;
        funcStruct.index = i;
//        funcStruct.mainShow = true;
        devicesList.append(funcStruct);
    }
    devicesList[PRINTER].nameString = QString("Printer");
    devicesList[PRINTER].namei18nString = QObject::tr("Printer");
    devicesList[MOUSE].nameString = QString("Mouse");
    devicesList[MOUSE].namei18nString = QObject::tr("Mouse");
    devicesList[TOUCHPAD].nameString = QString("Touchpad");
    devicesList[TOUCHPAD].namei18nString = QObject::tr("Touchpad");
    devicesList[TOUCHPAD].mainShow = false;
    devicesList[KEYBOARD].nameString = QString("Keyboard");
    devicesList[KEYBOARD].namei18nString = QObject::tr("Keyboard");
    devicesList[SHORTCUT].nameString = QString("Shortcut");
    devicesList[SHORTCUT].namei18nString = QObject::tr("Shortcut");
    devicesList[SHORTCUT].mainShow = false;
    devicesList[AUDIO].nameString = QString("Audio");
    devicesList[AUDIO].namei18nString = QObject::tr("Audio");
    devicesList[AUDIO].mainShow = false;
    devicesList[BLUETOOTH].nameString = QString("Bluetooth");
    devicesList[BLUETOOTH].namei18nString = QObject::tr("Bluetooth");
    devicesList[BLUETOOTH].mainShow = false;

    funcinfoList.append(devicesList);

    //personalized
    QList<FuncInfo> personalizedList;
    for (int i = 0; i < TOTALPERSFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = PERSONALIZED;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        personalizedList.append(funcStruct);
    }
    personalizedList[BACKGROUND].nameString = QString("Background");
    personalizedList[BACKGROUND].namei18nString = QObject::tr("Background");
    personalizedList[BACKGROUND].mainShow = false;
    personalizedList[THEME].nameString = QString("Theme");
    personalizedList[THEME].namei18nString = QObject::tr("Theme");
    personalizedList[SCREENLOCK].nameString = QString("Screenlock");
    personalizedList[SCREENLOCK].namei18nString = QObject::tr("Screenlock");
    personalizedList[FONTS].nameString = QString("Fonts");
    personalizedList[FONTS].namei18nString = QObject::tr("Fonts");
    personalizedList[SCREENSAVER].nameString = QString("Screensaver");
    personalizedList[SCREENSAVER].namei18nString = QObject::tr("Screensaver");
    personalizedList[SCREENSAVER].mainShow = false;
    personalizedList[DESKTOP].nameString = QString("Desktop");
    personalizedList[DESKTOP].namei18nString = QObject::tr("Desktop");
    personalizedList[DESKTOP].mainShow = false;
    funcinfoList.append(personalizedList);

    //network
    QList<FuncInfo> networkList;
    for (int i = 0; i < TOTALNETFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = NETWORK;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        networkList.append(funcStruct);
    }
    networkList[NETCONNECT].nameString = QString("Netconnect");
    networkList[NETCONNECT].namei18nString = QObject::tr("Connect");
    networkList[VPN].nameString = QString("Vpn");
    networkList[VPN].namei18nString = QObject::tr("Vpn");
    networkList[PROXY].nameString = QString("Proxy");
    networkList[PROXY].namei18nString = QObject::tr("Proxy");
    networkList[VINO].nameString = QString("Vino");
    networkList[VINO].namei18nString = QObject::tr("Vino");
    networkList[VINO].mainShow = false;

    funcinfoList.append(networkList);

    //account
    QList<FuncInfo> accountList;
    for (int i = 0; i < TOTALACCOUNTFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = ACCOUNT;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        accountList.append(funcStruct);
    }

    accountList[USERINFO].nameString = QString("Userinfo");
    accountList[USERINFO].namei18nString = QObject::tr("User Info");
    accountList[NETWORKACCOUNT].nameString = QString("Cloud Account");
    accountList[NETWORKACCOUNT].namei18nString = QObject::tr("Cloud Account");

    funcinfoList.append(accountList);

    //datetime
    QList<FuncInfo> datetimeList;
    for (int i = 0; i < TOTALDTFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = DATETIME;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        datetimeList.append(funcStruct);
    }

    datetimeList[DAT].nameString = QString("Date");
    datetimeList[DAT].namei18nString = QObject::tr("Date");
    datetimeList[AREA].nameString = QString("Area");
    datetimeList[AREA].namei18nString = QObject::tr("Area");

    funcinfoList.append(datetimeList);

    //securityupdates
    QList<FuncInfo> seupdatesList;
    for (int i = 0; i < TOTALSUFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = UPDATE;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        seupdatesList.append(funcStruct);
    }

    seupdatesList[SECURITYCENTER].nameString = QString("SecurityCenter");
    seupdatesList[SECURITYCENTER].namei18nString = QObject::tr("Security Center");
    seupdatesList[BACKUP].nameString = QString("Backup");
    seupdatesList[BACKUP].namei18nString = QObject::tr("Backup");
    seupdatesList[UPDATES].nameString = QString("Update");
    seupdatesList[UPDATES].namei18nString = QObject::tr("Update");
    seupdatesList[UPGRADE].nameString = QString("Upgrade");
    seupdatesList[UPGRADE].namei18nString = QObject::tr("Upgrade");

    funcinfoList.append(seupdatesList);

    //noticeandtasks
    QList<FuncInfo> natList;
    for (int i = 0; i < TOTALNATFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = NOTICEANDTASKS;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        natList.append(funcStruct);
    }

    natList[NOTICE].nameString = QString("Notice");
    natList[NOTICE].namei18nString = QObject::tr("Notice");
    natList[SEARCH].nameString = QString("Search");
    natList[SEARCH].namei18nString = QObject::tr("Search");
    natList[ABOUT].nameString = QString("About");
    natList[ABOUT].namei18nString = QObject::tr("About");
    natList[EXPERIENCEPLAN].nameString = QString("Experienceplan");
    natList[EXPERIENCEPLAN].namei18nString = QObject::tr("Experienceplan");

    funcinfoList.append(natList);
}

void FunctionSelect::pushRecordValue(int type, QString name){
    RecordFunc reFuncStruct;
    reFuncStruct.type = type;
    reFuncStruct.namei18nString = name;
    if (recordFuncStack.length() < 1)
        recordFuncStack.push(reFuncStruct);
    else if (QString::compare(recordFuncStack.last().namei18nString, name) != 0){
//        qDebug() << recordFuncStack.last().namei18nString << name;
        recordFuncStack.push(reFuncStruct);
    }
}

void FunctionSelect::popRecordValue() {
    if (!recordFuncStack.isEmpty()) {
        recordFuncStack.pop();
    }
}
