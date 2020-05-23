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
#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QWidget(parent) {
    this->parent();


    //Basic Widget Allocation
    vlayout = new QVBoxLayout;
    vlayout_p = new QVBoxLayout;
    waylayout = new QVBoxLayout;
    stack_widget = new QStackedWidget(this);
    widget_number = new QWidget(this);
    widget_phone = new QWidget(this);

    HBox_forget = new QHBoxLayout;
    HBox_forgett = new QHBoxLayout;

    account_phone = new QLineEdit(this);

    account_pass = new ql_lineedit_pass(widget_number);
    forgot_pass_btn = new QPushButton(tr("Forget"),this);

    valid_code = new QLineEdit(widget_phone);
    send_msg_submit = new QPushButton(tr("Send"),widget_phone);
    account_login_btn = new QPushButton(tr("User Sign in"),this);
    message_login_btn = new QPushButton(tr("Quick Sign in"),this);

    error_code = new QLabel(this);
    error_pass = new QLabel(this);

    mcode = new mcode_widget(widget_number);
    mcode_lineedit = new QLineEdit(widget_number);
    mcode_layout = new QHBoxLayout;

    error_code->setFixedHeight(32);
    error_pass->setFixedHeight(32);

    error_code->setContentsMargins(9,0,0,0);
    error_pass->setContentsMargins(9,0,0,0);
    error_code->setScaledContents(true);
    error_pass->setScaledContents(true);

    error_code->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                              "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    error_pass->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                              "&nbsp;&nbsp;"+code+"</span></p></body></html>");

    //Basic Widget Configuration



    this->setContentsMargins(0,0,0,0);

    stack_widget->addWidget(widget_number);
    stack_widget->addWidget(widget_phone);
    stack_widget->setContentsMargins(0,0,0,0);

    widget_number->adjustSize();
    widget_number->setContentsMargins(0,0,0,0);

    widget_phone->adjustSize();
    widget_phone->setContentsMargins(0,0,0,0);

    account_login_btn->setMaximumSize(90,36);
    account_login_btn->setMinimumSize(90,36);
    //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
    message_login_btn->setMaximumSize(90,36);
    message_login_btn->setMinimumSize(90,36);
    //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
    account_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);} "
                                     "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                     "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
    message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                     "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                     "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
    account_login_btn->setFocusPolicy(Qt::NoFocus);
    message_login_btn->setFocusPolicy(Qt::NoFocus);

    account_phone->setMaxLength(30);
    account_phone->setMaximumSize(338,36);
    account_phone->setMinimumSize(338,36);
    account_phone->setTextMargins(12,0,0,0);
    account_phone->setFocusPolicy(Qt::StrongFocus);
    account_phone->setPlaceholderText(tr("Your account here"));

    //account_phone->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                             "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                             "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");

    mcode_lineedit->setMaxLength(4);
    mcode_lineedit->setTextMargins(12,0,0,0);
    mcode_lineedit->setPlaceholderText(tr("Your code here"));
    //mcode_lineedit->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                              "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                              "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    account_phone->setContentsMargins(0,0,0,0);
    QHBoxLayout *HBox_way = new QHBoxLayout;
    HBox_way->setMargin(0);
    HBox_way->setSpacing(0);
    HBox_way->addWidget(account_login_btn);
    account_login_btn->setContentsMargins(0,0,0,0);
    HBox_way->addSpacing(36);
    message_login_btn->setContentsMargins(0,0,0,0);
    HBox_way->addWidget(message_login_btn);
    HBox_way->setAlignment(Qt::AlignLeft);

    stack_widget->setFixedSize(338,113);
    account_phone->setContentsMargins(0,0,0,0);
    stack_widget->setContentsMargins(0,0,0,0);

    waylayout->setMargin(0);
    waylayout->setSpacing(0);
    //waylayout->addSpacing(24);
    waylayout->addLayout(HBox_way);
    waylayout->addSpacing(16);
    waylayout->addWidget(account_phone);
    waylayout->addSpacing(8);
    waylayout->addWidget(stack_widget);
    waylayout->setAlignment(Qt::AlignTop);
    setLayout(waylayout);

    //Connect
    QWidget::connect(account_login_btn,SIGNAL(clicked()),this,SLOT(set_window2()));
    QWidget::connect(message_login_btn,SIGNAL(clicked()),this,SLOT(set_window1()));
    QWidget::connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
   //Subabstract Build
    login_account_thr_number();
    login_account_thr_phone();
    QRegExp regx("^[a-zA-Z0-9_@.-]+$");
    QValidator *validator = new QRegExpValidator(regx, account_phone );
    account_phone->setValidator(validator);
    //Initial configuration
    stack_widget->setCurrentIndex(0);
    //qDebug()<<stack_widget->currentIndex();
    adjustSize();
}

