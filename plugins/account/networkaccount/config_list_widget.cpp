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

config_list_widget::config_list_widget(QWidget *parent) : QWidget(parent) {
    client = new DbusHandleClient();

    stacked_widget = new QStackedWidget(this);
    stacked_widget->resize(550,400);
    stacked_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    code = client->check_login();
    client->connectdbus("finished_init_oss",this,SLOT(finished_load(int)));
    //connect(client,SIGNAL(backcall_start_download_signal()),this,SLOT(download_files()));
    client->connectdbus("backcall_start_download_signal",this,SLOT(download_files()));
    //connect(client,SIGNAL(backcall_end_download_signal()),this,SLOT(download_over()));
    client->connectdbus("backcall_end_download_signal",this,SLOT(download_over()));
    //connect(client,SIGNAL(backcall_start_push_signal()),this,SLOT(push_files()));
    client->connectdbus("backcall_start_push_signal",this,SLOT(push_files()));
    //connect(client,SIGNAL(backcall_end_push_signal()),this,SLOT(push_over()));
    client->connectdbus("backcall_end_push_signal",this,SLOT(push_over()));
    init_gui();
    if(code != "" && code !="201" && code != "203" && code != "401") {
        /*QFuture<int> res = QtConcurrent::run(this, &config_list_widget::oss_initial);
        int result = res.result();*/
        if(client->init_oss() == 0) {
            qDebug()<<"init oss is 0";
        }
        else {
            client->logout();
            stacked_widget->setCurrentWidget(null_widget);
        }
    } else {

        stacked_widget->setCurrentWidget(null_widget);
    }
}

void config_list_widget::init_gui() {
    //Allocator
    home = QDir::homePath() + "/.cache/kylinssoclient/All.conf";
    vboxlayout = new QVBoxLayout;
    tab = new QWidget(this);
    container = new QWidget(this);
    namewidget = new QWidget(this);
    list = new item_list;
    //ld = new LoginDialog(this);
    auto_syn = new network_item(this);
    title = new QLabel(this);
    info = new QLabel(namewidget);
    exit_page = new QPushButton(tr("Exit"),this);
    cvlayout = new QVBoxLayout;
    qDebug()<<"222222";
    login_dialog = new Dialog_login_reg;
    qDebug()<<"111111";
    edit_dialog = new EditPassDialog;
    qDebug()<<"000000";
    hbox = new QHBoxLayout;
    gif = new QLabel(exit_page);
    pm = new QMovie(":/new/image/autosync.gif");

    gif->hide();
    edit_dialog->hide();
    login_dialog->hide();
    edit_dialog->set_client(client);
    login_dialog->set_client(client);
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

    //Configuration
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
    //Configuration
    stacked_widget->addWidget(container);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //可选;

    //Resize (Only for QPushButton)
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

    //Layout
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
    connect(auto_syn->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_auto_syn(int,int)));
    connect(login,SIGNAL(clicked()),this,SLOT(on_login()));
    //Connect
    connect(edit,SIGNAL(clicked()),this,SLOT(neweditdialog()));
    connect(exit_page,SIGNAL(clicked()),this,SLOT(on_login_out()));
    connect(edit_dialog,SIGNAL(account_changed()),this,SLOT(on_login_out()));
    qDebug()<<"new debug 4";
    connect(login_dialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
    adjustSize();
    qDebug()<<"new debug 2";
    for(int btncnt = 0;btncnt < list->get_list().size();btncnt ++) {
        connect(list->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
    }
    qDebug()<<"new debug 3";
}

void config_list_widget::on_login() {
        login_dialog->setclear();
        login_dialog->show();
}

void config_list_widget::open_cloud() {
    code = client->check_login();
    if(code != "" && code != "201" && code != "203" && code != "401") {
        /*QFuture<int> res = QtConcurrent::run(this, &config_list_widget::oss_initial);
        int result = res.result();*/
        qDebug()<<"new debug 3";
        if(client->init_oss() == 0) {
        }else {
            client->logout();
            stacked_widget->setCurrentWidget(null_widget);
        }
    }

}

void config_list_widget::finished_load(int ret) {
    if (ret == 0) {
        if(client->init_conf() == 0) {
            login_dialog->on_close();
            QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_conf);
            info->setText(tr("Your account：%1").arg(code));
            stacked_widget->setCurrentWidget(container);
            if(client->manual_sync() == 0) {
                qDebug()<<"manual sync succes";
            }
        }
    } else if(ret == 401 || ret == 203 || ret == 201) {
        client->logout();
        stacked_widget->setCurrentWidget(null_widget);
    }
}

