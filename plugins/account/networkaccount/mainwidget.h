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
#include "itemlist.h"
#include "frameitem.h"
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include "editpushbutton.h"
#include "maindialog.h"
#include <QMessageBox>
#include "editpassdialog.h"
#include "configfile.h"
#include <QGraphicsSvgItem>
#include <QSvgWidget>
#include "dbushandleclient.h"
#include <QtDBus/QtDBus>
#include "tooltips.h"
#include <QDir>
#include "svghandler.h"
#include "blueeffect.h"

#define PATH "/.cache/kylinssoclient/All.conf"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit        MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    QLabel*         get_title();
    QLabel*         get_info();
    QWidget*        get_login_dialog();
    void            setshow(QWidget *w);
    void            init_gui();
    void            handle_conf();
    bool            judge_item(const QString &enable,const int &cur) const;
    void            handle_write(const int &on,const int &id);
    void            showDesktopNotify(const QString &message);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    ItemList       *m_itemList;
    FrameItem    *m_autoSyn;
    QLabel              *m_title;
    QLabel              *m_infoTab;
    Blueeffect          *m_blueEffect_sync;
    QPushButton     *m_exitCloud_btn;
    QWidget         *m_widgetContainer;
    QWidget         *m_infoTabWidget;
    QVBoxLayout     *m_vboxLayout;
    QPushButton     *m_openEditDialog_btn;
    EditPassDialog      *m_editDialog;
    QStackedWidget  *m_mainWidget;
    QWidget         *m_nullWidget;
    DbusHandleClient   *m_dbusClient;
    QPushButton         *m_login_btn;
    QLabel              *m_welcomeMsg;
    QSvgWidget              *m_welcomeImage;
    QVBoxLayout         *m_welcomeLayout;
    QVBoxLayout         *m_workLayout;
    QStackedWidget      *m_stackedWidget;
    QWidget             *m_nullwidgetContainer;
    QString             m_szCode = tr("Disconnected");
    QString             m_szConfPath;
    QStringList         m_szItemlist = {"wallpaper","ukui-screensaver","avatar","ukui-menu","ukui-panel","ukui-panel2",
                                        "mouse","touchpad","keyboard","shortcut","area","datetime","default-open",
                                        "notice","option","peony","boot","power","editor","terminal",
                                        "indicator-china-weather","kylin-video"};
    MainDialog*   m_mainDialog;
    QWidget             *m_infoWidget;
    QHBoxLayout         *m_infoLayout;
    QThread             *thread;
    bool                m_bAutoSyn = true;
    bool                m_bTokenValid = false;
    QTimer              *m_cLoginTimer;
    QTimer              *m_cSyncDelay;
    QString             m_szUuid;
    QTimer              *m_cRetry;
    QFileSystemWatcher m_fsWatcher;
    SVGHandler *m_svgHandler;
    Tooltips       *m_syncTooltips;
    QLabel          *m_syncTipsText;
    QHBoxLayout     *m_animateLayout;
    QHBoxLayout     *m_tipsLayout;
    QMap<QString,QString> m_itemMap;
    QStringList     m_keyInfoList;
    bool            __once__ = false;
    bool            __run__ = false;
    bool            m_bIsStopped = false;

public slots:
    void            neweditdialog();
    void            on_login_out();
    void            on_login();
    void            open_cloud();
    void            finished_load(const int &ret,const QString &m_szUuid);
    void            on_switch_button(const int &on,const int &id);
    void            on_auto_syn(const int &on,const int &id);
    void            download_files();
    void            push_files();
    void            download_over();
    void            push_over();
    void            setret_oss(const int &ret);
    void            setret_conf(const int &ret);
    void            setret_change(const int &ret);
    void            setret_logout(const int &ret);
    void            setret_man(const int &ret);
    void            setname(const QString &n);
    void            setret_check(const QString &ret);
    void            get_key_info(const QString &info);
signals:
    void dooss(const QString &m_szUuid);
    void doman();
    void dologout();
    void doconf();
    void dochange(const QString &name,const int &flag);
    void docheck();
};

#endif // CONFIG_LIST_WIDGET_H
