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
#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include <QObject>
#include <QtPlugin>
#include "mainui/interface.h"

#include <QWidget>
#include <QAbstractButton>
#include <QDir>

#include <QGSettings>
#include <QX11Info>

#include "kylin-mouse-interface.h"

#include "../../pluginsComponent/switchbutton.h"
#include "../../pluginsComponent/customwidget.h"

#define CURSORS_THEMES_PATH "/usr/share/icons/"

#define GNOME_TOUCHPAD_SCHEMA "org.gnome.desktop.peripherals.touchpad"
#define GNOME_ACTIVE_TOUCHPAD_KEY "send-events"
#define GNOME_TOUCHPAD_CLICK_KEY "tap-to-click"
#define GNOME_DISABLE_WHILE_TYPEING_KEY "disable-while-typing"
#define GNOME_DISABLE_SCROLLING_KEY "natural-scroll"
#define GNOME_SCROLLING_EDGE_KEY "edge-scrolling-enabled"
#define GNOME_SCROLLING_TWO_KEY "two-finger-scrolling-enabled"

#define TOUCHPAD_SCHEMA "org.ukui.peripherals-touchpad"
#define ACTIVE_TOUCHPAD_KEY "touchpad-enabled"
#define DISABLE_WHILE_TYPING_KEY "disable-while-typing"
#define TOUCHPAD_CLICK_KEY "tap-to-click"
#define V_EDGE_KEY "vertical-edge-scrolling"
#define H_EDGE_KEY "horizontal-edge-scrolling"
#define V_FINGER_KEY "vertical-two-finger-scrolling"
#define H_FINGER_KEY "horizontal-two-finger-scrolling"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

namespace Ui {
class MouseControl;
}

class MouseControl : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    MouseControl();
    ~MouseControl() Q_DECL_OVERRIDE;

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    CustomWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;

    void component_init();
    void status_init();

    bool find_synaptics();
    bool _supports_xinput_devices();

private:
    Ui::MouseControl *ui;

    QString pluginName;
    int pluginType;
    CustomWidget * pluginWidget;

    SwitchButton * activeBtn;

//    QGSettings * tpsettings;
    QGSettings * gnomeSettings;

    QStringList _get_cursors_themes();
    void _refresh_touchpad_widget_status();
    void _refresh_rolling_btn_status();


private slots:
    void mouseprimarykey_changed_slot(int id);
    void cursor_themes_changed_slot(QString text);
    void speed_value_changed_slot(int value);
    void sensitivity_value_changed_slot(int value);
    void show_pointer_position_slot(bool status);

    void active_touchpad_changed_slot(bool status);
    void disable_while_typing_clicked_slot(bool status);
    void touchpad_click_clicked_slot(bool status);
    void rolling_enable_clicked_slot(bool status);
    void rolling_kind_changed_slot(QAbstractButton * basebtn, bool status);

    void cursor_size_changed_slot();
};

#endif // MOUSECONTROL_H