/* 用户密码登录方式的界面设置 *
 * Setting the page of login that use password way. */ 
void LoginDialog::startaction_1() {
    if(stack_widget->currentIndex() == 0) {
        set_clear();
        account_phone->setFocus();
        account_phone->setText("");
        account_phone->setMaxLength(11);
        QRegExp regx("^((13[0-9])|(14[5,7])|(15[0-3,5-9])|(17[0,3,5-8])|(18[0-9])|166|198|199|(147))\\d{8}$");
        QValidator *validator = new QRegExpValidator(regx, account_phone );
        account_phone->setValidator(validator);
        account_login_btn->setMaximumSize(90,36);
        account_login_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        message_login_btn->setMaximumSize(90,36);
        message_login_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        account_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:#3D6BE5;} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        account_login_btn->setFocusPolicy(Qt::NoFocus);
        message_login_btn->setFocusPolicy(Qt::NoFocus);

        stack_widget->setCurrentIndex(1);
        account_phone->setPlaceholderText(tr("Your phone number here"));
    }
}

/* 同上 */
void LoginDialog::set_window1() {
    startaction_1();
}

void LoginDialog::set_window2() {
    startaction_2();
}

QString LoginDialog::get_user_name() {
    return this->account_phone->text();
}

QString LoginDialog::get_user_pass() {
    return this->account_pass->text();
}

void LoginDialog::startaction_2() {
    if(stack_widget->currentIndex() == 1) {
        set_clear();
        account_phone->setFocus();
        QRegExp regx("^[a-zA-Z0-9_@.-]+$");
        account_phone->setMaxLength(30);
        QValidator *validator = new QRegExpValidator(regx, account_phone );
        account_phone->setValidator(validator);
        account_login_btn->setMaximumSize(90,36);
        account_login_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        message_login_btn->setMaximumSize(90,36);
        message_login_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        account_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        account_login_btn->setFocusPolicy(Qt::NoFocus);
        message_login_btn->setFocusPolicy(Qt::NoFocus);
        stack_widget->setCurrentIndex(0);
        account_phone->setPlaceholderText(tr("Your account here"));
    }
}


bool LoginDialog::login_account_thr_number() {
    //Fill the container and allocation

    //Congfigurate the widgets
    account_phone->setFocus();
    QRegExp regx("^[a-zA-Z0-9_@.-]+$");
    QValidator *validator = new QRegExpValidator(regx, account_phone );
    account_phone->setValidator(validator);

    account_pass->setPlaceholderText(tr("Your password here"));
    account_pass->setMaximumSize(338,36);
    account_pass->setMinimumSize(338,36);
    account_pass->setTextMargins(12,0,0,0);
    account_pass->setMaxLength(30);
    //account_pass->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
    //                            "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
     //                           "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //account_phone->setGeometry(31 + sizeoff,148 + sizeoff,338,36);
    //account_pass->setGeometry(31 + sizeoff,192 + sizeoff,338,36);

    forgot_pass_btn->setMaximumSize(80,30);
    forgot_pass_btn->setMinimumSize(80,30);
    //forgot_pass_btn->setGeometry(289 + sizeoff,228 + sizeoff,80,30);
    forgot_pass_btn->setStyleSheet("QPushButton {font-size:14px;background: transparent;border-radius: 4px;}"
                                   "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                                   "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}");
    forgot_pass_btn->setFocusPolicy(Qt::NoFocus);
    mcode_lineedit->setFixedSize(202,36);

    //login_submit->setGeometry(31 + sizeoff,350 + sizeoff,338,36);
    //register_account->setGeometry(134 + sizeoff,406 + sizeoff,120,36);

    //Layout

    account_pass->setContentsMargins(0,0,0,0);
    vlayout->setMargin(0);
    vlayout->setSpacing(0);
    vlayout->addWidget(account_pass);
    vlayout->addSpacing(1);
    mcode_layout->addWidget(mcode_lineedit,0,Qt::AlignLeft);
    mcode_layout->addWidget(mcode,0,Qt::AlignRight);
    mcode_layout->setSpacing(0);
    mcode_layout->setContentsMargins(0,8,0,8);

    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, mcode_lineedit );
    mcode_lineedit->setValidator( validator_code );

    HBox_forget->addWidget(error_pass,0,Qt::AlignLeft);
    HBox_forget->setContentsMargins(0,8,0,8);
    HBox_forget->setSpacing(0);
    HBox_forget->addWidget(forgot_pass_btn,0,Qt::AlignRight);
    forgot_pass_btn->setContentsMargins(0,0,0,0);
    vlayout->addLayout(mcode_layout);
    vlayout->addSpacing(8);
    vlayout->addLayout(HBox_forget);
    widget_number->setLayout(vlayout);

    error_pass->hide();
    error_pass->setAttribute(Qt::WA_DontShowOnScreen);

    //Update Widgets
    return false;
}

