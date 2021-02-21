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
#include "mainwidget.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <sys/stat.h>
#include <QDesktopServices>
#include <QUrl>

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {

    initMemoryAlloc();

    QProcess proc;
    QStringList option;
    option << "-c" << "lsb_release -r | awk -F'\t' '{print $2}'";
    proc.start("/bin/bash",option);
    proc.waitForFinished();
    QByteArray ar = proc.readAll().toStdString().c_str();
    m_confName = "All-" + ar.replace("\n","") + ".conf";
    m_szConfPath = QDir::homePath() + "/.cache/kylinId/" + m_confName;

    QProcess kylinIDProc;
    QStringList kIdOptions;
    kIdOptions << "-c" << "ps aux | grep kylin-id";
    kylinIDProc.start("/bin/bash",kIdOptions);
    kylinIDProc.waitForFinished(-1);
    QByteArray result = kylinIDProc.readAll();
    if(result.contains("/usr/bin/kylin-id")) {
        m_bIsKylinId = true;
    }


    m_szUuid = QUuid::createUuid().toString();
    m_bHasNetwork = true;
    m_bTokenValid = false;


    init_gui();         //初始化gui

    initSignalSlots();
    layoutUI();
    dbusInterface();
}

void MainWidget::dbusInterface() {
    if(m_bIsKylinId) {
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedLogout", this, SLOT(finishedLogout(int)));
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedVerifyToken", this, SLOT(checkUserName(QString)));
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedPassLogin", this, SLOT(loginSuccess(int)));
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedPhoneLogin", this, SLOT(loginSuccess(int)));
        connect(this, &MainWidget::kylinIdLogOut, this, [=] () {
            QDBusMessage message = QDBusMessage::createMethodCall("org.kylinID.service","/org/kylinID/path",
                                                                  "org.kylinID.interface",
                                                                  "logout");
            QDBusConnection::sessionBus().call(message);
            m_mainWidget->setCurrentWidget(m_nullWidget);
        });

        connect(this, &MainWidget::kylinIdCheck, this, [=] () {
            QDBusMessage message = QDBusMessage::createMethodCall("org.kylinID.service","/org/kylinID/path",
                                                                  "org.kylinID.interface",
                                                                  "checkLogin");
            QDBusConnection::sessionBus().call(message);
        });
    }

    m_dbusClient->connectSignal("finished_init_oss",this,SLOT(finished_load(int,QString)));
    m_dbusClient->connectSignal("finishedConfLoad",this,SLOT(finished_conf(int)));
    m_dbusClient->connectSignal("backcall_start_download_signal",this,SLOT(download_files()));
    m_dbusClient->connectSignal("backcall_end_download_signal",this,SLOT(download_over()));
    m_dbusClient->connectSignal("backcall_start_push_signal",this,SLOT(push_files()));
    m_dbusClient->connectSignal("backcall_end_push_signal",this,SLOT(push_over()));
    m_dbusClient->connectSignal("backcall_key_info",this,SLOT(get_key_info(QString)));
    m_dbusClient->connectSignal("finishedVerifyToken",this,SLOT(checkUserName(QString)));
    m_dbusClient->connectSignal("finishedLogout",this,SLOT(finishedLogout(int)));
    connect(this, &MainWidget::docheck, m_dbusClient, [=]() {
        QList<QVariant> argList;
        m_szCode = m_dbusClient->callMethod("checkLogin",argList);

    });

    connect(m_dbusClient, &DBusUtils::infoFinished,this,[=] (const QString &name) {
        if(name != "0") {
            m_mainWidget->setCurrentWidget(m_nullWidget);
            return ;
        }
    });

    connect(this, &MainWidget::dooss, m_dbusClient, [=](QString uuid) {
        QList<QVariant> argList;
        argList << uuid;
        m_dbusClient->callMethod("init_oss",argList);
    });

    connect(this, &MainWidget::doconf, m_dbusClient, [=]() {
        QList<QVariant> argList;
        m_dbusClient->callMethod("init_conf",argList);
    });

    connect(this, &MainWidget::doman, m_dbusClient, [=]() {
        QList<QVariant> argList;
        m_dbusClient->callMethod("manual_sync",argList);
    });

    connect(this, &MainWidget::dochange, m_dbusClient, [=](QString name,int flag) {
        QList<QVariant> argList;
        argList << name << flag;
        m_dbusClient->callMethod("change_conf_value",argList);
    });

    connect(this, &MainWidget::doquerry, m_dbusClient, [=](QString name) {
        QList<QVariant> argList;
        argList << name;
        m_dbusClient->callMethod("querryUploaded",argList);
    });

    connect(this, &MainWidget::dosend, m_dbusClient, [=](QString info) {
        QList<QVariant>args;
        args<<info;
        m_dbusClient->callMethod("sendClientInfo",args);
    });


    connect(this, &MainWidget::dologout, m_dbusClient, [=]() {
        QList<QVariant> argList;
        m_dbusClient->callMethod("logout",argList);

    });

    connect(this, &MainWidget::dosingle, m_dbusClient, [=](QString key) {
        QList<QVariant> argList;
        argList << key;
        m_dbusClient->callMethod("single_sync",argList);
    });

    connect(this, &MainWidget::doselect, m_dbusClient, [=](QStringList keyList) {
        QList<QVariant> argList;
        argList << keyList;
        m_dbusClient->callMethod("selectSync",argList);
    });

    connect(m_dbusClient,&DBusUtils::taskFinished,this,[=] (const QString &taskName,int ret) {
        Q_UNUSED(taskName);
        if(ret == 504) {
            m_bHasNetwork = false;
            if(taskName == "logout") {
                m_mainWidget->setCurrentWidget(m_nullWidget);
            }
        } else {
            m_bHasNetwork = true;
        }

        if(taskName == "logout") {
            m_szCode = "";
            m_autoSyn->set_change(0,"0");
            m_autoSyn->set_active(true);
            m_keyInfoList.clear();

            m_mainWidget->setCurrentWidget(m_nullWidget);
            setshow(m_mainWidget);
            __once__ = false;
            __run__ = false;
            m_bIsStopped = true;
            bIsLogging = false;
        }
    });

    connect(m_dbusClient, &DBusUtils::querryFinished, this , [=] (const QStringList &list) {
        //qDebug() << "csacasacasca";
        QStringList keyList = list;
        m_isOpenDialog = false;
        if(m_szCode == "" || m_szCode =="201" || m_szCode == "203" ||
                m_szCode == "401" || m_szCode == "504" || m_szCode == "500" || m_szCode== "502" || m_szCode == tr("Disconnected")) {
            if(bIsLogging) {
                m_mainDialog->setnormal();
            }
            m_mainWidget->setCurrentWidget(m_nullWidget);
            return ;
        }
        if(m_cLoginTimer->isActive()) {
            m_cLoginTimer->stop();
        }
        QFile fileConf(m_szConfPath);
        if(m_pSettings != nullptr && fileConf.exists())
            m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
        else
            m_syncTimeLabel->setText(tr("Waiting for initialization..."));
        //qDebug() << "csacasacasca";
        if(keyList.size() > 2) {
            if(m_bHasNetwork == false) {
                showDesktopNotify(tr("Network can not reach!"));
                return ;
            }
            QList<QVariant> args;
            QFile file(QDir::homePath() + "/.cache/kylinId/keys");
            args << m_szCode;
            QString localDate;
            m_mainWidget->setCurrentWidget(m_widgetContainer);
            QFile fileFLag(QDir::homePath() + "/.config/gsettings-set/" + m_szCode + "/User_Save_Flag");
            if(fileFLag.exists() && fileFLag.open(QIODevice::ReadOnly)) {
                fileFLag.waitForReadyRead(-1);
                localDate = fileFLag.readAll().toStdString().c_str();
            }else {
                m_pSettings->setValue("Auto-sync/enable","true");
                emit closedialog();
                m_mainWidget->setCurrentWidget(m_widgetContainer);
                handle_conf();
                return;
            }
            if (localDate == keyList.at(0) || !file.exists()) {
                m_pSettings->setValue("Auto-sync/enable","true");
                emit closedialog();
                handle_conf();
            } else {
                m_autoSyn->make_itemoff();
                m_pSettings->sync();
                m_bAutoSyn = false;
                m_autoSyn->get_swbtn()->set_swichbutton_val(0);
                m_syncDialog = new SyncDialog(m_szCode,m_szConfPath);
                m_syncDialog->m_List = keyList.isEmpty() ? m_szItemlist : keyList;
                connect(m_syncDialog, &SyncDialog::sendKeyMap, this,[=] (QStringList keyList) {
                    Q_UNUSED(keyList);
                    m_autoSyn->make_itemon();
                    m_pSettings->setValue("Auto-sync/enable","true");
                    m_pSettings->sync();
                    m_bAutoSyn = true;
                    m_autoSyn->get_swbtn()->set_swichbutton_val(1);
                    emit doselect(keyList);
                    m_syncDialog->close();
                    handle_conf();
                });

                connect(m_syncDialog, &SyncDialog::coverMode, this, [=] () {
                    on_auto_syn(1,-1);
                    emit doman();
                    m_syncDialog->close();
                    handle_conf();
                });
                m_syncDialog->checkOpt();
                m_syncDialog->show();

            }
        } else {
            m_pSettings->setValue("Auto-sync/enable","true");
            emit closedialog();
            m_mainWidget->setCurrentWidget(m_widgetContainer);
            handle_conf();
        }
    });
    if(m_bIsKylinId) {
        emit kylinIdCheck();
    } else {
        emit docheck();
    }
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    m_dbusClient->moveToThread(thread);
    thread->start();    //线程开始
}

