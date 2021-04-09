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

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <net/if.h>

const int version[3] = {1, 4, 1};

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {

    initMemoryAlloc();

    //系统版本号检测
    QProcess proc;
    QStringList option;
    option << "-c" << "lsb_release -r | awk -F'\t' '{print $2}'";
    proc.start("/bin/bash",option);
    proc.waitForFinished();
    QByteArray ar = proc.readAll().toStdString().c_str();
    m_confName = "All-" + ar.replace("\n","") + ".conf";
    m_szConfPath = QDir::homePath() + "/.cache/kylinId/" + m_confName;

    //麒麟ID客户端检测
    QProcess kylinIDProc;
    QStringList kIdOptions;
    kIdOptions << "-c" << "ps aux | grep kylin-id";
    kylinIDProc.start("/bin/bash",kIdOptions);
    kylinIDProc.waitForFinished(-1);
    QByteArray result = kylinIDProc.readAll();
    if (result.contains("/usr/bin/kylin-id")) {
        m_bIsKylinId = true;
    }

    //版本控制
    QProcess kylinssoVerProc;
    QStringList kylinssoOptions;
    kylinssoOptions << "-c" << "dpkg -l | grep kylin-sso-client | awk -F' ' '{print $3}'";
    kylinssoVerProc.start("/bin/bash", kylinssoOptions);
    kylinssoVerProc.waitForFinished(-1);
    QByteArray kRet = kylinssoVerProc.readAll();
    QByteArrayList versionNum = kRet.split('.');

    if (versionNum.size() == 3) {
        for(int i = 0;i < 3;i ++) {
            if (versionNum.at(i).toInt() < version[i]) {
                m_bIsOldBackEnds = true;
                break;
            } else if (versionNum.at(i).toInt() > version[i]) {
                m_bIsOldBackEnds = false;
                break;
            }
        }
    } else {
        m_bIsOldBackEnds = true;
    }


    if (m_bIsOldBackEnds) {
        QDBusConnection::systemBus().connect(QString("org.freedesktop.NetworkManager"), QString("/org/freedesktop/NetworkManager"), QString("org.freedesktop.NetworkManager"),
                                          "PropertiesChanged", this, SLOT(checkNetWork(QVariantMap)));
    }


    m_szUuid = QUuid::createUuid().toString();
    m_bTokenValid = false;
    init_gui();         //初始化gui

    initSignalSlots();
    if (m_bIsOldBackEnds) {
        isNetWorkOnline();
    }

    if (m_bIsOnline == false ||m_szCode == tr("Disconnected")) {
        m_autoSyn->get_swbtn()->setDisabledFlag(true);
        for (int i = 0;i < m_szItemlist.size(); i ++ ) {
            m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(true);
        }
    } else {
        m_autoSyn->get_swbtn()->setDisabledFlag(false);
        for (int i = 0;i < m_szItemlist.size(); i ++ ) {
            m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(false);
        }
        m_checkTimer->setSingleShot(true);
        m_checkTimer->setInterval(500);
        m_checkTimer->start();
    }
    layoutUI();
    dbusInterface();

    QFile tokenFile(QDir::homePath() + "/.cache/kylinId/token");
    if (tokenFile.exists() && tokenFile.size() > 1) {
        m_mainWidget->setCurrentWidget(m_widgetContainer);
    } else {
        m_mainWidget->setCurrentWidget(m_nullWidget);
    }
}

void MainWidget::checkNetStatus(bool status) {
    emit isOnline(status);
}

