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
    m_normalLayout = new QVBoxLayout;
    m_phoneLayout = new QVBoxLayout;
    m_wayLayout = new QVBoxLayout;
    m_stackedWidget = new QStackedWidget(this);
    m_normalWidget = new QWidget(this);
    m_phoneWidget = new QWidget(this);

    m_subHBoxLayout = new QHBoxLayout;
    m_mcodeNormalLayout = new QHBoxLayout;

    m_accountLineEdit = new QLineEdit(this);

    m_passwordLineEdit = new PasswordLineEdit(m_normalWidget);
    m_forgetPasswordBtn = new QPushButton(tr("Forget"),this);

    m_mcodePhoneLineEdit = new QLineEdit(m_phoneWidget);
    m_sendMsgBtn = new QPushButton(tr("Send"),m_phoneWidget);
    m_accountLogin_btn = new QPushButton(tr("User Sign in"),this);
    m_phoneLogin_btn = new QPushButton(tr("Quick Sign in"),this);

    m_phoneTips = new Tips(this);
    m_passTips = new Tips(this);
    m_svgHandler = new SVGHandler(this);

    m_mcodeWidget = new MCodeWidget(m_normalWidget);
    m_mcodeNormalLineEdit = new QLineEdit(m_normalWidget);
    m_mcodePhoneLayout = new QHBoxLayout;

    m_phoneTips->setFixedHeight(32);
    m_passTips->setFixedHeight(32);

    m_phoneTips->setContentsMargins(0,0,0,0);
    m_passTips->setContentsMargins(0,0,0,0);

    //Basic Widget Configuration



    this->setContentsMargins(0,0,0,0);

    m_stackedWidget->addWidget(m_normalWidget);
    m_stackedWidget->addWidget(m_phoneWidget);
    m_stackedWidget->setContentsMargins(0,0,0,0);

    m_normalWidget->adjustSize();
    m_normalWidget->setContentsMargins(0,0,0,0);

    m_phoneWidget->adjustSize();
    m_phoneWidget->setContentsMargins(0,0,0,0);

    m_accountLogin_btn->setMaximumSize(126,36);
    m_accountLogin_btn->setMinimumSize(90,36);
    //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
    m_phoneLogin_btn->setMaximumSize(126,36);
    m_phoneLogin_btn->setMinimumSize(90,36);
    //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
    m_accountLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);} "
                                     "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                     "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
    m_phoneLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;} "
                                     "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                     "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
    m_accountLogin_btn->setFocusPolicy(Qt::NoFocus);
    m_phoneLogin_btn->setFocusPolicy(Qt::NoFocus);

    m_accountLineEdit->setMaxLength(30);
    m_accountLineEdit->setMaximumSize(338,36);
    m_accountLineEdit->setMinimumSize(338,36);
    m_accountLineEdit->setTextMargins(12,0,0,0);
    m_accountLineEdit->setFocusPolicy(Qt::StrongFocus);
    m_accountLineEdit->setPlaceholderText(tr("Your account/phone here"));

    //account_phone->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                             "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                             "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");

    m_mcodeNormalLineEdit->setMaxLength(4);
    m_mcodeNormalLineEdit->setTextMargins(12,0,0,0);
    m_mcodeNormalLineEdit->setPlaceholderText(tr("Your code here"));
    //mcode_lineedit->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                              "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                              "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    m_accountLineEdit->setContentsMargins(0,0,0,0);
    QHBoxLayout *HBox_way = new QHBoxLayout;
    HBox_way->setMargin(0);
    HBox_way->setSpacing(0);
    HBox_way->addWidget(m_accountLogin_btn);
    m_accountLogin_btn->setContentsMargins(0,0,0,0);
    HBox_way->addSpacing(36);
    m_phoneLogin_btn->setContentsMargins(0,0,0,0);
    HBox_way->addWidget(m_phoneLogin_btn);
    HBox_way->setAlignment(Qt::AlignLeft);

    m_stackedWidget->setFixedSize(338,113);
    m_accountLineEdit->setContentsMargins(0,0,0,0);
    m_stackedWidget->setContentsMargins(0,0,0,0);

    m_wayLayout->setMargin(0);
    m_wayLayout->setSpacing(0);
    //waylayout->addSpacing(24);
    m_wayLayout->addLayout(HBox_way);
    m_wayLayout->addSpacing(16);
    m_wayLayout->addWidget(m_accountLineEdit);
    m_wayLayout->addSpacing(8);
    m_wayLayout->addWidget(m_stackedWidget);
    m_wayLayout->setAlignment(Qt::AlignTop);
    setLayout(m_wayLayout);

    //Connect
    QWidget::connect(m_accountLogin_btn,SIGNAL(clicked()),this,SLOT(set_window2()));
    QWidget::connect(m_phoneLogin_btn,SIGNAL(clicked()),this,SLOT(set_window1()));
    QWidget::connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
   //Subabstract Build
    login_account_thr_number();
    login_account_thr_phone();
    QRegExp regx("[^\\s]+$");
    QValidator *validator = new QRegExpValidator(regx, m_accountLineEdit );
    m_accountLineEdit->setValidator(validator);
    //Initial configuration
    m_stackedWidget->setCurrentIndex(0);
    //qDebug()<<stack_widget->currentIndex();
    adjustSize();
}