void MainWidget::finishedLogout(int ret) {
    if(ret != 0 && ret != 401) {
        showDesktopNotify(tr("Logout failed,please check your connection"));
    }
}

void MainWidget::checkUserName(QString name) {
    m_szCode = name;
    if(name == "" || name =="201" || name == "203" || name == "401" || name == "500" || name == "502") {
        m_mainWidget->setCurrentWidget(m_nullWidget);
        if(m_bIsKylinId) {
            emit kylinIdLogOut();
        } else {
            emit dologout();
        }
        return ;
    }
    m_pSettings = new QSettings(m_szConfPath,QSettings::IniFormat);
    m_pSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    m_infoTab->setText(tr("Your account：%1").arg(m_szCode));
    QFile fileConf(m_szConfPath);
    if(m_pSettings != nullptr && fileConf.exists())
        m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    else
        m_syncTimeLabel->setText(tr("Waiting for initialization..."));
    //setshow(m_mainWidget);
    if(m_bTokenValid == false) {
        m_mainWidget->setCurrentWidget(m_widgetContainer);
        QtConcurrent::run([=] () {

            QProcess proc;
            QStringList options;
            options << "-c" << "ps -ef|grep kylin-sso-client";
            proc.start("/bin/bash",options);
            proc.waitForFinished();
            QString ifn = proc.readAll();

            if(ifn.contains("/usr/bin/kylin-sso-client")) {
                emit isRunning();
            }
        });
    }
   // qDebug() << "ssssss";
    m_autoSyn->set_change(0,"0");
    if(bIsLogging == false) {
        QFile file (m_szConfPath);
        QFile token (QDir::homePath() + "/.cache/kylinId/token");
        if(file.exists() == false && token.exists() == true && m_isOpenDialog == false) {
            emit dooss(m_szUuid);
        }
    }

    //dooss(m_szUuid);
    for(int i = 0;i < m_szItemlist.size();i ++) {
        m_itemList->get_item(i)->set_change(0,"0");
    }
    handle_conf();
}

