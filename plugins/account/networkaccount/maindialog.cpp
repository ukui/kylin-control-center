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
#include <QDesktopServices>
#include <QApplication>
#include <QUrl>


extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

MainDialog::MainDialog(QWidget *parent) : QDialog(parent)
{
    //内存分配
    m_uuid = QUuid::createUuid().toString();
    m_submitBtn = new QPushButton(tr("Sign in"),this);     //登录或者确认或者注册或者重置密码或者绑定手机按钮（重用）
    m_regBtn = new QPushButton(tr("Sign up"),this); //返回登录或者注册账户按钮（重用）
    m_loginDialog = new LoginDialog(this);      //登录页面
    //m_regDialog = new RegDialog(this);          //注册页面
    //m_BindDialog = new BindPhoneDialog(this);   //手机绑定页面
    //m_passDialog = new PassDialog(this);        //忘记密码页面
    m_containerWidget = new QWidget(this);            //业务逻辑主界面
    m_baseWidget = new QStackedWidget(this);  //用于切换成功页面和业务逻辑操作页面（包括登录等模块)

    m_titleLable = new QLabel(status,this);        //页面标题（业务逻辑重用）
    m_stackedWidget = new QStackedWidget(this);   //用于切换业务逻辑操作页面（包括登录，注册，绑定，忘记密码）
    m_workLayout = new QVBoxLayout;           //业务界面主体布局
    m_subLayout = new QHBoxLayout;           //切换登录模式按钮布局
    m_delBtn = new CloseButton(this);        //关闭按钮
    m_timer = new QTimer(this);
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
    m_timer->stop();
    //隐藏同步开关动画

    //控件尺寸以及布局设置
    this->setFixedSize(418,505);
    m_containerWidget->setFixedSize(418,505);
    m_stackedWidget->addWidget(m_loginDialog);
   // m_stackedWidget->addWidget(m_passDialog);
    //m_stackedWidget->addWidget(m_BindDialog);

    m_submitBtn->setFocusPolicy(Qt::NoFocus);
    m_titleLable->setFocusPolicy(Qt::NoFocus);
    m_regBtn->setFocusPolicy(Qt::NoFocus);

    m_titleLable->setText(status);
    m_titleLable->adjustSize();
    setFocusPolicy(Qt::NoFocus);
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
    m_baseWidget->setCurrentWidget(m_containerWidget);
    m_submitBtn->setContentsMargins(0,0,0,0);
    m_regBtn->setFocusPolicy(Qt::NoFocus);
    m_regBtn->setContentsMargins(0,0,0,0);
    m_delBtn->setMaximumSize(30,30);
    m_delBtn->setMinimumSize(30,30);
    m_delBtn->setGeometry(this->width() - 46,14,30,30);
//    m_delBtn->setFocusPolicy(Qt::NoFocus);

    m_submitBtn->setStyleSheet("QPushButton {background-color: #3790FA;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                                "QPushButton:hover {background-color: #40A9FB;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                                "QPushButton:click {background-color: #40A9FB;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    m_regBtn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;} "
                                    "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(55,144,250,0.85);}"
                                    "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(55,144,250,0.85);}");

//    m_delBtn->setFlat(true);
//    QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete.svg");
    m_delBtn->setIcon(QIcon(":/new/image/delete.svg"));

//    m_delBtn->installEventFilter(this);

    m_stackedWidget->setCurrentWidget(m_loginDialog);

    //主窗口布局样式设置
    //setStyleSheet("Dialog_login_reg{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::Tool);
    setModal(true);

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
   // m_forgetpassBtn = m_passDialog->get_send_msg_btn();
    m_forgetpassSendBtn = m_loginDialog->get_user_mcode();

    m_loginDialog->get_user_edit()->setFocus();



    m_loginPassLineEdit = m_loginDialog->get_login_pass();   //登录界面密码框
    m_loginLineEdit = m_loginDialog->get_login_code();   //登录界面手机框
    m_loginAccountLineEdit = m_loginDialog->get_user_edit();    //登录界面用户框
    m_loginMCodeLineEdit = m_loginDialog->get_mcode_lineedit();//登录界面验证码框

    //忘记密码错误提示
   // m_passTips = m_passDialog->get_passtips();

    //登录错误提示
    m_loginCodeStatusTips = m_loginDialog->get_tips_code();
    m_loginTips = m_loginDialog->get_tips_pass();
    m_loginPassLineEdit ->setContextMenuPolicy (Qt::NoContextMenu);

    //注册输入提示
    //m_errorPassTips = m_passDialog->get_tips();
    //注册错误消息提示

    /*界面逻辑有关信号与槽函数连接*/
    connect(m_delBtn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(m_loginDialog->get_forget_btn(),SIGNAL(clicked()),this,SLOT(on_pass_btn()));
    connect(m_regBtn,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
    connect(m_submitBtn,SIGNAL(clicked()),this,SLOT(on_login_btn()));
    //connect(m_forgetpassBtn,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    connect(m_forgetpassSendBtn,SIGNAL(clicked()),this,SLOT(on_send_code_log()));
    //connect(m_BindDialog->get_send_code(),SIGNAL(clicked()),this,SLOT(on_send_code_bind()));
    //connect(m_passPasswordLineEdit,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(m_loginDialog->get_mcode_lineedit(),SIGNAL(returnPressed()),this,SLOT(on_login_btn()));
    connect(m_loginDialog->get_login_code(),SIGNAL(returnPressed()),this,SLOT(on_login_btn()));
    //connect(m_BindDialog->get_code_lineedit(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    //connect(m_passDialog->get_valid_code(),SIGNAL(returnPressed()),m_submitBtn,SIGNAL(clicked()),Qt::UniqueConnection);
    connect(m_loginDialog->get_stack_widget(),&QStackedWidget::currentChanged,[this] (int) {
        m_blueEffect->stop();
    });

    connect(m_loginAccountLineEdit,&QLineEdit::textChanged,[this] (const QString &changed) {
        // qDebug() << changed;
       if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
           m_NameLogin = changed;
       } else {
           m_PhoneLogin = changed;
           QRegExp regExp("^1[3-9]\\d{9}$");
           if(regExp.exactMatch(m_PhoneLogin) == true) {
               if(m_loginCodeStatusTips->isHidden() == false) {
                   m_loginCodeStatusTips->hide();
               }
           }
       }
    });

    connect(m_loginDialog->get_stack_widget(),&QStackedWidget::currentChanged,[this] (int index) {
        if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginAccountLineEdit->setText(m_NameLogin);
        } else {
            m_loginAccountLineEdit->setText(m_PhoneLogin);
        }
    });
    //为各个子控件安装事件过滤
    m_submitBtn->installEventFilter(this);


    m_loginPassLineEdit->installEventFilter(this);
    m_loginLineEdit->installEventFilter(this);
    m_loginAccountLineEdit->installEventFilter(this);
    m_loginMCodeLineEdit->installEventFilter(this);


    m_stackedWidget->installEventFilter(this);

    //初始化一下验证码计时器激活时间
    timerout_num = 60;
}

/* 确认密码框如果遇到新密码或者注册密码改变，立即改变
 * 成空的状态，防止用户多余操作(舍弃）*/
void MainDialog::cleanconfirm(QString str) {
    //qDebug()<<str;
}

/* 登录按钮返回给上级控件接口 */
QPushButton * MainDialog::get_login_submit() {
    return m_submitBtn;
}

/* 设置DBUS客户端 */
void MainDialog::set_client(DBusUtils *c) {
    m_dbusClient = c;

    connect(this, &MainDialog::dologin, this, [=] (QString kylinID,QString pass) {
        QList<QVariant> argList;
        argList << kylinID << pass;
        m_dbusClient->callMethod("userLogin",argList);
    });

    connect(this, &MainDialog::dogetmcode_phone_log, this, [=] (QString phone) {
        QList<QVariant> argList;
        argList << phone;
        m_dbusClient->callMethod("getMCodeByPhone",argList);
    });

    connect(this, &MainDialog::dophonelogin, this, [=] (QString phone,QString code) {
        QList<QVariant> argList;
        argList << phone << code;
        m_dbusClient->callMethod("phoneLogin",argList);
    });

    connect(m_dbusClient,&DBusUtils::taskFinished,this,[=] (const QString &taskName,int ret) {
        if (taskName == "userLogin") {
            if (ret != 0) {
                emit on_login_failed();
                set_back();
                m_blueEffect->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
                m_submitBtn->setText(tr("Sign in"));
                m_loginDialog->get_mcode_lineedit()->setText("");
                if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
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
                //qDebug() << ret;
            }
        } else if (taskName == "phoneLogin") {
            if (m_stackedWidget->currentWidget() != m_loginDialog && m_loginDialog->get_stack_widget()->currentIndex()) {
                emit on_login_failed();
                set_back();
                //m_blueEffect->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
                m_submitBtn->setText(tr("Sign in"));
                return ;
            }
            if (ret != 0) {
                emit on_login_failed();
                set_back();
                m_blueEffect->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
                m_submitBtn->setText(tr("Sign in"));
                m_loginDialog->get_mcode_lineedit()->setText("");
                if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
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
        } else if (taskName == "getMCodeByPhone") {
            if (m_stackedWidget->currentWidget() != m_loginDialog && m_loginDialog->get_stack_widget()->currentIndex() != 1) {
                return ;
            }
            if (ret == 0) {
                //not do
            } else {
                m_loginDialog->get_mcode_lineedit()->setText("");
                m_loginDialog->set_code(messagebox(ret));
                m_loginDialog->get_user_mcode()->setEnabled(true);
                m_loginCodeStatusTips->show();
                setshow(m_stackedWidget);

                return ;
            }
        }
    });

    m_dbusClient->connectSignal( "finishedPassLogin", this, SLOT(on_login_finished(int)));
    m_dbusClient->connectSignal("finishedPhoneLogin",this,SLOT(on_login_finished(int)));
    m_dbusClient->connectSignal("finishedMCodeByPhone",this,SLOT(on_get_mcode_by_phone(int)));
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

LoginDialog* MainDialog::get_dialog() {
    return m_loginDialog;
}

/* 错误消息提示盒子，所有服务器消息基本上来源于此，默认
 * 返回未知代码，显示错误以及代码编号 */
QString MainDialog::messagebox(const int &code) const {
    QString ret = tr("Error code:") + QString::number(code,10)+ tr("!");
    switch(code) {
    case 101:ret = tr("Internal error occurred!");break;
    case 102:ret = tr("Failed to sign up!");break;
    case 103:ret = tr("Failed attempt to return value!");break;
    case 104:ret = tr("Check your connection!");break;
    case 105:ret = tr("Failed to get by phone!");break;
    case 106:ret = tr("Failed to get by user!");break;
    case 107:ret = tr("Failed to reset password!");break;
    case 108:ret = tr("Timeout!");break;
    case 109:ret = tr("Phone binding falied!");break;
    case 110:ret = tr("Please check your information!");break;
    case 401:ret = tr("Please check your account!");break;
    case 500:ret = tr("Failed due to server error!");break;
    case 501:ret = tr("User and passsword can't be empty!");break;
    case 502:ret = tr("User existing!");break;
    case 503:ret = tr("User doesn't exist!");break;
    case 504:ret = tr("Network can not reach!");break;
    case 505:ret = tr("Phone can't be empty!");break;
    case 511:ret = tr("Account or password error!");break;
    case 610:ret = tr("Phone number already in used!");break;
    case 611:ret = tr("Please check your format!");break;
    case 612:ret = tr("Your are reach the limit!");break;
    case 613:ret = tr("Please check your phone number!");break;
    case 614:ret = tr("Please check your code!");break;
    case 615:ret = tr("Account doesn't exist!");break;
    case 616:ret = tr("User has bound the phone!");break;
    case 619:ret = tr("Sending code error occurred!");break;
    case 632:ret = tr("Phone code is expired!");break;
    case 702:ret = tr("Phone code error!");break;
    case 703:ret = tr("Code can not be empty!");break;
    case 704:ret = tr("MCode can not be empty!");break;
    case -1:ret = tr("Please check your information!");break;

    }
    return ret;
}

/* 1.登录逻辑处理槽函数 */
void MainDialog::on_login_btn() {
    emit on_submit_clicked();
    m_baseWidget->setEnabled(false); //防止用户在登录按钮按完之后到处乱点，下同
    set_staus(false);
    m_delBtn->setEnabled(true);
   if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
       if (m_loginDialog->get_mcode_lineedit()->text().trimmed() == "") {
           m_loginDialog->set_code(messagebox(703));
           m_loginTips->show();
           m_baseWidget->setEnabled(true);
           set_staus(true);
           m_loginDialog->get_mcode_widget()->set_change(1);
           m_loginDialog->get_mcode_widget()->repaint();
           setshow(m_stackedWidget);
           m_loginDialog->get_mcode_lineedit()->setText("");
           m_loginDialog->get_mcode_widget()->set_change(0);
           emit on_login_failed();
           return ;
       }
       if (m_loginDialog->get_user_edit()->text().trimmed() == "" ||
               m_loginDialog->get_login_pass()->text().trimmed() == ""
               ) {
           m_loginDialog->set_code(messagebox(501));
           m_loginTips->show();
           m_baseWidget->setEnabled(true);
           set_staus(true);
           m_loginDialog->get_mcode_widget()->set_change(1);
           m_loginDialog->get_mcode_widget()->repaint();
           setshow(m_stackedWidget);
           m_loginDialog->get_mcode_lineedit()->setText("");
           m_loginDialog->get_mcode_widget()->set_change(0);
           emit on_login_failed();
           return ;
       }
       if (m_loginDialog->get_user_edit()->text().trimmed().contains("+")) {
           m_loginDialog->set_code(messagebox(503));
           m_loginTips->show();
           m_baseWidget->setEnabled(true);
           set_staus(true);
           m_loginDialog->get_mcode_widget()->set_change(1);
           m_loginDialog->get_mcode_widget()->repaint();
           setshow(m_stackedWidget);
           m_loginDialog->get_mcode_lineedit()->setText("");
           m_loginDialog->get_mcode_widget()->set_change(0);
           emit on_login_failed();
           return ;
       }
   }
    if (m_loginDialog->get_stack_widget()->currentIndex() == 1) {
        if (m_loginDialog->get_login_code()->text().trimmed() == "") {
            m_loginDialog->set_code(messagebox(704));
            m_loginCodeStatusTips->show();
            m_baseWidget->setEnabled(true);
            set_staus(true);
            setshow(m_stackedWidget);
            m_loginDialog->get_login_code()->setText("");
            emit on_login_failed();
            return ;
        }

        if (m_loginDialog->get_user_edit()->text().trimmed() == "") {
            m_loginDialog->set_code(messagebox(505));
            m_loginCodeStatusTips->show();
            m_baseWidget->setEnabled(true);
            set_staus(true);
            setshow(m_stackedWidget);
            m_loginDialog->get_login_code()->setText("");
            emit on_login_failed();
            return ;
        }
    }
    //如果验证码输入错误，执行此处
    if (m_loginDialog->get_stack_widget()->currentIndex() == 0 &&
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
        emit on_login_failed();
        return ;
    }
    if (m_loginDialog->get_user_name().length() < 11 && m_loginDialog->get_stack_widget()->currentIndex() == 1) {
        m_baseWidget->setEnabled(true);
        m_loginDialog->set_code(tr("Please check your phone!"));
        m_loginCodeStatusTips->show();
        set_staus(true);
        setshow(m_stackedWidget);
        emit on_login_failed();
        return ;
    }

    //如果信息正确可提交，执行此处
    if (m_loginDialog->get_user_name() != "" &&
        m_loginDialog->get_user_pass() != "" &&
        m_loginDialog->get_stack_widget()->currentIndex() == 0){
        m_szAccount = m_loginDialog->get_user_name();
        m_szPass = m_loginDialog->get_user_pass();
        m_szRegPass = m_szPass.trimmed();
        m_szRegAccount = m_szAccount.trimmed();
        //qDebug()<<m_szRegPass<<m_szRegAccount;

        m_submitBtn->setText("");
        m_blueEffect->startmoive();
        emit dologin(m_szRegAccount,m_szRegPass);            //触发登录信号，告知客户端进行登录操作

    } else if (m_loginDialog->get_user_name().trimmed() != ""
               && m_loginDialog->get_login_code()->text().trimmed() != ""
               && m_loginDialog->get_stack_widget()->currentIndex() == 1) {
        QString phone,mcode;                    //如果用户选择手机登录，执行此处
        mcode = m_loginDialog->get_login_code()->text().trimmed();
        phone = m_loginDialog->get_user_name().trimmed();
        m_submitBtn->setText("");
        m_blueEffect->startmoive();
        emit dophonelogin(phone,mcode);
    } else {
        emit on_login_failed();
        //信息填写不完整执行此处，包括密码登录以及手机登录
        if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(501));
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
            m_loginDialog->set_code(messagebox(505));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
            return ;
        }
    }
}

