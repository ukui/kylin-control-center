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
#include "config_list_widget.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <sys/stat.h>

config_list_widget::config_list_widget(QWidget *parent) : QWidget(parent) {
    client = new DbusHandleClient();    //创建一个通信客户端
    thread  = new QThread();            //为创建的客户端做异步处理
    client->moveToThread(thread);
    uuid = QUuid::createUuid().toString();
    connect(this,SIGNAL(dooss(QString)),client,SLOT(init_oss(QString)));
    connect(this,SIGNAL(docheck()),client,SLOT(check_login()));
    connect(this,SIGNAL(doconf()),client,SLOT(init_conf()));
    connect(this,SIGNAL(doman()),client,SLOT(manual_sync()));
    connect(this,SIGNAL(dochange(QString,int)),client,SLOT(change_conf_value(QString,int)));
    connect(this,SIGNAL(dologout()),client,SLOT(logout()));
    connect(client,SIGNAL(finished_oss(int)),this,SLOT(setret_oss(int)));
    connect(client,SIGNAL(finished_check_oss(QString)),this,SLOT(setname(QString)));
    connect(client,SIGNAL(finished_check(QString)),this,SLOT(setret_check(QString)));
    connect(client,SIGNAL(finished_conf(int)),this,SLOT(setret_conf(int)));
    connect(client,SIGNAL(finished_man(int)),this,SLOT(setret_man(int)));
    connect(client,SIGNAL(finished_change(int)),this,SLOT(setret_change(int)));
    connect(client,SIGNAL(finished_logout(int)),this,SLOT(setret_logout(int)));
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);

    thread->start();    //线程开始
    stacked_widget = new QStackedWidget(this);
    stacked_widget->resize(550,400);
    stacked_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
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
}
/* 检测第一次登录，为用户添加名字 */
void config_list_widget::setname(QString n) {
    //qDebug()<<n<<"2131231";
    code = n;
    if(code != "" && code !="201" && code != "203" && code != "401" && !ret_ok) {
        info->setText(tr("Your account：%1").arg(code));
        stacked_widget->setCurrentWidget(container);
        emit doconf();
        ret_ok = true;              //开启登录状态
        client->once = false;        //关闭第一次打开状态
        return ;
    }
}

/* 客户端回调函数集 */
void config_list_widget::setret_oss(int ret) {
    if(ret == 0) {
        emit docheck();
        //qDebug()<<"init oss is 0";
    } else {
        //emit dologout();
    }
}

void config_list_widget::setret_logout(int ret) {
    //do nothing
    if(ret == 0) {

    }
}

void config_list_widget::setret_conf(int ret) {
    //qDebug()<<ret<<"csacasca";
    if(ret == 0) {
        emit doman();
        QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
        //emit doman();
    } else {
        //emit dologout();
    }
}

void config_list_widget::setret_man(int ret) {
    if(ret == 0) {
        //emit doconf();
        //qDebug()<<"1111 manul";
    }
}

void config_list_widget::setret_check(QString ret) {
    code = ret;
    //qDebug()<<ret<<!ret_ok;
    if((code == "" || code =="201" || code == "203" || code == "401" ) && ret_ok) {
        //qDebug()<<"checked"<<code<<ret;
        emit dologout();
        client->once = true;
    } else if(!(code == "" || code =="201" || code == "203" || code == "401" ) &&!ret_ok){
        ret_ok = true;
        info->setText(tr("Your account：%1").arg(code));
        stacked_widget->setCurrentWidget(container);
        QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    } else if((code == "" || code =="201" || code == "203" || code == "401" ) && ret_ok == false){
        ret_ok = true;
    } else if(!(code == "" || code =="201" || code == "203" || code == "401" ) && ret_ok){
        info->setText(tr("Your account：%1").arg(code));
        stacked_widget->setCurrentWidget(container);
        QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    }
}

void config_list_widget::setret_change(int ret) {
    if(ret == 0) {
        //emit docheck();
    }
}