void MainWidget::initMemoryAlloc() {
    m_dbusClient = new DBusUtils;    //创建一个通信客户端
    thread  = new QThread();            //为创建的客户端做异步处理

    m_mainWidget = new QStackedWidget(this);

    m_vboxLayout = new QVBoxLayout;//整体布局
    m_infoTabWidget = new QWidget(this);//用户信息窗口
    m_widgetContainer = new QWidget(this);//业务逻辑窗口，包括用户信息以及同步
    m_infoWidget = new QWidget(this);//名字框
    m_itemList = new ItemList();//滑动按钮列表
    m_autoSyn = new FrameItem(this);//自动同步按钮
    m_title = new QLabel(this);//标题
    m_infoTab = new QLabel(m_infoWidget);//名字
    m_exitCloud_btn = new QPushButton(tr("Exit"),this);//退出按钮
    m_workLayout = new QVBoxLayout;//业务逻辑布局
    m_infoLayout = new QHBoxLayout;//信息框布局

    m_blueEffect_sync = new Blueeffect(m_exitCloud_btn); //同步动画
    m_exitCode = new QLabel(this);

    m_nullWidget = new QWidget(this);
    m_welcomeLayout = new QVBoxLayout;
    m_welcomeImage = new QSvgWidget(":/new/image/96_color.svg");
    m_welcomeMsg = new QLabel(this);
    m_login_btn  = new QPushButton(tr("Sign in"),this);
    m_svgHandler = new SVGHandler(this);
    m_syncTooltips = new Tooltips(m_exitCloud_btn);
    m_syncTipsText = new QLabel(m_syncTooltips);
    m_tipsLayout = new QHBoxLayout;
    m_stackedWidget = new QStackedWidget(this);
    m_nullwidgetContainer = new QWidget(this);
    m_syncTimeLabel = new QLabel(this);
    m_cLoginTimer = new QTimer(this);
    m_lazyTimer = new QTimer(this);
    m_listTimer = new QTimer(this);
    m_pSettings = nullptr;

    m_animateLayout = new QHBoxLayout;
}

