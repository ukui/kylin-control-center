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
    m_pSettings = new QSettings(m_szConfPath,QSettings::IniFormat);
    m_pSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));

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
    }
    
    layoutUI();
    dbusInterface();
    m_checkTimer->setSingleShot(true);
    m_checkTimer->setInterval(500);
    m_checkTimer->start();

    QFile tokenFile(QDir::homePath() + "/.cache/kylinId/token");
    if (tokenFile.exists() && tokenFile.size() > 1) {
        m_mainWidget->setCurrentWidget(m_widgetContainer);
    } else {
        m_mainWidget->setCurrentWidget(m_nullWidget);
        m_szCode = "-1";
    }

    checkBackEnd();
}

void MainWidget::checkNetStatus(bool status) {
    emit isOnline(status);
}

void MainWidget::checkNetWork(QVariantMap map) {

    QVariant ret = map.value("Connectivity");
    if (ret.toInt() == 0) {
        return ;
    }

    if (ret.toInt() != 1 && ret.toInt() != 3) {
        m_bIsOnline = true;
        m_autoSyn->get_swbtn()->setDisabledFlag(false);
        for (int i = 0;i < m_szItemlist.size(); i ++ ) {
            m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(false);
        }
        m_lazyTimer->setInterval(500);
        m_lazyTimer->setSingleShot(true);
        m_lazyTimer->start();
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
                    //emit isOnline(true);
                    return ;
                }
            }
        }
        //emit isOnline(false);
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
        m_dbusClient->callMethod("checkLogin",argList);

    });

    connect(m_dbusClient, &DBusUtils::infoFinished,this,[=] (const QString &name) {
        if (name != "0") {
            showDesktopNotify(tr("Network can not reach!"));
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
            QFile fileFLag(QDir::homePath() + "/.config/gsettings-set/" + m_szCode + "/User_Save_Flag");
            if (fileFLag.exists() && fileFLag.open(QIODevice::ReadOnly)) {
                fileFLag.waitForReadyRead(-1);
                localDate = fileFLag.readAll().toStdString().c_str();
            }else {
                m_manTimer->setSingleShot(true);
                m_manTimer->setInterval(1000);
                m_manTimer->start();
                emit isSync(true);
                return;
            }
            if (localDate == keyList.at(0) || !file.exists()) {
                m_manTimer->setSingleShot(true);
                m_manTimer->setInterval(1000);
                m_manTimer->start();
                emit isSync(true);
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
                    emit isSync(true);
                });
                m_syncDialog->checkOpt();
                m_syncDialog->show();

            }
        } else {
            m_manTimer->setSingleShot(true);
            m_manTimer->setInterval(1000);
            m_manTimer->start();
            emit isSync(true);
        }
    });
}

void MainWidget::finishedLogout(int ret) {
    if (ret != 0 && ret != 401) {
        showDesktopNotify(tr("Logout failed,please check your connection"));
    }
}

