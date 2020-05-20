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
#include "dialog_login_reg.h"
#include <QDesktopWidget>
#include <QApplication>

Dialog_login_reg::Dialog_login_reg(QWidget *parent) : QWidget(parent)
{
    //内存分配
    uuid = QUuid::createUuid().toString();
    login_submit = new QPushButton(tr("Sign in"),this);     //登录或者确认或者注册或者重置密码或者绑定手机按钮（重用）
    register_account = new QPushButton(tr("Sign up"),this); //返回登录或者注册账户按钮（重用）
    box_login = new LoginDialog(this);      //登录页面
    box_reg = new RegDialog(this);          //注册页面
    box_bind = new BindPhoneDialog(this);   //手机绑定页面
    box_pass = new PassDialog(this);        //忘记密码页面
    log_reg = new QWidget(this);            //业务逻辑主界面
    basewidegt = new QStackedWidget(this);  //用于切换成功页面和业务逻辑操作页面（包括登录等模块)

    title = new QLabel(status,this);        //页面标题（业务逻辑重用）
    stack_box = new QStackedWidget(this);   //用于切换业务逻辑操作页面（包括登录，注册，绑定，忘记密码）
    vboxlayout = new QVBoxLayout;           //业务界面主体布局
    hboxlayout = new QHBoxLayout;           //切换登录模式按钮布局
    del_btn = new QPushButton(this);        //关闭按钮
    timer = new QTimer(this);               //以下都是验证码倒计时计时器，后缀相互对应
    timer_reg  = new QTimer(this);
    timer_log = new QTimer(this);
    timer_bind = new QTimer(this);
    succ = new SuccessDiaolog(this);        //注册成功页面
    gif = new QLabel(login_submit);         //登录按钮过程动画
    QHBoxLayout *hbox = new QHBoxLayout;    //整体布局
    pm = new QMovie(":/new/image/login.gif");

    //计时器初始化
    timer->stop();
    timer_reg->stop();
    timer_log->stop();
    timer_bind->stop();

    //隐藏同步开关动画
    gif->hide();

    //控件尺寸以及布局设置
    this->setFixedSize(418,505);
    log_reg->setFixedSize(418,505);
    stack_box->addWidget(box_login);
    stack_box->addWidget(box_reg);
    stack_box->addWidget(box_pass);
    stack_box->addWidget(box_bind);

    login_submit->setFocusPolicy(Qt::NoFocus);
    title->setFocusPolicy(Qt::NoFocus);
    register_account->setFocusPolicy(Qt::NoFocus);

    title->setText(status);
    title->adjustSize();
    //setFocusPolicy(Qt::NoFocus);
    box_login->setContentsMargins(0,0,0,0);
    //title->setGeometry(31 + sizeoff,48 + sizeoff,160,24);
    title->setStyleSheet("font-size: 24px;font-weight:500;");

    login_submit->setFixedSize(338,36);
    login_submit->setFocusPolicy(Qt::NoFocus);
    register_account->setMaximumSize(120,36);
    register_account->setMinimumSize(120,36);
    stack_box->setFixedWidth(338);
    stack_box->setMinimumHeight(box_login->height());
    basewidegt->setFixedSize(418,505);
    basewidegt->setContentsMargins(0,0,0,0);
    basewidegt->addWidget(log_reg);
    basewidegt->addWidget(succ);
    succ->set_mode_text(2);
    basewidegt->setCurrentWidget(log_reg);
    login_submit->setContentsMargins(0,0,0,0);
    register_account->setFocusPolicy(Qt::NoFocus);
    register_account->setContentsMargins(0,0,0,0);
    del_btn->setMaximumSize(30,30);
    del_btn->setMinimumSize(30,30);
    del_btn->setGeometry(this->width() - 46,14,30,30);
    del_btn->setFocusPolicy(Qt::NoFocus);

    login_submit->setStyleSheet("QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                                "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                                "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    register_account->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                    "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                    "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");

    del_btn->setStyleSheet("QPushButton{width:30px;height:30px;border-style: flat;"
                           "background-image:url(:/new/image/delete.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;}"
                           "QPushButton:hover{background-color:#F86457;width:30px;height:30px;"
                           "background-image: url(:new/image/delete_click.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;"
                           "border-radius:4px}"
                           "QPushButton:click{background-color:#E44C50;width:30px;height:30px;"
                           "background-image: url(:new/image/delete_click.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;border-radius:4px}");

    stack_box->setCurrentWidget(box_login);

    //主窗口布局样式设置
    //setStyleSheet("Dialog_login_reg{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    vboxlayout->setSpacing(0);
    vboxlayout->setContentsMargins(41,55,41,36);
    vboxlayout->addWidget(title);
    title->setMargin(0);
    vboxlayout->addSpacing(20);
    stack_box->setContentsMargins(0,0,0,0);
    vboxlayout->addWidget(stack_box);
    login_submit->setContentsMargins(0,0,0,0);
    vboxlayout->addSpacing(0);
    vboxlayout->addWidget(login_submit);
    hboxlayout->setSpacing(0);
    hboxlayout->setContentsMargins(0,10,0,0);
    hboxlayout->addWidget(register_account);
    vboxlayout->addSpacing(10);
    vboxlayout->addLayout(hboxlayout);
    vboxlayout->addSpacing(20);
    vboxlayout->setEnabled(true);
    log_reg->setLayout(vboxlayout);

    log_reg->setContentsMargins(0,0,0,0);

    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(basewidegt);
    setLayout(hbox);

    stack_box->adjustSize();

    //注册绑定快捷键，让登录可以按Enter键登录
    login_submit->setShortcut(QKeySequence::InsertParagraphSeparator);
    login_submit->setShortcut(Qt::Key_Enter);
    login_submit->setShortcut(Qt::Key_Return);


    /* 子控件成员公共函数获取，主要是为了用eventFilter函数去更改控件Focus事件
     * 处理，防止一些没有必要的重写类生成，并方便及时更改控件子控件的状态属性，
     * 避免过度使用信号与槽*/
    send_btn_fgt = box_pass->get_send_msg_btn();
    send_btn_reg = box_reg->get_send_code();
    send_btn_log = box_login->get_user_mcode();

    box_login->get_user_edit()->setFocus();



    login_pass = box_login->get_login_pass();   //登录界面密码框
    login_code = box_login->get_login_code();   //登录界面手机框
    login_user = box_login->get_user_edit();    //登录界面用户框
    login_mcode = box_login->get_mcode_lineedit();//登录界面验证码框

    reg_user = box_reg->get_reg_user();         //注册界面用户框
    phone_user = box_reg->get_phone_user();     //注册界面手机框
    valid_code = box_reg->get_valid_code();     //注册界面验证码框
    reg_pass = box_reg->get_reg_pass();         //注册界面密码框
    reg_confirm = box_reg->get_reg_confirm();   //注册界面密码确认框

    pass_user = box_pass->get_reg_phone();      //忘记密码界面用户框
    pass_pwd = box_pass->get_reg_pass();        //忘记密码界面密码框
    pass_confirm = box_pass->get_reg_pass_confirm();//忘记密码确认密码框
    pass_code = box_pass->get_valid_code();     //忘记密码验证码框


    //忘记密码错误提示
    passtips = box_pass->get_passtips();

    //登录错误提示
    codelable = box_login->get_tips_code();
    passlabel = box_login->get_tips_pass();

    //注册输入提示
    pass_tips = box_pass->get_tips();
    reg_tips = box_reg->get_tips();
    //注册错误消息提示

    user_tip = box_reg->get_user_tip();
    pass_tip = box_reg->get_pass_tip();

    /*界面逻辑有关信号与槽函数连接*/
    connect(del_btn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(box_login->get_forget_btn(),SIGNAL(clicked()),this,SLOT(linked_forget_btn()));
    connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
    connect(send_btn_fgt,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    connect(timer_reg,SIGNAL(timeout()),this,SLOT(on_timer_reg_out()));
    connect(timer_log,SIGNAL(timeout()),this,SLOT(on_timer_log_out()));
    connect(timer_bind,SIGNAL(timeout()),this,SLOT(on_timer_bind_out()));
    connect(send_btn_reg,SIGNAL(clicked()),this,SLOT(on_send_code_reg()));
    connect(send_btn_log,SIGNAL(clicked()),this,SLOT(on_send_code_log()));
    connect(box_bind->get_send_code(),SIGNAL(clicked()),this,SLOT(on_send_code_bind()));
    connect(succ->back_login,SIGNAL(clicked()),this,SLOT(back_normal()));
    connect(pass_pwd,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(reg_pass,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(box_login->get_mcode_lineedit(),SIGNAL(returnPressed()),login_submit,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(box_reg->get_valid_code(),SIGNAL(returnPressed()),login_submit,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(box_bind->get_code_lineedit(),SIGNAL(returnPressed()),login_submit,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(box_pass->get_valid_code(),SIGNAL(returnPressed()),login_submit,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(box_login->get_stack_widget(),&QStackedWidget::currentChanged,[this] (int) {
        if(gif->isHidden() == false) {
            gif->hide();
            pm->stop();
        }
    });
    //为各个子控件安装事件过滤
    login_submit->installEventFilter(this);


    login_pass->installEventFilter(this);
    login_code->installEventFilter(this);
    login_user->installEventFilter(this);
    login_mcode->installEventFilter(this);

    pass_pwd->installEventFilter(this);
    pass_code->installEventFilter(this);
    pass_confirm->installEventFilter(this);
    pass_user->installEventFilter(this);

    reg_pass->installEventFilter(this);
    reg_user->installEventFilter(this);
    reg_confirm->installEventFilter(this);
    phone_user->installEventFilter(this);
    valid_code->installEventFilter(this);


    box_bind->get_code_lineedit()->installEventFilter(this);
    box_bind->get_phone_lineedit()->installEventFilter(this);

    stack_box->installEventFilter(this);

    //对话框模态处理
    setWindowModality(Qt::ApplicationModal);
    //把对话框放置屏幕中央
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

    //初始化一下验证码计时器激活时间
    timerout_num_bind = 60;
    timerout_num = 60;
    timerout_num_log = 60;
    timerout_num_reg = 60;
}

/* 确认密码框如果遇到新密码或者注册密码改变，立即改变
 * 成空的状态，防止用户多余操作*/
void Dialog_login_reg::cleanconfirm(QString str) {
    //qDebug()<<str;
    if(stack_box->currentWidget() == box_pass) {
        pass_confirm->setText("");
    } else if(stack_box->currentWidget() == box_reg) {
        reg_confirm->setText("");
    }
}

/* 登录按钮返回给上级控件接口 */
QPushButton * Dialog_login_reg::get_login_submit() {
    return login_submit;
}

/* 设置DBUS客户端 */
void Dialog_login_reg::set_client(DbusHandleClient *c,QThread *t) {
    client = c;
    thread  = t;
    connect(this,SIGNAL(dologin(QString,QString,QString)),client,SLOT(login(QString,QString,QString)));
    connect(this,SIGNAL(doreg(QString, QString, QString, QString,QString)),client,SLOT(registered(QString, QString, QString, QString,QString)));
    connect(this,SIGNAL(dobind(QString, QString, QString, QString,QString)),client,SLOT(bindPhone(QString, QString, QString, QString,QString)));
    connect(this,SIGNAL(dogetmcode_phone_reg(QString,QString)),client,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dogetmcode_phone_log(QString,QString)),client,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dogetmcode_number_pass(QString,QString)),client,SLOT(get_mcode_by_username(QString,QString)));
    connect(this,SIGNAL(dogetmcode_number_bind(QString,QString)),client,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dorest(QString, QString, QString,QString)),client,SLOT(user_resetpwd(QString, QString, QString,QString)));
    connect(this,SIGNAL(dophonelogin(QString,QString,QString)),client,SLOT(user_phone_login(QString,QString,QString)));
    connect(client,SIGNAL(finished_ret_log(int)),this,SLOT(setret_login(int)));
    connect(client,SIGNAL(finished_ret_reg(int)),this,SLOT(setret_reg(int)));
    connect(client,SIGNAL(finished_ret_phonelogin(int)),this,SLOT(setret_phone_login(int)));
    connect(client,SIGNAL(finished_ret_rest(int)),this,SLOT(setret_rest(int)));
    connect(client,SIGNAL(finished_ret_bind(int)),this,SLOT(setret_bind(int)));
    connect(client,SIGNAL(finished_ret_code_log(int)),this,SLOT(setret_code_phone_login(int)));
    connect(client,SIGNAL(finished_ret_code_reg(int)),this,SLOT(setret_code_phone_reg(int)));
    connect(client,SIGNAL(finished_ret_code_pass(int)),this,SLOT(setret_code_user_pass(int)));
    connect(client,SIGNAL(finished_ret_code_bind(int)),this,SLOT(setret_code_user_bind(int)));

    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface", "finished_login", this, SLOT(on_login_finished(int,QString)));
    //client->connectdbus("finished_login",this,SLOT(on_login_finished(int)));
    //connect(client,SIGNAL(finished_user_phone_login(int)),this,SLOT(on_login_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_user_phone_login",this,SLOT(on_login_finished(int,QString)));
    //connect(client,SIGNAL(finished_mcode_by_phone(int)),this,SLOT(on_get_mcode_by_phone(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_mcode_by_phone",this,SLOT(on_get_mcode_by_phone(int,QString)));
    //connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_pass_finished(int)));
    //connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_get_mcode_by_name(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_user_resetpwd",this,SLOT(on_pass_finished(int,QString)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_mcode_by_username",this,SLOT(on_get_mcode_by_name(int,QString)));
    //connect(client,SIGNAL(finished_registered(int)),this,SLOT(on_reg_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_registered",this,SLOT(on_reg_finished(int,QString)));
    //connect(client,SIGNAL(finished_bindPhone(int)),this,SLOT(on_bind_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_bindPhone",this,SLOT(on_bind_finished(int,QString)));
}

/* 窗口控件动态显示处理过渡处理函数，每次窗口布局显示或者
 * 隐藏都要调用这个函数，不然控件布局会发生错乱 */
void Dialog_login_reg::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

/* 客户端回调函数集，一般处理异常出现的情况，成功一般
 * 不处理，除非是成功之后还要执行操作的 */
void Dialog_login_reg::setret_reg(int ret) {
    if(ret != 0) {
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);
        return ;
    } else {
    }
}

void Dialog_login_reg::setret_login(int ret) {
    if(ret != 0) {
        box_login->get_mcode_lineedit()->setText("");
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();

            box_login->get_mcode_widget()->set_change(1);
            box_login->get_mcode_widget()->repaint();
            setshow(stack_box);
            box_login->get_mcode_widget()->set_change(0);
        } else {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
        }
        return ;
    } else {

    }
}

void Dialog_login_reg::setret_phone_login(int ret) {
    if(stack_box->currentWidget() != box_login && box_login->get_stack_widget()->currentIndex() != 1) {
        return ;
    }
    if(ret != 0) {
        box_login->get_mcode_lineedit()->setText("");
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();

            box_login->get_mcode_widget()->set_change(1);
            box_login->get_mcode_widget()->repaint();
            setshow(stack_box);
            box_login->get_mcode_widget()->set_change(0);
            return ;
        } else {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
            return ;
        }
    }
}

void Dialog_login_reg::setret_rest(int ret) {
    if(ret != 0) {
        box_pass->get_valid_code()->setText("");
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);

        return ;
    }else {

    }
}

void Dialog_login_reg::setret_bind(int ret) {
    if(ret != 0) {
        box_bind->get_code_lineedit()->setText("");
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        setshow(stack_box);

        return ;
    } else {
    }
}

void Dialog_login_reg::setret_code_phone_login(int ret) {
    if(stack_box->currentWidget() != box_login && box_login->get_stack_widget()->currentIndex() != 1) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        box_login->get_mcode_lineedit()->setText("");
        box_login->set_code(messagebox(ret));
        codelable->show();
        setshow(stack_box);

        return ;
    }
}

