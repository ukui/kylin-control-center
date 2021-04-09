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
#include "maindialog.h"
#include <QMessageBox>
#include "syncdialog.h"
#include <QSettings>
#include <QGraphicsSvgItem>
#include <QSvgWidget>
#include "dbusutils.h"
#include <QtDBus/QtDBus>
#include <QDir>
#include "configfile.h"
#include "svghandler.h"
#include "blueeffect.h"


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
    void            initMemoryAlloc();
    void            initSignalSlots();
    void            layoutUI();
    void            dbusInterface();
    void            handle_conf();
    bool            judge_item(const QString &enable,const int &cur) const;
    void            showDesktopNotify(const QString &message);
    void            isNetWorkOnline();
protected:
private:
    ItemList       *m_itemList;
    FrameItem    *m_autoSyn;
    QLabel              *m_title;
    QLabel              *m_infoTab;
    QLabel              *m_exitCode;
    Blueeffect          *m_blueEffect_sync;
    QPushButton     *m_exitCloud_btn;
    QWidget         *m_widgetContainer;
    QWidget         *m_infoTabWidget;
    QVBoxLayout     *m_vboxLayout;
    QStackedWidget  *m_mainWidget;
    QWidget         *m_nullWidget;
    DBusUtils   *m_dbusClient;
    QString             m_confName;
    QPushButton         *m_login_btn;
    QTimer              *m_lazyTimer;
    QTimer              *m_listTimer;
    QTimer              *m_singleTimer;
    QTimer              *m_manTimer;
    QTimer              *m_checkTimer;
    QLabel              *m_welcomeMsg;
    QSvgWidget              *m_welcomeImage;
    QVBoxLayout         *m_welcomeLayout;
    QVBoxLayout         *m_workLayout;
    QStackedWidget      *m_stackedWidget;
    QWidget             *m_nullwidgetContainer;
    QString             m_szCode = tr("Disconnected");
    QString             m_szConfPath;
    QStringList         m_szItemlist = {"wallpaper","ukui-screensaver","font","avatar","ukui-menu","ukui-panel","ukui-panel2",
                                        "themes","mouse","touchpad","keyboard","shortcut","area","datetime","default-open",
                                        "notice","option","peony","boot","power","editor","terminal",
                                        "indicator-china-weather","kylin-video"};
    MainDialog*   m_mainDialog;
    QWidget             *m_infoWidget;
    QHBoxLayout         *m_infoLayout;
    QThread             *thread;
    bool                m_bAutoSyn = true;
    bool                m_bTokenValid = false;
    bool                m_isOpenDialog = false;
    QTimer              *m_cLoginTimer;
    QString             m_szUuid;
    QFileSystemWatcher m_fsWatcher;
    SVGHandler *m_svgHandler;
    QHBoxLayout     *m_animateLayout;
    QMap<QString,QString> m_itemMap;
    QString         m_key;
    QStringList     m_keyInfoList;
    bool            __once__ = false;
    bool            __run__ = false;
    bool            m_bIsStopped = false;
    QLabel          *m_syncTimeLabel;
    int             m_indexChanged;
    bool             m_statusChanged;
    SyncDialog      *m_syncDialog;
    bool            bIsLogging = false;
    QSettings         *m_pSettings;
    bool            m_bIsKylinId = false;
    bool            m_bIsOnline = true;
    bool            m_bIsOldBackEnds = false;

public slots:
    void            on_login_out();
    void            on_login();
    void            open_cloud();
    void            finished_load(int ret,QString m_szUuid);
    void            finished_conf(int ret);
    void            on_auto_syn(bool checked);
    void            download_files();
    void            push_files();
    void            download_over();
    void            push_over();
    void            get_key_info(QString info);
    void            checkUserName(QString name);
    void            finishedLogout(int ret);
    void            loginSuccess(int ret);
    void            checkNetWork(QVariantMap map);
    void            checkNetStatus(bool status);
signals:
    void dooss(const QString &m_szUuid);
    void doman();
    void dologout();
    void doconf();
    void dochange(const QString &name,bool checked);
    void docheck();
    void dosingle(const QString &key);
    void doselect(QStringList keyList);
    void isRunning();
    void oldVersion();
    void doquerry(const QString &name);
    void dosend(const QString &info);

    void kylinIdLogOut();
    void kylinIdCheck();

    void isSync(bool checked);
    void isOnline(bool checked);
};

#endif // CONFIG_LIST_WIDGET_H
