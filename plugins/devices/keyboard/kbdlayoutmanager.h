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
#ifndef KBDLAYOUTMANAGER_H
#define KBDLAYOUTMANAGER_H

#include <QWidget>
#include <QDialog>
#include <QX11Info>
#include <QGSettings>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif


typedef struct _Layout Layout;

struct _Layout{
    QString desc;
    QString name;
};

namespace Ui {
class LayoutManager;
}

class KbdLayoutManager : public QDialog
{
    Q_OBJECT

public:
    explicit KbdLayoutManager(QWidget *parent = 0);
    ~KbdLayoutManager();

    QString kbd_get_description_by_id(const char *visible);

    void kbd_trigger_available_countries(char * countryid);
    void kbd_trigger_available_languages(char * languageid);


    void configRegistry();
    void setupComponent();
    void setupConnect();
    void rebuildSelectListWidget();
    void rebuildVariantCombo();

    void rebuild_listwidget();
    void preview();

protected:
    void paintEvent(QPaintEvent * event);

private:
    Ui::LayoutManager *ui;

    QStringList layoutsList;

    QGSettings * kbdsettings;

};

#endif // KBDLAYOUTMANAGER_H
