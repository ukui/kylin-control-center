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

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    m_dbusClient = new DbusHandleClient();    //创建一个通信客户端
    thread  = new QThread();            //为创建的客户端做异步处理
    m_dbusClient->moveToThread(thread);
    m_szUuid = QUuid::createUuid().toString();
    m_szConfPath = ConfigFile().GetPath();
    //m_configFile = new ConfigFile();
    connect(this,SIGNAL(dooss(QString)),m_dbusClient,SLOT(init_oss(QString)));
    connect(this,SIGNAL(docheck()),m_dbusClient,SLOT(check_login()));
    connect(this,SIGNAL(doconf()),m_dbusClient,SLOT(init_conf()));
    connect(this,SIGNAL(doman()),m_dbusClient,SLOT(manual_sync()));
    connect(this,SIGNAL(dochange(QString,int)),m_dbusClient,SLOT(change_conf_value(QString,int)));
    connect(this,SIGNAL(dologout()),m_dbusClient,SLOT(logout()));
    connect(this,SIGNAL(dosingle(QString)),m_dbusClient,SLOT(single_sync(QString)));
    connect(m_dbusClient,SIGNAL(finished_oss(int)),this,SLOT(setret_oss(int)));
    connect(m_dbusClient,SIGNAL(finished_check_oss(QString)),this,SLOT(setname(QString)));
    connect(m_dbusClient,SIGNAL(finished_check(QString)),this,SLOT(setret_check(QString)));
    connect(m_dbusClient,SIGNAL(finished_conf(int)),this,SLOT(setret_conf(int)));
    connect(m_dbusClient,SIGNAL(finished_man(int)),this,SLOT(setret_man(int)));
    connect(m_dbusClient,SIGNAL(finished_change(int)),this,SLOT(setret_change(int)));
    connect(m_dbusClient,SIGNAL(finished_logout(int)),this,SLOT(setret_logout(int)));
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    thread->start();    //线程开始
    m_mainWidget = new QStackedWidget(this);
    m_mainWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    emit docheck();     //检测是否登录
    init_gui();         //初始化gui
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_init_oss",this,SLOT(finished_load(int,QString)));
    //connect(client,SIGNAL(backcall_start_download_signal()),this,SLOT(download_files()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","backcall_start_download_signal",this,SLOT(download_files()));
    //connect(client,SIGNAL(backcall_end_download_signal()),this,SLOT(download_over()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","backcall_end_download_signal",this,SLOT(download_over()));
    //connect(client,SIGNAL(backcall_start_push_signal()),this,SLOT(push_files()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","backcall_start_push_signal",this,SLOT(push_files()));
    //connect(client,SIGNAL(backcall_end_push_signal()),this,SLOT(push_over()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","backcall_end_push_signal",this,SLOT(push_over()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","backcall_key_info",this,SLOT(get_key_info(QString)));
}
/* 检测第一次登录，为用户添加名字 */
void MainWidget::setname(QString n) {
    m_szCode = n;
    if(m_szCode != "" && m_szCode !="201" && m_szCode != "203" && m_szCode != "401" && !m_bTokenValid) {
        m_infoTab->setText(tr("Your account：%1").arg(m_szCode));
        m_mainWidget->setCurrentWidget(m_widgetContainer);
        //setshow(m_mainWidget);
        m_bTokenValid = true;              //开启登录状态
        m_autoSyn->set_change(0,"0");
        //dooss(m_szUuid);
        for(int i = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_change(0,"0");
        }

        m_dbusClient->m_bFirstAttempt = false;        //关闭第一次打开状态
        return ;
    }
}

/* 客户端回调函数集 */
void MainWidget::setret_oss(int ret) {
    if(ret == 0) {
        //qDebug()<<"init oss is 0";
    } else {
        //emit dologout();
    }
}

void MainWidget::setret_logout(int ret) {
    //do nothing
    //qDebug()<<ret<<"Coutner SatRieaf";
    if(ret == 0) {
        //m_mainDialog->set_back();
        m_mainWidget->setCurrentWidget(m_nullWidget);
        m_bIsStopped = true;
    }
}

void MainWidget::setret_conf(int ret) {
    //qDebug()<<ret<<"csacasca";
    if(ret == 0) {
        emit docheck();
        emit closedialog();
        //m_mainDialog->closedialog();
        //QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    } else {
        QtConcurrent::run([=] () {
            QProcess p;
            p.start("killall kylin-sso-client");
            p.waitForFinished();
            emit dologout();
        });
    }
}

