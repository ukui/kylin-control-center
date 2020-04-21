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
    login_submit = new QPushButton(tr("Sign in"),this);
    register_account = new QPushButton(tr("Sign up"),this);
    box_login = new LoginDialog(this);
    box_reg = new RegDialog(this);
    box_bind = new BindPhoneDialog(this);
    box_pass = new PassDialog(this);
    log_reg = new QWidget(this);
    basewidegt = new QStackedWidget(this);

    title = new QLabel(status,this);
    stack_box = new QStackedWidget(this);
    vboxlayout = new QVBoxLayout;
    hboxlayout = new QHBoxLayout;
    del_btn = new QPushButton(this);
    timer = new QTimer(this);
    timer_reg  = new QTimer(this);
    timer_log = new QTimer(this);
    succ = new SuccessDiaolog(this);
    gif = new QLabel(login_submit);
    pm = new QMovie(":/new/image/login.gif");

    timer->stop();
    timer_reg->stop();
    timer_log->stop();
    gif->hide();

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
    del_btn->setGeometry(this->width() - 46,10,30,30);
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

    //setStyleSheet("Dialog_login_reg{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlag(Qt::FramelessWindowHint);

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
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(basewidegt);
    setLayout(hbox);

    login_submit->setShortcut(QKeySequence::InsertParagraphSeparator);
    login_submit->setShortcut(Qt::Key_Enter);
    login_submit->setShortcut(Qt::Key_Return);


    send_btn_fgt = box_pass->get_send_msg_btn();
    send_btn_reg = box_reg->get_send_code();
    send_btn_log = box_login->get_user_mcode();

    box_login->get_user_edit()->setFocus();



    login_pass = box_login->get_login_pass();   //Login Password LineEdit
    login_code = box_login->get_login_code();   //Login Phone Code LineEdit
    login_user = box_login->get_user_edit();    //Login User LineEdit
    login_mcode = box_login->get_mcode_lineedit();

    reg_user = box_reg->get_reg_user();         //Reg User LineEdit
    phone_user = box_reg->get_phone_user();     //Reg Phone LineEdit
    valid_code = box_reg->get_valid_code();     //Reg Phone Code LineEdit
    reg_pass = box_reg->get_reg_pass();         //Reg Password LineEdit
    reg_confirm = box_reg->get_reg_confirm();

    pass_user = box_pass->get_reg_phone();
    pass_pwd = box_pass->get_reg_pass();
    passtips = box_pass->get_passtips();
    pass_confirm = box_pass->get_reg_pass_confirm();
    pass_code = box_pass->get_valid_code();


    //Tips
    codelable = box_login->get_tips_code();
    passlabel = box_login->get_tips_pass();
    pass_tips = box_pass->get_tips();
    reg_tips = box_reg->get_tips();
    //LineEdit end.

    user_tip = box_reg->get_user_tip();
    pass_tip = box_reg->get_pass_tip();

    connect(del_btn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(box_login->get_forget_btn(),SIGNAL(clicked()),this,SLOT(linked_forget_btn()));
    connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
    connect(send_btn_fgt,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    connect(timer_reg,SIGNAL(timeout()),this,SLOT(on_timer_reg_out()));
    connect(timer_log,SIGNAL(timeout()),this,SLOT(on_timer_log_out()));
    connect(send_btn_reg,SIGNAL(clicked()),this,SLOT(on_send_code_reg()));
    connect(send_btn_log,SIGNAL(clicked()),this,SLOT(on_send_code_log()));
    connect(succ->back_login,SIGNAL(clicked()),this,SLOT(back_normal()));
    connect(pass_pwd,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(reg_pass,SIGNAL(textChanged(QString)),this,SLOT(cleanconfirm(QString)));
    connect(box_bind->get_send_code(),SIGNAL(clicked()),this,SLOT(on_send_code_bind()));
    connect(timer_bind,SIGNAL(timeout()),this,SLOT(on_timer_bind_out()));

    login_submit->installEventFilter(this);

    stack_box->adjustSize();

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
    box_bind->get_pass_lineedit()->installEventFilter(this);
    box_bind->get_phone_lineedit()->installEventFilter(this);
    box_bind->get_account_lineedit()->installEventFilter(this);

    stack_box->installEventFilter(this);

    setWindowModality(Qt::ApplicationModal);
    QDesktopWidget* desktop = QApplication::desktop(); // =qApp->desktop();也可以
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
    timerout_num = 60;
    del_btn->raise();
}

void Dialog_login_reg::cleanconfirm(QString str) {
    qDebug()<<str;
    if(stack_box->currentWidget() == box_pass) {
        pass_confirm->setText("");
    } else if(stack_box->currentWidget() == box_reg) {
        reg_confirm->setText("");
    }
}

QPushButton * Dialog_login_reg::get_login_submit() {
    return login_submit;
}

void Dialog_login_reg::set_client(libkylinssoclient *c) {
    client = c;
    connect(client,SIGNAL(finished_login(int)),this,SLOT(on_login_finished(int)));
    connect(client,SIGNAL(finished_user_phone_login(int)),this,SLOT(on_login_finished(int)));
    connect(client,SIGNAL(finished_mcode_by_phone(int)),this,SLOT(on_get_mcode_by_phone(int)));
    connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_pass_finished(int)));
    connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_get_mcode_by_name(int)));
    connect(client,SIGNAL(finished_registered(int)),this,SLOT(on_reg_finished(int)));
    connect(client,SIGNAL(finished_bindPhone(int)),this,SLOT(on_bind_finished(int)));
}

