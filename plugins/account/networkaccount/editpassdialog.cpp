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
#include "editpassdialog.h"
#include <QDesktopWidget>
#include <QApplication>

EditPassDialog::EditPassDialog(QWidget *parent) : QWidget(parent)
{
    //Allocate the memory
    stackwidget = new QStackedWidget(this);
    title = new QLabel(this);
    del_btn = new QPushButton(this);
    account = new ql_lineedit_pass(this);
    newpass = new ql_lineedit_pass(this);
    confirm_pass = new ql_lineedit_pass(this);
    pass_tips = new QLabel(this);
    valid_code = new QLineEdit(this);
    get_code = new QPushButton(this);
    cancel = new QPushButton(this);
    confirm = new QPushButton(this);
    success = new SuccessDiaolog(this);
    content = new QWidget(this);

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    btnlayout = new QHBoxLayout;
    timer = new QTimer(this);
    tips = new QLabel(this);
    vboxlayout = new QVBoxLayout;



    //Configuration
    stackwidget->addWidget(content);
    stackwidget->addWidget(success);
    timer->stop();
    title->setText(tr("Edit Password"));
    account->setPlaceholderText(tr("Your password here"));
    newpass->setPlaceholderText(tr("Your new password here"));
    valid_code->setPlaceholderText(tr("Your code here"));
    get_code->setText(tr("Get phone code"));
    cancel->setText(tr("Cancel"));
    confirm->setText(tr("Confirm"));
    confirm_pass->setPlaceholderText(tr("Confirm your new password"));
    pass_tips->setText(tr("At least 6 bit, include letters and digt"));
    pass_tips->setStyleSheet("font-size:14px;");
    pass_tips->setFixedHeight(16);
    valid_code->setMaxLength(4);
    stackwidget->setCurrentWidget(content);

    //Resize
    del_btn->setMaximumSize(30,30);
    del_btn->setMinimumSize(30,30);
    title->setMinimumHeight(26);
    del_btn->setFixedSize(30,30);
    account->setFixedSize(338,36);
    newpass->setFixedSize(338,36);
    confirm_pass->setFixedSize(338,36);
    valid_code->setFixedSize(120,34);
    get_code->setFixedSize(198,34);
    cancel->setFixedSize(120,36);
    confirm->setFixedSize(120,36);
    setFixedSize(420,446);
    content->setFixedSize(420,446);
    success->setFixedSize(420,446);

    del_btn->setGeometry(this->width() - 46,10,30,30);

    //Set StyleSheet
    QString liness = "QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                     "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                     "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}";
    QString labelss = "font-size: 24px;";
    QString confirmbtnss = "QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                           "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                           "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}";
    QString btnss = "QPushButton {font-size:14px;background: #F4F4F4;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                    "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                    "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";
    QString btns = "QPushButton {font-size:14px;background: #E7E7E7;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                    "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                    "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";
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
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    title->setStyleSheet(labelss);
    //account->setStyleSheet(liness);
    //newpass->setStyleSheet(liness);
    //valid_code->setStyleSheet(liness);
    //confirm_pass->setStyleSheet(liness);
    //get_code->setStyleSheet(btnss);
    //cancel->setStyleSheet(btns);
    confirm->setStyleSheet(confirmbtnss);
    del_btn->setFocusPolicy(Qt::NoFocus);

    valid_code->setMaxLength(4);
    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, valid_code );
    valid_code->setValidator( validator_code );

    //Layout
    account->setTextMargins(16,0,0,0);
    newpass->setTextMargins(16,0,0,0);
    valid_code->setTextMargins(16,0,0,0);
    confirm_pass->setTextMargins(16,0,0,0);
    vlayout->addWidget(title,0,Qt::AlignLeft);
    vlayout->setContentsMargins(41,55,41,36);
    vlayout->setSpacing(0);
    vlayout->addSpacing(20);
    vlayout->addWidget(newpass);
    vlayout->addSpacing(8);
    vlayout->addWidget(pass_tips);
    vlayout->addSpacing(8);
    vlayout->addWidget(confirm_pass);
    vlayout->addSpacing(8);
    hlayout->addWidget(valid_code);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(get_code);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
    vlayout->addSpacing(8);
    vlayout->addWidget(tips);
    vlayout->addStretch();
    vlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    btnlayout->addStretch();
    btnlayout->setMargin(0);
    btnlayout->setSpacing(16);
    btnlayout->addWidget(cancel);
    btnlayout->addWidget(confirm);
    btnlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(btnlayout,0);
    content->setLayout(vlayout);

    vboxlayout->setMargin(0);
    vboxlayout->setAlignment(Qt::AlignCenter);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(stackwidget);
    setLayout(vboxlayout);

    pass_tips->setContentsMargins(16,0,0,0);
    tips->hide();
    tips->setAttribute(Qt::WA_DontShowOnScreen);
    pass_tips->hide();
    pass_tips->setAttribute(Qt::WA_DontShowOnScreen);

    account->installEventFilter(this);
    newpass->installEventFilter(this);
    valid_code->installEventFilter(this);

    //Connect
    connect(del_btn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(get_code,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(confirm,SIGNAL(clicked()),this,SLOT(on_edit_submit()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_start()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
    connect(success->back_login,SIGNAL(clicked()),this,SLOT(on_close()));

   // setStyleSheet("EditPassDialog{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    del_btn->raise();
    cancel->setFocusPolicy(Qt::NoFocus);
    get_code->setFocusPolicy(Qt::NoFocus);
    confirm->setFocusPolicy(Qt::NoFocus);
    confirm->setShortcut(QKeySequence::InsertParagraphSeparator);
    confirm->setShortcut(Qt::Key_Enter);
    confirm->setShortcut(Qt::Key_Return);
    account->setFocus();
    QDesktopWidget* desktop = QApplication::desktop(); // =qApp->desktop();也可以
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

void EditPassDialog::set_client(libkylinssoclient *c) {
    client = c;
    connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_edit_code_finished(int)));
    connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_edit_submit_finished(int)));
}

void EditPassDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

void EditPassDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

void EditPassDialog::on_send_code() {
    int ret = -1;
    char phone[32];
    if(!account->text().isNull()) {
        qstrcpy(phone,account->text().toStdString().c_str());
        ret = client->get_mcode_by_username(phone);
        if(ret != 0) {
            set_clear();
            set_code(messagebox(ret));
            tips->show();
            setshow(content);
        }
    }
}

void EditPassDialog::on_edit_submit() {
    int ret = -1;
    char cur_acc[32],new_pass[32],mcode[5],confirm_password[32];
    bool ok_cur = account->text().isNull();
    bool ok_new = newpass->text().isNull();
    bool ok_code = valid_code->text().isNull();
    bool ok_confirm = confirm_pass->text().isNull();
    if(!ok_cur && !ok_new && !ok_code && !ok_confirm) {
        qstrcpy(cur_acc,account->text().toStdString().c_str());
        qstrcpy(new_pass,newpass->text().toStdString().c_str());
        qstrcpy(mcode,valid_code->text().toStdString().c_str());
        qstrcpy(confirm_password,confirm_pass->text().toStdString().c_str());
        ret = client->user_resetpwd(cur_acc,new_pass,mcode);
        if(qstrcmp(confirm_password,new_pass) != 0) {
            set_code(tr("Please check your password!"));
            tips->show();
            setshow(content);
            return ;
        }
        if(ret == 0) {
            qDebug()<<cur_acc;
        } else {
            set_code(messagebox(ret));
            set_clear();
            tips->show();
            setshow(content);
        }
    }
}

void EditPassDialog::on_timer_start() {
    if(timerout_num > 0) {
        QString str = tr("Resend(") + QString::number(timerout_num,10) + tr(")");
        get_code->setText(tr(str.toStdString().c_str()));
        timerout_num --;
    }else if(timerout_num == 0) {
        timerout_num = 60;
        get_code->setEnabled(true);
        get_code->setText(tr("Send"));
        timer->stop();
    }
}

void EditPassDialog::on_edit_submit_finished(int req) {
    if(req == 0) {
        client->logout();
        emit account_changed();
        del_btn->hide();
        stackwidget->setCurrentWidget(success);
        success->back_login->setText(tr("Success！"));
    } else {
        set_code(messagebox(req));
        tips->show();
        setshow(stackwidget);
    }
}

QString EditPassDialog::messagebox(int code) {
    QString ret = tr("Error code:") + QString::number(code,10)+ tr("!");
    switch(code) {
    case 101:ret = tr("Internal error occurring!");break;
    case 102:ret = tr("Failed to sign up!");break;
    case 103:ret = tr("Failed attempt to return value!");break;
    case 104:ret = tr("Check your connection!");break;
    case 105:ret = tr("Failed to get by phone!");break;
    case 106:ret = tr("Failed to get by user!");break;
    case 107:ret = tr("Failed to reset password!");break;
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
    case 619:ret = tr("Sending code error occurring!");break;
    case -1:ret = tr("Please check your information!");break;

    }
    return ret;
}

void EditPassDialog::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

void EditPassDialog::on_edit_code_finished(int req) {
    if(req != 0) {
        set_code(messagebox(req));
        tips->show();
        setshow(stackwidget);
    } else if(req == 0) {
        timer->start();
        timer->setInterval(1000);
        get_code->setEnabled(false);
    }
}


void EditPassDialog::paintEvent(QPaintEvent *event)
{

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

void EditPassDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

void EditPassDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}

bool EditPassDialog::eventFilter(QObject *w, QEvent *e) {
    if(w == account) {
        if (e->type() == QEvent::FocusIn && !tips->isHidden()) {
            tips->hide();

            setshow(content);

        }
    }
    if(w == newpass) {
        if (e->type() == QEvent::FocusIn && !tips->isHidden()) {
            tips->hide();

            setshow(content);

        }
        if (e->type() == QEvent::FocusIn && pass_tips->isHidden()) {
            pass_tips->show();

            setshow(content);

        } else if (e->type() == QEvent::FocusOut && !pass_tips->isHidden()) {
            pass_tips->hide();

            setshow(content);
        }
    }
    if(w == valid_code) {
        if (e->type() == QEvent::FocusIn && !tips->isHidden()) {
            tips->hide();

            setshow(content);

        }
    }
    return QWidget::eventFilter(w,e);
}

void EditPassDialog::set_clear() {
    if(!tips->isHidden()) {
        tips->hide();
    }
    account->setText("");
    newpass->setText("");
    valid_code->setText("");
}

void EditPassDialog::on_close() {
    stackwidget->setCurrentIndex(0);
    set_clear();
    close();
}
