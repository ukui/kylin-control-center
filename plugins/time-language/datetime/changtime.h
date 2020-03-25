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
#ifndef CHANGTIME_H
#define CHANGTIME_H

#include <QDialog>
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QGSettings/QGSettings>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QStyledItemDelegate>
#include <QFile>
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

#define FORMAT_SCHEMA "org.ukui.panel.indicator.calendar"
#define TIME_FORMAT_KEY "use-24h-format"

namespace Ui {
class changtimedialog;
}

class ChangtimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangtimeDialog(bool hour, QWidget *parent = nullptr);
    ~ChangtimeDialog();

    void initUi();
    void initStatus();
    void hourComboxSetup();
    void ymdComboxSetup();

private:
    QTimer *m_chtimer = nullptr;
    Ui::changtimedialog *ui;

    QGSettings * m_formatsettings = nullptr;
    QDBusInterface *m_datetimeInterface = nullptr;
    bool m_isEFHour; //24小时制
    QString qss;        


private slots:
    void datetimeUpdateSlot();
    void dayUpdateSlot();
    void changtimeApplySlot();


};

#endif // CHANGTIME_H
