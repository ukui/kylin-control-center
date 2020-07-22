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
#ifndef AREA_H
#define AREA_H

#include <QWidget>
#include <QObject>
#include <QtPlugin>
#include <QStyledItemDelegate>

#include "shell/interface.h"
#include "dataformat.h"
#include "HoverWidget/hoverwidget.h"
#include "ImageUtil/imageutil.h"

#include <QProcess>
#include <QDBusInterface>
#include <QDBusReply>
#include <QGSettings>
#include <QListView>


/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
}


namespace Ui {
class Area;
}

class Area : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Area();
    ~Area();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;

    QStringList readFile(const QString& filePath);

private:
    Ui::Area *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    QString objpath;

    QDBusInterface *m_areaInterface;

    QGSettings *m_gsettings = nullptr;
    QTimer *m_itimer = nullptr;
    QString hourformat;
    QString qss;

    HoverWidget *addWgt;

protected:
//    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initComponent();
    QStringList getUserDefaultLanguage();

private slots:
    void initFormatData();
    void run_external_app_slot();
    void change_language_slot(int);
    void change_area_slot(int);
    void datetime_update_slot();
    void add_lan_btn_slot();
    void changeform_slot();
};

#endif // AREA_H