QLineEdit* LoginDialog::get_user_edit() {
    return this->account_phone;
}

QPushButton* LoginDialog::get_forget_btn() {
    return this->forgot_pass_btn;
}

ql_lineedit_pass* LoginDialog::get_login_pass() {
    return account_pass;
}

QLabel* LoginDialog::get_tips_pass() {
    return error_pass;
}

QLineEdit* LoginDialog::get_login_code() {
    return valid_code;
}

QLabel* LoginDialog::get_tips_code() {
    return error_code;
}

QPushButton* LoginDialog::get_user_mcode() {
    return send_msg_submit;
}

void LoginDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

void LoginDialog::setstyleline() {
    if(stack_widget->currentIndex() == 0) {
        error_pass->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                                  "&nbsp;&nbsp;"+code+"</span></p></body></html>");
        //qDebug()<<"<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                    "&nbsp;&nbsp;"+code+"</span></p></body></html>";
    } else {
        error_code->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                                  "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    }
}

QStackedWidget* LoginDialog::get_stack_widget() {
    return stack_widget;
}

mcode_widget* LoginDialog::get_mcode_widget() {
    return mcode;
}

QLineEdit* LoginDialog::get_mcode_lineedit() {
    return mcode_lineedit;
}

bool LoginDialog::login_account_thr_phone() {
    //Fill the container and allocation
    //qDebug() <<"Hello";

    //Congfigurate the widgets
    account_phone->setFocus();
    QRegExp regx("^((13[0-9])|(14[5,7])|(15[0-3,5-9])|(17[0,3,5-8])|(18[0-9])|166|198|199|(147))\\d{8}$");
    QValidator *validator = new QRegExpValidator(regx, account_phone );
    account_phone->setValidator(validator);
    valid_code->setMaximumSize(192,36);
    valid_code->setMaxLength(4);
    valid_code->setPlaceholderText(tr("Your code here"));
    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, valid_code );
    valid_code->setValidator( validator_code );
    send_msg_submit->setMaximumSize(130,36);
    valid_code->setMinimumSize(192,36);
    send_msg_submit->setMinimumSize(130,36);
    send_msg_submit->setMaximumSize(130,36);
    valid_code->setTextMargins(12,0,0,0);
    //valid_code->setGeometry(31 + sizeoff,192 + sizeoff,192,36);
    //send_msg_submit->setGeometry(239 + sizeoff,192 + sizeoff,130,36);
    //valid_code->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #F4F4F4;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                          "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                          "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //send_msg_submit->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
     //                              "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
      //                             "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");
    send_msg_submit->setFocusPolicy(Qt::NoFocus);
    QWidget::setTabOrder(account_phone, valid_code);

    //Layout
    HBox_forgett->addWidget(valid_code);
    HBox_forgett->setSpacing(0);
    HBox_forgett->setMargin(0);
    HBox_forgett->addSpacing(16);
    HBox_forgett->addWidget(send_msg_submit);
    HBox_forgett->setAlignment(Qt::AlignLeft| Qt::AlignTop);
    send_msg_submit->setContentsMargins(0,0,0,0);
    valid_code->setContentsMargins(0,0,0,0);
    vlayout_p->addLayout(HBox_forgett);
    vlayout_p->addWidget(error_code);
    vlayout_p->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout_p->setMargin(0);
    vlayout_p->setSpacing(0);
    widget_phone->setLayout(vlayout_p);

    error_code->hide();
    error_code->setAttribute(Qt::WA_DontShowOnScreen);

    //Update Widgets
    return false;
}

/* 清空登录框 */
void LoginDialog::set_clear() {
    if(!error_code->isHidden()) {
        error_code->hide();
    }
    if(!error_pass->isHidden()) {
        error_pass->hide();
    }
    account_pass->get_visble()->setChecked(false);
    account_pass->setText("");
    account_phone->setText("");
    valid_code->setText("");
    mcode_lineedit->setText("");
}