QString MainDialog::replace_blank(QString &str) {
    QString filter = "";
    QString ret = "";
    bool first = false;
    for(QChar c : str) {
        if (c != ' ' && !first) {
            filter.push_front(c);
            first = true;
        } else if (first) {
            filter.push_front(c);
        }
    }
    for(QChar c : qAsConst(filter)) {
        if (c != ' ' && !first) {
            ret.push_front(c);
            first = true;
        } else if (first) {
            ret.push_front(c);
        }
    }
    return ret;
}

/* 2.注册逻辑处理槽函数 */
void MainDialog::on_reg_btn() {
    QDesktopServices::openUrl(QUrl("https://id.kylinos.cn/registered"));
}

/* 3.忘记密码进入按钮处理槽函数 */
void MainDialog::on_pass_btn() {
    QDesktopServices::openUrl(QUrl("https://id.kylinos.cn/find"));
}



/* 手机登录验证码发送按钮处理 */
void MainDialog::on_send_code_log() {
    //qDebug() <<m_loginDialog->get_user_name().length();
    QString phone;
    m_loginDialog->get_user_mcode()->setEnabled(false);
    if (m_loginDialog->get_user_name().length() < 11) {
        m_baseWidget->setEnabled(true);
        m_loginDialog->get_user_mcode()->setEnabled(true);
        m_loginDialog->set_code(tr("Please check your phone!"));
        m_loginCodeStatusTips->show();
        setshow(m_stackedWidget);
        return ;
    }
    if (m_loginCodeStatusTips->isHidden() == false) {
        m_loginCodeStatusTips->hide();
    }
    if (m_loginDialog->get_user_name() != "") {
        phone = m_loginDialog->get_user_name();
        emit dogetmcode_phone_log(phone);
    } else {
        m_loginDialog->get_user_mcode()->setEnabled(true);
        m_loginDialog->get_mcode_lineedit()->setText("");
        m_loginDialog->set_code(messagebox(704));
        m_loginCodeStatusTips->show();
        setshow(m_stackedWidget);

        return ;
    }
}