/* 用户密码登录方式的界面设置 *
 * Setting the page of login that use password way. */ 
void LoginDialog::startaction_1() {
    if (m_stackedWidget->currentIndex() == 0) {
        set_clear();
        m_accountLineEdit->setFocus();
        //m_accountLineEdit->setText("");
        m_accountLineEdit->setMaxLength(11);
        QRegExp regx("^1[3-9]\\d{9}$");
        QValidator *validator = new QRegExpValidator(regx, m_accountLineEdit );
        m_accountLineEdit->setValidator(validator);
        m_accountLogin_btn->setMaximumSize(126,36);
        m_accountLogin_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        m_phoneLogin_btn->setMaximumSize(126,36);
        m_phoneLogin_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        m_accountLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;} "
                                         "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        m_phoneLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;color:#3D6BE5;} "
                                         "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        m_accountLogin_btn->setFocusPolicy(Qt::NoFocus);
        m_phoneLogin_btn->setFocusPolicy(Qt::NoFocus);

        m_stackedWidget->setCurrentIndex(1);
        m_accountLineEdit->setPlaceholderText(tr("Your phone number here"));
    }
}

/* 同上 */
void LoginDialog::set_window1() {
    startaction_1();
}

void LoginDialog::set_window2() {
    startaction_2();
}

QString LoginDialog::get_user_name() const {
    return this->m_accountLineEdit->text();
}

QString LoginDialog::get_user_pass() const {
    return this->m_passwordLineEdit->text();
}

void LoginDialog::startaction_2() {
    if (m_stackedWidget->currentIndex() == 1) {
        set_clear();
        m_accountLineEdit->setFocus();
        QRegExp regx("[^\\s]+$");
        m_accountLineEdit->setMaxLength(30);
        QValidator *validator = new QRegExpValidator(regx, m_accountLineEdit );
        m_accountLineEdit->setValidator(validator);
        m_accountLogin_btn->setMaximumSize(126,36);
        m_accountLogin_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        m_phoneLogin_btn->setMaximumSize(126,36);
        m_phoneLogin_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        m_accountLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);} "
                                         "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        m_phoneLogin_btn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;} "
                                         "QPushButton:hover{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        m_accountLogin_btn->setFocusPolicy(Qt::NoFocus);
        m_phoneLogin_btn->setFocusPolicy(Qt::NoFocus);
        m_stackedWidget->setCurrentIndex(0);
        m_accountLineEdit->setPlaceholderText(tr("Your account/phone/email here"));
    }
}


