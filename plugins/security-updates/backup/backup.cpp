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
#include "backup.h"
#include "ui_backup.h"

#include <QProcess>
#include <QFile>
#include <QDebug>

Backup::Backup()
{
    ui = new Ui::Backup;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Backup");
    pluginType = UPDATE;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->backupLabel->setStyleSheet("QLabel{color: #A6000000;}");
//    ui->restoreLabel->setStyleSheet("QLabel{color: #A6000000;}");

//    ui->backBtn->setStyleSheet("QPushButton{background: #E9E9E9; border: none; border-radius: 4px;}");
//    ui->restoreBtn->setStyleSheet("QPushButton{background: #E9E9E9; border: none; border-radius: 4px;}");

    connect(ui->backBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        btnClicked();
    });

    connect(ui->restoreBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        btnClicked();
    });
}

Backup::~Backup()
{
    delete ui;
}

QString Backup::get_plugin_name(){
    return pluginName;
}

int Backup::get_plugin_type(){
    return pluginType;
}

QWidget *Backup::get_plugin_ui(){
    return pluginWidget;
}

void Backup::plugin_delay_control(){

}

void Backup::btnClicked(){
    QString cmd = "/usr/bin/deja-dup";

    QString versionPath = "/etc/os-release";
    QStringList osRes =  readFile(versionPath);
    QString version;

    for (QString str : osRes) {
        if (str.contains("PRETTY_NAME=")) {
            int index = str.indexOf("PRETTY_NAME=");
            int startIndex = index + 13;
            int length = str.length() - startIndex - 1;
            version = str.mid(startIndex, length);
        }
    }

    if (version == "Kylin V10" || version == "Kylin V10.1") {
        cmd = "/usr/bin/kybackup";
    }

    QProcess process(this);
    process.startDetached(cmd);
}

QStringList Backup::readFile(QString filepath)
{
    QStringList fileCont;
    QFile file(filepath);
    if(file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while(!textStream.atEnd()) {
            QString line= textStream.readLine();
            line.remove('\n');
            fileCont<<line;
        }
        file.close();
        return fileCont;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return QStringList();
    }
}