void config_list_widget::handle_conf() {
    if(Config_File(home).Get("Auto-sync","enable").toString() == "true") {
        auto_syn->make_itemon();
        for(int i  = 0;i < mapid.size();i ++) {
            list->get_item(i)->set_active(true);
        }
    } else {
        auto_syn->make_itemoff();
        auto_ok = false;
        QString enable;
        for(int i  = 0;i < mapid.size();i ++) {
            Config_File(home).Get(mapid[i],"enable").toString();
            judge_item(Config_File(home).Get(mapid[i],"enable").toString(),i);
        }
        for(int i  = 0;i < mapid.size();i ++) {
            list->get_item(i)->set_active(auto_ok);
        }
        return ;
    }
    QString enable;
    for(int i  = 0;i < mapid.size();i ++) {
        judge_item(Config_File(home).Get(mapid[i],"enable").toString(),i);
    }
}

bool config_list_widget::judge_item(QString enable,int cur) {
    if(enable == "true") {
        list->get_item(cur)->make_itemon();
    } else {
        list->get_item(cur)->make_itemoff();
    }
    return true;
}

void config_list_widget::handle_write(int on, int id) {
    char name[32];
    if(id == -1) {
        qstrcpy(name,"Auto-sync");
    } else {
        qstrcpy(name,mapid[id].toStdString().c_str());
    }
    client->change_conf_value(name,on);
}

void config_list_widget::on_switch_button(int on,int id) {
    QString code = client->check_login();
    if(code == "401" || code == "203" || code == "201" || code == "") {
        client->logout();
        stacked_widget->setCurrentWidget(null_widget);
        return ;
    }
    if(!auto_ok) {
        return ;
    }
    QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_write,on,id);
}

void config_list_widget::on_auto_syn(int on,int id) {
    QString code = client->check_login();
    if(code == "401" || code == "203" || code == "201" || code == "") {
        client->logout();
        stacked_widget->setCurrentWidget(null_widget);
        return ;
    }
    auto_ok = on;
    for(int i  = 0;i < mapid.size();i ++) {
        list->get_item(i)->set_active(auto_ok);
    }
    QFuture<void> res1 = QtConcurrent::run(this, &config_list_widget::handle_write,on,-1);
}

void config_list_widget::on_login_out() {
    if(client->logout() == 0) {
        login_dialog->setclear();
        stacked_widget->setCurrentWidget(null_widget);
    }
}

void config_list_widget::neweditdialog() {
    edit_dialog->set_clear();
    edit_dialog->show();
    edit_dialog->raise();
}


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

void config_list_widget::download_files() {
    QString code = client->check_login();
    if(code == "401" || code == "203" || code == "201" || code == "") {
        client->logout();
        stacked_widget->setCurrentWidget(null_widget);
        return ;
    }
    if(exit_page->property("on") == false) {
        exit_page->setProperty("on",true);
        exit_page->setText("");
        gif->setMovie(pm);
        gif->show();
        pm->start();
    }
}

void config_list_widget::push_files() {
    QString code = client->check_login();
    if(code == "401" || code == "203" || code == "201" || code == "") {
        client->logout();
        stacked_widget->setCurrentWidget(null_widget);
        return ;
    }
    if(exit_page->property("on") == false) {
        exit_page->setText("");
        exit_page->setProperty("on",true);
        gif->setMovie(pm);
        gif->show();
        pm->start();
    }
}

void config_list_widget::download_over() {
    if(exit_page->property("on") == true) {
        gif->hide();
        exit_page->setText(tr("Exit"));
        exit_page->setProperty("on",false);
    }
}

void config_list_widget::push_over() {
    if(exit_page->property("on") == true) {
        gif->hide();
        exit_page->setText(tr("Exit"));
        exit_page->setProperty("on",false);
    }
}

config_list_widget::~config_list_widget() {
    delete client;
}

