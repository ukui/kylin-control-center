#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QWidget(parent) {
    this->parent();


    //Basic Widget Allocation
    vlayout = new QVBoxLayout;
    vlayout_p = new QVBoxLayout;
    waylayout = new QVBoxLayout;
    stack_widget = new QStackedWidget(this);
    widget_number = new QWidget;
    widget_phone = new QWidget;

    HBox_forget = new QHBoxLayout;
    HBox_forgett = new QHBoxLayout;

    account_phone = new QLineEdit;

    account_pass = new QLineEdit(widget_number);
    forgot_pass_btn = new QPushButton(tr("忘记密码"));

    valid_code = new QLineEdit(widget_phone);
    send_msg_submit = new QPushButton(tr("发送验证码"),widget_phone);
    account_login_btn = new QPushButton(tr("账号密码登录"));
    message_login_btn = new QPushButton(tr("短信快捷登录"));

    error_code = new QLabel;
    error_pass = new QLabel;

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


    this->setMaximumSize(338,220);
    this->setMinimumSize(338,220);
    this->setContentsMargins(0,0,0,0);
    this->resize(338,220);

    stack_widget->addWidget(widget_number);
    stack_widget->addWidget(widget_phone);
    stack_widget->setContentsMargins(0,0,0,0);

    widget_number->setMaximumSize(338,113);
    widget_number->setMinimumSize(338,113);
    widget_number->setContentsMargins(0,0,0,0);

    widget_phone->setMaximumSize(338,36);
    widget_phone->setMinimumSize(338,36);
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
    message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(0,0,0,0.85);} "
                                     "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                     "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
    account_login_btn->setFocusPolicy(Qt::NoFocus);
    message_login_btn->setFocusPolicy(Qt::NoFocus);

    account_phone->setMaxLength(11);
    account_phone->setMaximumSize(338,36);
    account_phone->setMinimumSize(338,36);
    account_phone->setTextMargins(16,0,0,0);
    account_phone->setFocusPolicy(Qt::StrongFocus);
    account_phone->setPlaceholderText(tr("用户名"));

    account_phone->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
                                 "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                                 "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");

    mcode_lineedit->setTextMargins(16,0,0,0);
    mcode_lineedit->setPlaceholderText("输入验证码");
    mcode_lineedit->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);}"
                                  "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                                  "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
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
    waylayout->addSpacing(1);
    //waylayout->addSpacing(24);
    waylayout->addLayout(HBox_way);
    waylayout->addSpacing(13);
    waylayout->addWidget(account_phone);
    waylayout->addSpacing(4);
    waylayout->addWidget(stack_widget);
    setLayout(waylayout);

    //Connect
    QWidget::connect(account_login_btn,SIGNAL(clicked()),this,SLOT(set_window2()));
    QWidget::connect(message_login_btn,SIGNAL(clicked()),this,SLOT(set_window1()));
    QWidget::connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
   //Subabstract Build
    login_account_thr_number();
    login_account_thr_phone();

    //Initial configuration
    stack_widget->setCurrentIndex(0);
    //qDebug()<<stack_widget->currentIndex();
}

void LoginDialog::startaction_1() {
    if(stack_widget->currentIndex() == 0) {
        account_login_btn->setMaximumSize(90,36);
        account_login_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        message_login_btn->setMaximumSize(90,36);
        message_login_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        account_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(0,0,0,0.85);} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:#3D6BE5;} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        account_login_btn->setFocusPolicy(Qt::NoFocus);
        message_login_btn->setFocusPolicy(Qt::NoFocus);

        stack_widget->setCurrentIndex(1);
        account_phone->setPlaceholderText(tr("手机号码"));
    }
    qDebug()<<"2'13212312";
}

void LoginDialog::set_window1() {
    emit startaction_1();
}

void LoginDialog::set_window2() {
    emit startaction_2();
}