/* 忘记密码验证码的计时器处理 */
void MainDialog::on_timer_timeout() {
    if (timerout_num > 0) {

        m_timer->start(1000);
        m_forgetpassSendBtn->setEnabled(false);
        m_forgetpassSendBtn->setText(tr("Resend ( %1 )").arg(timerout_num));
        timerout_num --;
    } else if (timerout_num == 0) {
        timerout_num = 60;
        m_forgetpassSendBtn->setEnabled(true);
        m_forgetpassSendBtn->setText(tr("Get"));
        m_timer->stop();
    }
}

/* 登录回调槽函数，登录回执消息后执行此处 */
void MainDialog::on_login_finished(int ret) {
    //qDebug() << "ssssssssssssssss2";
    //qDebug()<<ret;
    //无手机号码绑定，进入手机号码绑定页面
    //登录返回成功，执行此处
    if (ret == 0) {
        //m_blueEffect->stop();
        //m_submitBtn->setText(tr("Sign in"));
        emit on_login_success(); //发送成功登录信号给主页面
    } else {
        //qDebug() << "cscacacasca";
        emit on_login_failed();
        set_back();
        m_blueEffect->stop();             //登录失败，执行此处，关闭登录执行过程效果，并打印错误消息
        m_submitBtn->setText(tr("Sign in"));
        if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginTips->show();
            m_loginDialog->get_mcode_widget()->set_change(1);
            m_loginDialog->get_mcode_widget()->repaint();
            setshow(m_stackedWidget);
            return ;
        } else if (m_loginDialog->get_stack_widget()->currentIndex() == 1) {
            m_loginDialog->set_code(messagebox(ret));
            m_loginCodeStatusTips->show();
            setshow(m_stackedWidget);
            return ;
        }
    }
    //qDebug()<<"scascasca";
}