void MainWidget::checkNetWork(QVariantMap map) {
    QVariant ret = map.value("Connectivity");
    if (ret.toInt() == 0) {
        return ;
    }
    if (ret.toInt() != 1 && ret.toInt() != 3 ) {
        m_bIsOnline = true;
        m_autoSyn->get_swbtn()->setDisabledFlag(false);
        for (int i = 0;i < m_szItemlist.size(); i ++ ) {
            m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(false);
        }
        m_lazyTimer->setInterval(500);
        m_lazyTimer->setSingleShot(true);
        m_lazyTimer->start();

        return ;
    }
    m_bIsOnline = false;
    m_autoSyn->get_swbtn()->setDisabledFlag(true);
    for (int i = 0;i < m_szItemlist.size(); i ++ ) {
        m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(true);
    }
}

void MainWidget::isNetWorkOnline()
{
    QtConcurrent::run([=] {
        QVariant ret;
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.NetworkManager",
                                                              "/org/freedesktop/NetworkManager",
                                                              "org.freedesktop.NetworkManager",
                                                              "CheckConnectivity");
        QDBusMessage response =  QDBusConnection::systemBus().call(message);

        if (response.type() == QDBusMessage::ReplyMessage) {
            QDBusVariant value = qvariant_cast<QDBusVariant>(response.arguments().takeFirst());
            ret = value.variant();
            if (ret.isValid() == false) {
                ret = response.arguments().takeFirst();
                if (ret.toInt() != 3 && ret.toInt() != 1) {
                    emit isOnline(true);
                    return ;
                }
            }
        }
        emit isOnline(false);
        return ;
    });

}

