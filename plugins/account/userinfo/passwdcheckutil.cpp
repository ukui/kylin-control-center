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
#include "passwdcheckutil.h"

#include <QFile>
#include <QDebug>

#define PAM_CONF_FILE "/etc/pam.d/common-password"

PasswdCheckUtil::PasswdCheckUtil(QObject *parent) : QObject(parent)
{

}

bool PasswdCheckUtil::getCurrentPamState(){
    QFile * readFile = new QFile(PAM_CONF_FILE);
    if (!readFile->open(QIODevice::ReadOnly | QIODevice::Text)){
        readFile->close();
        qDebug() << QString("Open conf file %1 failed!").arg(PAM_CONF_FILE);
        return false;
    } else {
        QTextStream stream(readFile);
        while(!stream.atEnd()){
            QString line = stream.readLine();
            if (line.contains("pam_pwquality.so"))
                return true;
        }
        return false;
    }
}