QString LoginDialog::get_user_name() {
    return this->account_phone->text();
}

QString LoginDialog::get_user_pass() {
    return this->account_pass->text();
}

void LoginDialog::startaction_2() {
    if(stack_widget->currentIndex() == 1) {
        account_login_btn->setMaximumSize(90,36);
        account_login_btn->setMinimumSize(90,36);
        //account_login_btn->setGeometry(31 + sizeoff,96 + sizeoff,90,36);
        message_login_btn->setMaximumSize(90,36);
        message_login_btn->setMinimumSize(90,36);
        //message_login_btn->setGeometry(161 + sizeoff,96 + sizeoff,90,36);
        account_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        message_login_btn->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;color:rgba(0,0,0,0.85);} "
                                         "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                         "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");
        account_login_btn->setFocusPolicy(Qt::NoFocus);
        message_login_btn->setFocusPolicy(Qt::NoFocus);
        stack_widget->setCurrentIndex(0);
        account_phone->setPlaceholderText(tr("用户名"));
    }
}

bool LoginDialog::login_account_thr_number() {
    //Fill the container and allocation

    //Congfigurate the widgets
    account_phone->setFocus();

    account_pass->setPlaceholderText(tr("输入密码"));
    account_pass->setMaximumSize(338,36);
    account_pass->setMinimumSize(338,36);
    account_pass->setTextMargins(16,0,0,0);
    account_pass->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                                "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                                "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //account_phone->setGeometry(31 + sizeoff,148 + sizeoff,338,36);
    //account_pass->setGeometry(31 + sizeoff,192 + sizeoff,338,36);

    account_pass->setEchoMode(QLineEdit::Password);

    forgot_pass_btn->setMaximumSize(80,30);
    forgot_pass_btn->setMinimumSize(80,30);
    //forgot_pass_btn->setGeometry(289 + sizeoff,228 + sizeoff,80,30);
    forgot_pass_btn->setStyleSheet("QPushButton {font-size:14px;background: transparent;color:rgba(0,0,0,0.65);border-radius: 4px;}"
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

QLineEdit* LoginDialog::get_login_pass() {
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
        qDebug()<<"<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
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

    valid_code->setMaximumSize(192,36);
    valid_code->setPlaceholderText(tr("输入验证码"));
    send_msg_submit->setMaximumSize(130,36);
    valid_code->setMinimumSize(192,36);
    send_msg_submit->setMinimumSize(130,36);
    send_msg_submit->setMaximumSize(130,36);
    valid_code->setTextMargins(16,0,0,0);
    //valid_code->setGeometry(31 + sizeoff,192 + sizeoff,192,36);
    //send_msg_submit->setGeometry(239 + sizeoff,192 + sizeoff,130,36);
    valid_code->setStyleSheet("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #F4F4F4;font-size: 14px;color: rgba(0,0,0,0.85);}"
                              "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                              "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    send_msg_submit->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
                                   "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
                                   "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");
    send_msg_submit->setFocusPolicy(Qt::NoFocus);
    QWidget::setTabOrder(account_phone, valid_code);

    //Layout
    HBox_forget->addWidget(error_code,0,Qt::AlignRight);
    HBox_forgett->addWidget(valid_code,0,Qt::AlignLeft);
    HBox_forgett->setSpacing(0);
    HBox_forgett->setMargin(0);
    HBox_forgett->addSpacing(16);
    HBox_forgett->addWidget(send_msg_submit,0,Qt::AlignLeft);
    send_msg_submit->setContentsMargins(0,0,0,0);
    valid_code->setContentsMargins(0,0,0,0);
    vlayout_p->addLayout(HBox_forgett);
    vlayout_p->setMargin(0);
    vlayout_p->setSpacing(0);
    widget_phone->setLayout(vlayout_p);

    error_code->hide();
    error_code->setAttribute(Qt::WA_DontShowOnScreen);

    //Update Widgets
    return false;
}