void Dialog_login_reg::setret_code_user_bind(int ret) {
    if(stack_box->currentWidget() != box_bind) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        box_bind->get_code_lineedit()->setText("");
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::setret_code_phone_reg(int ret) {
    if(stack_box->currentWidget() != box_reg) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);

        return ;
    }
}

void Dialog_login_reg::setret_code_user_pass(int ret) {
    if(stack_box->currentWidget() != box_pass) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        box_pass->get_valid_code()->setText("");
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);
        return ;
    }
}

/* 错误消息提示盒子，所有服务器消息基本上来源于此，默认
 * 返回未知代码，显示错误以及代码编号 */
QString Dialog_login_reg::messagebox(int code) {
    QString ret = tr("Error code:") + QString::number(code,10)+ tr("!");
    switch(code) {
    case 101:ret = tr("Internal error occurring!");break;
    case 102:ret = tr("Failed to sign up!");break;
    case 103:ret = tr("Failed attempt to return value!");break;
    case 104:ret = tr("Check your connection!");break;
    case 105:ret = tr("Failed to get by phone!");break;
    case 106:ret = tr("Failed to get by user!");break;
    case 107:ret = tr("Failed to reset password!");break;
    case 109:ret = tr("Phone binding falied!");break;
    case 110:ret = tr("Please check your information!");break;
    case 401:ret = tr("Please check your account!");break;
    case 500:ret = tr("Failed due to server error!");break;
    case 501:ret = tr("Please check your information!");break;
    case 502:ret = tr("User existing!");break;
    case 610:ret = tr("Phone number already in used!");break;
    case 611:ret = tr("Please check your format!");break;
    case 612:ret = tr("Your are reach the limit!");break;
    case 613:ret = tr("Please check your phone number!");break;
    case 614:ret = tr("Please check your code!");break;
    case 615:ret = tr("Account doesn't exist!");break;
    case 616:ret = tr("User has bound the phone!");break;
    case 619:ret = tr("Sending code error occurring!");break;
    case -1:ret = tr("Please check your information!");break;

    }
    return ret;
}

