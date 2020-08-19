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
#ifndef ABOUT_H
#define ABOUT_H

#include <QObject>
#include <QtPlugin>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>

#include <QSysInfo>
#include <QLabel>
#include <QStringList>

#define MANUFACTURER "Manufacturer"
#define VERSION "Version"
#define PRODUCTNAME "Product Name"
#define SERIALNUMBER "Serial Number"

#include "shell/interface.h"

namespace Ui {
class About;
}

class About : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    About();
    ~About();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initUI();
    QStringList  readFile(QString filePath);

    void initSearchText();
    void _call_dbus_get_computer_info();
    void _data_init();
    void setupDesktopComponent();
    void setupKernelCompenent();
    void setupVersionCompenent();
    void setupSerialComponent();

private:
    Ui::About *ui;
    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    QLabel * envlogoLabel;
    QLabel * logoLabel;

    QDBusInterface * interface;

    QString computerinfo;
    QMap<QString, QString> infoMap;

private slots:
//    void call_finished_slot(QDBusPendingCallWatcher * call);
    void runActiveWindow();
    void showPdf();
};

#endif // ABOUT_H
