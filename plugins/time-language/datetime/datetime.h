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
#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>
#include <QtPlugin>


#include "shell/interface.h"
#include "changtime.h"

#include <QWidget>
#include <QLabel>
#include <QAbstractButton>

#include <QGSettings>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

#include "worldMap/timezonechooser.h"
#include "worldMap/zoneinfo.h"
#include "SwitchButton/switchbutton.h"


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


#define TZ_DATA_FILE "/usr/share/zoneinfo/zoneUtc"
#define DEFAULT_TZ "Asia/Shanghai"

namespace Ui {
class DateTime;
}

class DateTime : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    DateTime();
    ~DateTime();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

    void component_init();
    void status_init();

    bool fileIsExits(const QString& filepath);

private:
    Ui::DateTime *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

    QGSettings * m_formatsettings  = nullptr;


    QDBusInterface *m_datetimeiface = nullptr;
    QDBusInterface *m_datetimeiproperties = nullptr;

    QMap<QString, int> tzindexMapEn;
    QMap<QString, int> tzindexMapCN;


    SwitchButton *m_formTimeBtn = nullptr;
    QLabel *m_formTimeLabel = nullptr;
    QTimer * m_itimer = nullptr;

    TimeZoneChooser *m_timezone;
    ZoneInfo* m_zoneinfo;

    QDateTime current;

Q_SIGNALS:
    void changed();

private slots:
    void datetime_update_slot();
    void changetime_slot();
    void changezone_slot();
    void changezone_slot(QString );
    void time_format_clicked_slot(bool);
    void rsync_with_network_slot();
    void showendLabel();
    void hidendLabel();

private:
    void loadHour();
    void connectGSetting();
    QString getLocalTimezoneName(QString timezone, QString locale);

};

#endif // DATETIME_H