void MainWidget::checkBackEnd() {
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


void MainWidget::refreshSyncDate() {
    QFile fileConf(m_szConfPath);
    if (m_pSettings != nullptr && fileConf.exists())
        m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
    else
        m_syncTimeLabel->setText(tr("Waiting for initialization..."));
}

//更新用户信息，获取用户名
void MainWidget::checkUserName(QString name) {
    //检测登录状态
    if (name == "401") {
        m_firstLoad = true;
        //token无效执行登出
        if (m_mainWidget->currentWidget() != m_nullWidget) {
            if (m_bIsKylinId) {
                emit kylinIdLogOut();
            } else {
                emit dologout();
            }
        } else {
            //本身就在登出界面
            m_szCode = tr("Disconnected");
        }
        return ;
    }
    //其它由于网络问题登录状态错误，触发网络错误状态
    if (name == "" || name =="201" || name == "203" || name == "500" || name == "502") {
        ctrlAutoSync(NETWORK_FAILURE);
        return ;
    }
    //刚登录进来，进行OSS初始化处理
    if(m_szCode == tr("Disconnected") && m_firstLoad == true) {
        emit dooss(m_szUuid);
    }
    //当前用户名为用户名

    //这里要根据上次同步的情况来设置显示情况 to do
    if (m_pSettings->value("Auto-sync/run").toString() == "failed") {
        ctrlAutoSync(SYNC_FAILURE);
        m_bIsFailed = true;
    } else {
        m_bIsFailed = false;
        ctrlAutoSync(SYNC_NORMAL);
    }
    m_szCode = name;
    //设置用户名
    m_infoTab->setText(tr("Your account：%1").arg(m_szCode));
    //刷新同步时间
    refreshSyncDate();

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
    m_workLayout->addSpacing(4);
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

void MainWidget::singleExecutor(QTimer *timer, int mesc) {
    timer->setSingleShot(true);
    timer->setInterval(mesc);
    timer->start();
}

void MainWidget::setTokenWatcher() {
    QString tokenFile = QDir::homePath() + "/.cache/kylinId/token";
    m_fsWatcher.addPath(tokenFile);

    connect(&m_fsWatcher,&QFileSystemWatcher::fileChanged,this,[=] () {
        QFile token(tokenFile);
        //可能存在token为空的情况，故应该保证token.size()大于TOKEN_MIN_SIZE才能为有效token
        if (!token.exists() || token.size() < TOKEN_MIN_SIZE) {
            if (m_mainWidget->currentWidget() != m_nullWidget) {
                m_mainWidget->setCurrentWidget(m_nullWidget);
            }
        } else {
            singleExecutor(m_checkTimer,500);
        }
    });
}

bool MainWidget::isAvaliable() {
    if (m_bIsOnline == false) {
        showDesktopNotify(tr("Network can not reach!"));
        return false;
     }
    return true;
}

void MainWidget::initSignalSlots() {
    for (int btncnt = 0;btncnt < m_itemList->get_list().size();btncnt ++) {

        //同步项开关响应槽函数
        connect(m_itemList->get_item(btncnt), &FrameItem::itemChanged, this, [=] (const QString &name,bool checked) {
            if (m_bIsOldBackEnds) {
                isNetWorkOnline();
            }
            //这段代码是否需要值得商讨？
            if (m_mainWidget->currentWidget() == m_nullWidget) {
                return ;
            }
            //没有网络不做任何操作
            if (m_bIsOnline == false) {
                showDesktopNotify(tr("Network can not reach!"));
                return ;
            }
            //如果有同步正在运行，什么都不做。如果无同步运行，且打开按钮，则开启单项同步
            if ( m_exitCloud_btn->property("on") == true) {
                return ;
            } else if (checked == true && m_exitCloud_btn->property("on") == false){
                m_key = m_itemMap.key(name);
                if (m_key != "") {
                    //这样的执行顺序是正确的
                    singleExecutor(m_singleTimer,1000);
                }

            }
            //特殊同步项提示消息：快捷键
            if (m_itemMap.key(name) == "shortcut" && checked == true) {
                showDesktopNotify(tr("This operation may cover your settings!"));
            }
            emit dochange(m_itemMap.key(name),checked);
        });
    }

    //网络状态判断结果处理槽函数，参数为true说明有网，反之网络不通
    connect(this, &MainWidget::isOnline, [=] (bool checked) {
        //如果本身发生了错误，进入同步错误状态，则不处理，接受同步错误状态的处理
        bool tmpOnline = m_bIsOnline;

        if (m_mainWidget->currentWidget() == m_nullWidget) {
            return ;
        }

        m_bIsOnline = checked;
        if (m_bIsFailed) {
            if(checked == true) {

                ctrlAutoSync(SYNC_FAILURE);
            }
           return ;
        }
        //如果有网，进入正常状态
        if (checked == true ) {
            //本身就有网，之前没网，执行一次信息更新操作
            if(tmpOnline == false) {
                singleExecutor(m_checkTimer,500);
            }
            ctrlAutoSync(SYNC_NORMAL);

            //网络状态记录为有网状态，并进入正常状态


        } else {
            //检测到没网络，网络状态记录为无网状态，并进入无网状态
            if(tmpOnline == true) {
                showDesktopNotify(tr("Network can not reach!"));
                ctrlAutoSync(NETWORK_FAILURE);
            }
        }
        handle_conf();
    });

    //如果正在同步中，直接将开关按钮设置为失效
    connect(this, &MainWidget::isSync, [=] (bool checked) {
        if (checked == false) {
            m_autoSyn->get_swbtn()->setDisabled(false);
            for(int itemCnt = 0;itemCnt < m_szItemlist.size(); itemCnt ++) {
                if (m_itemList->get_item(itemCnt) != nullptr) {
                    m_itemList->get_item(itemCnt)->get_swbtn()->setDisabled(false);
                }
            }
        } else {
            m_autoSyn->get_swbtn()->setDisabled(true);
            for(int itemCnt = 0;itemCnt < m_szItemlist.size(); itemCnt ++) {
                if (m_itemList->get_item(itemCnt) != nullptr) {
                    m_itemList->get_item(itemCnt)->get_swbtn()->setDisabled(true);
                }
            }
        }

    });

    //检测是否为旧版本云账户处理槽函数
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

    //自动同步按钮槽函数
    connect(m_autoSyn->get_swbtn(),SIGNAL(checkedChanged(bool)),this,SLOT(on_auto_syn(bool)));
    //登录按钮槽函数
    connect(m_login_btn,SIGNAL(clicked()),this,SLOT(on_login()));
    //退出按钮槽函数
    connect(m_exitCloud_btn,SIGNAL(clicked()),this,SLOT(on_login_out()));

    //检测是否同步正在运行的槽函数
    connect(this,&MainWidget::isRunning,this,[=] {
        download_files();
    });
    //Token监控
    setTokenWatcher();


    connect(m_singleTimer, &QTimer::timeout,this, [this] () {
        if (isAvaliable() == false) {
            m_singleTimer->stop();
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
        if (isAvaliable() == false) {
            m_listTimer->stop();
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
        if (isAvaliable() == false) {
            m_manTimer->stop();
            return ;
        }
        m_pSettings->setValue("Auto-sync/run","done");
        m_pSettings->sync();
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

    connect(m_cLoginTimer,&QTimer::timeout,this,[this]() {
        if (m_mainDialog == nullptr) {
            return ;
        }

        if (m_mainWidget->currentWidget()  == m_widgetContainer) {
            m_cLoginTimer->stop();
            return ;
        } else if (m_mainWidget->currentWidget() == m_nullWidget) {
            m_mainDialog->setnormal();
            //on_login_out();
            QFile token(QDir::homePath() + "/.cache/kylinId/token");
            if (token.exists()) {
                token.remove();
            }
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
        m_mainDialog = new MainDialog(this);
        m_mainDialog->setAttribute(Qt::WA_DeleteOnClose);
        //m_editDialog->m_bIsUsed = false;
        m_mainDialog->set_client(m_dbusClient);
        m_mainDialog->is_used = true;
        m_mainDialog->set_clear();
        m_exitCode->setText(" ");

        //登录成功调用槽函数
        connect(m_mainDialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
        //点击登录调用槽函数
        connect(m_mainDialog, &MainDialog::on_submit_clicked, this, [=] (){
            //点击登录，超时计时开始，登录过程开始，登录状态为正在登录
            m_bIsStopped = false;
            bIsLogging = true;
            //超时10秒
            singleExecutor(m_cLoginTimer,10000);
        });
        connect(m_mainDialog,&MainDialog::on_login_failed,this, [this] () {
            //登录失败，超时计时结束，登录过程结束，登录状态为未登录
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
    //更新用户信息
    singleExecutor(m_checkTimer,500);
    //关闭登录窗口，回收内存
    m_mainDialog->on_close();
    m_mainDialog = nullptr;

    bIsLogging = false;
    //此时用户不能随意点击
    emit isSync(true);
    //切换界面
    m_mainWidget->setCurrentWidget(m_widgetContainer);
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

    if (isAvaliable() == false) {
        return ;
    }

    if (m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    if (ret == 301) {
        showDesktopNotify(tr("Unauthorized device or OSS falied.\nPlease retry or relogin!"));
        return ;
    }
    if (ret == 401 || ret == 201) {
        m_exitCode->setText(tr("Authorization failed!"));
        on_login_out();
        return ;
    }
    if (uuid != this->m_szUuid) {
        return ;
    }
    m_bIsStopped = false;
    if (ret == 0 ) {
        emit doconf();
    }
}

/*  读取滑动按钮列表
    @功能：    调用之后显示实时开关状态
    @参数：    无
    @返回类型： void
*/
void MainWidget::handle_conf() {
    if (__once__  || m_pSettings == nullptr) {
        return ;
    }
    bool ret = m_pSettings->value("Auto-sync/enable").toString() == "false";
    if (ret) {
        m_autoSyn->make_itemoff();
        //此时云端和本地开关本来就是关闭的，不可能进行同步
        emit isSync(false);
    } else {
        m_autoSyn->make_itemon();
    }
    for (int i  = 0;i < m_szItemlist.size();i ++) {
        judge_item(  ConfigFile(m_szConfPath).Get(m_szItemlist.at(i),"enable").toString(),i);
        if(ret) {
            m_itemList->get_item(i)->get_swbtn()->setDisabledFlag(true);
        }
    }

}

void MainWidget::ctrlAutoSync(int status) {
    if (status == NETWORK_FAILURE) {
        m_bIsFailed = false;
        m_autoSyn->set_active(false);

        for (int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
    } else if (status == SYNC_FAILURE) {
        m_bIsFailed = true;
        m_autoSyn->make_itemoff();
        m_pSettings->sync();
        m_autoSyn->set_active(true);
        m_autoSyn->set_change(-1,"Failed!");
        for (int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(false);
        }
    } else if (status == SYNC_NORMAL) {
        m_bIsFailed = false;
        m_autoSyn->set_active(true);
        m_autoSyn->set_change(0,"0");
        for (int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(true);
            m_itemList->get_item(i)->set_change(0,"0");
        }
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
    if (isAvaliable() == false) {
        return ;
     }
    if (checked == true) {
        m_keyInfoList.clear();

        //用户试图打开自动同步，将同步尝试设置为正常状态
        ctrlAutoSync(SYNC_NORMAL);

        //用户打开自动按钮开关，进行下载同步，要考虑到用户token有效，但是没有All.conf的情况出现
        QFile file( m_szConfPath);
        if (file.exists() == false) {
            emit dooss(m_szUuid);
            return ;
        } else {
            emit doquerry(m_szCode);
        }
        m_stackedWidget->setCurrentWidget(m_itemList);
    } else {
        m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
    }

    emit dochange("Auto-sync",checked);
}

/* 登出处理事件 */
void MainWidget::on_login_out() {
    //没有进行的同步，直接退出
    if (m_exitCloud_btn->property("on") == false)  {
        if (m_bIsKylinId) {
            emit kylinIdLogOut();
        } else {
            emit dologout();
        }
        m_szCode = tr("Disconnected");
        m_bTokenValid = false; //Token失效
        m_firstLoad = true;

    } else {
        //同步正在开始，结束同步
        QProcess proc;
        proc.startDetached("killall kylin-sso-client");
        push_over();
    }
}


QLabel* MainWidget::get_info() {
    return m_infoTab;
}

QLabel* MainWidget::get_title() {
    return m_title;
}

/* 同步回调函数集 */
void MainWidget::download_files() {
    if (m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_exitCloud_btn->setText("");
        m_exitCloud_btn->setToolTip(tr("Stop sync"));
        m_blueEffect_sync->startmoive();
        emit isSync(true);
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

}

void MainWidget::push_files() {
    if (m_exitCloud_btn->property("on") == false) {
        m_exitCloud_btn->setText("");
        m_exitCloud_btn->setProperty("on",true);
        m_exitCloud_btn->style()->unpolish(m_exitCloud_btn);
        m_exitCloud_btn->style()->polish(m_exitCloud_btn);
        m_exitCloud_btn->update();
        m_exitCloud_btn->setToolTip(tr("Stop sync"));
        m_blueEffect_sync->startmoive();
        emit isSync(true);
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +   ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

}

void MainWidget::download_over() {
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
    }
    if(m_bIsFailed == true) return ;
     m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

}

void MainWidget::push_over() {
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
    }
    m_syncTimeLabel->setText(tr("The latest time sync is: ") +  ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
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
    //如果存在多个同步项，则用逗号隔开，如果只有一个直接插入失败列表
    if (info.contains(",")) {
        m_keyInfoList = info.split(',');
    } else {
        m_keyInfoList << info;
    }
    //对失败同步项进行错误提示处理
    for (QString key : m_keyInfoList) {
        //如果key没被目前已知同步项列表包含，则跳过
        if (!m_szItemlist.contains(key)) {
            continue;
        }
        if (m_itemMap.value(key).isEmpty() == false) {
            m_itemList->get_item_by_name(m_itemMap.value(key))->set_change(-1,"Failed!");
        }
    }
    m_bIsFailed = true;
    if(m_bIsOnline == true)
        ctrlAutoSync(SYNC_FAILURE);
    else
        ctrlAutoSync(NETWORK_FAILURE);
    m_keyInfoList.clear();
}

void MainWidget::showDesktopNotify(const QString &message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(tr("Kylin Cloud Account"))
    <<((unsigned int) 0)
    <<QString("kylin-cloud-account")
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
       m_mainWidget->setCurrentWidget(m_widgetContainer);
       emit kylinIdCheck();
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
