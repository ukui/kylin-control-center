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
#ifndef PROXY_H
#define PROXY_H


#include <QObject>
#include <QtPlugin>

#include <QDialog>
#include <QLineEdit>
#include "certificationdialog.h"

#include <QGSettings>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

#include <glib.h>
#include <gio/gio.h>

struct GSData
{
    QString key;
    QString schema;
};


typedef enum{
    NONE,
    MANUAL,
    AUTO
}ProxyMode;

//自定义类型使用QVariant需要使用 Q_DECLARE_METATYPE 注册
Q_DECLARE_METATYPE(ProxyMode)
Q_DECLARE_METATYPE(GSData)

namespace Ui {
class Proxy;
}

class Proxy : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Proxy();
    ~Proxy();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initSearchText();
    void setupStylesheet();
    void setupComponent();
    void setupConnect();
    void initProxyModeStatus();
    void initAutoProxyStatus();
    void initManualProxyStatus();
    void initIgnoreHostStatus();

    void manualProxyTextChanged(QString txt);

    void showCertificationDialog();

    int _getCurrentProxyMode();
    void _setSensitivity();

private:
    Ui::Proxy *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

private:
    SwitchButton * autoSwitchBtn;
    SwitchButton * manualSwitchBtn;

    QGSettings * proxysettings;
    QGSettings * httpsettings;
    QGSettings * securesettings;
    QGSettings * ftpsettings;
    QGSettings * sockssettings;

    bool settingsCreate;


public slots:
    void proxyModeChangedSlot(bool checked);

};

#endif // PROXY_H