/* 1.登录逻辑处理槽函数 */
void Dialog_login_reg::on_login_btn() {
    basewidegt->setEnabled(false); //防止用户在登录按钮按完之后到处乱点，下同
    del_btn->setEnabled(true);
    //如果验证码输入错误，执行此处
    if(box_login->get_stack_widget()->currentIndex() == 0 &&
        QString(box_login->get_mcode_widget()->get_verificate_code()) != box_login->get_mcode_lineedit()->text()) {
        box_login->set_code(tr("Your code is wrong!"));
        passlabel->show();
        basewidegt->setEnabled(true);
        box_login->get_mcode_widget()->set_change(1);
        box_login->get_mcode_widget()->repaint();
        setshow(stack_box);
        box_login->get_mcode_lineedit()->setText("");
        box_login->get_mcode_widget()->set_change(0);
        return ;
    }
    //如果信息正确可提交，执行此处
    if(box_login->get_user_name() != "" &&
        box_login->get_user_pass() != "" &&
        box_login->get_stack_widget()->currentIndex() == 0){
        QString name,pass;
        account = box_login->get_user_name();
        passwd = box_login->get_user_pass();
        name = box_login->get_user_name();
        //qDebug()<<"1111111";
        pass = box_login->get_user_pass();
        login_submit->setText("");
        pm->start();
        gif->setMovie(pm);
        gif->show();
        emit dologin(name,pass,uuid);            //触发登录信号，告知客户端进行登录操作

    } else if(box_login->get_user_name() != ""
               && box_login->get_login_code()->text() != ""
               && box_login->get_stack_widget()->currentIndex() == 1) {
        QString phone,mcode;                    //如果用户选择手机登录，执行此处
        phone = box_login->get_user_name();
        mcode = box_login->get_login_code()->text();
        emit dophonelogin(phone,mcode,uuid);
    } else {
        //信息填写不完整执行此处，包括密码登录以及手机登录
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(-1));
            passlabel->show();
            basewidegt->setEnabled(true);
            box_login->get_mcode_widget()->set_change(1);
            box_login->get_mcode_widget()->repaint();
            setshow(stack_box);
            box_login->get_mcode_widget()->set_change(0);
            return ;
        } else {
            box_login->get_mcode_lineedit()->setText("");
            basewidegt->setEnabled(true);
            box_login->set_code(messagebox(-1));
            codelable->show();
            setshow(stack_box);
            return ;
        }
    }
}