void MainWidget::layoutUI() {
    QVBoxLayout *VBox_tab = new QVBoxLayout;
    QHBoxLayout *HBox_tab_sub = new QHBoxLayout;
    QHBoxLayout *HBox_tab_btn_sub = new QHBoxLayout;

    m_animateLayout->addWidget(m_blueEffect_sync);
    m_animateLayout->setMargin(0);
    m_animateLayout->setSpacing(0);
    m_animateLayout->setAlignment(Qt::AlignCenter);
    m_exitCloud_btn->setLayout(m_animateLayout);

    m_stackedWidget->addWidget(m_itemList);
    m_stackedWidget->addWidget(m_nullwidgetContainer);
    m_stackedWidget->setContentsMargins(0,0,0,0);

    m_tipsLayout->addWidget(m_syncTipsText);
    m_tipsLayout->setMargin(0);
    m_tipsLayout->setSpacing(0);
    m_tipsLayout->setAlignment(Qt::AlignCenter);
    m_syncTooltips->setLayout(m_tipsLayout);

    HBox_tab_sub->addWidget(m_title,0,Qt::AlignLeft);
    HBox_tab_sub->setMargin(0);
    HBox_tab_sub->setSpacing(0);

    m_infoLayout->addWidget(m_infoTab);
    m_infoLayout->setMargin(0);
    m_infoLayout->setSpacing(4);
    m_infoLayout->setAlignment(Qt::AlignCenter);
    m_infoWidget->setLayout(m_infoLayout);
    m_infoWidget->adjustSize();
    HBox_tab_btn_sub->addWidget(m_infoWidget,0,Qt::AlignLeft);
    HBox_tab_btn_sub->setMargin(0);
    HBox_tab_btn_sub->addWidget(m_exitCloud_btn,0,Qt::AlignRight);

    VBox_tab->addLayout(HBox_tab_sub);  //need fixing
    VBox_tab->setSpacing(16);
    VBox_tab->addSpacing(0);
    VBox_tab->setMargin(0);
    VBox_tab->addLayout(HBox_tab_btn_sub);
    m_infoTabWidget->setLayout(VBox_tab);
    m_infoTabWidget->setContentsMargins(0,0,0,0);
    m_widgetContainer->setMinimumWidth(550);

    m_syncTimeLabel->setText(tr("Waitting for sync!"));

    m_syncTimeLabel->setContentsMargins(20,0,0,0);

    m_workLayout->addWidget(m_infoTabWidget);
    m_workLayout->setSpacing(0);
    m_workLayout->setContentsMargins(1,0,1,0);
    m_workLayout->addSpacing(16);
    m_workLayout->addWidget(m_autoSyn->get_widget());
    m_workLayout->addSpacing(16);
    m_workLayout->addWidget(m_syncTimeLabel);
    m_workLayout->addSpacing(16);
    m_workLayout->addWidget(m_stackedWidget);
    m_widgetContainer->setLayout(m_workLayout);


    m_welcomeLayout->addSpacing(120);
    m_welcomeLayout->addWidget(m_welcomeImage,0,Qt::AlignCenter);
    m_welcomeLayout->setMargin(0);
    m_welcomeLayout->setSpacing(0);
    m_welcomeLayout->addSpacing(20);
    m_welcomeLayout->addWidget(m_welcomeMsg,0,Qt::AlignCenter);
    m_welcomeLayout->addSpacing(8);
    m_welcomeLayout->addWidget(m_exitCode,0,Qt::AlignCenter);
    m_welcomeLayout->addWidget(m_login_btn,0,Qt::AlignCenter);
    m_welcomeLayout->addStretch();
    m_welcomeLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    m_nullWidget->setLayout(m_welcomeLayout);
    m_nullWidget->adjustSize();
    m_mainWidget->addWidget(m_nullWidget);
    m_mainWidget->setCurrentWidget(m_nullWidget);
    m_vboxLayout->addWidget(m_mainWidget);
    m_vboxLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    this->setLayout(m_vboxLayout);

}