/* 手机号直接发送验证码回调函数，发送手机验证码回执消息后执行此处 */
void MainDialog::on_get_mcode_by_phone(int ret) {
    //qDebug() << ret;
    if (ret != 0) {
        if (m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->get_user_mcode()->setEnabled(true);
            m_loginDialog->get_login_pass()->setText("");
            m_loginDialog->get_mcode_lineedit()->setText("");
            m_loginDialog->set_code(messagebox(ret));
            if (m_loginDialog->get_stack_widget()->currentIndex() == 0){
                m_loginTips->show();
            } else if (m_loginDialog->get_stack_widget()->currentIndex() == 1) {
                m_loginCodeStatusTips->show();
            }
            setshow(m_stackedWidget);
        }
        return ;
    } else if (ret == 0) {
        m_forgetpassSendBtn->setEnabled(false);
        timerout_num = 60;
        m_timer->start(1000);
    }
}

/* 窗口重绘，加入阴影 */
void MainDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}

/* 子控件事件过滤，主要针对获得或者失去焦点时捕捉 */
bool MainDialog::eventFilter(QObject *w, QEvent *e) {

//    if (w == m_delBtn) {
//        if (e->type() == QEvent::Enter) {
//            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete_click.svg");
//            m_delBtn->setIcon(pixmap);
//        }
//        if (e->type() == QEvent::Leave) {
//            QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete.svg");
//            m_delBtn->setIcon(pixmap);
//        }
//    }
    //手机绑定的四个控件捕捉

    //登录页面的控件捕捉
    if (m_stackedWidget->currentWidget() == m_loginDialog) {
        if (w == m_loginPassLineEdit) {
            if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
                m_loginTips->hide();

               // setshow(m_stackedWidget);
            }
            if (e->type() == QEvent::KeyPress) {   
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);   
                if (keyEvent->matches(QKeySequence::Paste)) {   
                    return true;   
                }   
                if (keyEvent->matches(QKeySequence::Cut)) {   
                    return true;   
                }   
                if (keyEvent->matches(QKeySequence::Copy)) {   
                    return true;   
                }   
                if (keyEvent->matches(QKeySequence::Undo)) {   
                    return true;   
                }   
                if (keyEvent->matches(QKeySequence::Redo)) {   
                    return true;   
                }   
            }   
            if (e->type() == QEvent::MouseButtonRelease) {   
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);   
                if (mouseEvent->button() == Qt::MidButton) {   
                    return true;   
                }   
            }   
        }
        if (w ==m_loginAccountLineEdit) {
            if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
                m_loginTips->hide();

               // setshow(m_stackedWidget);
            }
            if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden()) {
                m_loginCodeStatusTips->hide();

              //  setshow(m_stackedWidget);

            }
        }
        if (w == m_loginMCodeLineEdit) {
            if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden()) {
                m_loginTips->hide();

               // setshow(m_stackedWidget);
            }
        }
        if (w == m_loginLineEdit) {
            if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden()) {
                m_loginCodeStatusTips->hide();

               // setshow(m_stackedWidget);

            }
        }
    }

    //忘记密码页面的控件捕捉



    if (w == m_submitBtn) {
        if (e->type() == QEvent::FocusIn && !m_loginTips->isHidden() && m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginTips->hide();
           // setshow(m_stackedWidget);

        }
        if (e->type() == QEvent::FocusIn && !m_loginCodeStatusTips->isHidden() && m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginCodeStatusTips->hide();
           // setshow(m_stackedWidget);

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
    if (m_stackedWidget->currentWidget() == m_loginDialog) {
        m_loginDialog->set_clear();
    }
    m_loginDialog->set_window2();
    //m_delBtn->raise();
}

void MainDialog::set_staus(const bool &ok) {
    if (m_baseWidget->currentWidget() == m_containerWidget) {
        if (m_stackedWidget->currentWidget() == m_loginDialog) {
            m_loginDialog->set_staus(ok);
        }
        m_stackedWidget->setEnabled(ok);
        m_submitBtn->setEnabled(ok);
        m_regBtn->setEnabled(ok);
    }
}

void MainDialog::set_back() {
    m_baseWidget->setEnabled(true);
    set_staus(true);
    m_blueEffect->stop();
    m_submitBtn->setText(tr("Sign in"));
    m_loginDialog->set_code(messagebox(108));
    m_loginDialog->get_mcode_widget()->set_change(1);
    m_loginTips->show();
    setshow(m_stackedWidget);
}

void MainDialog::setnormal() {
    m_baseWidget->setEnabled(true);
    set_staus(true);
    m_blueEffect->stop();
    m_submitBtn->setText(tr("Sign in"));
    if (m_loginDialog->get_stack_widget()->currentIndex() == 0) {
        m_loginDialog->set_code(messagebox(108));
        m_loginDialog->get_mcode_widget()->set_change(1);
        m_loginTips->show();
    } else {
        m_loginDialog->set_code(messagebox(108));
        m_loginCodeStatusTips->show();
    }
    setshow(m_stackedWidget);

}

/* 关闭按钮触发处理 */
void MainDialog::on_close() {
    close();
}

void MainDialog::closedialog() {
    close();

}

MainDialog::~MainDialog() {
}
