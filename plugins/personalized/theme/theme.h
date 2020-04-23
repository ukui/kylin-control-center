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
#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QtPlugin>
#include <QLayout>

#include <QMap>
#include <QDir>

#include "shell/interface.h"

#include <QDir>
#include <QSettings>
#include <QtDBus/QDBusMessage>
#include <QGSettings/QGSettings>
#include <QtDBus/QtDBus>


class QPushButton;
class SwitchButton;
class QGSettings;
class WidgetGroup;

namespace Ui {
class Theme;
}

class Theme : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Theme();
    ~Theme();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;

public:

    void setupStylesheet();
    void setupSettings();
    void setupComponent();
    void initThemeMode();
    void initIconTheme();
    void setupControlTheme();
    void initCursorTheme();
    void initEffectSettings();
    void initConnection();

    void buildThemeModeBtn(QPushButton * button, QString name, QString icon);

    QString dullTranslation(QString str);

    QStringList _getSystemCursorThemes();

private:
    void clearLayout(QLayout* mlayout, bool deleteWidgets);

private:
    Ui::Theme *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    QGSettings * gtkSettings;
    QGSettings * qtSettings;
    QGSettings * curSettings;
    QSettings  * kwinSettings;

    QGSettings * kwinGsettings;

    SwitchButton * effectSwitchBtn;

    WidgetGroup * iconThemeWidgetGroup;

    bool settingsCreate;

private slots:
    void resetBtnClickSlot();
    void writeKwinSettings(bool change);

};

#endif // THEME_H

