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
#include "sysdbusregister.h"

#include <QDebug>
#include <QSettings>
#include <QSharedPointer>

SysdbusRegister::SysdbusRegister()
{
}

SysdbusRegister::~SysdbusRegister()
{
}

//QString SysdbusRegister::name () const{
//    return m_name;
//}

//void SysdbusRegister::SetName(QString name){
//    m_name = name;
//}

void SysdbusRegister::exitService(){
    qApp->exit(0);
}

QString SysdbusRegister::GetComputerInfo(){
    QByteArray ba;
    FILE * fp = NULL;
    char cmd[128];
    char buf[1024];
    sprintf(cmd, "dmidecode -t system");

    if ((fp = popen(cmd, "r")) != NULL){
        rewind(fp);
        while (!feof(fp)) {
            fgets(buf, sizeof (buf), fp);
            ba.append(buf);
        }
        pclose(fp);
        fp = NULL;
    }
    return QString(ba);
}

void SysdbusRegister::systemRun(QString cmd){
    QProcess::execute(cmd);
}

//获取免密登录状态
QString SysdbusRegister::getNoPwdLoginStatus(){
    QByteArray ba;
    FILE * fp = NULL;
    char cmd[128];
    char buf[1024];
    sprintf(cmd, "cat /etc/group |grep nopasswdlogin");
    if ((fp = popen(cmd, "r")) != NULL){
        rewind(fp);
        fgets(buf, sizeof (buf), fp);
        ba.append(buf);
        pclose(fp);
        fp = NULL;
    }else{
        qDebug()<<"popen文件打开失败"<<endl;
    }
    return QString(ba);
}

//设置免密登录状态
void SysdbusRegister::setNoPwdLoginStatus(bool status,QString username){

    QString cmd;
    if(true == status){
         cmd = QString("gpasswd  -a %1 nopasswdlogin").arg(username);
    } else{
        cmd = QString("gpasswd  -d %1 nopasswdlogin").arg(username);
    }
    systemRun(cmd);
}

// 设置自动登录状态
void SysdbusRegister::setAutoLoginStatus(QString username)
{
    QString filename = "/etc/lightdm/lightdm.conf";
    QSharedPointer<QSettings>  autoSettings = QSharedPointer<QSettings>(new QSettings(filename, QSettings::IniFormat));
    autoSettings->beginGroup("SeatDefaults");
    autoSettings->clear();

    autoSettings->setValue("autologin-user", username);
    autoSettings->endGroup();
    autoSettings->sync();
}

QMap<QString, QVariant> SysdbusRegister::getModuleHideStatus() {
    qDebug() << Q_FUNC_INFO;
    QMap<QString, QVariant> moudleRes;
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-hide.json";

    QString json = readFile(filename);
    if (json.isEmpty()) {
        qFatal("Could not read JSON file!");
        return moudleRes;
    }

    bool ok;
    JsonObject result = QtJson::parse(json, ok).toMap();
    QMap<QString, QVariant> keyMap = QtJson::parse(json, ok).toMap();
    if (!ok) {
        qFatal("An error occurred during parsing");
        return moudleRes;
    }

    return getJsonInfo(result, keyMap.keys());
}

QString SysdbusRegister::readFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

QMap<QString, QVariant> SysdbusRegister::getJsonInfo(const QtJson::JsonObject &result, const QStringList &keys) {
    QMap<QString, QVariant> moduleMap;

    for (int i = 0; i < keys.length(); i++) {
        QMap<QString, QVariant> childMap = result[keys.at(i)].toMap();
        QMapIterator<QString, QVariant> it(childMap);
        while (it.hasNext()) {
            it.next();
            moduleMap.insert(it.key(), it.value());
        }
    }
    return moduleMap;
}

