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
#include "maindialog.h"
#include <QDesktopWidget>
#include <QApplication>

MainDialog::MainDialog(QWidget *parent) : QWidget(parent)
{
    //内存分配
    m_uuid = QUuid::createUuid().toString();
    m_submitBtn = new QPushButton(tr("Sign in"),this);     //登录或者确认或者注册或者重置密码或者绑定手机按钮（重用）
    m_regBtn = new QPushButton(tr("Sign up"),this); //返回登录或者注册账户按钮（重用）
    m_loginDialog = new LoginDialog(this);      //登录页面
    m_regDialog = new RegDialog(this);          //注册页面
    m_BindDialog = new BindPhoneDialog(this);   //手机绑定页面
    m_passDialog = new PassDialog(this);        //忘记密码页面
    m_containerWidget = new QWidget(this);            //业务逻辑主界面
    m_baseWidget = new QStackedWidget(this);  //用于切换成功页面和业务逻辑操作页面（包括登录等模块)

    m_titleLable = new QLabel(status,this);        //页面标题（业务逻辑重用）
    m_stackedWidget = new QStackedWidget(this);   //用于切换业务逻辑操作页面（包括登录，注册，绑定，忘记密码）
    m_workLayout = new QVBoxLayout;           //业务界面主体布局
    m_subLayout = new QHBoxLayout;           //切换登录模式按钮布局
    m_delBtn = new QPushButton(this);        //关闭按钮
    m_cPassTimer = new QTimer(this);               //以下都是验证码倒计时计时器，后缀相互对应
    m_cRegTimer  = new QTimer(this);
    m_cLogTimer = new QTimer(this);
    m_cBindTimer = new QTimer(this);
    m_successDialog = new SuccessDiaolog(this);        //注册成功页面
    QHBoxLayout *hbox = new QHBoxLayout;    //整体布局
    m_blueEffect = new Blueeffect(m_submitBtn);
    m_animateLayout = new QHBoxLayout;
    m_animateLayout->addWidget(m_blueEffect);
    m_animateLayout->setMargin(0);
    m_animateLayout->setSpacing(0);
    m_animateLayout->setAlignment(Qt::AlignCenter);
    m_submitBtn->setLayout(m_animateLayout);
    m_blueEffect->settext(tr("Login in progress"));
    m_svgHandler = new SVGHandler(this);


    //计时器初始化
    m_cPassTimer->stop();
    m_cRegTimer->stop();
    m_cLogTimer->stop();
    m_cBindTimer->stop();

    //隐藏同步开关动画

    //控件尺寸以及布局设置
    this->setFixedSize(418,505);
    m_containerWidget->setFixedSize(418,505);
    m_stackedWidget->addWidget(m_loginDialog);
    m_stackedWidget->addWidget(m_regDialog);
    m_stackedWidget->addWidget(m_passDialog);
    m_stackedWidget->addWidget(m_BindDialog);

    m_submitBtn->setFocusPolicy(Qt::NoFocus);
    m_titleLable->setFocusPolicy(Qt::NoFocus);
    m_regBtn->setFocusPolicy(Qt::NoFocus);

    m_titleLable->setText(status);
    m_titleLable->adjustSize();
    //setFocusPolicy(Qt::NoFocus);
    m_loginDialog->setContentsMargins(0,0,0,0);
    //title->setGeometry(31 + sizeoff,48 + sizeoff,160,24);
    m_titleLable->setStyleSheet("font-size: 24px;font-weight:500;");

    m_submitBtn->setFixedSize(338,36);
    m_submitBtn->setFocusPolicy(Qt::NoFocus);
    m_regBtn->setMaximumSize(120,36);
    m_regBtn->setMinimumSize(120,36);
    m_stackedWidget->setFixedWidth(338);
    m_stackedWidget->setMinimumHeight(m_loginDialog->height());
    m_baseWidget->setFixedSize(418,505);
    m_baseWidget->setContentsMargins(0,0,0,0);
    m_baseWidget->addWidget(m_containerWidget);
    m_baseWidget->addWidget(m_successDialog);
    m_successDialog->set_mode_text(2);
    m_baseWidget->setCurrentWidget(m_containerWidget);
    m_submitBtn->setContentsMargins(0,0,0,0);
    m_regBtn->setFocusPolicy(Qt::NoFocus);
    m_regBtn->setContentsMargins(0,0,0,0);
    m_delBtn->setMaximumSize(30,30);
    m_delBtn->setMinimumSize(30,30);
    m_delBtn->setGeometry(this->width() - 46,14,30,30);
    m_delBtn->setFocusPolicy(Qt::NoFocus);

    m_submitBtn->setStyleSheet("QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                                "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                                "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    m_regBtn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                    "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                    "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");

    m_delBtn->setFlat(true);
    QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete.svg");
    m_delBtn->setIcon(pixmap);
    m_delBtn->setStyleSheet("QPushButton{background:transparent;border-radius:4px;}"
                           "QPushButton:hover{background:transparent;background-color:#F86457;"
                           "border-radius:4px}"
                           "QPushButton:click{background:transparent;background-color:#E44C50;border-radius:4px}");

    m_delBtn->installEventFilter(this);

    m_stackedWidget->setCurrentWidget(m_loginDialog);

    //主窗口布局样式设置
    //setStyleSheet("Dialog_login_reg{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    m_workLayout->setSpacing(0);
    m_workLayout->setContentsMargins(41,55,41,36);
    m_workLayout->addWidget(m_titleLable);
    m_titleLable->setMargin(0);
    m_workLayout->addSpacing(20);
    m_stackedWidget->setContentsMargins(0,0,0,0);
    m_workLayout->addWidget(m_stackedWidget);
    m_submitBtn->setContentsMargins(0,0,0,0);
    m_workLayout->addSpacing(0);
    m_workLayout->addWidget(m_submitBtn);
    m_subLayout->setSpacing(0);
    m_subLayout->setContentsMargins(0,10,0,0);
    m_subLayout->addWidget(m_regBtn);
    m_workLayout->addSpacing(10);
    m_workLayout->addLayout(m_subLayout);
    m_workLayout->addSpacing(20);
    m_workLayout->setEnabled(true);
    m_containerWidget->setLayout(m_workLayout);

    m_containerWidget->setContentsMargins(0,0,0,0);

    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(m_baseWidget);
    setLayout(hbox);

    m_stackedWidget->adjustSize();

    //注册绑定快捷键，让登录可以按Enter键登录
    m_submitBtn->setShortcut(QKeySequence::InsertParagraphSeparator);
    m_submitBtn->setShortcut(Qt::Key_Enter);
    m_submitBtn->setShortcut(Qt::Key_Return);


    /* 子控件成员公共函数获取，主要是为了用eventFilter函数去更改控件Focus事件
     * 处理，防止一些没有必要的重写类生成，并方便及时更改控件子控件的状态属性，
     * 避免过度使用信号与槽*/
    m_forgetpassBtn = m_passDialog->get_send_msg_btn();
    m_regSendCodeBtn = m_regDialog->get_send_code();
    m_forgetpassSendBtn = m_loginDialog->get_user_mcode();

    m_loginDialog->get_user_edit()->setFocus();



    m_loginPassLineEdit = m_loginDialog->get_login_pass();   //登录界面密码框
    m_loginLineEdit = m_loginDialog->get_login_code();   //登录界面手机框
    m_loginAccountLineEdit = m_loginDialog->get_user_edit();    //登录界面用户框
    m_loginMCodeLineEdit = m_loginDialog->get_mcode_lineedit();//登录界面验证码框

    m_regAccountLineEdit = m_regDialog->get_reg_user();         //注册界面用户框
    m_phoneLineEdit = m_regDialog->get_phone_user();     //注册界面手机框
    m_mcodeLineEdit = m_regDialog->get_valid_code();     //注册界面验证码框
    m_regPassLineEdit = m_regDialog->get_reg_pass();         //注册界面密码框
    m_regConfirmLineEdit = m_regDialog->get_reg_confirm();   //注册界面密码确认框

    m_passLineEdit = m_passDialog->get_reg_phone();      //忘记密码界面用户框
    m_passPasswordLineEdit = m_passDialog->get_reg_pass();        //忘记密码界面密码框
    m_passConfirmLineEdit = m_passDialog->get_reg_pass_confirm();//忘记密码确认密码框
    m_passMCodeLineEdit = m_passDialog->get_valid_code();     //忘记密码验证码框


    //忘记密码错误提示
    m_passTips = m_passDialog->get_passtips();

    //登录错误提示
    m_loginCodeStatusTips = m_loginDialog->get_tips_code();
    m_loginTips = m_loginDialog->get_tips_pass();

    //注册输入提示
    m_errorPassTips = m_passDialog->get_tips();
    m_errorRegTips = m_regDialog->get_tips();
    //注册错误消息提示

    m_accountTips = m_regDialog->get_user_tip();
    m_regTips = m_regDialog->get_pass_tip();

    /*界面逻辑有关信号与槽函数连接*/
    connect(m_delBtn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(m_loginDialog->get_forget_btn(),SIGNAL(clicked()),this,SLOT(linked_forget_btn()));
    connect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
    connect(m_forgetpassBtn,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(m_cPassTimer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    connect(m_cRegTimer,SIGNAL(timeout()),this,SLOT(on_timer_reg_out()));
    connect(m_cLogTimer,SIGNAL(timeout()),this,SLOT(on_timer_log_out()));
    connect(m_cBindTimer,SIGNAL(timeout()),this,SLOT(on_timer_bind_out()));
    connect(m_regSendCodeBtn,SIGNAL(clicked()),this,SLOT(on_send_code_reg()));
    connect(m_forgetpassSendBtn,SIGNAL(clicked()),this,SLOT(on_send_code_log()));
    connect(m_BindDialog->get_send_code(),SIGNAL(clicked()),this,SLOT(on_send_code_bind()));
    connect(m_successDialog->m_backloginBtn,SIGNAL(clicked()),this,SLOT(back_normal()));
    connect(m_passPasswordLineEdit,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(m_regPassLineEdit,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(m_loginDialog->get_mcode_lineedit(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_loginDialog->get_login_code(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_regDialog->get_valid_code(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_BindDialog->get_code_lineedit(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_passDialog->get_valid_code(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_loginDialog->get_stack_widget(),&QStackedWidget::currentChanged,[this] (int) {
        m_blueEffect->stop();
    });
    //为各个子控件安装事件过滤
    m_submitBtn->installEventFilter(this);


    m_loginPassLineEdit->installEventFilter(this);
    m_loginLineEdit->installEventFilter(this);
    m_loginAccountLineEdit->installEventFilter(this);
    m_loginMCodeLineEdit->installEventFilter(this);

    m_passPasswordLineEdit->installEventFilter(this);
    m_passMCodeLineEdit->installEventFilter(this);
    m_passConfirmLineEdit->installEventFilter(this);
    m_passLineEdit->installEventFilter(this);

    m_regPassLineEdit->installEventFilter(this);
    m_regAccountLineEdit->installEventFilter(this);
    m_regConfirmLineEdit->installEventFilter(this);
    m_phoneLineEdit->installEventFilter(this);
    m_mcodeLineEdit->installEventFilter(this);


    m_BindDialog->get_code_lineedit()->installEventFilter(this);
    m_BindDialog->get_phone_lineedit()->installEventFilter(this);

    m_stackedWidget->installEventFilter(this);

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
 * 成空的状态，防止用户多余操作(舍弃）*/
void MainDialog::cleanconfirm(QString str) {
    //qDebug()<<str;
    if(m_stackedWidget->currentWidget() == m_passDialog) {
        //m_passConfirmLineEdit->setText("");
    } else if(m_stackedWidget->currentWidget() == m_regDialog) {
        //m_regConfirmLineEdit->setText("");
    }
}

/* 登录按钮返回给上级控件接口 */
QPushButton * MainDialog::get_login_submit() {
    return m_submitBtn;
}

/* 设置DBUS客户端 */
void MainDialog::set_client(DbusHandleClient *c,QThread *t) {
    m_dbusClient = c;
    m_workThread  = t;
    connect(this,SIGNAL(dologin(QString,QString,QString)),m_dbusClient,SLOT(login(QString,QString,QString)));
    connect(this,SIGNAL(doreg(QString, QString, QString, QString,QString)),m_dbusClient,SLOT(registered(QString, QString, QString, QString,QString)));
    connect(this,SIGNAL(dobind(QString, QString, QString, QString,QString)),m_dbusClient,SLOT(bindPhone(QString, QString, QString, QString,QString)));
    connect(this,SIGNAL(dogetmcode_phone_reg(QString,QString)),m_dbusClient,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dogetmcode_phone_log(QString,QString)),m_dbusClient,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dogetmcode_number_pass(QString,QString)),m_dbusClient,SLOT(get_mcode_by_username(QString,QString)));
    connect(this,SIGNAL(dogetmcode_number_bind(QString,QString)),m_dbusClient,SLOT(get_mcode_by_phone(QString,QString)));
    connect(this,SIGNAL(dorest(QString, QString, QString,QString)),m_dbusClient,SLOT(user_resetpwd(QString, QString, QString,QString)));
    connect(this,SIGNAL(dophonelogin(QString,QString,QString)),m_dbusClient,SLOT(user_phone_login(QString,QString,QString)));
    connect(m_dbusClient,SIGNAL(finished_ret_log(int)),this,SLOT(setret_login(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_reg(int)),this,SLOT(setret_reg(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_phonelogin(int)),this,SLOT(setret_phone_login(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_rest(int)),this,SLOT(setret_rest(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_bind(int)),this,SLOT(setret_bind(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_code_log(int)),this,SLOT(setret_code_phone_login(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_code_reg(int)),this,SLOT(setret_code_phone_reg(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_code_pass(int)),this,SLOT(setret_code_user_pass(int)));
    connect(m_dbusClient,SIGNAL(finished_ret_code_bind(int)),this,SLOT(setret_code_user_bind(int)));

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
void MainDialog::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

/* 客户端回调函数集，一般处理异常出现的情况，成功一般
 * 不处理，除非是成功之后还要执行操作的 */
void MainDialog::setret_reg(int ret) {
    if(ret != 0) {
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(messagebox(ret));
        m_errorRegTips->show();
        setshow(m_stackedWidget);
        return ;
    } else {
    }
}

void MainDialog::setret_login(int ret) {
    if(ret != 0) {
        m_loginDialog->get_mcode_lineedit()->setText("");
        if(m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginTips->show();

            m_loginDialog->get_mcode_widget()->set_change(1);
            m_loginDialog->get_mcode_widget()->repaint();
            setshow(m_stackedWidget);
            m_loginDialog->get_mcode_widget()->set_change(0);
        } else {
            m_loginDialog->set_code(messagebox(ret));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
        }
        return ;
    } else {

    }
}

void MainDialog::setret_phone_login(int ret) {
    if(m_stackedWidget->currentWidget() != m_loginDialog && m_loginDialog->get_stack_widget()->currentIndex()) {
        return ;
    }
    if(ret != 0) {
        m_loginDialog->get_mcode_lineedit()->setText("");
        if(m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginTips->show();

            m_loginDialog->get_mcode_widget()->set_change(1);
            m_loginDialog->get_mcode_widget()->repaint();
            setshow(m_stackedWidget);
            m_loginDialog->get_mcode_widget()->set_change(0);
            return ;
        } else {
            m_loginDialog->set_code(messagebox(ret));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
            return ;
        }
    }
}

void MainDialog::setret_rest(int ret) {
    if(ret != 0) {
        m_passDialog->get_valid_code()->setText("");
        m_passDialog->set_code(messagebox(ret));
        m_errorPassTips->show();
        setshow(m_stackedWidget);

        return ;
    }else {

    }
}

void MainDialog::setret_bind(int ret) {
    if(ret != 0) {
        m_BindDialog->get_code_lineedit()->setText("");
        m_BindDialog->set_code(messagebox(ret));
        m_BindDialog->get_tips()->show();
        setshow(m_stackedWidget);

        return ;
    } else {
    }
}

void MainDialog::setret_code_phone_login(int ret) {
    if(m_stackedWidget->currentWidget() != m_loginDialog && m_loginDialog->get_stack_widget()->currentIndex() != 1) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        m_loginDialog->get_mcode_lineedit()->setText("");
        m_loginDialog->set_code(messagebox(ret));
        m_loginCodeStatusTips->show();
        setshow(m_stackedWidget);

        return ;
    }
}

void MainDialog::setret_code_user_bind(int ret) {
    if(m_stackedWidget->currentWidget() != m_BindDialog) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        m_BindDialog->get_code_lineedit()->setText("");
        m_BindDialog->set_code(messagebox(ret));
        m_BindDialog->get_tips()->show();
        setshow(m_stackedWidget);
        return ;
    }
}

void MainDialog::setret_code_phone_reg(int ret) {
    if(m_stackedWidget->currentWidget() != m_regDialog) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(messagebox(ret));
        m_errorRegTips->show();
        setshow(m_stackedWidget);

        return ;
    }
}

void MainDialog::setret_code_user_pass(int ret) {
    if(m_stackedWidget->currentWidget() != m_passDialog) {
        return ;
    }
    if(ret == 0) {
        //not do
    } else {
        m_passDialog->get_valid_code()->setText("");
        m_passDialog->set_code(messagebox(ret));
        m_errorPassTips->show();
        setshow(m_stackedWidget);
        return ;
    }
}

LoginDialog* MainDialog::get_dialog() {
    return m_loginDialog;
}

/* 错误消息提示盒子，所有服务器消息基本上来源于此，默认
 * 返回未知代码，显示错误以及代码编号 */
QString MainDialog::messagebox(int code) {
    QString ret = tr("Error code:") + QString::number(code,10)+ tr("!");
    switch(code) {
    case 101:ret = tr("Internal error occurred!");break;
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
    case 619:ret = tr("Sending code error occurred!");break;
    case -1:ret = tr("Please check your information!");break;

    }
    return ret;
}

/* 1.登录逻辑处理槽函数 */
void MainDialog::on_login_btn() {
    m_baseWidget->setEnabled(false); //防止用户在登录按钮按完之后到处乱点，下同
    set_staus(false);
    m_delBtn->setEnabled(true);
    //如果验证码输入错误，执行此处
    if(m_loginDialog->get_stack_widget()->currentIndex() == 0 &&
        QString(m_loginDialog->get_mcode_widget()->get_verificate_code()) != m_loginDialog->get_mcode_lineedit()->text() &&
            m_bAutoLogin == false) {
        m_loginDialog->set_code(tr("Your code is wrong!"));
        m_loginTips->show();
        m_baseWidget->setEnabled(true);
        set_staus(true);
        m_loginDialog->get_mcode_widget()->set_change(1);
        m_loginDialog->get_mcode_widget()->repaint();
        setshow(m_stackedWidget);
        m_loginDialog->get_mcode_lineedit()->setText("");
        m_loginDialog->get_mcode_widget()->set_change(0);
        return ;
    }
    //如果信息正确可提交，执行此处
    if(m_loginDialog->get_user_name() != "" &&
        m_loginDialog->get_user_pass() != "" &&
        m_loginDialog->get_stack_widget()->currentIndex() == 0){
        m_szAccount = m_loginDialog->get_user_name();
        m_szPass = m_loginDialog->get_user_pass();

        m_szRegPass = m_szPass;
        m_szRegAccount = m_szAccount;
        //qDebug()<<"1111111";

        m_submitBtn->setText("");
        m_blueEffect->startmoive();
        emit dologin(m_szRegAccount,m_szRegPass,m_uuid);            //触发登录信号，告知客户端进行登录操作

    } else if(m_loginDialog->get_user_name() != ""
               && m_loginDialog->get_login_code()->text() != ""
               && m_loginDialog->get_stack_widget()->currentIndex() == 1) {
        QString phone,mcode;                    //如果用户选择手机登录，执行此处
        phone = m_loginDialog->get_user_name();
        mcode = m_loginDialog->get_login_code()->text();
        emit dophonelogin(phone,mcode,m_uuid);
    } else {
        //信息填写不完整执行此处，包括密码登录以及手机登录
        if(m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(-1));
            m_loginTips->show();
            m_baseWidget->setEnabled(true);
            set_staus(true);
            m_loginDialog->get_mcode_widget()->set_change(1);
            m_loginDialog->get_mcode_widget()->repaint();
            setshow(m_stackedWidget);
            m_loginDialog->get_mcode_widget()->set_change(0);
            return ;
        } else {
            m_loginDialog->get_mcode_lineedit()->setText("");
            m_baseWidget->setEnabled(true);
            set_staus(true);
            m_loginDialog->set_code(messagebox(-1));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
            return ;
        }
    }
}

/* 2.注册逻辑处理槽函数 */
void MainDialog::on_reg_btn() {
    m_baseWidget->setEnabled(false);
    m_delBtn->setEnabled(true);
    bool ok_mcode = m_regDialog->get_user_mcode() != "";
    bool ok_phone = m_regDialog->get_user_phone() != "";
    bool ok_account = m_regDialog->get_user_account() != "";
    bool ok_passwd = m_regDialog->get_user_passwd() != "";
    bool ok_confirm = m_regDialog->get_reg_confirm()->text() != "";
    int ret = -1;
    if(ok_mcode && ok_phone && ok_account && ok_passwd &&ok_confirm) {
        QString account,passwd,phone,mcode,confirm;
        //qstrcpy(account,box_reg->get_user_account().toStdString().c_str());
        account = m_regDialog->get_user_account();
        phone = m_regDialog->get_user_phone();
        //qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        //qstrcpy(passwd,box_reg->get_user_passwd().toStdString().c_str());
        //qstrcpy(mcode,box_reg->get_user_mcode().toStdString().c_str());
        //qstrcpy(confirm,box_reg->get_reg_confirm()->text().toStdString().c_str());
        passwd = m_regDialog->get_user_passwd();
        mcode = m_regDialog->get_user_mcode();
        confirm = m_regDialog->get_reg_confirm()->text();
        if(confirm != passwd) {
            m_baseWidget->setEnabled(true);
            m_passDialog->set_code(tr("Please check your password!"));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
            return ;
        }
        if(!m_regDialog->get_reg_pass()->check()) {
            m_baseWidget->setEnabled(true);
            m_passDialog->set_code(tr("Please check your password!"));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
            return ;
        }
        m_szRegAccount = account;
        m_szRegPass = passwd;
        emit doreg(account,passwd,phone,mcode,m_uuid);
    } else {
        m_baseWidget->setEnabled(true);
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(messagebox(ret));
        m_errorRegTips->show();
        setshow(m_stackedWidget);
        return ;
    }
}

/* 3.忘记密码进入按钮处理槽函数 */
void MainDialog::on_pass_btn() {
    int ret = -1;
    m_baseWidget->setEnabled(false);
    m_delBtn->setEnabled(true);
    bool ok_phone = m_passDialog->get_user_name() == "";
    bool ok_pass = m_passDialog->get_user_newpass() == "";
    bool ok_confirm = m_passDialog->get_user_confirm() == "";
    bool ok_code = m_passDialog->get_user_mcode() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_confirm) {
        QString phone,pass,code,confirm;
        phone = m_passDialog->get_user_name();
        pass = m_passDialog->get_user_newpass();
        confirm = m_passDialog->get_user_confirm();
        code = m_passDialog->get_user_mcode();
//        qstrcpy(phone,box_pass->get_user_name().toStdString().c_str());
//        qstrcpy(pass,box_pass->get_user_newpass().toStdString().c_str());
//        qstrcpy(confirm,box_pass->get_user_confirm().toStdString().c_str());
//        qstrcpy(code,box_pass->get_user_mcode().toStdString().c_str());

        if(m_passDialog->get_reg_pass()->check() == false) {
            m_baseWidget->setEnabled(true);
            m_passDialog->set_code(tr("At least 6 bit, include letters and digt"));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
            return ;
        }
        if(confirm != pass) {
            m_baseWidget->setEnabled(true);
            m_passDialog->set_code(tr("Please check your password!"));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
            return ;
        }
        //qDebug()<<phone<<pass<<code;
        emit dorest(phone,pass,code,m_uuid);
    }else {
        m_baseWidget->setEnabled(true);
        m_passDialog->get_valid_code()->setText("");
        m_passDialog->set_code(messagebox(ret));
        m_errorPassTips->show();
        setshow(m_stackedWidget);
        return ;
    }
}

/* 4.绑定手机号码逻辑处理槽函数 */
void MainDialog::on_bind_btn() {
    int ret = -1;
    m_baseWidget->setEnabled(false);
    m_delBtn->setEnabled(true);
    bool ok_phone = m_BindDialog->get_phone() == "";
    bool ok_pass = m_szPass == "";
    bool ok_account = m_szAccount == "";
    bool ok_code = m_BindDialog->get_code() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_account) {
        QString phone,pass,account_s,code;
        phone = m_BindDialog->get_phone();
        //qstrcpy(pass,passwd.toStdString().c_str());
        pass = m_szPass;
        //qstrcpy(account_s,account.toStdString().c_str());
        account_s = m_szAccount;
        //qstrcpy(code,box_bind->get_code().toStdString().c_str());
        code = m_BindDialog->get_code();
        emit dobind(account_s,pass,phone,code,m_uuid);
    }else {
        m_BindDialog->get_code_lineedit()->setText("");
        m_BindDialog->set_code(messagebox(ret));
        m_BindDialog->get_tips()->show();
        m_baseWidget->setEnabled(true);
        setshow(m_stackedWidget);
        return ;
    }
}


/* 从成功注册，修改密码成功界面返回所需要的处理 */
void MainDialog::back_normal() {
    //回到登录框

    m_delBtn->show();
    //qDebug()<<"back normal";
    m_baseWidget->setCurrentWidget(m_containerWidget);
    m_successDialog->hide();
    setshow(m_baseWidget);
    m_titleLable->setText(tr("Sign in Cloud"));
    m_stackedWidget->setCurrentWidget(m_loginDialog);
    m_loginDialog->set_clear();

    //设置注册时候的账号密码为登录账号密码，然后执行登录逻辑
    if(m_bAutoLogin == true) {
        m_loginDialog->get_login_pass()->setText(m_szRegPass);
        m_loginDialog->get_user_edit()->setText(m_szRegAccount);
        m_submitBtn->click();
    }
}

/* 从忘记密码或者注册界面或者或者手机绑定
 * 界面返回到登录界面的必要操作 */
void MainDialog::back_login_btn() {
    //qDebug()<<stack_box->currentIndex();
    if(m_stackedWidget->currentWidget() != m_loginDialog) {
        m_titleLable->setText(tr("Sign in Cloud"));
        if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_regDialog->get_reg_pass()->clear();
            m_regDialog->get_reg_user()->clear();
            m_regDialog->get_phone_user()->clear();
            m_regDialog->get_valid_code()->clear();
            disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
            connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_passDialog->get_reg_pass()->clear();
            m_passDialog->get_reg_phone()->clear();
            m_passDialog->get_reg_pass_confirm()->clear();
            m_passDialog->get_valid_code()->clear();
            //qDebug()<<"back login";
            disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
            connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_BindDialog->setclear();
            disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
            connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        }
        m_loginDialog->set_clear();
        m_stackedWidget->setCurrentWidget(m_loginDialog);
        m_regBtn->setText(tr("Sign up"));
        m_submitBtn->setText(tr("Sign in"));
        m_regDialog->hide();
        setshow(m_stackedWidget);
        m_passDialog->hide();
        setshow(m_stackedWidget);

        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    }
}

/* 进入忘记密码界面的一些必要处理,重用了登录按钮 */
void MainDialog::linked_forget_btn() {
    if(m_stackedWidget->currentWidget()!= m_passDialog) {

        m_titleLable->setText(tr("Forget"));
        m_stackedWidget->setCurrentWidget(m_passDialog);
        m_submitBtn->setText(tr("Set"));
        m_regBtn->setText(tr("Back"));
        m_passDialog->set_clear();
        m_regDialog->hide();
        setshow(m_stackedWidget);
        m_loginDialog->hide();

        setshow(m_stackedWidget);

        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}

/* 进入注册界面的一些必要处理,重用了登录按钮 */
void MainDialog::linked_register_btn() {
    if(m_stackedWidget->currentWidget()!= m_regDialog) {

        m_titleLable->setText(tr("Create Account"));
        m_stackedWidget->setCurrentWidget(m_regDialog);
        m_regBtn->setText(tr("Back"));
        m_submitBtn->setText(tr("Sign up now"));
        m_passDialog->hide();
        m_loginDialog->hide();
        m_regDialog->set_clear();

        setshow(m_stackedWidget);
        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}

/* 注册验证码发送按钮处理 */
void MainDialog::on_send_code_reg() {
    QString phone;
    m_regDialog->get_send_code()->setEnabled(false);
    if( m_regDialog->get_user_account() == "" || m_regDialog->get_user_phone() == "") {
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(messagebox(-1));
        m_errorRegTips->show();
        m_regDialog->get_send_code()->setEnabled(true);
        setshow(m_stackedWidget);
        return ;
    }
    if(m_regDialog->get_reg_pass()->check() == false) {
        m_regDialog->get_send_code()->setEnabled(true);
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(tr("At least 6 bit, include letters and digt"));
            m_errorRegTips->show();
        setshow(m_stackedWidget);
        return ;
    }
    if(m_regDialog->get_user_phone() != "") {
        phone = m_regDialog->get_user_phone();
        emit dogetmcode_phone_reg(phone,m_uuid);
    } else {
        m_regDialog->get_send_code()->setEnabled(true);
        m_regDialog->get_valid_code()->setText("");
        m_regDialog->set_code(messagebox(-1));
        m_errorRegTips->show();
        setshow(m_stackedWidget);

        return ;
    }
}

/* 手机登录验证码发送按钮处理 */
void MainDialog::on_send_code_log() {
    QString phone;
    m_loginDialog->get_user_mcode()->setEnabled(false);
    if(m_loginDialog->get_user_name() != "") {
        phone = m_loginDialog->get_user_name();
        emit dogetmcode_phone_log(phone,m_uuid);
    } else {
        m_loginDialog->get_user_mcode()->setEnabled(true);
        m_loginDialog->get_mcode_lineedit()->setText("");
        m_loginDialog->set_code(messagebox(-1));
        m_loginCodeStatusTips->show();
        setshow(m_stackedWidget);

        return ;
    }
}

/* 手机绑定验证码发送按钮处理 */
void MainDialog::on_send_code_bind() {
    QString name;
    int ret = -1;
    m_BindDialog->get_send_code()->setEnabled(false);
    if(m_BindDialog->get_phone() != "") {
        //qstrcpy(name,box_bind->get_phone().toStdString().c_str());
        name = m_BindDialog->get_phone();
        emit dogetmcode_number_bind(name,m_uuid);
    }else {
        m_BindDialog->get_code_lineedit()->setText("");
        m_BindDialog->set_code(messagebox(ret));
        m_BindDialog->get_tips()->show();
        m_BindDialog->get_send_code()->setEnabled(true);
        setshow(m_stackedWidget);
        return ;
    }
}

/* 忘记密码验证码发送按钮处理 */
void MainDialog::on_send_code() {
    QString name;
    m_passDialog->get_send_msg_btn()->setEnabled(false);
    if(m_passDialog->get_reg_pass()->check() == false) {
        m_passDialog->get_valid_code()->setText("");
        m_passDialog->set_code(tr("At least 6 bit, include letters and digt"));
        m_errorPassTips->show();
        m_passDialog->get_send_msg_btn()->setEnabled(true);
        setshow(m_stackedWidget);
        return ;
    }
    if(m_passDialog->get_user_name() != "" && m_passDialog->get_user_confirm() !="" && m_passDialog->get_user_newpass() != "") {
        //qstrcpy(name,box_pass->get_user_name().toStdString().c_str());
        name = m_passDialog->get_user_name();
        emit dogetmcode_number_pass(name,m_uuid);
    }else {
        m_passDialog->get_valid_code()->setText("");
        m_errorPassTips->show();
        m_passDialog->set_code(messagebox(-1));
        m_passDialog->get_send_msg_btn()->setEnabled(true);
        setshow(m_stackedWidget);
        return ;
    }
}


/* 忘记密码验证码的计时器处理 */
void MainDialog::on_timer_timeout() {
    if(timerout_num > 0) {
        m_forgetpassBtn->setText(tr("Resend ( %1 )").arg(timerout_num));
        timerout_num --;
    }else if(timerout_num == 0) {
        timerout_num = 60;
        m_forgetpassBtn->setEnabled(true);
        m_forgetpassBtn->setText(tr("Get phone code"));
        m_cPassTimer->stop();
    }
}

/* 绑定手机验证码的计时器处理 */
void MainDialog::on_timer_bind_out() {
    if(timerout_num_bind > 0) {
        m_BindDialog->get_send_code()->setText(tr("Resend ( %1 )").arg(timerout_num_bind));
        timerout_num_bind --;
    }else if(timerout_num_bind == 0) {
        timerout_num_bind = 60;
        m_BindDialog->get_send_code()->setEnabled(true);
        m_BindDialog->get_send_code()->setText(tr("Get phone code"));
        m_cBindTimer->stop();
    }
}

/* 手机登录验证码的计时器处理 */
void MainDialog::on_timer_log_out() {
    if(timerout_num_log > 0) {
        m_forgetpassSendBtn->setText(tr("Resend ( %1 )").arg(timerout_num_log));
        timerout_num_log --;
    }else if(timerout_num_log == 0) {
        timerout_num_log = 60;
        m_forgetpassSendBtn->setEnabled(true);
        m_forgetpassSendBtn->setText(tr("Send"));
        m_cLogTimer->stop();
    }
}

/* 注册账户验证码的计时器处理 */
void MainDialog::on_timer_reg_out() {
    if(timerout_num_reg > 0) {
        m_regSendCodeBtn->setText(tr("Resend ( %1 )").arg(timerout_num_reg));
        timerout_num_reg --;
    }else if(timerout_num_reg == 0) {
        timerout_num_reg = 60;
        m_regSendCodeBtn->setEnabled(true);
        m_regSendCodeBtn->setText(tr("Send"));
        m_cRegTimer->stop();
    }
}

/* 登录回调槽函数，登录回执消息后执行此处 */
void MainDialog::on_login_finished(int ret,QString uuid) {
    if(uuid != this->m_uuid) {
        qDebug()<<uuid<<this->m_uuid;
        return ;
    }
    qDebug()<<ret;
    //无手机号码绑定，进入手机号码绑定页面
    if(ret == 119) {
        set_back();
        m_blueEffect->stop();
        m_titleLable->setText(tr("Binding Phone"));
        m_stackedWidget->setCurrentWidget(m_BindDialog);
        m_regBtn->setText(tr("Back"));
        m_submitBtn->setText(tr("Bind now"));
        m_BindDialog->setclear();
        setshow(m_stackedWidget);
        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        return ;
    }
    //登录返回成功，执行此处
    if(ret == 0) {
        //m_blueEffect->stop();
        //m_submitBtn->setText(tr("Sign in"));
        emit on_login_success(); //发送成功登录信号给主页面
    } else {
        set_back();
        m_blueEffect->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
        m_submitBtn->setText(tr("Sign in"));
        if(m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginTips->show();
            m_loginDialog->get_mcode_widget()->set_change(1);
            m_loginDialog->get_mcode_widget()->repaint();
            setshow(m_stackedWidget);
            return ;
        } else if(m_loginDialog->get_stack_widget()->currentIndex() == 1) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
            return ;
        }
    }
}

/* 手机绑定回调槽函数，手机绑定回执消息后执行此处 */
void MainDialog::on_bind_finished(int ret,QString uuid) {
    if(uuid != this->m_uuid) {
        return ;
    }
    m_baseWidget->setEnabled(true);
    if(ret == 0) {
        timerout_num_bind = 0;
        m_cBindTimer->stop();
        m_submitBtn->setText(tr("Sign in"));
        m_BindDialog->get_send_code()->setEnabled(true);
        m_BindDialog->get_send_code()->setText(tr("Send"));
        m_BindDialog->setclear();
        m_titleLable->setText(tr("Sign in Cloud"));
        m_regBtn->setText(tr("Sign up"));
        m_stackedWidget->setCurrentWidget(m_loginDialog);
        setshow(m_stackedWidget);
        m_bAutoLogin = true;
        m_baseWidget->setCurrentWidget(m_successDialog);
        m_successDialog->set_mode_text(3);
        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    } else {
        m_BindDialog->set_code(messagebox(ret));
        m_BindDialog->get_tips()->show();
        setshow(m_stackedWidget);
        return ;
    }
}

/* 注册回调槽函数，注册回执消息后执行此处 */
void MainDialog::on_reg_finished(int ret,QString uuid) {
    if(this->m_uuid != uuid) {
        return ;
    }
    m_baseWidget->setEnabled(true);
    //qDebug()<<ret;
    if(ret == 0) {
        timerout_num_reg = 0;
        m_cRegTimer->stop();
        m_regSendCodeBtn->setEnabled(true);
        m_regSendCodeBtn->setText(tr("Send"));
        m_submitBtn->setText(tr("Sign in"));
        m_regDialog->get_reg_pass()->clear();
        m_regDialog->get_reg_user()->clear();
        m_regDialog->get_phone_user()->clear();
        m_regDialog->get_valid_code()->clear();
        m_bAutoLogin = true;
        m_baseWidget->setCurrentWidget(m_successDialog);
        m_delBtn->hide();
        m_successDialog->set_mode_text(0);
        m_regBtn->setText(tr("Sign up"));
        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    } else {
        m_regDialog->set_code(messagebox(ret));
        m_errorRegTips->show();
        setshow(m_stackedWidget);
        return ;
    }

}

/* 忘记密码回调槽函数，忘记密码回执消息后执行此处 */
void MainDialog::on_pass_finished(int ret,QString uuid) {
    if(uuid != this->m_uuid) {
        return ;
    }
    if(is_used == false) {
        return ;
    }
    m_baseWidget->setEnabled(true);
    if(ret == 0) {
        //qDebug()<<"cascascascascascascascascascascasca";
        timerout_num = 0;
        m_cPassTimer->stop();
        //qDebug()<<"wb11";
        m_forgetpassBtn->setEnabled(true);
        m_forgetpassBtn->setText(tr("Send"));
        m_submitBtn->setText(tr("Sign in"));
        //qDebug()<<"wb22";
        m_passDialog->get_reg_pass()->clear();
        m_passDialog->get_reg_phone()->clear();
        m_passDialog->get_reg_pass_confirm()->clear();
        m_passDialog->get_valid_code()->clear();
        //qDebug()<<"wb33";
        m_baseWidget->setCurrentWidget(m_successDialog);
        m_delBtn->hide();
        m_successDialog->set_mode_text(1);
        //qDebug()<<"wb44";
        m_regBtn->setText(tr("Sign up"));
        //qDebug()<<"wb55";
        disconnect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(m_regBtn,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(m_regBtn,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        //qDebug()<<"wb66";
    } else {
        m_passDialog->set_code(messagebox(ret));
        m_errorPassTips->show();
        setshow(m_stackedWidget);
        return ;
    }
}

/* 手机号直接发送验证码回调函数，发送手机验证码回执消息后执行此处 */
void MainDialog::on_get_mcode_by_phone(int ret,QString uuid) {
    if(uuid != this->m_uuid) {
        return ;
    }
    if(ret != 0) {
        if(m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->get_user_mcode()->setEnabled(true);
            m_loginDialog->get_login_pass()->setText("");
            m_loginDialog->get_mcode_lineedit()->setText("");
            m_loginDialog->set_code(messagebox(ret));
            if(m_loginDialog->get_stack_widget()->currentIndex() == 0){
                m_loginTips->show();
            } else if(m_loginDialog->get_stack_widget()->currentIndex() == 1) {
                m_loginCodeStatusTips->show();
            }
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_regDialog->get_send_code()->setEnabled(true);
            m_regDialog->get_valid_code()->setText("");
            m_regDialog->set_code(messagebox(ret));
            m_errorRegTips->show();
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_passDialog->get_valid_code()->setText("");
            m_passDialog->get_send_msg_btn()->setEnabled(true);
            m_passDialog->set_code(messagebox(ret));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_BindDialog->get_send_code()->setEnabled(true);
            m_BindDialog->get_code_lineedit()->setText("");
            m_BindDialog->set_code(messagebox(ret));
            m_BindDialog->get_tips()->show();
            setshow(m_stackedWidget);
        }
        return ;
    } else if(ret == 0) {
        if(m_stackedWidget->currentWidget() == m_loginDialog) {
            m_cLogTimer->start(1000);
            timerout_num_log = 60;
            m_forgetpassSendBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_cRegTimer->start(1000);
            timerout_num_reg = 60;
            m_regSendCodeBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_cPassTimer->start(1000);
            timerout_num = 60;
            m_forgetpassBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_cBindTimer->start(1000);
            timerout_num_bind = 60;
            m_BindDialog->get_send_code()->setEnabled(false);
        }
    }
}

/* 根据用户名发送验证码回调函数，发送手机验证码回执消息后执行此处 */
void MainDialog::on_get_mcode_by_name(int ret,QString uuid) {
    if(uuid != this->m_uuid) {
        return ;
    }

    if(is_used == false) {
        return ;
    }
    if(ret != 0) {
        if(m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->get_user_mcode()->setEnabled(true);
            m_loginDialog->get_login_pass()->setText("");
            m_loginDialog->get_mcode_lineedit()->setText("");
            m_loginDialog->set_code(messagebox(ret));
            if(m_loginDialog->get_stack_widget()->currentIndex() == 0){
                m_loginTips->show();
            } else if(m_loginDialog->get_stack_widget()->currentIndex() == 1) {
                m_loginCodeStatusTips->show();
            }
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_regDialog->get_send_code()->setEnabled(true);
            m_regDialog->get_valid_code()->setText("");
            m_regDialog->set_code(messagebox(ret));
            m_errorRegTips->show();
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_passDialog->get_send_msg_btn()->setEnabled(true);
            m_passDialog->get_valid_code()->setText("");
            m_passDialog->set_code(messagebox(ret));
            m_errorPassTips->show();
            setshow(m_stackedWidget);
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_BindDialog->get_send_code()->setEnabled(true);
            m_BindDialog->get_code_lineedit()->setText("");
            m_BindDialog->set_code(messagebox(ret));
            m_BindDialog->get_tips()->show();
            setshow(m_stackedWidget);
        }
        return ;
    }  else if(ret == 0) {
        if(m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->get_user_mcode()->setEnabled(true);
            m_cLogTimer->start(1000);
            timerout_num_log = 60;
            m_forgetpassSendBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_regDialog->get_send_code()->setEnabled(true);
            m_cRegTimer->start(1000);
            timerout_num_reg = 60;
            m_regSendCodeBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_passDialog->get_send_msg_btn()->setEnabled(true);
            m_cPassTimer->start(1000);
            timerout_num = 60;
            m_forgetpassBtn->setEnabled(false);
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_BindDialog->get_send_code()->setEnabled(true);
            m_cBindTimer->start(1000);
            timerout_num_bind = 60;
            m_BindDialog->get_send_code()->setEnabled(false);
        }
    }
}

/* 窗口重绘，加入阴影 */
void MainDialog::paintEvent(QPaintEvent *event)
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
void MainDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

/* 按下鼠标使得窗口可以任意移动 */
void MainDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}

/* 子控件事件过滤，主要针对获得或者失去焦点时捕捉 */
bool MainDialog::eventFilter(QObject *w, QEvent *e) {

    if(w == m_delBtn) {
        if(e->type() == QEvent::Enter) {
            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete_click.svg");
            m_delBtn->setIcon(pixmap);
        }
        if(e->type() == QEvent::Leave) {
            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete.svg");
            m_delBtn->setIcon(pixmap);
        }
    }
    //手机绑定的四个控件捕捉
    if(w == m_BindDialog->get_code_lineedit()) {
        if (e->type() == QEvent::FocusIn && !m_BindDialog->get_tips()->isHidden()) {
            m_BindDialog->get_tips()->hide();
            setshow(m_stackedWidget);
        }
    }
    if(w == m_BindDialog->get_phone_lineedit()) {
        if (e->type() == QEvent::FocusIn && !m_BindDialog->get_tips()->isHidden()) {
            m_BindDialog->get_tips()->hide();
            setshow(m_stackedWidget);
        }
    }

    //注册页面的控件捕捉
    if(w == m_regAccountLineEdit) {
        if (e->type() == QEvent::FocusIn && m_accountTips->isHidden()) {

            m_accountTips->show();

            setshow(m_stackedWidget);
        } else if(e->type() == QEvent::FocusOut){
            m_accountTips->hide();
            m_accountTips->adjustSize();
            setshow(m_stackedWidget);
        }
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);
        }
    }
    if(w == m_regPassLineEdit) {
        if (e->type() == QEvent::FocusIn && m_regTips->isHidden()) {
            m_regTips->show();
            m_regTips->adjustSize();

            setshow(m_stackedWidget);
        } else if(e->type() == QEvent::FocusOut){
            m_regTips->hide();

            setshow(m_stackedWidget);
        }
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);
        }
    }
    if(w == m_regConfirmLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);
        }
    }
    if(w == m_mcodeLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);
        }
    }
    if(w == m_phoneLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);
        }
    }

    //登录页面的控件捕捉
    if(w == m_loginPassLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
            m_loginTips->hide();

            setshow(m_stackedWidget);
        }
    }
    if(w ==m_loginAccountLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
            m_loginTips->hide();

            setshow(m_stackedWidget);
        }
        if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden()) {
            m_loginCodeStatusTips->hide();

            setshow(m_stackedWidget);

        }
    }
    if(w == m_loginMCodeLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
            m_loginTips->hide();

            setshow(m_stackedWidget);
        }
    }
    if(w == m_loginLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden()) {
            m_loginCodeStatusTips->hide();

            setshow(m_stackedWidget);

        }
    }

    //忘记密码页面的控件捕捉
    if(w == m_passPasswordLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorPassTips->isHidden()) {
            m_errorPassTips->hide();

            setshow(m_stackedWidget);

        }
        if (e->type() == QEvent::FocusIn && m_passTips->isHidden()) {
            m_passTips->show();

            setshow(m_stackedWidget);

        } else if (e->type() == QEvent::FocusOut && !m_passTips->isHidden()) {
            m_passTips->hide();

            setshow(m_stackedWidget);
        }
    }
    if(w == m_passConfirmLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorPassTips->isHidden()) {
            m_errorPassTips->hide();

            setshow(m_stackedWidget);

        }
    }
    if(w == m_passMCodeLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorPassTips->isHidden()) {
            m_errorPassTips->hide();

            setshow(m_stackedWidget);

        }
    }
    if(w == m_passLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_errorPassTips->isHidden()) {
            m_errorPassTips->hide();

            setshow(m_stackedWidget);

        }
    }


    if(w == m_stackedWidget) {
        if(e->type() == QEvent::FocusOut) {
            setshow(m_stackedWidget);
        }
    }

    if(w == m_submitBtn) {
        if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
            m_loginTips->hide();
            setshow(m_stackedWidget);

        }
        if (e->type() == QEvent::FocusIn && !m_errorRegTips->isHidden()) {
            m_errorRegTips->hide();
            setshow(m_stackedWidget);

        }
        if (e->type() == QEvent::FocusIn && !m_errorPassTips->isHidden()) {
            m_errorPassTips->hide();
            setshow(m_stackedWidget);

        }
        if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden()) {
            m_loginCodeStatusTips->hide();
            setshow(m_stackedWidget);

        }
    }
    return QWidget::eventFilter(w,e);
}

