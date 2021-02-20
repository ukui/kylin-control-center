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

#ifdef signals
#undef signals
#endif

extern "C" {
#include <gio/gdesktopappinfo.h>
#include <glib.h>
}

Backup::Backup()
{
    ui = new Ui::Backup;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Backup");
    pluginType = UPDATE;

    initTitleLabel();
    connect(ui->backBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        btnClicked();
    });

    connect(ui->restoreBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        restoreSlots();
    });
}

Backup::~Backup()
{
    delete ui;
    ui = nullptr;
}

QString Backup::get_plugin_name() {
    return pluginName;
}

int Backup::get_plugin_type() {
    return pluginType;
}

QWidget *Backup::get_plugin_ui() {
    return pluginWidget;
}

void Backup::plugin_delay_control() {

}

const QString Backup::name() const {

    return QStringLiteral("backup");
}

void Backup::initTitleLabel() {

    //~ contents_path /backup/Backup
    ui->titleLabel->setText(tr("Backup"));
    //~ contents_path /backup/Restore
    ui->title2Label->setText(tr("Restore"));

    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    ui->title2Label->setFont(font);
}

void Backup::btnClicked() {
    QString desktopfp = "/usr/share/applications/yhkylin-backup-tools.desktop";
    GDesktopAppInfo *desktopAppInfo = g_desktop_app_info_new_from_filename(desktopfp.toLocal8Bit().data());
    g_app_info_launch(G_APP_INFO(desktopAppInfo), nullptr, nullptr, nullptr);
    g_object_unref(desktopAppInfo);
    return;
}

void Backup::restoreSlots() {
    QString desktopfp = "/usr/share/applications/yhkylin-backup-tools.desktop";
    GDesktopAppInfo *desktopAppInfo = g_desktop_app_info_new_from_filename(desktopfp.toLocal8Bit().data());

    GList *arg = NULL;
    arg = g_list_append(arg, gpointer("--restore"));
    g_app_info_launch_uris(G_APP_INFO(desktopAppInfo), arg, nullptr, nullptr);
    g_object_unref(desktopAppInfo);
    return;
}
