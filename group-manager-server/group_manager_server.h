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

#include <QObject>
#include <QList>
#include <QTextStream>
#include <QDebug>
#include <QProcess>

#include "custom_struct.h"

class group_manager_server : public QObject
{
	Q_OBJECT
    //定义Interface名称
    Q_CLASSINFO("D-Bus Interface", "org.ukui.groupmanager.interface")
public:
    explicit group_manager_server();

public slots:
    QVariantList get();
    bool add(QString groupName, QString groupId);
    bool set(QString groupName, QString groupId);
    bool del(QString groupName);
    bool addUserToGroup(QString groupName, QString userName);
    bool delUserFromGroup(QString groupName, QString userName);

private:
    QList<custom_struct> value;

signals:
    void message();
};