bool LoginDialog::login_account_thr_number() {
    //Fill the container and allocation

    //Congfigurate the widgets
    m_accountLineEdit->setFocus();
    QRegExp regx("[^\\s]+$");
    QValidator *validator = new QRegExpValidator(regx, m_accountLineEdit );
    m_accountLineEdit->setValidator(validator);

    m_passwordLineEdit->setPlaceholderText(tr("Your password here"));
    m_passwordLineEdit->setMaximumSize(338,36);
    m_passwordLineEdit->setMinimumSize(338,36);
    //m_passwordLineEdit->setTextMargins(12,0,0,0);
    m_passwordLineEdit->setMaxLength(30);
    //account_pass->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
    //                            "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
     //                           "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //account_phone->setGeometry(31 + sizeoff,148 + sizeoff,338,36);
    //account_pass->setGeometry(31 + sizeoff,192 + sizeoff,338,36);

    m_forgetPasswordBtn->setMaximumSize(80,30);
    m_forgetPasswordBtn->setMinimumSize(80,30);
    //forgot_pass_btn->setGeometry(289 + sizeoff,228 + sizeoff,80,30);
    m_forgetPasswordBtn->setStyleSheet("QPushButton {background: transparent;border-radius: 4px;}"
                                   "QPushButton:hover{color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                                   "QPushButton:click{color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}");
    m_forgetPasswordBtn->setFocusPolicy(Qt::NoFocus);
    m_mcodeNormalLineEdit->setFixedSize(202,36);

    //login_submit->setGeometry(31 + sizeoff,350 + sizeoff,338,36);
    //register_account->setGeometry(134 + sizeoff,406 + sizeoff,120,36);

    //Layout

    m_passwordLineEdit->setContentsMargins(0,0,0,0);
    m_normalLayout->setMargin(0);
    m_normalLayout->setSpacing(0);
    m_normalLayout->addWidget(m_passwordLineEdit);
    m_normalLayout->addSpacing(1);
    m_mcodeNormalLayout->addWidget(m_mcodeNormalLineEdit,0,Qt::AlignLeft);
    m_mcodeNormalLayout->addWidget(m_mcodeWidget,0,Qt::AlignRight);
    m_mcodeNormalLayout->setSpacing(0);
    m_mcodeNormalLayout->setContentsMargins(0,8,0,8);

    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, m_mcodeNormalLineEdit );
    m_mcodeNormalLineEdit->setValidator( validator_code );

    m_subHBoxLayout->addWidget(m_passTips,0,Qt::AlignLeft);
    m_subHBoxLayout->setContentsMargins(0,8,0,8);
    m_subHBoxLayout->setSpacing(0);
    m_subHBoxLayout->addWidget(m_forgetPasswordBtn,0,Qt::AlignRight);
    m_forgetPasswordBtn->setContentsMargins(0,0,0,0);
    m_normalLayout->addLayout(m_mcodeNormalLayout);
    m_normalLayout->addSpacing(8);
    m_normalLayout->addLayout(m_subHBoxLayout);
    m_normalWidget->setLayout(m_normalLayout);

    m_passTips->hide();
    m_passTips->setAttribute(Qt::WA_DontShowOnScreen);

    //Update Widgets
    return false;
}

QLineEdit* LoginDialog::get_user_edit() {
    return this->m_accountLineEdit;
}

QPushButton* LoginDialog::get_forget_btn() {
    return this->m_forgetPasswordBtn;
}

PasswordLineEdit* LoginDialog::get_login_pass() {
    return m_passwordLineEdit;
}

Tips* LoginDialog::get_tips_pass() {
    return m_passTips;
}

QLineEdit* LoginDialog::get_login_code() {
    return m_mcodePhoneLineEdit;
}

Tips* LoginDialog::get_tips_code() {
    return m_phoneTips;
}

QPushButton* LoginDialog::get_user_mcode() {
    return m_sendMsgBtn;
}

void LoginDialog::set_code(const QString &codenum) {
    m_szCode = codenum;
    emit code_changed();
}

void LoginDialog::setstyleline() {
    if (m_stackedWidget->currentIndex() == 0) {
        m_passTips->set_text(m_szCode);
    } else {
        m_phoneTips->set_text(m_szCode);
    }
}

QStackedWidget* LoginDialog::get_stack_widget() {
    return m_stackedWidget;
}

MCodeWidget* LoginDialog::get_mcode_widget() {
    return m_mcodeWidget;
}