/* 2.注册逻辑处理槽函数 */
void Dialog_login_reg::on_reg_btn() {
    basewidegt->setEnabled(false);
    del_btn->setEnabled(true);
    bool ok_mcode = box_reg->get_user_mcode() != "";
    bool ok_phone = box_reg->get_user_phone() != "";
    bool ok_account = box_reg->get_user_account() != "";
    bool ok_passwd = box_reg->get_user_passwd() != "";
    bool ok_confirm = box_reg->get_reg_confirm()->text() != "";
    int ret = -1;
    if(ok_mcode && ok_phone && ok_account && ok_passwd &&ok_confirm) {
        QString account,passwd,phone,mcode,confirm;
        //qstrcpy(account,box_reg->get_user_account().toStdString().c_str());
        account = box_reg->get_user_account();
        phone = box_reg->get_user_phone();
        //qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        //qstrcpy(passwd,box_reg->get_user_passwd().toStdString().c_str());
        //qstrcpy(mcode,box_reg->get_user_mcode().toStdString().c_str());
        //qstrcpy(confirm,box_reg->get_reg_confirm()->text().toStdString().c_str());
        passwd = box_reg->get_user_passwd();
        mcode = box_reg->get_user_mcode();
        confirm = box_reg->get_reg_confirm()->text();
        if(confirm != passwd) {
            basewidegt->setEnabled(true);
            box_pass->set_code(tr("Please check your password!"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        if(!box_reg->get_reg_pass()->check()) {
            basewidegt->setEnabled(true);
            box_pass->set_code(tr("Please check your password!"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        emit doreg(account,passwd,phone,mcode,uuid);
    } else {
        basewidegt->setEnabled(true);
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);
        return ;
    }
}

/* 3.忘记密码进入按钮处理槽函数 */
void Dialog_login_reg::on_pass_btn() {
    int ret = -1;
    basewidegt->setEnabled(false);
    del_btn->setEnabled(true);
    bool ok_phone = box_pass->get_user_name() == "";
    bool ok_pass = box_pass->get_user_newpass() == "";
    bool ok_confirm = box_pass->get_user_confirm() == "";
    bool ok_code = box_pass->get_user_mcode() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_confirm) {
        QString phone,pass,code,confirm;
        phone = box_pass->get_user_name();
        pass = box_pass->get_user_newpass();
        confirm = box_pass->get_user_confirm();
        code = box_pass->get_user_mcode();
//        qstrcpy(phone,box_pass->get_user_name().toStdString().c_str());
//        qstrcpy(pass,box_pass->get_user_newpass().toStdString().c_str());
//        qstrcpy(confirm,box_pass->get_user_confirm().toStdString().c_str());
//        qstrcpy(code,box_pass->get_user_mcode().toStdString().c_str());

        if(box_pass->get_reg_pass()->check() == false) {
            basewidegt->setEnabled(true);
            box_pass->set_code(tr("At least 6 bit, include letters and digt"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        if(confirm != pass) {
            basewidegt->setEnabled(true);
            box_pass->set_code(tr("Please check your password!"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        //qDebug()<<phone<<pass<<code;
        emit dorest(phone,pass,code,uuid);
    }else {
        basewidegt->setEnabled(true);
        box_pass->get_valid_code()->setText("");
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);
        return ;
    }
}

/* 4.绑定手机号码逻辑处理槽函数 */
void Dialog_login_reg::on_bind_btn() {
    int ret = -1;
    basewidegt->setEnabled(false);
    del_btn->setEnabled(true);
    bool ok_phone = box_bind->get_phone() == "";
    bool ok_pass = passwd == "";
    bool ok_account = account == "";
    bool ok_code = box_bind->get_code() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_account) {
        QString phone,pass,account_s,code;
        phone = box_bind->get_phone();
        //qstrcpy(pass,passwd.toStdString().c_str());
        pass = passwd;
        //qstrcpy(account_s,account.toStdString().c_str());
        account_s = account;
        //qstrcpy(code,box_bind->get_code().toStdString().c_str());
        code = box_bind->get_code();
        emit dobind(account_s,pass,phone,code,uuid);
    }else {
        box_bind->get_code_lineedit()->setText("");
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        basewidegt->setEnabled(true);
        setshow(stack_box);
        return ;
    }
}


/* 从成功注册，修改密码成功界面返回所需要的处理 */
void Dialog_login_reg::back_normal() {
    if(gif->isHidden() == false) {
        pm->stop();
        gif->hide();
    }
    del_btn->show();
    //qDebug()<<"back normal";
    basewidegt->setCurrentWidget(log_reg);
    succ->hide();
    setshow(basewidegt);
    title->setText(tr("Sign in Cloud"));
    stack_box->setCurrentWidget(box_login);
    box_login->set_clear();
}

/* 从忘记密码或者注册界面或者或者手机绑定
 * 界面返回到登录界面的必要操作 */
void Dialog_login_reg::back_login_btn() {
    //qDebug()<<stack_box->currentIndex();
    if(gif->isHidden() == false) {
        pm->stop();
        gif->hide();
    }
    if(stack_box->currentWidget() != box_login) {
        title->setText(tr("Sign in Cloud"));
        if(stack_box->currentWidget() == box_reg) {
            box_reg->get_reg_pass()->clear();
            box_reg->get_reg_user()->clear();
            box_reg->get_phone_user()->clear();
            box_reg->get_valid_code()->clear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->get_reg_pass()->clear();
            box_pass->get_reg_phone()->clear();
            box_pass->get_reg_pass_confirm()->clear();
            box_pass->get_valid_code()->clear();
            //qDebug()<<"back login";
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(stack_box->currentWidget() == box_bind) {
            box_bind->setclear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        }
        box_login->set_clear();
        stack_box->setCurrentWidget(box_login);
        register_account->setText(tr("Sign up"));
        login_submit->setText(tr("Sign in"));
        box_reg->hide();
        setshow(stack_box);
        box_pass->hide();
        setshow(stack_box);

        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    }
}

/* 进入忘记密码界面的一些必要处理,重用了登录按钮 */
void Dialog_login_reg::linked_forget_btn() {
    if(stack_box->currentWidget()!= box_pass) {
        if(gif->isHidden() == false) {
            pm->stop();
            gif->hide();
        }
        title->setText(tr("Forget"));
        stack_box->setCurrentWidget(box_pass);
        login_submit->setText(tr("Set"));
        register_account->setText(tr("Back"));
        box_pass->set_clear();
        box_reg->hide();
        setshow(stack_box);
        box_login->hide();

        setshow(stack_box);

        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}

/* 进入注册界面的一些必要处理,重用了登录按钮 */
void Dialog_login_reg::linked_register_btn() {
    if(stack_box->currentWidget()!= box_reg) {
        if(gif->isHidden() == false) {
            pm->stop();
            gif->hide();
        }
        title->setText(tr("Create Account"));
        stack_box->setCurrentWidget(box_reg);
        register_account->setText(tr("Back"));
        login_submit->setText(tr("Sign up now"));
        box_pass->hide();
        box_login->hide();
        box_reg->set_clear();

        setshow(stack_box);
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}

/* 注册验证码发送按钮处理 */
void Dialog_login_reg::on_send_code_reg() {
    QString phone;
    box_reg->get_send_code()->setEnabled(false);
    if( box_reg->get_user_account() == "" || box_reg->get_user_phone() == "") {
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(messagebox(-1));
        reg_tips->show();
        box_reg->get_send_code()->setEnabled(true);
        setshow(stack_box);
        return ;
    }
    if(box_reg->get_reg_pass()->check() == false) {
        box_reg->get_send_code()->setEnabled(true);
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(tr("At least 6 bit, include letters and digt"));
            reg_tips->show();
        setshow(stack_box);
        return ;
    }
    if(box_reg->get_user_phone() != "") {
        phone = box_reg->get_user_phone();
        emit dogetmcode_phone_reg(phone,uuid);
    } else {
        box_reg->get_send_code()->setEnabled(true);
        box_reg->get_valid_code()->setText("");
        box_reg->set_code(messagebox(-1));
        reg_tips->show();
        setshow(stack_box);

        return ;
    }
}

/* 手机登录验证码发送按钮处理 */
void Dialog_login_reg::on_send_code_log() {
    QString phone;
    box_login->get_user_mcode()->setEnabled(false);
    if(box_login->get_user_name() != "") {
        phone = box_login->get_user_name();
        emit dogetmcode_phone_log(phone,uuid);
    } else {
        box_login->get_user_mcode()->setEnabled(true);
        box_login->get_mcode_lineedit()->setText("");
        box_login->set_code(messagebox(-1));
        codelable->show();
        setshow(stack_box);

        return ;
    }
}

/* 手机绑定验证码发送按钮处理 */
void Dialog_login_reg::on_send_code_bind() {
    QString name;
    int ret = -1;
    box_bind->get_send_code()->setEnabled(false);
    if(box_bind->get_phone() != "") {
        //qstrcpy(name,box_bind->get_phone().toStdString().c_str());
        name = box_bind->get_phone();
        emit dogetmcode_number_bind(name,uuid);
    }else {
        box_bind->get_code_lineedit()->setText("");
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        box_bind->get_send_code()->setEnabled(true);
        setshow(stack_box);
        return ;
    }
}

/* 忘记密码验证码发送按钮处理 */
void Dialog_login_reg::on_send_code() {
    QString name;
    box_pass->get_send_msg_btn()->setEnabled(false);
    if(box_pass->get_reg_pass()->check() == false) {
        box_pass->get_valid_code()->setText("");
        box_pass->set_code(tr("At least 6 bit, include letters and digt"));
        pass_tips->show();
        box_pass->get_send_msg_btn()->setEnabled(true);
        setshow(stack_box);
        return ;
    }
    if(box_pass->get_user_name() != "" && box_pass->get_user_confirm() !="" && box_pass->get_user_newpass() != "") {
        //qstrcpy(name,box_pass->get_user_name().toStdString().c_str());
        name = box_pass->get_user_name();
        emit dogetmcode_number_pass(name,uuid);
    }else {
        box_pass->get_valid_code()->setText("");
        pass_tips->show();
        box_pass->set_code(messagebox(-1));
        box_pass->get_send_msg_btn()->setEnabled(true);
        setshow(stack_box);
        return ;
    }
}


/* 忘记密码验证码的计时器处理 */
void Dialog_login_reg::on_timer_timeout() {
    if(timerout_num > 0) {
        send_btn_fgt->setText(tr("Resend ( %1 )").arg(timerout_num));
        timerout_num --;
    }else if(timerout_num == 0) {
        timerout_num = 60;
        send_btn_fgt->setEnabled(true);
        send_btn_fgt->setText(tr("Get phone code"));
        timer->stop();
    }
}

/* 绑定手机验证码的计时器处理 */
void Dialog_login_reg::on_timer_bind_out() {
    if(timerout_num_bind > 0) {
        box_bind->get_send_code()->setText(tr("Resend ( %1 )").arg(timerout_num_bind));
        timerout_num_bind --;
    }else if(timerout_num_bind == 0) {
        timerout_num_bind = 60;
        box_bind->get_send_code()->setEnabled(true);
        box_bind->get_send_code()->setText(tr("Get phone code"));
        timer_bind->stop();
    }
}

/* 手机登录验证码的计时器处理 */
void Dialog_login_reg::on_timer_log_out() {
    if(timerout_num_log > 0) {
        send_btn_log->setText(tr("Resend ( %1 )").arg(timerout_num_log));
        timerout_num_log --;
    }else if(timerout_num_log == 0) {
        timerout_num_log = 60;
        send_btn_log->setEnabled(true);
        send_btn_log->setText(tr("Send"));
        timer_log->stop();
    }
}

/* 注册账户验证码的计时器处理 */
void Dialog_login_reg::on_timer_reg_out() {
    if(timerout_num_reg > 0) {
        send_btn_reg->setText(tr("Resend ( %1 )").arg(timerout_num_reg));
        timerout_num_reg --;
    }else if(timerout_num_reg == 0) {
        timerout_num_reg = 60;
        send_btn_reg->setEnabled(true);
        send_btn_reg->setText(tr("Send"));
        timer_reg->stop();
    }
}

/* 登录回调槽函数，登录回执消息后执行此处 */
void Dialog_login_reg::on_login_finished(int ret,QString uuid) {
    if(uuid != this->uuid) {
        qDebug()<<uuid<<this->uuid;
        return ;
    }
    qDebug()<<ret;
    basewidegt->setEnabled(true);
    //无手机号码绑定，进入手机号码绑定页面
    if(ret == 119) {
        pm->stop();
        gif->hide();
        title->setText(tr("Binding Phone"));
        stack_box->setCurrentWidget(box_bind);
        register_account->setText(tr("Back"));
        login_submit->setText(tr("Bind now"));
        box_bind->setclear();
        setshow(stack_box);
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        return ;
    }
    //登录返回成功，执行此处
    if(ret == 0) {
        timerout_num_log = 0;
        timer_log->stop();
        send_btn_log->setEnabled(true);
        send_btn_log->setText(tr("Send"));
        login_submit->setText(tr("Sign in"));
        emit on_login_success(); //发送成功登录信号给主页面
    } else {
        pm->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
        gif->hide();
        login_submit->setText(tr("Sign in"));
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();
            box_login->get_mcode_widget()->set_change(1);
            box_login->get_mcode_widget()->repaint();
            setshow(stack_box);
            return ;
        } else if(box_login->get_stack_widget()->currentIndex() == 1) {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
            return ;
        }
    }
}

/* 手机绑定回调槽函数，手机绑定回执消息后执行此处 */
void Dialog_login_reg::on_bind_finished(int ret,QString uuid) {
    if(uuid != this->uuid) {
        return ;
    }
    basewidegt->setEnabled(true);
    if(ret == 0) {
        timerout_num_bind = 0;
        timer_bind->stop();
        login_submit->setText(tr("Sign in"));
        box_bind->get_send_code()->setEnabled(true);
        box_bind->get_send_code()->setText(tr("Send"));
        box_bind->setclear();
        title->setText(tr("Sign in Cloud"));
        register_account->setText(tr("Sign up"));
        stack_box->setCurrentWidget(box_login);
        setshow(stack_box);
        basewidegt->setCurrentWidget(succ);
        succ->set_mode_text(3);
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    } else {
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        setshow(stack_box);
        return ;
    }
}

/* 注册回调槽函数，注册回执消息后执行此处 */
void Dialog_login_reg::on_reg_finished(int ret,QString uuid) {
    if(this->uuid != uuid) {
        return ;
    }
    basewidegt->setEnabled(true);
    //qDebug()<<ret;
    if(ret == 0) {
        timerout_num_reg = 0;
        timer_reg->stop();
        send_btn_reg->setEnabled(true);
        send_btn_reg->setText(tr("Send"));
        login_submit->setText(tr("Sign in"));
        box_reg->get_reg_pass()->clear();
        box_reg->get_reg_user()->clear();
        box_reg->get_phone_user()->clear();
        box_reg->get_valid_code()->clear();
        basewidegt->setCurrentWidget(succ);
        del_btn->hide();
        succ->set_mode_text(0);
        register_account->setText(tr("Sign up"));
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    } else {
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);
        return ;
    }

}

/* 忘记密码回调槽函数，忘记密码回执消息后执行此处 */
void Dialog_login_reg::on_pass_finished(int ret,QString uuid) {
    if(uuid != this->uuid) {
        return ;
    }
    if(is_used == false) {
        return ;
    }
    basewidegt->setEnabled(true);
    if(ret == 0) {
        //qDebug()<<"cascascascascascascascascascascasca";
        timerout_num = 0;
        timer->stop();
        //qDebug()<<"wb11";
        send_btn_fgt->setEnabled(true);
        send_btn_fgt->setText(tr("Send"));
        login_submit->setText(tr("Sign in"));
        //qDebug()<<"wb22";
        box_pass->get_reg_pass()->clear();
        box_pass->get_reg_phone()->clear();
        box_pass->get_reg_pass_confirm()->clear();
        box_pass->get_valid_code()->clear();
        //qDebug()<<"wb33";
        basewidegt->setCurrentWidget(succ);
        del_btn->hide();
        succ->set_mode_text(1);
        //qDebug()<<"wb44";
        register_account->setText(tr("Sign up"));
        //qDebug()<<"wb55";
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        //qDebug()<<"wb66";
    } else {
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);
        return ;
    }
}

/* 手机号直接发送验证码回调函数，发送手机验证码回执消息后执行此处 */
void Dialog_login_reg::on_get_mcode_by_phone(int ret,QString uuid) {
    if(uuid != this->uuid) {
        return ;
    }
    if(ret != 0) {
        if(stack_box->currentWidget() == box_login) {
            box_login->get_user_mcode()->setEnabled(true);
            box_login->get_login_pass()->setText("");
            box_login->get_mcode_lineedit()->setText("");
            box_login->set_code(messagebox(ret));
            if(box_login->get_stack_widget()->currentIndex() == 0){
                passlabel->show();
            } else if(box_login->get_stack_widget()->currentIndex() == 1) {
                codelable->show();
            }
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_reg) {
            box_reg->get_send_code()->setEnabled(true);
            box_reg->get_valid_code()->setText("");
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->get_valid_code()->setText("");
            box_pass->get_send_msg_btn()->setEnabled(true);
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_bind) {
            box_bind->get_send_code()->setEnabled(true);
            box_bind->get_code_lineedit()->setText("");
            box_bind->set_code(messagebox(ret));
            box_bind->get_tips()->show();
            setshow(stack_box);
        }
        return ;
    } else if(ret == 0) {
        if(stack_box->currentWidget() == box_login) {
            timer_log->start(1000);
            timerout_num_log = 60;
            send_btn_log->setEnabled(false);
        } else if(stack_box->currentWidget() == box_reg) {
            timer_reg->start(1000);
            timerout_num_reg = 60;
            send_btn_reg->setEnabled(false);
        } else if(stack_box->currentWidget() == box_pass) {
            timer->start(1000);
            timerout_num = 60;
            send_btn_fgt->setEnabled(false);
        } else if(stack_box->currentWidget() == box_bind) {
            timer_bind->start(1000);
            timerout_num_bind = 60;
            box_bind->get_send_code()->setEnabled(false);
        }
    }
}

/* 根据用户名发送验证码回调函数，发送手机验证码回执消息后执行此处 */
void Dialog_login_reg::on_get_mcode_by_name(int ret,QString uuid) {
    if(uuid != this->uuid) {
        return ;
    }

    if(is_used == false) {
        return ;
    }
    if(ret != 0) {
        if(stack_box->currentWidget() == box_login) {
            box_login->get_user_mcode()->setEnabled(true);
            box_login->get_login_pass()->setText("");
            box_login->get_mcode_lineedit()->setText("");
            box_login->set_code(messagebox(ret));
            if(box_login->get_stack_widget()->currentIndex() == 0){
                passlabel->show();
            } else if(box_login->get_stack_widget()->currentIndex() == 1) {
                codelable->show();
            }
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_reg) {
            box_reg->get_send_code()->setEnabled(true);
            box_reg->get_valid_code()->setText("");
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->get_send_msg_btn()->setEnabled(true);
            box_pass->get_valid_code()->setText("");
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_bind) {
            box_bind->get_send_code()->setEnabled(true);
            box_bind->get_code_lineedit()->setText("");
            box_bind->set_code(messagebox(ret));
            box_bind->get_tips()->show();
            setshow(stack_box);
        }
        return ;
    }  else if(ret == 0) {
        if(stack_box->currentWidget() == box_login) {
            box_login->get_user_mcode()->setEnabled(true);
            timer_log->start(1000);
            timerout_num_log = 60;
            send_btn_log->setEnabled(false);
        } else if(stack_box->currentWidget() == box_reg) {
            box_reg->get_send_code()->setEnabled(true);
            timer_reg->start(1000);
            timerout_num_reg = 60;
            send_btn_reg->setEnabled(false);
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->get_send_msg_btn()->setEnabled(true);
            timer->start(1000);
            timerout_num = 60;
            send_btn_fgt->setEnabled(false);
        } else if(stack_box->currentWidget() == box_bind) {
            box_bind->get_send_code()->setEnabled(true);
            timer_bind->start(1000);
            timerout_num_bind = 60;
            box_bind->get_send_code()->setEnabled(false);
        }
    }
}

/* 窗口重绘，加入阴影 */
void Dialog_login_reg::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter painter(this);
    QColor m_defaultBackgroundColor = qRgb(0, 0, 0);
    QPainterPath path1;
    path1.setFillRule(Qt::WindingFill);
    path1.addRoundedRect(10, 10, this->width() - 20, this->height() - 20, 6, 6);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path1, QBrush(QColor(m_defaultBackgroundColor.red(),
                                          m_defaultBackgroundColor.green(),
                                          m_defaultBackgroundColor.blue())));

    QColor color(0, 0, 0, 15);
    for (int i = 0; i < 6; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(10 - i, 10 - i, this->width() - (10 - i) * 2, this->height() - (10 - i) * 2, 6, 6);
        color.setAlpha(120 - qSqrt(i) * 50);
        painter.setPen(color);
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(palette().color(QPalette::Base)));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setX(10);
    rect.setY(10);
    rect.setWidth(rect.width() - 10);
    rect.setHeight(rect.height() - 10);
    // rect: 绘制区域  10 圆角弧度　6px
    painter.drawRoundedRect(rect, 6, 6);
}

/* 使得窗口可以任意移动 */
void Dialog_login_reg::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

/* 按下鼠标使得窗口可以任意移动 */
void Dialog_login_reg::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}

/* 子控件事件过滤，主要针对获得或者失去焦点时捕捉 */
bool Dialog_login_reg::eventFilter(QObject *w, QEvent *e) {

    //手机绑定的四个控件捕捉
    if(w == box_bind->get_code_lineedit()) {
        if (e->type() == QEvent::FocusIn && !box_bind->get_tips()->isHidden()) {
            box_bind->get_tips()->hide();
            setshow(stack_box);
        }
    }
    if(w == box_bind->get_phone_lineedit()) {
        if (e->type() == QEvent::FocusIn && !box_bind->get_tips()->isHidden()) {
            box_bind->get_tips()->hide();
            setshow(stack_box);
        }
    }

    //注册页面的控件捕捉
    if(w == reg_user) {
        if (e->type() == QEvent::FocusIn && user_tip->isHidden()) {

            user_tip->show();

            setshow(stack_box);
        } else if(e->type() == QEvent::FocusOut){
            user_tip->hide();
            user_tip->adjustSize();
            setshow(stack_box);
        }
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);
        }
    }
    if(w == reg_pass) {
        if (e->type() == QEvent::FocusIn && pass_tip->isHidden()) {
            pass_tip->show();
            pass_tip->adjustSize();

            setshow(stack_box);
        } else if(e->type() == QEvent::FocusOut){
            pass_tip->hide();

            setshow(stack_box);
        }
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);
        }
    }
    if(w == reg_confirm) {
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);
        }
    }
    if(w == valid_code) {
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);
        }
    }
    if(w == phone_user) {
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);
        }
    }

    //登录页面的控件捕捉
    if(w == login_pass) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();

            setshow(stack_box);
        }
    }
    if(w ==login_user) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();

            setshow(stack_box);
        }
        if (e->type() == QEvent::FocusIn && !codelable->isHidden()) {
            codelable->hide();

            setshow(stack_box);

        }
    }
    if(w == login_mcode) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();

            setshow(stack_box);
        }
    }
    if(w == login_code) {
        if (e->type() == QEvent::FocusIn && !codelable->isHidden()) {
            codelable->hide();

            setshow(stack_box);

        }
    }

    //忘记密码页面的控件捕捉
    if(w == pass_pwd) {
        if (e->type() == QEvent::FocusIn && !pass_tips->isHidden()) {
            pass_tips->hide();

            setshow(stack_box);

        }
        if (e->type() == QEvent::FocusIn && passtips->isHidden()) {
            passtips->show();

            setshow(stack_box);

        } else if (e->type() == QEvent::FocusOut && !passtips->isHidden()) {
            passtips->hide();

            setshow(stack_box);
        }
    }
    if(w == pass_confirm) {
        if (e->type() == QEvent::FocusIn && !pass_tips->isHidden()) {
            pass_tips->hide();

            setshow(stack_box);

        }
    }
    if(w == pass_code) {
        if (e->type() == QEvent::FocusIn && !pass_tips->isHidden()) {
            pass_tips->hide();

            setshow(stack_box);

        }
    }
    if(w == pass_user) {
        if (e->type() == QEvent::FocusIn && !pass_tips->isHidden()) {
            pass_tips->hide();

            setshow(stack_box);

        }
    }


    if(w == stack_box) {
        if(e->type() == QEvent::FocusOut) {
            setshow(stack_box);
        }
    }

    if(w == login_submit) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();
            setshow(stack_box);

        }
        if (e->type() == QEvent::FocusIn && !reg_tips->isHidden()) {
            reg_tips->hide();
            setshow(stack_box);

        }
        if (e->type() == QEvent::FocusIn && !pass_tips->isHidden()) {
            pass_tips->hide();
            setshow(stack_box);

        }
        if (e->type() == QEvent::FocusIn && !codelable->isHidden()) {
            codelable->hide();
            setshow(stack_box);

        }
    }
    return QWidget::eventFilter(w,e);
}