/* 页面的清空，包括所有子页面的清空 */
void MainDialog::set_clear() {

    m_delBtn->show();
    //qDebug()<<"11111back normal";
    m_baseWidget->setCurrentWidget(m_containerWidget);
    setshow(m_baseWidget);
    m_titleLable->setText(tr("Sign in Cloud"));
    m_stackedWidget->setCurrentWidget(m_loginDialog);
    m_loginDialog->set_clear();
    if(m_stackedWidget->currentWidget() == m_loginDialog) {
        m_loginDialog->set_clear();
    }else if(m_stackedWidget->currentWidget() == m_regDialog) {
        m_regDialog->set_clear();
        emit m_regBtn->clicked();
    }else if(m_stackedWidget->currentWidget() == m_passDialog) {
        m_passDialog->set_clear();
        emit m_regBtn->clicked();
    }
    m_loginDialog->set_window2();
    m_delBtn->raise();
    setshow(m_baseWidget);
}

void MainDialog::set_staus(bool ok) {
    if(m_baseWidget->currentWidget() == m_containerWidget) {
        if(m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->set_staus(ok);
        } else if(m_stackedWidget->currentWidget() == m_BindDialog) {
            m_BindDialog->set_staus(ok);
        } else if(m_stackedWidget->currentWidget() == m_passDialog) {
            m_passDialog->set_staus(ok);
        } else if(m_stackedWidget->currentWidget() == m_regDialog) {
            m_regDialog->set_staus(ok);
        }
        m_stackedWidget->setEnabled(ok);
        m_submitBtn->setEnabled(ok);
        m_regBtn->setEnabled(ok);
    }
}

void MainDialog::set_back() {
    m_baseWidget->setEnabled(true);
    set_staus(true);
}

/* 关闭按钮触发处理 */
void MainDialog::on_close() {
    //qDebug()<<"yes";
    m_forgetpassSendBtn->setEnabled(true);
    m_forgetpassSendBtn->setText(tr("Send"));
    m_baseWidget->setEnabled(true);
    m_blueEffect->stop();
    set_staus(true);

    m_loginDialog->get_mcode_widget()->set_change(1);
    back_login_btn();
    set_clear();
    close();
}

void MainDialog::closedialog() {
    m_forgetpassSendBtn->setEnabled(true);
    m_forgetpassSendBtn->setText(tr("Send"));
    m_baseWidget->setEnabled(true);
    m_blueEffect->stop();
    m_bAutoLogin = false;
    set_staus(true);
    m_submitBtn->setText(tr("Sign in"));
    m_loginDialog->get_mcode_widget()->set_change(1);

    back_login_btn();
    set_clear();
    close();

}

MainDialog::~MainDialog() {
}