QLineEdit* LoginDialog::get_mcode_lineedit() {
    return m_mcodeNormalLineEdit;
}

bool LoginDialog::login_account_thr_phone() {
    //Fill the container and allocation
    //qDebug() <<"Hello";

    //Congfigurate the widgets
    m_accountLineEdit->setFocus();
    QRegExp regx("^1[3-9]\\d{9}$");
    QValidator *validator = new QRegExpValidator(regx, m_accountLineEdit );
    m_accountLineEdit->setValidator(validator);
    m_mcodePhoneLineEdit->setMaximumSize(192,36);
    m_mcodePhoneLineEdit->setMaxLength(4);
    m_mcodePhoneLineEdit->setPlaceholderText(tr("Your code here"));
    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, m_mcodePhoneLineEdit );
    m_mcodePhoneLineEdit->setValidator( validator_code );
    m_mcodePhoneLineEdit->setMinimumSize(192,36);
    m_sendMsgBtn->setMinimumSize(130,36);
    m_sendMsgBtn->setMaximumSize(136,36);
    m_sendMsgBtn->resize(130,36);
    m_mcodePhoneLineEdit->setTextMargins(12,0,0,0);
    //valid_code->setGeometry(31 + sizeoff,192 + sizeoff,192,36);
    //send_msg_submit->setGeometry(239 + sizeoff,192 + sizeoff,130,36);
    //valid_code->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #F4F4F4;font-size: 14px;color: rgba(0,0,0,0.85);}"
    //                          "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
    //                          "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //send_msg_submit->setStyleSheet("QPushButton{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
     //                              "QPushButton:hover{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
      //                             "QPushButton:click{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");
    m_sendMsgBtn->setFocusPolicy(Qt::NoFocus);
    QWidget::setTabOrder(m_accountLineEdit, m_mcodePhoneLineEdit);

    //Layout
    m_mcodePhoneLayout->addWidget(m_mcodePhoneLineEdit);
    m_mcodePhoneLayout->setSpacing(0);
    m_mcodePhoneLayout->setMargin(0);
    m_mcodePhoneLayout->addSpacing(16);
    m_mcodePhoneLayout->addWidget(m_sendMsgBtn);
    m_mcodePhoneLayout->setAlignment(Qt::AlignLeft| Qt::AlignTop);
    m_sendMsgBtn->setContentsMargins(0,0,0,0);
    m_mcodePhoneLineEdit->setContentsMargins(0,0,0,0);
    m_phoneLayout->addLayout(m_mcodePhoneLayout);
    m_phoneLayout->addWidget(m_phoneTips);
    m_phoneLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_phoneLayout->setMargin(0);
    m_phoneLayout->setSpacing(0);
    m_phoneWidget->setLayout(m_phoneLayout);

    m_phoneTips->hide();
    m_phoneTips->setAttribute(Qt::WA_DontShowOnScreen);

    //Update Widgets
    return false;
}

void LoginDialog::set_staus(const bool &ok) {
    m_accountLogin_btn->setEnabled(ok);
    m_phoneLogin_btn->setEnabled(ok);
    m_passwordLineEdit->setEnabled(ok);
    m_accountLineEdit->setEnabled(ok);
    m_mcodePhoneLineEdit->setEnabled(ok);
    m_mcodeNormalLineEdit->setEnabled(ok);
    m_sendMsgBtn->setEnabled(ok);
    m_forgetPasswordBtn->setEnabled(ok);

}

QLineEdit *& LoginDialog::phoneLineEdit() {
    return m_phoneLineEdit;
}

QLineEdit *& LoginDialog::mCodeLineEdit() {
    return m_mcodePhoneLineEdit;
}

/* 清空登录框 */
void LoginDialog::set_clear() {
    if (!m_phoneTips->isHidden()) {
        m_phoneTips->hide();
    }
    if (!m_passTips->isHidden()) {
        m_passTips->hide();
    }
    m_passwordLineEdit->get_visble()->setChecked(false);
    m_passwordLineEdit->setText("");
    m_mcodePhoneLineEdit->setText("");
    m_mcodeNormalLineEdit->setText("");
}

