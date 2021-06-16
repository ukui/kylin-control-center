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
#include "configfile.h"
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QtConcurrent/QtConcurrent>

ConfigFile::ConfigFile( QString qstrfilename)
{
    if (qstrfilename.isEmpty())
    {
        QProcess proc;
        QStringList option;
        option << "-c" << "lsb_release -r | awk -F'\t' '{print $2}'";
        proc.start("/bin/bash",option);
        proc.waitForFinished();
        QByteArray ar = proc.readAll().toStdString().c_str();
        QString m_confName = "All-" + ar.replace("\n","") + ".conf";
        m_qstrFileName = QDir::homePath() + "/.cache/kylinId/" + m_confName;
    }
    else
    {
        m_qstrFileName = qstrfilename;
    }
    m_psetting = new QSettings(m_qstrFileName, QSettings::IniFormat);
}

ConfigFile::~ConfigFile()
{
    delete m_psetting;
    m_psetting = nullptr;
}

QString ConfigFile::GetPath() const {
    return m_qstrFileName;
}

void ConfigFile::Set(const QString &qstrnodename,const QString &qstrkeyname,const QVariant &qvarvalue)
{

    m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
    m_psetting->sync();
}

QVariant ConfigFile::Get(const QString &qstrnodename,const QString &qstrkeyname) const
{
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;
}
