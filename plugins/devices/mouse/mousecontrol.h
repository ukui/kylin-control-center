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
#include <QListView>
#include <QX11Info>
#include <QGSettings>
#include <QStyledItemDelegate>
#include <QProcess>
#include <QLabel>
#include <QStringList>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"

namespace Ui {
class MouseControl;
}

class MyLabel : public QLabel {
    Q_OBJECT
public:
    MyLabel();
    ~MyLabel();

public:
    QGSettings * mSettings;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

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
    QWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

    void initSearchText();
    void initStyle();
    void setupComponent();
    void initHandHabitStatus();
    void initPointerStatus();
    void initCursorStatus();
    void initWheelStatus();

private slots:
    void mouseSizeChange();
private:
    Ui::MouseControl *ui;

    QWidget * pluginWidget;

    SwitchButton * visiblityBtn;
    SwitchButton * flashingBtn;
    SwitchButton * mAccelBtn;


    QGSettings *settings;
    QGSettings *sesstionSetttings;
    QGSettings *desktopSettings;
    QGSettings *mThemeSettings;

    int pluginType;

    QString leftStr;
    QString rightStr;
    QString pluginName;

    QStringList mouseKeys;

    bool mFirstLoad;
};

#endif // MOUSECONTROL_H
