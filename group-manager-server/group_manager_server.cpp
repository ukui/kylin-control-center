/*
* Copyright (C) 2020 Tianjin KYLIN Information Technology Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/

#include "group_manager_server.h"
#include "custom_struct.h"
#include <stdio.h>

group_manager_server::group_manager_server()
{
	
}

// 解析组文件
QVariantList group_manager_server::getGroup()
{
    const QString fileName = "/etc/group";
    QFile groupFile(fileName);

    QVariantList value;
    QVariant cnt;
    int lineCount = 1;

    if(!groupFile.exists()){
        printf("/etc/group file not exist \n");
    }
    if(!groupFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        printf("open /etc/group fail \n");
    }

    QTextStream in(&groupFile);
    QString line = in.readLine();
    struct custom_struct demo[200];

    while(!line.isNull()){
        QStringList lineList = line.split(":");
        line = in.readLine();
        demo[lineCount].groupname   = lineList.at(0);
        demo[lineCount].passphrase  = lineList.at(1);
        demo[lineCount].groupid     = lineList.at(2);
        demo[lineCount].usergroup   = lineList.at(3);
        cnt = QVariant::fromValue(demo[lineCount]);
        value << cnt;
        lineCount ++;
    }
    return value;
}

// 解析passwd文件
QVariantList group_manager_server::getPasswd()
{
    const QString fileName = "/etc/passwd";
    QFile passwdFile(fileName);

    QVariantList value;
    QVariant cnt;
    int lineCount = 1;

    if(!passwdFile.exists()){
        printf("/etc/passwd file not exist \n");
    }
    if(!passwdFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        printf("open /etc/passwd fail \n");
    }

    QTextStream in(&passwdFile);
    QString line = in.readLine();
    struct custom_struct demo[200];

    while(!line.isNull()){
        QStringList lineList = line.split(":");
        line = in.readLine();
        demo[lineCount].groupname   = lineList.at(0);
        demo[lineCount].passphrase  = lineList.at(1);
        demo[lineCount].groupid     = lineList.at(3);
        cnt = QVariant::fromValue(demo[lineCount]);
        value << cnt;
        lineCount ++;
    }
    return value;
}

// 添加组
bool group_manager_server::add(QString groupName, QString groupId)
{
    QString groupadd = "/usr/sbin/groupadd";
    QString addgroup = "/usr/sbin/addgroup";
    QString command;
    QFile groupaddFile("/usr/sbin/addgroup");
    QFile addgroupFile("/usr/sbin/groupadd");

    QProcess p(0);
    QStringList args;

    if(!addgroupFile.exists()){
        printf("/usr/sbin/addgroup file not exist \n");
        if(!groupaddFile.exists()){
            return false;
        }
        command = groupadd;
        args.append("-g");
        args.append(groupId);
        args.append(groupName);
    }else{
        command = addgroup;
        args.append("-gid");
        args.append(groupId);
        args.append(groupName);
    }


    p.execute(command,args);//command是要执行的命令,args是参数
    p.waitForFinished(-1);
//    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
    return true;
}

// 修改组
bool group_manager_server::set(QString groupName, QString groupId)
{
    QString groupmod = "/usr/sbin/groupmod";
    QFile groupmodFile(groupmod);
    QProcess p(0);
    QStringList args;

    if(!groupmodFile.exists()){
        printf("/usr/sbin/groupmod file not exist \n");
        return false;
    }
    args.append("-g");
    args.append(groupId);
    //args.append("-n");
    args.append(groupName);

    p.execute(groupmod,args);//command是要执行的命令,args是参数
    p.waitForFinished(-1);
//    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
    return true;
}

// 删除组
bool group_manager_server::del(QString groupName)
{
    QString groupdel = "/usr/sbin/groupdel";
    QFile groupdelFile(groupdel);
    QProcess p(0);
    QStringList args;

    if(!groupdelFile.exists()){
        printf("/usr/sbin/groupdel file not exist \n");
        return false;
    }
    args.append(groupName);

    p.execute(groupdel,args);//command是要执行的命令,args是参数
    p.waitForFinished(-1);
//    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
    return true;
}

// 添加用户到组
bool group_manager_server::addUserToGroup(QString groupName, QString userName)
{
    QString usermod = "/usr/sbin/usermod";
    QString gpasswd = "/usr/bin/gpasswd";
    QString command;

    QFile usermodFile(usermod);
    QFile gpasswdFile(gpasswd);

    QProcess p(0);
    QStringList args;

    if(!usermodFile.exists()){
        printf("/usr/sbin/usermod file not exist \n");
        if(!gpasswdFile.exists()){
            printf("/usr/sbin/gpasswd file not exist \n");
            return false;
        }
        command = gpasswd;
        args.append("-a");
        args.append(userName);
        args.append(groupName);
    } else {
        command = usermod;
        args.append("-a");
        args.append("-G");
        args.append(groupName);
        args.append(userName);
    }
    p.execute(command,args);//command是要执行的命令,args是参数
    p.waitForFinished(-1);
//    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
    return true;
}

// 删除用户从组
bool group_manager_server::delUserFromGroup(QString groupName, QString userName)
{
    QString gpasswd = "/usr/bin/gpasswd";
    QString command;

    QFile gpasswdFile(gpasswd);

    QProcess p(0);
    QStringList args;

    if(!gpasswdFile.exists()){
        printf("/usr/sbin/gpasswd file not exist \n");
        return false;
    }
    command = gpasswd;
    args.append("-d");
    args.append(userName);
    args.append(groupName);

    p.execute(command,args);//command是要执行的命令,args是参数
    p.waitForFinished(-1);
//    qDebug() << QString::fromLocal8Bit(p.readAllStandardError());
    return true;
}
