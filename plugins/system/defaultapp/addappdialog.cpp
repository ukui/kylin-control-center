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
#include "addappdialog.h"
#include "ui_addappdialog.h"

#include <QDebug>

AddAppDialog::AddAppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAppDialog)
{
    ui->setupUi(this);

    connect(ui->cancelPushBtn, SIGNAL(clicked(bool)), this, SLOT(close()));

    apps = QDir("/usr/share/applications/");
    QStringList filters;
    filters << "*.desktop";
    apps.setNameFilters(filters);

    foreach (QString name, apps.entryList(QDir::Files)) {
        qDebug() << name;
        QByteArray ba = apps.absoluteFilePath(name).toUtf8();
        GDesktopAppInfo * appinfo = g_desktop_app_info_new_from_filename(ba.constData());
        QString appname = g_app_info_get_name(G_APP_INFO(appinfo));

        const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(appinfo)));
        QIcon appicon;
        if (QIcon::hasThemeIcon(QString(iconname)))
            appicon = QIcon::fromTheme(QString(iconname));

        QListWidgetItem * item = new QListWidgetItem(appicon, appname, ui->appListWidget);
        ui->appListWidget->addItem(item);
    }
}

AddAppDialog::~AddAppDialog()
{
    delete ui;
    ui = nullptr;
}
