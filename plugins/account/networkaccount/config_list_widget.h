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
#ifndef CONFIG_LIST_WIDGET_H
#define CONFIG_LIST_WIDGET_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QMovie>
#include "item_list.h"
#include "network_item.h"
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include "ql_pushbutton_edit.h"
#include "dialog_login_reg.h"
#include <QMessageBox>
#include "editpassdialog.h"
#include "config_file.h"
#include <QGraphicsSvgItem>
#include <QSvgWidget>
#include "dbushandleclient.h"
#include <QtDBus/QtDBus>
#include "qtooltips.h"
#include <QDir>
#include "ql_svg_handler.h"

class config_list_widget : public QWidget
{
    Q_OBJECT
public:
    explicit        config_list_widget(QWidget *parent = nullptr);
    ~config_list_widget();
    QLabel*         get_title();
    QLabel*         get_info();
    QWidget*        get_login_dialog();
    void            setshow(QWidget *w);
    void            init_gui();
    void            handle_conf();
    bool            judge_item(QString enable,int cur);
    void            handle_write(int on,int id);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    item_list       *list;
    network_item    *auto_syn;
    QLabel          *title;
    QLabel          *info;
    QLabel          *gif;
    QMovie          *pm;
    QPushButton     *exit_page;
    LoginDialog     *ld;
    QWidget         *container;
    QWidget         *tab;
    QListWidgetItem *items[6];
    QVBoxLayout     *vboxlayout;
    QPushButton     *edit;
    QGraphicsDropShadowEffect *shadow;
    EditPassDialog      *edit_dialog;
    QStackedWidget  *stacked_widget;
    QWidget         *null_widget;
    DbusHandleClient   *client;
    QPushButton         *login;
    QLabel              *logout;
    QSvgWidget              *title2;
    QVBoxLayout         *vlayout;
    QVBoxLayout         *cvlayout;
    QString             code = tr("Disconnected");
    QString             home;
    QStringList         mapid = {"wallpaper","ukui-menu","ukui-panel","ukui-panel2","indicator-china-weather","kylin-video"};
    Dialog_login_reg*   login_dialog;
    QWidget             *namewidget;
    QHBoxLayout         *hbox;
    QThread             *thread;
    bool                auto_ok = true;
    bool                ret_ok = false;
    QTimer              *login_cloud;
    QTimer              *mansync;
    QString             uuid;
    QFileSystemWatcher fsWatcher;
    ql_svg_handler *svg_hd;
    QToolTips       *tooltips;
    QLabel          *texttips;

public slots:
    void            neweditdialog();
    void            on_login_out();
    void            on_login();
    void            open_cloud();
    void            finished_load(int ret,QString uuid);
    void            on_switch_button(int on,int id);
    void            on_auto_syn(int on,int id);
    void            download_files();
    void            push_files();
    void            download_over();
    void            push_over();
    void            setret_oss(int ret);
    void            setret_conf(int ret);
    void            setret_change(int ret);
    void            setret_logout(int ret);
    void            setret_man(int ret);
    void            setname(QString n);
    void            setret_check(QString ret);
signals:
    void dooss(QString uuid);
    void doman();
    void dologout();
    void doconf();
    void dochange(QString name,int flag);
    void docheck();
};

#endif // CONFIG_LIST_WIDGET_H
