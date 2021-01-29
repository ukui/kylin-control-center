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
#ifndef ADDAUTOBOOT_H
#define ADDAUTOBOOT_H

#include <QDialog>
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

namespace Ui {
class AddAutoBoot;
}

class AddAutoBoot : public QDialog
{
    Q_OBJECT

public:
    explicit AddAutoBoot(QWidget *parent = 0);
    ~AddAutoBoot();

    void resetBeforeClose();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::AddAutoBoot *ui;
    QString selectFile;
    QString mDesktopExec;
    QString mDesktopIcon;

private:
    void initStyle();
    void initConnection();

private slots:
    void open_desktop_dir_slots();
    void execLinEditSlot(const QString &fileName);

Q_SIGNALS:
    void autoboot_adding_signals(QString path, QString name, QString exec, QString comment, QString icon);
};

#endif // ADDAUTOBOOT_H