void MainWidget::initSignalSlots() {
    for(int btncnt = 0;btncnt < m_itemList->get_list().size();btncnt ++) {
        connect(m_itemList->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
    }

    connect(this,&MainWidget::oldVersion,[=] () {
        if(m_mainWidget->currentWidget() != m_nullWidget) {
            on_login_out();
        }
        m_login_btn->hide();
        m_welcomeMsg->setText(tr("The Cloud Account Service version is out of date!"));
    });

    //连接信号
    connect(m_mainWidget,&QStackedWidget::currentChanged,this,[this] (int index) {
       if(m_mainWidget->widget(index) == m_nullWidget) {
           setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
           download_over();
           m_mainWidget->adjustSize();
           adjustSize();
       } else {
           setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
           m_mainWidget->adjustSize();
           adjustSize();
       }
    });

    connect(m_autoSyn->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_auto_syn(int,int)));
    connect(m_login_btn,SIGNAL(clicked()),this,SLOT(on_login()));
    connect(m_exitCloud_btn,SIGNAL(clicked()),this,SLOT(on_login_out()));

    connect(this,&MainWidget::isRunning,this,[=] {
        download_files();
    });
    //All.conf的
    QString all_conf_path = QDir::homePath() + "/.cache/kylinId";
    m_fsWatcher.addPath(all_conf_path);


    connect(&m_fsWatcher,&QFileSystemWatcher::directoryChanged,this,[this] () {
        QFile conf( m_szConfPath);
        if(conf.exists() == true && m_pSettings != nullptr) {
            QFile fileConf(m_szConfPath);
            if(m_pSettings != nullptr && fileConf.exists())
                m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
            else
                m_syncTimeLabel->setText(tr("Waiting for initialization..."));
            if(m_autoSyn->get_swbtn()->get_active() == 1)
                handle_conf();
        }
    });


    connect(m_lazyTimer,&QTimer::timeout,this,[this] () {
       //emit doman();
        m_lazyTimer->stop();
    });

    connect(m_listTimer,&QTimer::timeout,this,[this] () {
        if(m_bHasNetwork == false) {
            m_listTimer->stop();
            showDesktopNotify(tr("Network can not reach!"));
            return ;
        }
        emit doselect(m_syncDialog->m_List);
        m_listTimer->stop();
    });

    connect(this,&MainWidget::closedialog,this,[this] () {
        if(m_bHasNetwork == false) {
            showDesktopNotify(tr("Network can not reach!"));
            return ;
        }
        emit doman();
    });

    connect(m_stackedWidget, &QStackedWidget::currentChanged,this, [this] (int index) {
        Q_UNUSED(index);
        if(m_stackedWidget->currentWidget() == m_itemList) {
            setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        } else {
            setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        }
    });

    connect(m_autoSyn->get_swbtn(),&SwitchButton::status,this ,[=] (int on,int id) {
        Q_UNUSED(id);
       if(on == 1 && m_pSettings != nullptr) {
           m_stackedWidget->setCurrentWidget(m_itemList);
           m_keyInfoList.clear();
           __once__ = false;

           m_autoSyn->set_change(0,"0");
           for(int i  = 0;i < m_szItemlist.size();i ++) {
               if(m_itemList->get_item(i)->get_swbtn()->get_swichbutton_val() == 1) {
                   m_itemList->get_item(i)->set_change(0,"0");
               }
           }
           QFile file( m_szConfPath);
           if(file.exists() == false) {
               if(m_bHasNetwork == false) {
                   showDesktopNotify(tr("Network can not reach!"));
                   return ;
               }
               emit dooss(m_szUuid);
               return ;
           }  else {
               if(m_bHasNetwork == false) {
                   showDesktopNotify(tr("Network can not reach!"));
                   return ;
               }
               emit doman();
           }
       } else {
           m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
       }
    });
}

