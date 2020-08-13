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
#ifndef NOTICE_H
#define NOTICE_H

#include <QObject>
#include <QtPlugin>
#include <QPushButton>
#include <QDebug>
#include <QVector>

#include <QGSettings>

#include <shell/interface.h>
#include "SwitchButton/switchbutton.h"


//typedef _NoticeEntry NoticeEntry;
//struct _NoticeEntry : QObjectUserData{
////    int keyval;
//    QString gsSchema;
//    QString keyStr;
//};

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

QT_BEGIN_NAMESPACE
namespace Ui { class Notice; }
QT_END_NAMESPACE

class Notice : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Notice();
    ~Notice();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

    void setupComponent();
    void setupGSettings();
    void initNoticeStatus();
    void initOriNoticeStatus();
    void initGSettings();

private:
    void changeAppstatus(bool checked, QString name,SwitchButton *appBtn);
    void setHiddenNoticeApp(bool status);

private:
    Ui::Notice *ui;
    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    SwitchButton * newfeatureSwitchBtn;
    SwitchButton * enableSwitchBtn;
    SwitchButton * lockscreenSwitchBtn;       

    QMap<QString, bool> appMap;

    QGSettings * nSetting;
    QGSettings * oriSettings;
    QStringList appsName;
    QStringList appsKey;
    QVector<QGSettings*> *vecGsettins;
};
#endif // NOTICE_H