void Dialog_login_reg::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

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


void Dialog_login_reg::on_login_btn() {
    int ret = -1;
    qDebug()<<QString(box_login->get_mcode_widget()->get_verificate_code()) <<box_login->get_mcode_lineedit()->text();
    if(box_login->get_stack_widget()->currentIndex() == 0 &&
        QString(box_login->get_mcode_widget()->get_verificate_code()) != box_login->get_mcode_lineedit()->text()) {
        box_login->set_code(tr("Your code is wrong!"));
        passlabel->show();
        
        del_btn->hide();
        box_login->get_mcode_widget()->set_change(1);
        box_login->get_mcode_widget()->repaint();
        setshow(stack_box);
        box_login->get_mcode_widget()->set_change(0);
        return ;
    }
    if(box_login->get_user_name() != "" &&
        box_login->get_user_pass() != "" &&
        box_login->get_stack_widget()->currentIndex() == 0){
        char name[32],pass[32];
        qstrcpy(name,box_login->get_user_name().toStdString().c_str());
        qstrcpy(pass,box_login->get_user_pass().toStdString().c_str());
        qDebug()<<"1111111";
        ret = client->login(name,pass);
        qDebug()<<ret;
        if(ret != 0) {
            box_login->set_clear();
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

            login_submit->setText("");
            gif->setMovie(pm);
            gif->show();
            pm->start();
        }
        qDebug()<<"2222222";

    } else if(box_login->get_user_name() != ""
               && box_login->get_login_code()->text() != ""
               && box_login->get_stack_widget()->currentIndex() == 1) {
        char phone[32],mcode[5];
        qstrcpy(phone,box_login->get_user_name().toStdString().c_str());
        qstrcpy(mcode,box_login->get_login_code()->text().toStdString().c_str());
        ret = client->user_phone_login(phone,mcode);
        if(ret != 0) {
            box_login->set_clear();
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
        qDebug()<<phone<<mcode;
    } else {
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

void Dialog_login_reg::on_login_finished(int ret) {
    qDebug()<< "wb1111" <<ret;
    pm->stop();
    gif->hide();
    if(ret == 119) {
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
    }
    if(ret == 0) {
        timerout_num_log = 0;
        timer_log->stop();
        login_submit->setText(tr("Sign in"));
        emit on_login_success(); //ka zhu le bu duan fa xin hao ; notice:keyi ding yi yige tag zhi fa yi ci
        on_close();
    } else {
        login_submit->setText(tr("Sign in"));
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();

            box_login->get_mcode_widget()->set_change(1);
            box_login->get_mcode_widget()->repaint();
            setshow(stack_box);
            return ;
            box_login->get_mcode_widget()->set_change(0);
        } else if(box_login->get_stack_widget()->currentIndex() == 1) {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
            return ;
        }
    }
}

void Dialog_login_reg::on_bind_finished(int ret) {
    if(ret == 0) {
        timerout_num_bind = 0;
        timer_bind->stop();
        login_submit->setText(tr("Sign in"));
        box_bind->setclear();
        register_account->setText(tr("Sign up"));
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_bind_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        close();
    } else {
        box_bind->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_reg_finished(int ret) {
    qDebug()<<ret;
    if(ret == 0) {
        timerout_num_reg = 0;
        timer_reg->stop();
        login_submit->setText(tr("Sign in"));
        box_reg->get_reg_pass()->clear();
        box_reg->get_reg_user()->clear();
        box_reg->get_phone_user()->clear();
        box_reg->get_valid_code()->clear();
        basewidegt->setCurrentWidget(succ);
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

void Dialog_login_reg::on_pass_finished(int ret) {
    if(ret == 0) {
        timerout_num = 0;
        timer->stop();
        login_submit->setText(tr("Sign in"));
        box_pass->get_reg_pass()->clear();
        box_pass->get_reg_phone()->clear();
        box_pass->get_reg_pass_confirm()->clear();
        box_pass->get_valid_code()->clear();
        basewidegt->setCurrentWidget(succ);
        del_btn->hide();
        succ->set_mode_text(1);
        register_account->setText(tr("Sign up"));
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    } else {
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_timer_timeout() {
    qDebug()<<timerout_num;
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


void Dialog_login_reg::on_get_mcode_by_phone(int ret) {
    qDebug()<<ret;
    if(ret != 0) {
        if(stack_box->currentWidget() == box_login) {
            box_login->set_clear();
            box_login->set_code(messagebox(ret));
            if(box_login->get_stack_widget()->currentIndex() == 0){
                passlabel->show();
            } else if(box_login->get_stack_widget()->currentIndex() == 1) {
                codelable->show();
            }
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_reg) {
            box_reg->set_clear();
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->set_clear();
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);
        }
        return ;
    } else if(ret == 0) {
        if(stack_box->currentWidget() == box_login) {
            timer_log->start(1000);
            send_btn_log->setEnabled(false);
        } else if(stack_box->currentWidget() == box_reg) {
            timer_reg->start(1000);
            send_btn_reg->setEnabled(false);
        } else if(stack_box->currentWidget() == box_pass) {
            timer->start(1000);
            send_btn_fgt->setEnabled(false);
        } else if(stack_box->currentWidget() == box_bind) {
            timer_bind->start(1000);
            box_bind->get_send_code()->setEnabled(false);
        }
    }
}

void Dialog_login_reg::on_get_mcode_by_name(int ret) {
    if(ret != 0) {
        if(stack_box->currentWidget() == box_login) {
            box_login->set_clear();
            box_login->set_code(messagebox(ret));
            if(box_login->get_stack_widget()->currentIndex() == 0){
                passlabel->show();
            } else if(box_login->get_stack_widget()->currentIndex() == 1) {
                codelable->show();
            }
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_reg) {
            box_reg->set_clear();
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);
        } else if(stack_box->currentWidget() == box_pass) {
            box_pass->set_clear();
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);
        }
        return ;
    }  else if(ret == 0) {
        if(stack_box->currentWidget() == box_login) {
            timer_log->start(1000);
            send_btn_log->setEnabled(false);
        } else if(stack_box->currentWidget() == box_reg) {
            timer_reg->start(1000);
            send_btn_reg->setEnabled(false);
        } else if(stack_box->currentWidget() == box_pass) {
            timer->start(1000);
            send_btn_fgt->setEnabled(false);
        } else if(stack_box->currentWidget() == box_bind) {
            timer_bind->start(1000);
            box_bind->get_send_code()->setEnabled(false);
        }
    }
}

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

void Dialog_login_reg::on_pass_btn() {
    int ret = -1;
    bool ok_phone = box_pass->get_user_name() == "";
    bool ok_pass = box_pass->get_user_newpass() == "";
    bool ok_confirm = box_pass->get_user_confirm() == "";
    bool ok_code = box_pass->get_user_mcode() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_confirm) {
        char phone[32],pass[32],confirm[32],code[5];
        qstrcpy(phone,box_pass->get_user_name().toStdString().c_str());
        qstrcpy(pass,box_pass->get_user_newpass().toStdString().c_str());
        qstrcpy(confirm,box_pass->get_user_confirm().toStdString().c_str());
        qstrcpy(code,box_pass->get_user_mcode().toStdString().c_str());
        if(qstrcmp(confirm,pass) != 0) {
            box_pass->set_code(tr("Please check your password!"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        ret = client->user_resetpwd(phone,pass,code);
        if(ret != 0) {
            box_pass->set_clear();
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);

            return ;
        }else {

        }
    }else {
        box_pass->set_clear();
        box_pass->set_code(messagebox(ret));
        pass_tips->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_send_code_reg() {
    char phone[32];
    int ret = -1;
    if(box_reg->get_user_phone() != "") {
        qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        ret = client->get_mcode_by_phone(phone);
        if(ret == 0) {
            //not do
        } else {
            box_reg->set_clear();
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);

            return ;
        }
    } else {
        box_reg->set_clear();
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);

        return ;
    }
}

void Dialog_login_reg::on_bind_btn() {
    int ret = -1;
    bool ok_phone = box_bind->get_phone() == "";
    bool ok_pass = box_bind->get_pass() == "";
    bool ok_account = box_bind->get_account() == "";
    bool ok_code = box_bind->get_code() == "";
    if(!ok_phone && !ok_pass && !ok_code && !ok_account) {
        char phone[32],pass[32],account[32],code[5];
        qstrcpy(phone,box_bind->get_phone().toStdString().c_str());
        qstrcpy(pass,box_bind->get_pass().toStdString().c_str());
        qstrcpy(account,box_bind->get_account().toStdString().c_str());
        qstrcpy(code,box_bind->get_code().toStdString().c_str());
        ret = client->bindPhone(account,pass,phone,code);
        if(ret != 0) {
            box_bind->setclear();
            box_bind->set_code(messagebox(ret));
            box_bind->get_tips()->show();
            setshow(stack_box);

            return ;
        } else {
        }
    }else {
        box_bind->setclear();
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_send_code_log() {
    char phone[32];
    int ret = -1;
    if(box_login->get_user_name() != "") {
        qstrcpy(phone,box_login->get_user_name().toStdString().c_str());
        ret = client->get_mcode_by_phone(phone);
        qDebug()<<"get_mcode_by_phone ret is "<<ret;
        if(ret == 0) {
            //not do
        } else {
            box_login->set_clear();
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);

            return ;
        }
    } else {
        box_login->set_clear();
        box_login->set_code(messagebox(ret));
        codelable->show();
        setshow(stack_box);

        return ;
    }
}

void Dialog_login_reg::on_send_code_bind() {
    char name[32];
    int ret = -1;
    if(box_bind->get_account() != "") {
        qstrcpy(name,box_bind->get_account().toStdString().c_str());
        ret = client->get_mcode_by_username(name);
        if(ret == 0) {
            //not do
        } else {
            box_bind->setclear();
            box_bind->set_code(messagebox(ret));
            box_bind->get_tips()->show();
            setshow(stack_box);
            return ;
        }
    }else {
        box_bind->setclear();
        box_bind->set_code(messagebox(ret));
        box_bind->get_tips()->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_send_code() {
    char name[32];
    int ret = -1;
    if(box_pass->get_user_name() != "") {
        qstrcpy(name,box_pass->get_user_name().toStdString().c_str());
        ret = client->get_mcode_by_username(name);
        if(ret == 0) {
            //not do
        } else {
            box_pass->set_clear();
            box_pass->set_code(messagebox(ret));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
    }else {
        box_pass->set_clear();
        pass_tips->show();
        box_pass->set_code(messagebox(ret));
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::on_reg_btn() {
    bool ok_mcode = box_reg->get_user_mcode() != "";
    bool ok_phone = box_reg->get_user_phone() != "";
    bool ok_account = box_reg->get_user_account() != "";
    bool ok_passwd = box_reg->get_user_passwd() != "";
    bool ok_confirm = box_reg->get_reg_confirm()->text() != "";
    int ret = -1;
    if(ok_mcode && ok_phone && ok_account && ok_passwd &&ok_confirm) {
        char account[32],passwd[32],phone[32],mcode[32],confirm[32];
        qstrcpy(account,box_reg->get_user_account().toStdString().c_str());
        qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        qstrcpy(passwd,box_reg->get_user_passwd().toStdString().c_str());
        qstrcpy(mcode,box_reg->get_user_mcode().toStdString().c_str());
        qstrcpy(confirm,box_reg->get_reg_confirm()->text().toStdString().c_str());
        ret = client->registered(account,passwd,phone,mcode);
        if(qstrcmp(confirm,passwd) != 0) {
            box_pass->set_code(tr("Please check your password!"));
            pass_tips->show();
            setshow(stack_box);
            return ;
        }
        if(ret != 0) {
            box_reg->set_clear();
            box_reg->set_code(messagebox(ret));
            reg_tips->show();
            setshow(stack_box);
            return ;
        } else {
        }
    } else {
        box_reg->set_clear();
        box_reg->set_code(messagebox(ret));
        reg_tips->show();
        setshow(stack_box);
        return ;
    }
}

void Dialog_login_reg::back_normal() {
    basewidegt->setCurrentWidget(log_reg);
    del_btn->show();
    del_btn->raise();
    succ->hide();
    setshow(basewidegt);

    stack_box->setCurrentWidget(box_login);
    box_login->set_clear();
}

void Dialog_login_reg::back_login_btn() {
    qDebug()<<stack_box->currentIndex();
    if(stack_box->currentIndex() != 0) {
        title->setText(tr("Sign in Cloud"));
        if(stack_box->currentIndex() == 1) {
            box_reg->get_reg_pass()->clear();
            box_reg->get_reg_user()->clear();
            box_reg->get_phone_user()->clear();
            box_reg->get_valid_code()->clear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(stack_box->currentIndex() == 2) {
            box_pass->get_reg_pass()->clear();
            box_pass->get_reg_phone()->clear();
            box_pass->get_reg_pass_confirm()->clear();
            box_pass->get_valid_code()->clear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(stack_box->currentIndex() == 3) {
            box_bind->setclear();
            del_btn->show();
            del_btn->raise();
            client->logout();
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

void Dialog_login_reg::linked_forget_btn() {
    if(stack_box->currentIndex() != 2) {
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

void Dialog_login_reg::linked_register_btn() {
    if(stack_box->currentIndex() != 1) {
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
    // rect: 绘制区域  15：圆角弧度
    painter.drawRoundedRect(rect, 6, 6);
}

void Dialog_login_reg::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

void Dialog_login_reg::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}

bool Dialog_login_reg::eventFilter(QObject *w, QEvent *e) {

    //Bind 4
    if(w == box_bind->get_code_lineedit()) {
        if (e->type() == QEvent::FocusIn && !box_bind->get_tips()->isHidden()) {
            box_bind->get_tips()->hide();
            setshow(stack_box);
        }
    }
    if(w == box_bind->get_account_lineedit()) {
        if (e->type() == QEvent::FocusIn && !box_bind->get_tips()->isHidden()) {
            box_bind->get_tips()->hide();
            setshow(stack_box);
        }
    }
    if(w == box_bind->get_pass_lineedit()) {
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

    //Reg 4
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

    //Login 4
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

    //Pass 4
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

void Dialog_login_reg::setclear() {
    basewidegt->setCurrentWidget(log_reg);
    if(stack_box->currentWidget() == box_login) {
        box_login->set_clear();
    }else if(stack_box->currentWidget() == box_reg) {
        box_reg->set_clear();
        emit register_account->clicked();
    }else if(stack_box->currentWidget() == box_pass) {
        box_pass->set_clear();
        emit register_account->clicked();
    }
    del_btn->raise();
    box_login->set_window2();
    setshow(basewidegt);
}

void Dialog_login_reg::on_close() {
    stack_box->setCurrentWidget(box_login);
    timer_log = timer_reg = timer_bind = timer = 0;
    send_btn_fgt->setEnabled(true);
    send_btn_log->setEnabled(true);
    send_btn_reg->setEnabled(true);
    box_bind->get_send_code()->setEnabled(true);
    setclear();
    close();
}