/* 初始化GUI */
void config_list_widget::init_gui() {
    //Allocator
    home = QDir::homePath() + "/.cache/kylinssoclient/All.conf"; //All.conf文件地址
    vboxlayout = new QVBoxLayout;//整体布局
    tab = new QWidget(this);//用户信息窗口
    container = new QWidget(this);//业务逻辑窗口，包括用户信息以及同步
    namewidget = new QWidget(this);//名字框
    list = new item_list;//滑动按钮列表
    //ld = new LoginDialog(this);
    auto_syn = new network_item(this);//自动同步按钮
    title = new QLabel(this);//标题
    info = new QLabel(namewidget);//名字
    exit_page = new QPushButton(tr("Exit"),this);//退出按钮
    cvlayout = new QVBoxLayout;//业务逻辑布局
    //qDebug()<<"222222";
    login_dialog = new Dialog_login_reg;//登录窗口
    //qDebug()<<"111111";
    edit_dialog = new EditPassDialog;//修改密码窗口
    //qDebug()<<"000000";
    hbox = new QHBoxLayout;//信息框布局
    gif = new QLabel(exit_page);//同步动画
    pm = new QMovie(":/new/image/autosync.gif");
    login_cloud = new QTimer(this);
    login_cloud->stop();

    gif->hide();
    edit_dialog->hide();
    login_dialog->hide();
    edit_dialog->set_client(client,thread);//安装客户端通信
    login_dialog->set_client(client,thread);
    QVBoxLayout *VBox_tab = new QVBoxLayout;
    QHBoxLayout *HBox_tab_sub = new QHBoxLayout;
    QHBoxLayout *HBox_tab_btn_sub = new QHBoxLayout;

    QString btns = "QPushButton {font-size:14px;background: #E7E7E7;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                   "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                   "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";

    null_widget = new QWidget(this);
    vlayout = new QVBoxLayout;
    title2 = new QSvgWidget(":/new/image/96_color.svg");
    logout = new QLabel(this);
    login  = new QPushButton(tr("Sign in"),this);

    //    gif = new QLabel(status);
    //    gif->setWindowFlags(Qt::FramelessWindowHint);//无边框
    //    gif->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    //    pm = new QMovie(":/new/image/gif.gif");
    edit = new ql_pushbutton_edit(namewidget);
    //login->setStyleSheet(btns);

    //控件初始化设置
    tab->setFocusPolicy(Qt::NoFocus);
    title->setText(tr("Sync your settings"));
    title->setStyleSheet("font-size:18px;font-weight:500;");


    info->setText(tr("Your account:%1").arg(code));
    info->setStyleSheet("font-size:14px;");
    //    status->setText(syn[0]);
    //    status->setProperty("objectName","status");  //give object a name
    //    status->setStyleSheet(qss_btn_str);
    //    status->setProperty("is_on",false);
    //    status->style()->unpolish(status);
    //    status->style()->polish(status);
    //    status->update();
    //gif->setStyleSheet("border-radius:4px;border:none;");

    auto_syn->set_itemname(tr("Auto sync"));
    auto_syn->make_itemon();
    auto_syn->get_swbtn()->set_id(mapid.size());
    container->setFocusPolicy(Qt::NoFocus);
    edit->setStyleSheet("QPushButton{border-style: flat;"
                        "background-image:url(:/new/image/edit.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;}"
                        "QPushButton:hover{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;"
                        "border-radius:4px}"
                        "QPushButton:click{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;border-radius:4px}");
    edit->installEventFilter(this);
    stacked_widget->addWidget(container);

    //控件大小尺寸设置
    setContentsMargins(0,0,0,0);
    setMinimumWidth(550);
    tab->resize(200,72);
    stacked_widget->adjustSize();
    list->resize(550,container->size().height());
    auto_syn->get_widget()->setFixedHeight(50);
    info->setFixedHeight(40);

    tab->setSizeIncrement(QSize(size().width(),1));
    container->setSizeIncrement(QSize(size().width(),size().height()));
    list->setSizeIncrement(QSize(size().width(),size().height()));

    namewidget->setFixedHeight(36);
    title2->setFixedSize(96,96);

//    gif->setMinimumSize(120,36);
//    gif->setMaximumSize(120,36);
//    gif->resize(120,36);

    //布局
    HBox_tab_sub->addWidget(title,0,Qt::AlignLeft);
    HBox_tab_sub->setMargin(0);
    HBox_tab_sub->setSpacing(0);

    hbox->addWidget(info);
    hbox->setMargin(0);
    hbox->setSpacing(4);
    hbox->addWidget(edit);
    hbox->setAlignment(Qt::AlignBottom);
    namewidget->setLayout(hbox);
    namewidget->adjustSize();
    HBox_tab_btn_sub->addWidget(namewidget,0,Qt::AlignLeft);
    HBox_tab_btn_sub->setMargin(0);
    HBox_tab_btn_sub->addWidget(exit_page,0,Qt::AlignRight);

    VBox_tab->addLayout(HBox_tab_sub);  //need fixing
    VBox_tab->setSpacing(16);
    VBox_tab->addSpacing(0);
    VBox_tab->setMargin(0);
    VBox_tab->addLayout(HBox_tab_btn_sub);
    tab->setLayout(VBox_tab);
    tab->setContentsMargins(0,0,0,0);



    cvlayout->addWidget(tab);
    cvlayout->setSpacing(0);
    cvlayout->setContentsMargins(1,0,1,0);
    cvlayout->addSpacing(16);
    cvlayout->addWidget(auto_syn->get_widget());
    cvlayout->addSpacing(16);
    cvlayout->addWidget(list);
    container->setLayout(cvlayout);

    login->setFixedSize(180,36);

    null_widget->resize(550,892);
    logout->setText(tr("Synchronize your personalized settings and data"));
    logout->setStyleSheet("font-size:18px;");

    exit_page->setProperty("on",false);

    exit_page->setFixedSize(120,36);


    vlayout->addSpacing(120);
    vlayout->addWidget(title2,0,Qt::AlignCenter);
    vlayout->setMargin(0);
    vlayout->setSpacing(0);
    vlayout->addSpacing(20);
    vlayout->addWidget(logout,0,Qt::AlignCenter);
    vlayout->addSpacing(32);
    vlayout->addWidget(login,0,Qt::AlignCenter);
    vlayout->addStretch();
    vlayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    null_widget->setLayout(vlayout);
    null_widget->adjustSize();
    stacked_widget->addWidget(null_widget);
    vboxlayout->addWidget(stacked_widget);
    vboxlayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    this->setLayout(vboxlayout);
    logout->adjustSize();
    list->adjustSize();
    container->adjustSize();


    exit_page->setFocusPolicy(Qt::NoFocus);

    //连接信号
    connect(auto_syn->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_auto_syn(int,int)));
    connect(login,SIGNAL(clicked()),this,SLOT(on_login()));
    connect(edit,SIGNAL(clicked()),this,SLOT(neweditdialog()));
    connect(exit_page,SIGNAL(clicked()),this,SLOT(on_login_out()));
    connect(edit_dialog,SIGNAL(account_changed()),this,SLOT(on_login_out()));
    connect(login_dialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
    connect(login_dialog->get_login_submit(),&QPushButton::clicked, [this] () {
        login_cloud->start();
    });
    connect(login_cloud,SIGNAL(timeout()),login_dialog,SLOT(set_back()));
    for(int btncnt = 0;btncnt < list->get_list().size();btncnt ++) {
        connect(list->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
    }

    struct stat buffer;
    char conf_path[512]={0};
    //All.conf的
    QString all_conf_path = QDir::homePath() + "/.cache/kylinssoclient/All.conf";
    qstrcpy(conf_path,all_conf_path.toStdString().c_str());

    //
    if(stat(conf_path, &buffer) == 0) {
        QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
    }
    setMaximumWidth(960);
    adjustSize();
}

/* 打开登录框处理事件 */
void config_list_widget::on_login() {
    edit_dialog->is_used = false;
    login_dialog->is_used = true;
    login_dialog->set_clear();
    //qDebug()<<"login";
    login_dialog->show();
}

/* 登录过程处理事件 */
void config_list_widget::open_cloud() {
    emit dooss(uuid);
    login_dialog->on_close();
}

/* 登录成功处理事件 */
void config_list_widget::finished_load(int ret,QString uuid) {
    //qDebug()<<"wb111"<<ret;
    if(uuid != this->uuid) {
        return ;
    }
   // qDebug()<<"wb222"<<ret;
    if (ret == 0) {
        emit doconf();
    } else if(ret == 401 || ret == 203 || ret == 201) {
        emit dologout();
    }
}

/* 读取滑动按钮列表 */
void config_list_widget::handle_conf() {
    if(Config_File(home).Get("Auto-sync","enable").toString() == "true") {
        auto_syn->make_itemon();
        for(int i  = 0;i < mapid.size();i ++) {
            list->get_item(i)->set_active(true);
        }
    } else {
        auto_syn->make_itemoff();
        auto_ok = false;
        for(int i  = 0;i < mapid.size();i ++) {
            judge_item(Config_File(home).Get(mapid[i],"enable").toString(),i);
        }
        for(int i  = 0;i < mapid.size();i ++) {
            list->get_item(i)->set_active(auto_ok);
        }
        return ;
    }
    for(int i  = 0;i < mapid.size();i ++) {
        judge_item(Config_File(home).Get(mapid[i],"enable").toString(),i);
    }
}

/* 判断功能是否开启 */
bool config_list_widget::judge_item(QString enable,int cur) {
    if(enable == "true") {
        list->get_item(cur)->make_itemon();
    } else {
        list->get_item(cur)->make_itemoff();
    }
    return true;
}

/* 滑动按钮点击后改变功能状态 */
void config_list_widget::handle_write(int on, int id) {
    char name[32];
    if(id == -1) {
        qstrcpy(name,"Auto-sync");
    } else {
        qstrcpy(name,mapid[id].toStdString().c_str());
    }
    emit dochange(name,on);
}

/* 滑动按钮点击处理事件 */
void config_list_widget::on_switch_button(int on,int id) {
    if(stacked_widget->currentWidget() == null_widget) {
        return ;
    }
    if(!auto_ok) {
        return ;
    }
    //emit docheck();
    handle_write(on,id);
}

/* 自动同步滑动按钮点击后改变功能状态 */
void config_list_widget::on_auto_syn(int on,int id) {
    if(stacked_widget->currentWidget() == null_widget) {
        return ;
    }
    //emit docheck();
    auto_ok = on;
    for(int i  = 0;i < mapid.size();i ++) {
        list->get_item(i)->set_active(auto_ok);
    }
    handle_write(on,-1);
}

/* 登出处理事件 */
void config_list_widget::on_login_out() {
    ret_ok = false;
    client->once = true;
    //qDebug()<< "wb777";
    emit dologout();
    if(edit_dialog->isVisible() == true) {
        edit_dialog->close();
    }
    //qDebug()<<"1213131";
    code = "";
    login_dialog->set_clear();
    edit_dialog->set_clear();
    stacked_widget->setCurrentWidget(null_widget);
}

/* 修改密码打开处理事件 */
void config_list_widget::neweditdialog() {
    //emit docheck();
    edit_dialog->is_used = true;
    login_dialog->is_used = false;
    edit_dialog->set_clear();
    edit_dialog->name  = code;
    edit_dialog->show();
    edit_dialog->raise();
}

/* 动态布局显示处理函数 */
void config_list_widget::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

QLabel* config_list_widget::get_info() {
    return info;
}

QLabel* config_list_widget::get_title() {
    return title;
}

/* 同步回调函数集 */
void config_list_widget::download_files() {
    if(stacked_widget->currentWidget() == null_widget) {
        return ;
    }
    //emit docheck();
    if(exit_page->property("on") == false) {
        exit_page->setProperty("on",true);
        exit_page->setText("");
        pm->start();
        gif->setMovie(pm);
        gif->show();
    }
}

void config_list_widget::push_files() {
    if(stacked_widget->currentWidget() == null_widget) {
        return ;
    }
   // emit docheck();
    if(exit_page->property("on") == false) {
        exit_page->setText("");
        exit_page->setProperty("on",true);
        pm->start();
        gif->setMovie(pm);
        gif->show();
    }
}

void config_list_widget::download_over() {
    //emit docheck();
    if(exit_page->property("on") == true) {
        gif->hide();
        exit_page->setText(tr("Exit"));
        exit_page->setProperty("on",false);
    }
}

void config_list_widget::push_over() {
    //emit docheck();
    if(exit_page->property("on") == true) {
        gif->hide();
        exit_page->setText(tr("Exit"));
        exit_page->setProperty("on",false);
    }
}

/* 析构函数 */
config_list_widget::~config_list_widget() {
    delete login_dialog;
    delete edit_dialog;
    delete client;
    if(thread)
    {
        thread->quit();
    }
    thread->wait();
}


