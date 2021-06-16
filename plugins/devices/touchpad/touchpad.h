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
#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include <QObject>
#include <QtPlugin>
#include <QListView>

#include <QGSettings>
#include <QX11Info>
#include <QDBusInterface>
#include <QProcess>
#include <QDBusReply>
#include <QVector>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"

const QString kSession = "wayland";

namespace Ui {
class Touchpad;
}

class Touchpad : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    explicit Touchpad();
    ~Touchpad();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void setupComponent();
    void initConnection();
    void initTouchpadStatus();

    QString _findKeyScrollingType();

private:
    void setModuleVisible(bool visible);
    void isWaylandPlatform();

    void initWaylandDbus();
    void initWaylandTouchpadStatus();
    void initWaylandConnection();

private:
    Ui::Touchpad *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

private:
    SwitchButton * enableBtn;
    SwitchButton * typingBtn;
    SwitchButton * clickBtn;
    SwitchButton * mMouseDisTouchBtn;

    QGSettings * tpsettings;

    bool mFirstLoad;
    bool mIsWayland;
    bool mExistTouchpad;

    QDBusInterface *mWaylandIface;
    QDBusInterface *mDeviceIface;

};

#endif // TOUCHPAD_H