/* 初始化GUI */
void MainWidget::init_gui() {
    //m_mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_syncTipsText->setText(tr("Stop sync"));
    m_exitCloud_btn->installEventFilter(this);
    m_exitCode->setFixedHeight(24);

    m_mainWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);


    m_syncTooltips->setFixedSize(86,44);

    m_login_btn->setFixedSize(180,36);
    m_welcomeMsg->setText(tr("Synchronize your personalized settings and data"));

    m_welcomeMsg->setStyleSheet("font-size:18px;");

    m_exitCloud_btn->setStyleSheet("QPushButton[on=true]{background-color:#3D6BE5;border-radius:4px;}");
    m_exitCloud_btn->setProperty("on",false);

    m_exitCloud_btn->setFixedSize(120,36);

    m_exitCode->setStyleSheet("QLabel{color:#F53547}");

    m_blueEffect_sync->settext(tr("Sync"));

    //控件初始化设置
    m_infoTabWidget->setFocusPolicy(Qt::NoFocus);
    m_title->setText(tr("Sync your settings"));
    m_title->setStyleSheet("font-size:18px;font-weight:500;");


    m_infoTab->setText(tr("Your account:%1").arg(m_szCode));
    m_autoSyn->set_itemname(tr("Auto sync"));
    m_autoSyn->make_itemon();
    m_autoSyn->get_swbtn()->set_id(m_szItemlist.size());
    m_widgetContainer->setFocusPolicy(Qt::NoFocus);
    m_mainWidget->addWidget(m_widgetContainer);

    //控件大小尺寸设置
    setContentsMargins(0,0,32,0);
    setMinimumWidth(550);
    m_infoTabWidget->resize(200,72);
    m_stackedWidget->adjustSize();
    m_autoSyn->get_widget()->setFixedHeight(50);
    m_infoTab->setFixedHeight(40);


    m_infoWidget->setFixedHeight(36);
    m_mainWidget->setMinimumWidth(550);
    m_widgetContainer->setMinimumWidth(550);
    m_welcomeImage->setFixedSize(96,96);

    m_key = "";
    m_exitCode->setText(" ");

    m_exitCloud_btn->setFocusPolicy(Qt::NoFocus);

    if(m_mainWidget->currentWidget() == m_nullWidget) {
        setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    } else {
        setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    }
    int cItem = 0;


    for(const QString &key : qAsConst(m_szItemlist)) {
        m_itemMap.insert(key,m_itemList->get_item(cItem)->get_itemname());
        cItem ++;
    }
    setMaximumWidth(960);
    m_welcomeMsg->adjustSize();
    m_itemList->adjustSize();
    m_stackedWidget->adjustSize();
    m_widgetContainer->adjustSize();
    m_mainWidget->adjustSize();
    adjustSize();
}

/* 打开登录框处理事件 */
void MainWidget::on_login() {
    m_isOpenDialog = true;
    if (m_bIsKylinId) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.kylinID.service","/org/kylinID/path",
                                                              "org.kylinID.interface",
                                                              "openKylinID");
        QDBusMessage response =  QDBusConnection::sessionBus().call(message);

        if(response.type() == QDBusMessage::ReplyMessage) {
            QVariant var =  response.arguments().takeFirst();
            if(var.toInt() != 0) {
                showDesktopNotify(tr("KylinID open error!"));
            }
        }
    } else {
        m_mainDialog = new MainDialog;
        m_mainDialog->setAttribute(Qt::WA_DeleteOnClose);
        //m_editDialog->m_bIsUsed = false;
        m_mainDialog->set_client(m_dbusClient,thread);
        m_mainDialog->is_used = true;
        m_mainDialog->set_clear();
        m_exitCode->setText(" ");

        connect(m_mainDialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
        connect(m_mainDialog,&MainDialog::on_login_success, this,[this] () {
            m_cLoginTimer->setSingleShot(true);
            m_cLoginTimer->setInterval(10000);
            m_cLoginTimer->start();
            m_bIsStopped = false;
            bIsLogging = true;
        });
        connect(m_mainDialog,&MainDialog::on_login_failed,this, [this] () {
            m_cLoginTimer->stop();
            m_bIsStopped = true;
            bIsLogging = false;
        });

        connect(m_cLoginTimer,&QTimer::timeout,m_mainWidget,[this]() {
            m_cLoginTimer->stop();
            if(m_bIsStopped) {
                return ;
            }

            if(m_mainWidget->currentWidget()  == m_widgetContainer) {
            } else if (m_mainWidget->currentWidget() == m_nullWidget) {
                m_mainDialog->setnormal();
                on_login_out();
            }
        });
        m_mainDialog->show();
    }
}

/* 登录过程处理事件 */
void MainWidget::open_cloud() {
    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    emit docheck();
    m_mainDialog->on_close();
    bIsLogging = false;
    emit dooss(m_szUuid);
    //m_mainDialog->on_close();
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_exitCloud_btn) {
        if(event->type() == QEvent::Enter && m_syncTooltips->isHidden() == true && m_exitCloud_btn->property("on") == true) {
            QPoint pos;
            pos.setX(m_exitCloud_btn->mapToGlobal(QPoint(0, 0)).x() + 34);
            pos.setY(m_exitCloud_btn->mapToGlobal(QPoint(0, 0)).y() + 34);
            m_syncTooltips->move(pos);
            m_syncTooltips->show();
        }
        if((event->type() == QEvent::Leave && m_syncTooltips->isHidden() == false) || m_exitCloud_btn->property("on") == false) {
            m_syncTooltips->hide();
        }
    }
    return QWidget::eventFilter(watched,event);
}

void MainWidget::finished_conf(int ret) {
    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    if(ret == 0) {
        m_pSettings->setValue("Auto-sync/enable","false");
        m_bTokenValid = true;
         emit doquerry(m_szCode);
    }
}

