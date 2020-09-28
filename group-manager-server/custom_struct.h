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

#include <QDBusArgument>
#include <QString>

#ifndef CUSTOM_STRUCT
#define	CUSTOM_STRUCT
struct custom_struct
{
    QString groupname;
    QString passphrase;
    QString groupid;
    QString usergroup;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const custom_struct&mystruct)
    {
        argument.beginStructure();
        argument << mystruct.groupname << mystruct.passphrase << mystruct.groupid << mystruct.usergroup;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, custom_struct&mystruct)
    {
        argument.beginStructure();
        argument >> mystruct.groupname >> mystruct.passphrase >> mystruct.groupid >> mystruct.usergroup;
        argument.endStructure();
        return argument;
    }

};

Q_DECLARE_METATYPE(custom_struct)
#endif