void MainWidget::setret_man(int ret) {
    if(ret == 0) {
    }
}

void MainWidget::setret_check(QString ret) {
    //qDebug()<<ret<<"!ret_ok;";
    if((ret == "" || ret =="201" || ret == "203" || ret == "401" ) && m_bTokenValid) {
        //qDebug()<<"checked"<<ret<<ret;
        emit dologout();
        m_dbusClient->m_bFirstAttempt = true;
    } else if(!(ret == "" || ret =="201" || ret == "203" || ret == "401" ) &&!m_bTokenValid){
        m_bTokenValid = true;
        m_szCode = ret;
        //dooss(m_szUuid);
        m_infoTab->setText(tr("Your account：%1").arg(ret));
        m_mainWidget->setCurrentWidget(m_widgetContainer);
        //setshow(m_mainWidget);


        handle_conf();

        //QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    } else if((ret == "" || ret =="201" || ret == "203" || ret == "401" ) && m_bTokenValid == false){
        m_bTokenValid = true;
        m_mainWidget->setCurrentWidget(m_nullWidget);
       // setshow(m_mainWidget);
    } else if(!(ret == "" || ret =="201" || ret == "203" || ret == "401" ) && m_bTokenValid){
        dooss(m_szUuid);
        m_infoTab->setText(tr("Your account：%1").arg(ret));
        m_szCode = ret;
        m_mainWidget->setCurrentWidget(m_widgetContainer);
       // setshow(m_mainWidget);
        QFile all_conf_file(ConfigFile(m_szConfPath).GetPath());
        if(all_conf_file.exists() == false) {
            //doconf();
        } else {
            handle_conf();
        }

        //QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    }
}

void MainWidget::setret_change(int ret) {
    if(ret == 0) {
        // We will do nothing here
    } else {
        if(m_statusChanged == 1) {
            m_itemList->get_item(m_indexChanged)->make_itemoff();
            ConfigFile().Set(m_szItemlist.at(m_indexChanged),"enable",false);
        } else {
            m_itemList->get_item(m_indexChanged)->make_itemon();
            ConfigFile().Set(m_szItemlist.at(m_indexChanged),"enable",true);
        }
    }
}

/* 初始化GUI */
void MainWidget::init_gui() {
    //Allocator
    m_vboxLayout = new QVBoxLayout;//整体布局
    m_infoTabWidget = new QWidget(this);//用户信息窗口
    m_widgetContainer = new QWidget(this);//业务逻辑窗口，包括用户信息以及同步
    m_infoWidget = new QWidget(this);//名字框
    m_itemList = new ItemList();//滑动按钮列表
    //ld = new LoginDialog(this);
    m_autoSyn = new FrameItem(this);//自动同步按钮
    m_title = new QLabel(this);//标题
    m_infoTab = new QLabel(m_infoWidget);//名字
    m_exitCloud_btn = new QPushButton(tr("Exit"),this);//退出按钮
    m_workLayout = new QVBoxLayout;//业务逻辑布局
    //qDebug()<<"222222";
    //qDebug()<<"111111";;
    //qDebug()<<"000000";
    m_infoLayout = new QHBoxLayout;//信息框布局
    //gif = new QLabel(exit_page);//同步动画
    //pm = new QMovie(":/new/image/autosync.gif");
    m_blueEffect_sync = new Blueeffect(m_exitCloud_btn); //同步动画
    m_exitCode = new QLabel(this);
    m_blueEffect_sync->settext(tr("Sync"));

    m_animateLayout = new QHBoxLayout;
    m_animateLayout->addWidget(m_blueEffect_sync);
    m_animateLayout->setMargin(0);
    m_animateLayout->setSpacing(0);
    m_animateLayout->setAlignment(Qt::AlignCenter);
    m_exitCloud_btn->setLayout(m_animateLayout);

    QVBoxLayout *VBox_tab = new QVBoxLayout;
    QHBoxLayout *HBox_tab_sub = new QHBoxLayout;
    QHBoxLayout *HBox_tab_btn_sub = new QHBoxLayout;

    QString btns = "QPushButton {background: #E7E7E7;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                   "QPushButton:hover{color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                   "QPushButton:click{color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";

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

    //m_mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_stackedWidget->addWidget(m_itemList);
    m_stackedWidget->addWidget(m_nullwidgetContainer);
    m_stackedWidget->setContentsMargins(0,0,0,0);

    m_tipsLayout->addWidget(m_syncTipsText);
    m_tipsLayout->setMargin(0);
    m_tipsLayout->setSpacing(0);
    m_tipsLayout->setAlignment(Qt::AlignCenter);
    m_syncTooltips->setLayout(m_tipsLayout);
    m_syncTipsText->setText(tr("Stop sync"));
    m_exitCloud_btn->installEventFilter(this);
    m_exitCode->setFixedHeight(24);


    m_syncTooltips->setFixedSize(86,44);
    //    gif = new QLabel(status);
    //    gif->setWindowFlags(Qt::FramelessWindowHint);//无边框
    //    gif->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    //    pm = new QMovie(":/new/image/gif.gif");
    m_openEditDialog_btn = new EditPushButton(m_infoWidget);
    //login->setStyleSheet(btns);

    //控件初始化设置
    m_infoTabWidget->setFocusPolicy(Qt::NoFocus);
    m_title->setText(tr("Sync your settings"));
    m_title->setStyleSheet("font-size:18px;font-weight:500;");


    m_infoTab->setText(tr("Your account:%1").arg(m_szCode));
    m_infoTab->setStyleSheet("");
    //    status->setText(syn[0]);
    //    status->setProperty("objectName","status");  //give object a name
    //    status->setStyleSheet(qss_btn_str);
    //    status->setProperty("is_on",false);
    //    status->style()->unpolish(status);
    //    status->style()->polish(status);
    //    status->update();
    //gif->setStyleSheet("border-radius:4px;border:none;");
    m_autoSyn->set_itemname(tr("Auto sync"));
    m_autoSyn->make_itemon();
    m_autoSyn->get_swbtn()->set_id(m_szItemlist.size());
    m_widgetContainer->setFocusPolicy(Qt::NoFocus);
    m_openEditDialog_btn->setFixedSize(34,34);
    m_openEditDialog_btn->installEventFilter(this);
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

//    gif->setMinimumSize(120,36);
//    gif->setMaximumSize(120,36);
//    gif->resize(120,36);

    //布局
    HBox_tab_sub->addWidget(m_title,0,Qt::AlignLeft);
    HBox_tab_sub->setMargin(0);
    HBox_tab_sub->setSpacing(0);

    m_infoLayout->addWidget(m_infoTab);
    m_infoLayout->setMargin(0);
    m_infoLayout->setSpacing(4);
    m_infoLayout->addWidget(m_openEditDialog_btn);
    m_infoLayout->setAlignment(Qt::AlignBottom);
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

    m_syncTimeLabel->setText(tr("The latest time sync is: ") + ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

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

    m_login_btn->setFixedSize(180,36);
    m_openEditDialog_btn->setFlat(true);
    m_openEditDialog_btn->setStyleSheet("QPushButton{background:transparent;}");
    m_welcomeMsg->setText(tr("Synchronize your personalized settings and data"));

    m_welcomeMsg->setStyleSheet("font-size:18px;");

    m_exitCloud_btn->setStyleSheet("QPushButton[on=true]{background-color:#3D6BE5;border-radius:4px;}");
    m_exitCloud_btn->setProperty("on",false);

    m_exitCloud_btn->setFixedSize(120,36);

    m_exitCode->setStyleSheet("QLabel{color:#F53547}");


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
    m_vboxLayout->addWidget(m_mainWidget);
    m_vboxLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    this->setLayout(m_vboxLayout);

    m_key = "";


    m_exitCode->setText(" ");

    m_exitCloud_btn->setFocusPolicy(Qt::NoFocus);
    QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/edit.svg");
    m_openEditDialog_btn->setIcon(pixmap);
    QtConcurrent::run([=] () {

        for(int btncnt = 0;btncnt < m_itemList->get_list().size();btncnt ++) {
            connect(m_itemList->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
        }
        int cItem = 0;


        for(QString key : m_szItemlist) {
            m_itemMap.insert(key,m_itemList->get_item(cItem)->get_itemname());
            cItem ++;
        }

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

    if(m_mainWidget->currentWidget() == m_nullWidget) {
        setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    } else {
        setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    }

    //连接信号
    connect(m_mainWidget,&QStackedWidget::currentChanged,[this] (int index) {
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
    connect(m_openEditDialog_btn,SIGNAL(clicked()),this,SLOT(neweditdialog()));
    connect(m_exitCloud_btn,SIGNAL(clicked()),this,SLOT(on_login_out()));

    connect(this,&MainWidget::isRunning,this,[=] {
        download_files();
    });

    //All.conf的
    QString all_conf_path = QDir::homePath() + QString(PATH).remove("/All.conf");
    m_fsWatcher.addPath(all_conf_path);

    connect(&m_fsWatcher,&QFileSystemWatcher::directoryChanged,[this] () {
        m_syncTimeLabel->setText(tr("The latest time sync is: ") + ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
        QFile conf(ConfigFile(m_szConfPath).GetPath());
        if(conf.exists() == true) {
            handle_conf();
        }
    });

    connect(this,&MainWidget::closedialog,[this] () {
          m_mainDialog->on_close();
          qDebug() << "scscacsacsac============ascas";
          //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
          emit doman();
          qDebug() << "scscacsacsacsssssssssssssssascas";

    });

    connect(m_stackedWidget, &QStackedWidget::currentChanged, [this] (int index) {
        if(m_stackedWidget->currentWidget() == m_itemList) {
            setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        } else {
            setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        }
    });

    connect(m_autoSyn->get_swbtn(),&SwitchButton::status,[=] (int on,int id) {
       if(on == 1) {
           m_stackedWidget->setCurrentWidget(m_itemList);
           m_keyInfoList.clear();
           __once__ = false;

           m_autoSyn->set_change(0,"0");
           //qDebug() <<"ssssssssssssssssssssssssssssssssssss";
           for(int i  = 0;i < m_szItemlist.size();i ++) {
              // qDebug() << m_itemList->size();
               if(m_itemList->get_item(i)->get_swbtn()->get_swichbutton_val() == 1) {
                   m_itemList->get_item(i)->set_change(0,"0");
               }
           }
           //qDebug() <<"ssssssssssssssssssssssssssssssssssss";
           QFile file(ConfigFile(m_szConfPath).GetPath());
            //qDebug() <<"ssssssssssssssssssssssssssssssssssss";
           if(file.exists() == false) {
               // emit doconf();
           }  else {
               //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
               emit doman();
           }

       } else {
           m_stackedWidget->setCurrentWidget(m_nullwidgetContainer);
       }
    });
    //
    setMaximumWidth(960);
    m_welcomeMsg->adjustSize();
    m_itemList->adjustSize();
    m_stackedWidget->adjustSize();
    m_widgetContainer->adjustSize();
    m_mainWidget->adjustSize();
    adjustSize();
    qDebug() <<"ssssssssssssssssssssssssssssssssssss";
}

/* 打开登录框处理事件 */
void MainWidget::on_login() {
    m_mainDialog = new MainDialog;
    m_mainDialog->setAttribute(Qt::WA_DeleteOnClose);
    //m_editDialog->m_bIsUsed = false;
    m_mainDialog->set_client(m_dbusClient,thread);
    m_mainDialog->is_used = true;
    m_mainDialog->set_clear();
    m_exitCode->setText(" ");
    //qDebug()<<"login";

    connect(m_mainDialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
    connect(m_mainDialog,&MainDialog::on_login_success, [this] () {
        m_cLoginTimer->setSingleShot(true);
        m_cLoginTimer->setInterval(15000);
        m_cLoginTimer->start();
        m_bIsStopped = false;
    });
    connect(m_mainDialog,&MainDialog::on_login_failed,[this] () {
        m_cLoginTimer->stop();
        m_bIsStopped = true;
    });

    connect(m_mainDialog, &MainDialog::on_close_event, [this] () {
        m_cLoginTimer->stop();
        m_bIsStopped = true;
    });

    connect(m_cLoginTimer,&QTimer::timeout,[this]() {
        m_cLoginTimer->stop();
        if(m_bIsStopped) {
            return ;
        }

        if(m_mainWidget->currentWidget()  == m_widgetContainer) {
        } else if (m_mainWidget->currentWidget() == m_nullWidget) {
            m_mainDialog->setnormal();
            emit dologout();
        }
    });
    m_mainDialog->exec();
    qDebug() << "scscacsacsacascas";
}

/* 登录过程处理事件 */
void MainWidget::open_cloud() {
    emit dooss(m_szUuid);
    //m_mainDialog->on_close();
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_openEditDialog_btn) {
        if(event->type() == QEvent::Enter) {
            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/edit_hover.svg");
            m_openEditDialog_btn->setIcon(pixmap);
        }
        if(event->type() == QEvent::Leave) {
            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/edit.svg");
            m_openEditDialog_btn->setIcon(pixmap);
        }
    }
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

/* 登录成功处理事件 */
void MainWidget::finished_load(int ret, QString uuid) {
    //qDebug()<<"wb111"<<ret;
    if(ret == 301 || ret == 401 || ret == 201) {
        if(m_mainWidget->currentWidget() != m_nullWidget) {
            showDesktopNotify(tr("Unauthorized device or OSS falied.\nPlease retry for login!"));
            m_exitCode->setText(tr("Your account is sign on on other device already!"));
            on_login_out();
            return ;
        }
    }
    //qDebug()<<uuid<<this->m_szUuid;
    if(uuid != this->m_szUuid) {
        return ;
    }
    m_bIsStopped = false;
    //qDebug()<<"wb222"<<ret;
    if (ret == 0) {
        emit doconf();
    }
}

/* 读取滑动按钮列表 */
void MainWidget::handle_conf() {
    if(__once__ ) {
        return ;
    }


    if(ConfigFile(m_szConfPath).Get("Auto-sync","enable").toString() == "true") {
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
            judge_item(ConfigFile(m_szConfPath).Get(m_szItemlist[i],"enable").toString(),i);
        }
        for(int i  = 0;i < m_szItemlist.size();i ++) {
            m_itemList->get_item(i)->set_active(m_bAutoSyn);
        }
        return ;
    }
    for(int i  = 0;i < m_szItemlist.size();i ++) {
        judge_item(ConfigFile(m_szConfPath).Get(m_szItemlist[i],"enable").toString(),i);
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
    char name[32];
    if(id == -1) {
        qstrcpy(name,"Auto-sync");
    } else {
        qstrcpy(name,m_szItemlist[id].toStdString().c_str());
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

        m_bAutoSyn = false;

        if(m_key != "") {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
            emit dosingle(m_key);
        }

    }

    if(m_szItemlist.at(id) == "shortcut" && on == 1) {
        showDesktopNotify(tr("This operation may cover your settings!"));
    }
    //emit docheck();
    handle_write(on,id);
}

/* 自动同步滑动按钮点击后改变功能状态 */
void MainWidget::on_auto_syn(int on, int id) {
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }
    //emit docheck();
    m_bAutoSyn = on;
    for(int i  = 0;i < m_szItemlist.size();i ++) {
        m_itemList->get_item(i)->set_active(m_bAutoSyn);
    }
    handle_write(on,-1);
}

/* 登出处理事件 */
void MainWidget::on_login_out() {
    m_bTokenValid = false;
    m_dbusClient->m_bFirstAttempt = true;
    //qDebug()<< "wb777";
    emit dologout();
    //qDebug()<<"1213131";
    m_szCode = "";
    m_autoSyn->set_change(0,"0");
    m_autoSyn->set_active(true);
    m_keyInfoList.clear();

    m_mainWidget->setCurrentWidget(m_nullWidget);
    setshow(m_mainWidget);
    __once__ = false;
    __run__ = false;
    m_bIsStopped = true;
}

/* 修改密码打开处理事件 */
void MainWidget::neweditdialog() {
    //emit docheck();
    m_editDialog = new EditPassDialog;
    m_editDialog->setAttribute(Qt::WA_DeleteOnClose);
    m_editDialog->set_client(m_dbusClient,thread);
    m_editDialog->m_bIsUsed = true;
    //m_mainDialog->is_used = false;
    m_editDialog->set_clear();
    m_editDialog->m_szCode  = m_szCode;
    connect(m_editDialog,SIGNAL(account_changed()),this,SLOT(on_login_out()));
    m_editDialog->show();
    m_editDialog->raise();
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
    if(__once__ == true) {
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


    if(m_autoSyn->get_swbtn()->get_swichbutton_val() == 0) {
        return ;
    }
    m_autoSyn->set_change(1,"0");

}

void MainWidget::download_over() {
    //emit docheck();
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
    if(__once__ == false) {
        m_syncTimeLabel->setText(tr("The latest time sync is: ") + ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());

        m_autoSyn->set_change(0,"0");
    }

}

void MainWidget::push_over() {
    //emit docheck();
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
    ConfigFile files;
    if(__once__ == false) {
        m_syncTimeLabel->setText(tr("The latest time sync is: ") + ConfigFile(m_szConfPath).Get("Auto-sync","time").toString().toStdString().c_str());
        m_autoSyn->set_change(0,"0");
    }
}

void MainWidget::get_key_info(QString info) {
    qDebug() << info;
    if(m_mainWidget->currentWidget() == m_nullWidget) {
        return ;
    }

    if(info == "Upload") {
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


/* 析构函数 */
MainWidget::~MainWidget() {

    m_fsWatcher.removePath(QDir::homePath() + "/.cache/kylinId/");
    delete m_itemList;
    delete m_dbusClient;
    delete m_welcomeImage;
    if(thread)
    {
        thread->quit();
    }
    thread->wait();
}