/* 登录成功处理事件 */
void MainWidget::finished_load(int ret, QString uuid) {

    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    //qDebug() << ret;
    if(ret == 301) {
        if(m_mainWidget->currentWidget() != m_nullWidget) {
            showDesktopNotify(tr("Unauthorized device or OSS falied.\nPlease retry or relogin!"));
            // m_exitCode->setText(tr("Please check your connection!"));
            return ;
        }
    }
    if(ret == 401 || ret == 201) {
        if(m_mainWidget->currentWidget() != m_nullWidget) {
            m_exitCode->setText(tr("Authorization failed!"));
            on_login_out();
            return ;
        }
    }
    if(uuid != this->m_szUuid) {
        return ;
    }
    m_bIsStopped = false;
    if (ret == 0) {
        m_autoSyn->set_change(0,"0");
        for(int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_change(0,"0");
        }
        m_autoSyn->make_itemoff();
        for(int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
        emit doconf();
    }
}

/* 读取滑动按钮列表 */
void MainWidget::handle_conf() {
    if(__once__  || m_pSettings == nullptr) {
        return ;
    }

    if( m_pSettings != nullptr &&  ConfigFile(m_szConfPath).Get("Auto-sync","enable").toString() == "true") {
        m_stackedWidget->setCurrentWidget(m_itemList);
        m_autoSyn->make_itemon();
        for(int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(true);
        }
        m_bAutoSyn = true;
    } else {
        m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
        m_autoSyn->make_itemoff();
        m_bAutoSyn = false;
        for(int i  = 0;i < m_szItemlist.size();i ++) {
            judge_item( ConfigFile(m_szConfPath).Get(m_szItemlist.at(i),"enable").toString(),i);
        }
        for(int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(m_bAutoSyn);
        }
        return ;
    }
    for(int i  = 0;i < m_szItemlist.size();i ++) {
        judge_item(  ConfigFile(m_szConfPath).Get(m_szItemlist.at(i),"enable").toString(),i);
    }
}

/* 判断功能是否开启 */
bool MainWidget::judge_item(const QString &enable,const int &cur) const {
    if(enable == "true") {
        m_itemList->get_item(cur)->make_itemon();
    } else {
        m_itemList->get_item(cur)->make_itemoff();
    }
    return true;
}

/* 滑动按钮点击后改变功能状态 */
void MainWidget::handle_write(const int &on,const int &id) {
    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    char name[32] = {0};
    if(id == -1) {

        strncpy(name,"Auto-sync",31);
    } else {
        strncpy(name,m_szItemlist[id].toStdString().c_str(),31);
    }
    m_statusChanged = on;
    m_indexChanged = id;
    emit dochange(name,on);
}

/* 滑动按钮点击处理事件 */
void MainWidget::on_switch_button(int on,int id) {
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    if( m_exitCloud_btn->property("on") == true || !m_bAutoSyn) {
        if(m_itemList->get_item(id)->get_swbtn()->get_swichbutton_val() == 1)
            m_itemList->get_item(id)->make_itemoff();
        else {
            m_itemList->get_item(id)->make_itemon();
        }
        return ;
    } else if(on == 1 && m_exitCloud_btn->property("on") == false && m_bAutoSyn){
        m_key = m_szItemlist.at(id);

        if(m_key != "") {
            if(m_bHasNetwork == false) {
                showDesktopNotify(tr("Network can not reach!"));
                return ;
            }
            //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
            emit dosingle(m_key);
        }

    }

    if(m_szItemlist.at(id) == "shortcut" && on == 1) {
        showDesktopNotify(tr("This operation may cover your settings!"));
    }
    //emit docheck();
    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    handle_write(on,id);
}

/* 自动同步滑动按钮点击后改变功能状态 */
void MainWidget::on_auto_syn(int on, int id) {
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    //emit docheck();
    //m_bAutoSyn = on;
    for(int i  = 0;i < m_szItemlist.size();i ++) {
        m_itemList->get_item(i)->set_active(m_bAutoSyn);
    }

    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    handle_write(on,-1);
}

/* 登出处理事件 */
void MainWidget::on_login_out() {

    if(m_exitCloud_btn->property("on") == false)  {
        if(m_bIsKylinId) {
            emit kylinIdLogOut();
        } else {
            emit dologout();
        }

    } else {
        emit dosend("exit");
        QProcess proc;
        proc.startDetached("killall kylin-sso-client");
        push_over();
    }

}


/* 动态布局显示处理函数 */
void MainWidget::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

QLabel* MainWidget::get_info() {
    return m_infoTab;
}

QLabel* MainWidget::get_title() {
    return m_title;
}

/* 同步回调函数集 */
void MainWidget::download_files() {
    if(__once__ == true || m_pSettings == nullptr) {
        return ;
    }
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    //emit docheck();
    if(m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_exitCloud_btn->setText("");
        m_blueEffect_sync->startmoive();
        //showDesktopNotify("同步开始");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());


    if(m_autoSyn->get_swbtn()->get_swichbutton_val() == 0) {
        return ;
    }
    m_autoSyn->set_change(1,"0");

}

void MainWidget::push_files() {

    if(__once__ == true) {
        return ;
    }

    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
   // emit docheck();
    if(m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setText("");
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_blueEffect_sync->startmoive();
       // showDesktopNotify("同步开始");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

    if(m_autoSyn->get_swbtn()->get_swichbutton_val() == 0) {
        return ;
    }
    m_autoSyn->set_change(1,"0");

}

void MainWidget::download_over() {
    //emit docheck();
    if(m_pSettings == nullptr) return;

    if(m_exitCloud_btn->property("on") == true) {
        m_blueEffect_sync->stop();
        m_exitCloud_btn->setText(tr("Exit"));
        m_exitCloud_btn->setProperty("on",false);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_bAutoSyn = true;
        //showDesktopNotify("同步结束");
    }
     m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    if(__once__ == false) {

        m_autoSyn->set_change(0,"0");
    }

}

void MainWidget::push_over() {
    //emit docheck();
     if(m_pSettings == nullptr) return;
    if(m_exitCloud_btn->property("on") == true) {
        m_blueEffect_sync->stop();
        m_exitCloud_btn->setText(tr("Exit"));
        m_exitCloud_btn->setProperty("on",false);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_bAutoSyn = true;
        //showDesktopNotify("同步结束");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    if(__once__ == false) {
        m_autoSyn->set_change(0,"0");
    }
}

void MainWidget::get_key_info(QString info) {
    qDebug() << info;
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }

    if(info.contains("Upload")) {
        return ;
    }
    if(info == "Download") {
        return ;
    }

    bool bIsFailed = false;
    //qDebug()<<"networkaccount:"+info;
    if(info.contains(",")) {
        m_keyInfoList = info.split(',');
    } else {
        m_keyInfoList << info;
    }

    if(m_keyInfoList.size() == 1) {
        m_autoSyn->set_change(-1,m_keyInfoList[0]);
        m_autoSyn->make_itemoff();
        for(int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
        handle_write(0,-1);
        __once__ = true;
        return ;
    } else if(m_keyInfoList.size() > 1){
        bIsFailed = true;
    } else {
         m_autoSyn->set_change(0,"0");
         for(int i  = 0;i < m_szItemlist.size();i ++) {
             if(m_itemList->get_item(i)->get_swbtn()->get_swichbutton_val() == 1) {
                 m_itemList->get_item(i)->set_change(0,"0");
             }
         }
         return ;
    }

    //m_keyInfoList.size() > 1的情况
    //说明size大于2
    if(bIsFailed) {
        QString keys = "";
        for(QString key : m_keyInfoList) {
            if(key != m_keyInfoList.last()) {

                if(m_itemMap.value(key).isEmpty() == false) {
                    m_itemList->get_item_by_name(m_itemMap.value(key))->set_change(-1,"Failed!");
                    keys.append(tr("%1,").arg(m_itemMap.value(key)));
                }
            }
        }

        //if(keys != "")
            //showDesktopNotify("同步这些项目失败：" + keys);

        m_autoSyn->make_itemoff();
        for(int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
        m_autoSyn->set_change(-1,"Failed!");
        handle_write(0,-1);
        __once__ = true;
    }
    m_keyInfoList.clear();
}

void MainWidget::showDesktopNotify(const QString &message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("Cloud ID desktop message") //显示的是什么类型的信息
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWidget::loginSuccess(int ret) {
    if(m_bHasNetwork == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    if (ret == 0) {
       emit kylinIdCheck();
       emit dooss(m_szUuid);
    }
}


/* 析构函数 */
MainWidget::~MainWidget() {

    m_fsWatcher.removePath(QDir::homePath() + "/.cache/kylinId/");
    delete m_itemList;
    m_itemList = nullptr;
    delete m_welcomeImage;
    m_welcomeImage = nullptr;
    delete m_dbusClient;
    m_dbusClient = nullptr;
    thread->requestInterruption();
    if(thread != nullptr)
    {
        thread->quit();
    }
    thread->wait();
}
