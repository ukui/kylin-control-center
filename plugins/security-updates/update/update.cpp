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
#include "update.h"
#include "ui_update.h"

#include <QMovie>
#include <QDebug>

Update::Update()
{
    ui = new Ui::Update;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Update");
    pluginType = UPDATE;

    ui_init();

    connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(update_btn_clicked()));
}

Update::~Update()
{
    delete ui;
}

QString Update::get_plugin_name(){
    return pluginName;
}

int Update::get_plugin_type(){
    return pluginType;
}

QWidget *Update::get_plugin_ui(){
    return pluginWidget;
}

void Update::plugin_delay_control(){

}

void Update::ui_init(){
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    QString filename = QDir::homePath() + "/.config/ukccUpdate.conf";
    syncSettings = new QSettings(filename, QSettings::IniFormat);

    syncSettings->beginGroup("updateTime");

    QString time = syncSettings->value("time", "").toString();

    if (time.isEmpty()) {
        QDateTime current =QDateTime::currentDateTime();
        QString current_date_time =current.toString(QString("yyyy-MM-dd hh:mm:ss"));
        time = current_date_time;
    }
    ui->updatetimeLabel->setText(time);

    syncSettings->endGroup();

    ui->logoLabel->setPixmap(QPixmap(":/img/plugins/update/update.svg"));
}

QStringList Update::readFile(QString filepath)
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

void Update::update_btn_clicked(){
    QString cmd = "/usr/bin/update-manager";


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
        cmd = "/usr/bin/kylin-update-manager";
    }

    QProcess process(this);
    process.startDetached(cmd);
    QDateTime current =QDateTime::currentDateTime();
    QString current_date_time =current.toString(QString("yyyy-MM-dd hh:mm:ss"));

    syncSettings->beginGroup("updateTime");
    syncSettings->setValue("time", current_date_time);
    syncSettings->endGroup();

    syncSettings->sync();
}

