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
#ifndef KEYBOARDCONTROL_H
#define KEYBOARDCONTROL_H

#include <QObject>
#include <QtPlugin>
#include <QStyledItemDelegate>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"
#include "HoverWidget/hoverwidget.h"
#include "ImageUtil/imageutil.h"

#include "kbdlayoutmanager.h"

namespace Ui {
class KeyboardControl;
}

class QGSettings;

class KeyboardControl : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    KeyboardControl();
    ~KeyboardControl() Q_DECL_OVERRIDE;

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;

    void setupStylesheet();
    void setupComponent();
    void setupConnect();
    void initGeneralStatus();
    void rebuildLayoutsComBox();
    void setKeyboardVisible(bool checked);

private:
    Ui::KeyboardControl *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    QGSettings * settings;
    QGSettings * kbdsettings;
    QGSettings * osdSettings;

    SwitchButton * keySwitchBtn;
    SwitchButton * tipKeyboardSwitchBtn;
    SwitchButton * numLockSwitchBtn;

    KbdLayoutManager * layoutmanagerObj;

    HoverWidget * addWgt;

    bool settingsCreate;
};

#endif // KEYBOARDCONTROL_H