void MainWidget::dbusInterface() {
    if (m_bIsKylinId) {
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedLogout", this, SLOT(finishedLogout(int))); //登出结果反馈
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedVerifyToken", this, SLOT(checkUserName(QString))); //用户凭据验证结果反馈
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedPassLogin", this, SLOT(loginSuccess(int)));//登录结果反馈
        QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinID/path"), QString("org.kylinID.interface"),
                                              "finishedPhoneLogin", this, SLOT(loginSuccess(int)));//登录结果反馈

        //登出接口调用
        connect(this, &MainWidget::kylinIdLogOut, this, [=] () {
            QDBusMessage message = QDBusMessage::createMethodCall("org.kylinID.service","/org/kylinID/path",
                                                                  "org.kylinID.interface",
                                                                  "logout");
            QDBusConnection::sessionBus().call(message);
            m_mainWidget->setCurrentWidget(m_nullWidget);
        });

        //验证Token接口调用
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
    if (!m_bIsOldBackEnds) {
        m_dbusClient->connectSignal("isOnline",this,SLOT(checkNetStatus(bool)));
    }
    connect(this, &MainWidget::docheck, m_dbusClient, [=]() {
        QList<QVariant> argList;
        m_szCode = m_dbusClient->callMethod("checkLogin",argList);

    });

    connect(m_dbusClient, &DBusUtils::infoFinished,this,[=] (const QString &name) {
        if (name != "0") {
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

    connect(this, &MainWidget::dochange, m_dbusClient, [=](QString name,bool flag) {
        QList<QVariant> argList;
        int var = flag ? 1 : 0;
        argList << name << var;
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
        if (ret == 504) {
            if (taskName == "logout") {
                m_mainWidget->setCurrentWidget(m_nullWidget);
            }
        }

        if (taskName == "logout") {
            m_szCode = "";
            m_autoSyn->set_change(0,"0");
            m_autoSyn->set_active(true);
            m_keyInfoList.clear();

            m_mainWidget->setCurrentWidget(m_nullWidget);
            __once__ = false;
            __run__ = false;
            m_bIsStopped = true;
            bIsLogging = false;
        }
    });

    connect(m_dbusClient, &DBusUtils::querryFinished, this , [=] (const QStringList &list) {
        QStringList keyList = list;
        m_isOpenDialog = false;
        if (m_szCode == "" || m_szCode =="201" || m_szCode == "203" ||
                m_szCode == "401" || m_szCode == "504" || m_szCode == "500" || m_szCode== "502" || m_szCode == tr("Disconnected")) {
            if (bIsLogging) {
                m_mainDialog->setnormal();
            }
            m_mainWidget->setCurrentWidget(m_nullWidget);
            return ;
        }
        if (m_cLoginTimer->isActive()) {
            m_cLoginTimer->stop();
        }
        QFile fileConf(m_szConfPath);
        if (m_pSettings != nullptr && fileConf.exists() && fileConf.size() > 1)
            m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
        else
            m_syncTimeLabel->setText(tr("Waiting for initialization..."));
        if (keyList.size() > 2) {
            if (m_bIsOnline == false) {
                showDesktopNotify(tr("Network can not reach!"));
                return ;
            }
            QList<QVariant> args;
            QFile file(QDir::homePath() + "/.cache/kylinId/keys");
            args << m_szCode;
            QString localDate;
            m_mainWidget->setCurrentWidget(m_widgetContainer);
            QFile fileFLag(QDir::homePath() + "/.config/gsettings-set/" + m_szCode + "/User_Save_Flag");
            if (fileFLag.exists() && fileFLag.open(QIODevice::ReadOnly)) {
                fileFLag.waitForReadyRead(-1);
                localDate = fileFLag.readAll().toStdString().c_str();
            }else {
                m_manTimer->setSingleShot(true);
                m_manTimer->setInterval(1000);
                m_manTimer->start();
                m_mainWidget->setCurrentWidget(m_widgetContainer);
                return;
            }
            if (localDate == keyList.at(0) || !file.exists()) {
                m_manTimer->setSingleShot(true);
                m_manTimer->setInterval(1000);
                m_manTimer->start();
            } else {
                m_autoSyn->make_itemoff();
                m_pSettings->setValue("Auto-sync/enable","false");
                m_pSettings->sync();
                m_bAutoSyn = false;
                m_syncDialog = new SyncDialog(m_szCode,m_szConfPath);
                m_syncDialog->m_List = keyList.isEmpty() ? m_szItemlist : keyList;
                connect(m_syncDialog, &SyncDialog::sendKeyMap, this,[=] (QStringList keyList) {
                    Q_UNUSED(keyList);
                    m_bAutoSyn = true;
                    m_autoSyn->make_itemon();
                    m_pSettings->setValue("Auto-sync/enable","true");
                    m_pSettings->sync();
                    m_syncDialog->close();
                    m_listTimer->setSingleShot(true);
                    m_listTimer->setInterval(1000);
                    m_listTimer->start();
                });

                connect(m_syncDialog, &SyncDialog::coverMode, this, [=] () {
                    m_bAutoSyn = true;
                    m_autoSyn->make_itemon();
                    m_pSettings->setValue("Auto-sync/enable","true");
                    m_pSettings->sync();
                    m_syncDialog->close();
                    m_manTimer->setSingleShot(true);
                    m_manTimer->setInterval(1000);
                    m_manTimer->start();
                });
                m_syncDialog->checkOpt();
                m_syncDialog->show();

            }
        } else {
            m_manTimer->setSingleShot(true);
            m_manTimer->setInterval(1000);
            m_manTimer->start();
        }
    });
}

void MainWidget::finishedLogout(int ret) {
    if (ret != 0 && ret != 401) {
        showDesktopNotify(tr("Logout failed,please check your connection"));
    }
}

void MainWidget::checkUserName(QString name) {
    m_szCode = name;
    if (name == "" || name =="201" || name == "203" || name == "401" || name == "500" || name == "502") {
        if (m_mainWidget->currentWidget() != m_nullWidget) {
            m_mainWidget->setCurrentWidget(m_nullWidget);
        }
        if (m_bIsKylinId) {
            emit kylinIdLogOut();
        } else {
            emit dologout();
        }
        return ;
    }

    m_autoSyn->get_swbtn()->setDisabled(false);
    m_autoSyn->get_swbtn()->setDisabledFlag(false);
    for(int itemCnt = 0;itemCnt < m_szItemlist.size(); itemCnt ++) {
        if (m_itemList->get_item(itemCnt) != nullptr) {
            m_itemList->get_item(itemCnt)->get_swbtn()->setDisabledFlag(false);
        }
    }

    m_pSettings = new QSettings(m_szConfPath,QSettings::IniFormat);
    m_pSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    m_infoTab->setText(tr("Your account：%1").arg(m_szCode));
    QFile fileConf(m_szConfPath);
    if (m_pSettings != nullptr && fileConf.exists())
        m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    else
        m_syncTimeLabel->setText(tr("Waiting for initialization..."));
    //setshow(m_mainWidget);
    if (m_bTokenValid == false) {
        if (m_mainWidget->currentWidget() != m_widgetContainer) {
            m_mainWidget->setCurrentWidget(m_widgetContainer);
        }
        QtConcurrent::run([=] () {

            QProcess proc;
            QStringList options;
            options << "-c" << "ps -ef|grep kylin-sso-client";
            proc.start("/bin/bash",options);
            proc.waitForFinished();
            QString ifn = proc.readAll();

            if (ifn.contains("/usr/bin/kylin-sso-client")) {
                emit isRunning();
            }
        });
    }
    m_autoSyn->set_change(0,"0");
    if (bIsLogging == false) {
        QFile file (m_szConfPath);
        QFile token (QDir::homePath() + "/.cache/kylinId/token");
        if (file.exists() == false && token.exists() == true && m_isOpenDialog == false && token.size() > 1) {
            emit dooss(m_szUuid);
        }
    }

    //dooss(m_szUuid);
    for (int i = 0;i < m_szItemlist.size();i ++) {
       m_itemList->get_item(i)->set_change(0,"0");
    }
    handle_conf();
}

void MainWidget::initMemoryAlloc() {
    m_dbusClient = new DBusUtils(this);    //创建一个通信客户端

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
    m_stackedWidget = new QStackedWidget(this);
    m_nullwidgetContainer = new QWidget(this);
    m_syncTimeLabel = new QLabel(this);
    m_cLoginTimer = new QTimer(this);
    m_lazyTimer = new QTimer(this);
    m_listTimer = new QTimer(this);
    m_singleTimer = new QTimer(this);
    m_checkTimer = new QTimer(this);
    m_manTimer = new QTimer(this);
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
    for (int btncnt = 0;btncnt < m_itemList->get_list().size();btncnt ++) {
        connect(m_itemList->get_item(btncnt), &FrameItem::itemChanged, this, [=] (const QString &name,bool checked) {
            if (m_bIsOldBackEnds) {
                isNetWorkOnline();
            }
            if (m_mainWidget->currentWidget() == m_nullWidget) {
                return ;
            }

            if (m_bIsOnline == false) {
                showDesktopNotify(tr("Network can not reach!"));
                return ;
            }
            emit dochange(m_itemMap.key(name),checked);
            if ( m_exitCloud_btn->property("on") == true || !m_bAutoSyn) {
                if (m_itemList->get_item_by_name(name)->get_swbtn()->getDisabledFlag() == true)
                    m_itemList->get_item_by_name(name)->set_active(false);
                else {
                    m_itemList->get_item_by_name(name)->set_active(true);
                }
                return ;
            } else if (checked == true && m_exitCloud_btn->property("on") == false && m_bAutoSyn){
                m_key = m_itemMap.key(name);
                if (m_key != "") {
                    //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
                    m_singleTimer->setSingleShot(true);
                    m_singleTimer->setInterval(1000);
                    m_singleTimer->start();
                }

            }
            if (m_itemMap.key(name) == "shortcut" && checked == true) {
                showDesktopNotify(tr("This operation may cover your settings!"));
            }
        });
    }

    connect(this, &MainWidget::isOnline, [=] (bool checked) {
        if (checked == true) {
            if(m_bIsOnline  == true) {
                m_checkTimer->setSingleShot(true);
                m_checkTimer->setInterval(500);
                m_checkTimer->start();
                return ;
            }
            m_bIsOnline = true;
            m_autoSyn->get_swbtn()->setDisabledFlag(false);
            for (int i = 0;i < m_szItemlist.size(); i ++ ) {
                m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(false);
            }
        } else {
            m_bIsOnline = false;
            m_autoSyn->get_swbtn()->setDisabledFlag(true);
            for (int i = 0;i < m_szItemlist.size(); i ++ ) {
                m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(true);
            }
        }
    });

    connect(this, &MainWidget::isSync, [=] (bool checked) {
        if (checked == false) {
            for(int itemCnt = 0;itemCnt < m_szItemlist.size(); itemCnt ++) {
                if (m_itemList->get_item(itemCnt) != nullptr) {
                    m_itemList->get_item(itemCnt)->get_swbtn()->setDisabled(false);
                }
            }
        } else {
            for(int itemCnt = 0;itemCnt < m_szItemlist.size(); itemCnt ++) {
                if (m_itemList->get_item(itemCnt) != nullptr) {
                    m_itemList->get_item(itemCnt)->get_swbtn()->setDisabled(true);
                }
            }
        }

    });

    connect(this,&MainWidget::oldVersion,[=] () {
        if (m_mainWidget->currentWidget() != m_nullWidget) {
            on_login_out();
        }
        m_login_btn->hide();
        m_welcomeMsg->setText(tr("The Cloud Account Service version is out of date!"));
    });

    //连接信号
    connect(m_mainWidget,&QStackedWidget::currentChanged,this,[this] (int index) {
       if (m_mainWidget->widget(index) == m_nullWidget) {
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

    connect(m_autoSyn->get_swbtn(),SIGNAL(checkedChanged(bool)),this,SLOT(on_auto_syn(bool)));
    connect(m_login_btn,SIGNAL(clicked()),this,SLOT(on_login()));
    connect(m_exitCloud_btn,SIGNAL(clicked()),this,SLOT(on_login_out()));

    connect(this,&MainWidget::isRunning,this,[=] {
        download_files();
    });
    //All.conf的
    QString tokenFile = QDir::homePath() + "/.cache/kylinId/token";
    m_fsWatcher.addPath(tokenFile);


    connect(&m_fsWatcher,&QFileSystemWatcher::fileChanged,this,[=] () {
        QFile token(tokenFile);
        if (!token.exists() || token.size() < 1) {
            if (m_mainWidget->currentWidget() != m_nullWidget) {
                m_mainWidget->setCurrentWidget(m_nullWidget);
            }
        } else {
            m_checkTimer->setSingleShot(true);
            m_checkTimer->setInterval(500);
            m_checkTimer->start();
        }
    });


    connect(m_singleTimer, &QTimer::timeout,this, [this] () {
        if (m_bIsOnline == false) {
            m_listTimer->stop();
            showDesktopNotify(tr("Network can not reach!"));
            return ;
        }
        if (m_key == "") {
            return ;
        }

        emit dosingle(m_key);
        m_singleTimer->stop();
    });

    connect(m_lazyTimer,&QTimer::timeout,this,[this] () {
       //emit doman();
        if (m_bIsKylinId) {
            emit kylinIdCheck();
        } else {
            emit docheck();
        }
        m_lazyTimer->stop();
    });

    connect(m_listTimer,&QTimer::timeout,this,[this] () {
        if (m_bIsOnline == false) {
            m_listTimer->stop();
            showDesktopNotify(tr("Network can not reach!"));
            return ;
        }
        emit doselect(m_syncDialog->m_List);
        m_listTimer->stop();
    });

    connect(m_checkTimer, &QTimer::timeout, this, [this] () {
        if (m_bIsKylinId) {
            emit kylinIdCheck();
        } else {
            emit docheck();
        }
        m_checkTimer->stop();
    });

    connect(m_manTimer, &QTimer::timeout, this, [this] () {
        if (m_bIsOnline == false) {
            m_manTimer->stop();
            showDesktopNotify(tr("Network can not reach!"));
            return ;
        }
        emit doman();
        m_manTimer->stop();
    });

    connect(m_stackedWidget, &QStackedWidget::currentChanged,this, [this] (int index) {
        Q_UNUSED(index);
        if (m_stackedWidget->currentWidget() == m_itemList) {
            setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        } else {
            setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        }
    });

    connect(m_autoSyn->get_swbtn(),&SwitchButton::checkedChanged, this, [=] (bool checked) {
       if (checked == true && m_pSettings != nullptr) {
           m_stackedWidget->setCurrentWidget(m_itemList);
           m_keyInfoList.clear();
           __once__ = false;

           m_autoSyn->set_change(0,"0");
           for (int i  = 0;i < m_szItemlist.size();i ++) {
               if (m_itemList->get_item(i)->get_swbtn()->getDisabledFlag() == true) {
                   m_itemList->get_item(i)->set_change(0,"0");
               }
           }
           QFile file( m_szConfPath);
           if (file.exists() == false) {
               if (m_bIsOnline == false) {
                   showDesktopNotify(tr("Network can not reach!"));
                   return ;
               }
               emit dooss(m_szUuid);
               return ;
           } else {
               if (m_bIsOnline == false) {
                   showDesktopNotify(tr("Network can not reach!"));
                   return ;
               }
               emit doquerry(m_szCode);
           }
       } else {
           m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
       }
    });
    connect(m_cLoginTimer,&QTimer::timeout,this,[this]() {
        qDebug() << "ssss";
        if (m_mainDialog == nullptr) {
            return ;
        }

        if (m_mainWidget->currentWidget()  == m_widgetContainer) {
        } else if (m_mainWidget->currentWidget() == m_nullWidget) {
            m_mainDialog->setnormal();
            on_login_out();
        }
        m_cLoginTimer->stop();
    });

}

/* 初始化GUI */
void MainWidget::init_gui() {
    //m_mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_exitCode->setFixedHeight(24);

    m_mainWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    m_login_btn->setFixedSize(180,36);
    m_welcomeMsg->setText(tr("Synchronize your personalized settings and data"));

    m_welcomeMsg->setStyleSheet("font-size:18px;");

    m_exitCloud_btn->setStyleSheet("QPushButton[on=true]{background-color:#3790FA;border-radius:4px;}");
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

    if (m_mainWidget->currentWidget() == m_nullWidget) {
        setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    } else {
        setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    }
    int cItem = 0;


    for (const QString &key : qAsConst(m_szItemlist)) {
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

        if (response.type() == QDBusMessage::ReplyMessage) {
            QVariant var =  response.arguments().takeFirst();
            if (var.toInt() != 0) {
                showDesktopNotify(tr("KylinID open error!"));
            }
        }
    } else {
        m_mainDialog = new MainDialog;
        m_mainDialog->setAttribute(Qt::WA_DeleteOnClose);
        //m_editDialog->m_bIsUsed = false;
        m_mainDialog->set_client(m_dbusClient);
        m_mainDialog->is_used = true;
        m_mainDialog->set_clear();
        m_exitCode->setText(" ");

        connect(m_mainDialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
        connect(m_mainDialog, &MainDialog::on_submit_clicked, this, [=] (){
            m_bIsStopped = false;
            qDebug() << "info";
            bIsLogging = true;
            m_cLoginTimer->setSingleShot(true);
            m_cLoginTimer->setInterval(10000);
            m_cLoginTimer->start();
        });
        connect(m_mainDialog,&MainDialog::on_login_failed,this, [this] () {
            m_cLoginTimer->stop();
            m_bIsStopped = true;
            bIsLogging = false;
        });
        m_mainDialog->show();
    }
}

/* 登录过程处理事件 */
void MainWidget::open_cloud() {
    if (m_bIsOnline == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    m_checkTimer->setSingleShot(true);
    m_checkTimer->setInterval(500);
    m_checkTimer->start();
    m_mainDialog->on_close();
    m_mainDialog = nullptr;
    bIsLogging = false;
    emit dooss(m_szUuid);
    //m_mainDialog->on_close();
}


void MainWidget::finished_conf(int ret) {
    if (m_bIsOnline == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    if (ret == 0) {
        m_bTokenValid = true;
        emit doquerry(m_szCode);
    }
}

/* 登录成功处理事件 */
void MainWidget::finished_load(int ret, QString uuid) {

    if (m_bIsOnline == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    if (ret == 301) {
        if (m_mainWidget->currentWidget() != m_nullWidget) {
            showDesktopNotify(tr("Unauthorized device or OSS falied.\nPlease retry or relogin!"));
            // m_exitCode->setText(tr("Please check your connection!"));
            return ;
        }
    }
    if (ret == 401 || ret == 201) {
        if (m_mainWidget->currentWidget() != m_nullWidget) {
            m_exitCode->setText(tr("Authorization failed!"));
            on_login_out();
            return ;
        }
    }
    if (uuid != this->m_szUuid) {
        return ;
    }
    m_bIsStopped = false;
    if (ret == 0) {
        m_autoSyn->set_change(0,"0");
        for (int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_change(0,"0");
        }
        emit doconf();
    }
}

/* 读取滑动按钮列表 */
void MainWidget::handle_conf() {
    if (__once__  || m_pSettings == nullptr) {
        return ;
    }

    if (m_pSettings != nullptr &&  m_pSettings->value("Auto-sync/enable").toString() == "true") {
        m_stackedWidget->setCurrentWidget(m_itemList);
        m_autoSyn->make_itemon();
        m_autoSyn->set_active(true);
        for (int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->make_itemon();
            m_itemList->get_item(i)->set_active(true);
        }
        m_bAutoSyn = true;
    } else {
        m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
        m_autoSyn->make_itemoff();
        m_bAutoSyn = false;
        for (int i  = 0;i < m_szItemlist.size();i ++) {
            judge_item( ConfigFile(m_szConfPath).Get(m_szItemlist.at(i),"enable").toString(),i);
        }
        for (int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(m_bAutoSyn);
        }
        return ;
    }
    for (int i  = 0;i < m_szItemlist.size();i ++) {
        judge_item(  ConfigFile(m_szConfPath).Get(m_szItemlist.at(i),"enable").toString(),i);
    }
}

/* 判断功能是否开启 */
bool MainWidget::judge_item(const QString &enable,const int &cur) const {
    if (enable == "true") {
        m_itemList->get_item(cur)->make_itemon();
    } else {
        m_itemList->get_item(cur)->make_itemoff();
    }
    return true;
}


/* 自动同步滑动按钮点击后改变功能状态 */
void MainWidget::on_auto_syn(bool checked) {
    if (m_bIsOldBackEnds) {
        isNetWorkOnline();
    }
    if (m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    //emit docheck();
    //m_bAutoSyn = on;
    for (int i  = 0;i < m_szItemlist.size();i ++) {
        m_itemList->get_item(i)->set_active(m_bAutoSyn);
    }

    if (m_bIsOnline == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return ;
    }
    emit dochange("Auto-sync",checked);
}

/* 登出处理事件 */
void MainWidget::on_login_out() {

    if (m_exitCloud_btn->property("on") == false)  {
        if (m_bIsKylinId) {
            emit kylinIdLogOut();
        } else {
            emit dologout();
        }
        m_bTokenValid = false;

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
    if (__once__ == true || m_pSettings == nullptr) {
        return ;
    }
    if (m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    //emit docheck();
    if (m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_exitCloud_btn->setText("");
        m_exitCloud_btn->setToolTip(tr("Stop sync"));
        m_blueEffect_sync->startmoive();
        emit isSync(true);
        //showDesktopNotify("同步开始");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());


    if (m_autoSyn->get_swbtn()->getDisabledFlag() == true) {
        return ;
    }
    m_autoSyn->set_change(1,"0");

}

void MainWidget::push_files() {

    if (__once__ == true) {
        return ;
    }

    if (m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
   // emit docheck();
    if (m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setText("");
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_exitCloud_btn->setToolTip(tr("Stop sync"));
        m_blueEffect_sync->startmoive();
        emit isSync(true);
       // showDesktopNotify("同步开始");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

    if (m_autoSyn->get_swbtn()->getDisabledFlag() == true) {
        return ;
    }
    m_autoSyn->set_change(1,"0");

}

void MainWidget::download_over() {
    //emit docheck();
    if (m_pSettings == nullptr) return;

    if (m_exitCloud_btn->property("on") == true) {
        m_blueEffect_sync->stop();
        m_exitCloud_btn->setText(tr("Exit"));
        m_exitCloud_btn->setProperty("on",false);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->setToolTip("");
        m_exitCloud_btn->update();
        m_bAutoSyn = true;
        emit isSync(false);
        //showDesktopNotify("同步结束");
    }
     m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    if (__once__ == false) {

        m_autoSyn->set_change(0,"0");
    }

}

void MainWidget::push_over() {
    //emit docheck();
     if (m_pSettings == nullptr) return;
    if (m_exitCloud_btn->property("on") == true) {
        m_blueEffect_sync->stop();
        m_exitCloud_btn->setText(tr("Exit"));
        m_exitCloud_btn->setProperty("on",false);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->setToolTip("");
        m_exitCloud_btn->update();
        m_bAutoSyn = true;
        emit isSync(false);
        //showDesktopNotify("同步结束");
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    if (__once__ == false) {
        m_autoSyn->set_change(0,"0");
    }
}

void MainWidget::get_key_info(QString info) {
    qDebug() << info;
    if (m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }

    if (info.contains("Upload")) {
        return ;
    }
    if (info == "Download") {
        return ;
    }

    bool bIsFailed = false;
    if (info.contains(",")) {
        m_keyInfoList = info.split(',');
    } else {
        m_keyInfoList << info;
    }

    if (m_keyInfoList.size() == 1) {
        m_autoSyn->set_change(-1,m_keyInfoList[0]);
        m_autoSyn->make_itemoff();
        for (int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
        emit dochange("Auto-sync",false);
        __once__ = true;
        return ;
    } else if (m_keyInfoList.size() > 1){
        bIsFailed = true;
    } else {
         m_autoSyn->set_change(0,"0");
         for (int i  = 0;i < m_szItemlist.size();i ++) {
             if (m_itemList->get_item(i)->get_swbtn()->getDisabledFlag() == false) {
                 m_itemList->get_item(i)->set_change(0,"0");
             }
         }
         return ;
    }

    //m_keyInfoList.size() > 1的情况
    //说明size大于2
    if (bIsFailed) {
        QString keys = "";
        for (QString key : m_keyInfoList) {
            if (key != m_keyInfoList.last()) {

                if (m_itemMap.value(key).isEmpty() == false) {
                    m_itemList->get_item_by_name(m_itemMap.value(key))->set_change(-1,"Failed!");
                    keys.append(tr("%1,").arg(m_itemMap.value(key)));
                }
            }
        }
        m_autoSyn->make_itemoff();
        for (int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
        m_autoSyn->set_change(-1,"Failed!");
        emit dochange("Auto-sync",false);
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
    <<QString("/usr/share/icons/ukui-icon-theme-default/scalable/apps/kylin-cloud-account.svg")
    <<tr("Cloud ID desktop message") //显示的是什么类型的信息
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWidget::loginSuccess(int ret) {
    if (m_bIsOnline == false) {
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
}