/* 页面的清空，包括所有子页面的清空 */
void Dialog_login_reg::set_clear() {
    if(gif->isHidden() == false) {
        pm->stop();
        gif->hide();
    }
    del_btn->show();
    //qDebug()<<"11111back normal";
    basewidegt->setCurrentWidget(log_reg);
    setshow(basewidegt);
    title->setText(tr("Sign in Cloud"));
    stack_box->setCurrentWidget(box_login);
    box_login->set_clear();
    if(stack_box->currentWidget() == box_login) {
        box_login->set_clear();
    }else if(stack_box->currentWidget() == box_reg) {
        box_reg->set_clear();
        emit register_account->clicked();
    }else if(stack_box->currentWidget() == box_pass) {
        box_pass->set_clear();
        emit register_account->clicked();
    }
    box_login->set_window2();
    del_btn->raise();
    setshow(basewidegt);
}

void Dialog_login_reg::set_back() {
    basewidegt->setEnabled(true);
}

/* 关闭按钮触发处理 */
void Dialog_login_reg::on_close() {
    //qDebug()<<"yes";
    basewidegt->setEnabled(true);
    pm->stop();
    gif->hide();
    login_submit->setText(tr("Sign in"));
    box_login->get_mcode_widget()->set_change(1);
    back_login_btn();
    set_clear();
    close();
}

