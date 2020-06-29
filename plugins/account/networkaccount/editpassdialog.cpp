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
    uuid = QUuid::createUuid().toString();
    //内存分配
    stackwidget = new QStackedWidget(this);     //切换成功页面与业务逻辑页面
    title = new QLabel(this);                   //标题
    del_btn = new QPushButton(this);            //关闭按钮，定点摆放
    //account = new ql_lineedit_pass(this);
    newpass = new ql_lineedit_pass(this);       //新密码框
    confirm_pass = new ql_lineedit_pass(this);  //确认密码框
    pass_tips = new QLabel(this);               //密码提示
    valid_code = new QLineEdit(this);           //验证码框
    get_code = new QPushButton(this);           //发送验证码按钮
    cancel = new QPushButton(this);             //取消按钮
    confirm = new QPushButton(this);            //确认按钮
    success = new SuccessDiaolog(this);         //成功页面
    content = new QWidget(this);                //业务逻辑页面

    vlayout = new QVBoxLayout;                  //输入框布局
    hlayout = new QHBoxLayout;                  //验证码布局
    btnlayout = new QHBoxLayout;                //按钮布局
    timer = new QTimer(this);                   //验证码计时器
    tips = new ql_label_info(this);                    //错误提示
    vboxlayout = new QVBoxLayout;               //主窗口界面布局
    svg_hd = new ql_svg_handler(this);          //SVG控制器



    //控件初始化设置
    stackwidget->addWidget(content);
    stackwidget->addWidget(success);
    timer->stop();
    title->setText(tr("Edit Password"));
    //account->setPlaceholderText(tr("Your password here"));
    newpass->setPlaceholderText(tr("Your new password here"));
    valid_code->setPlaceholderText(tr("Your code"));
    get_code->setText(tr("Get phone code"));
    cancel->setText(tr("Cancel"));
    confirm->setText(tr("Confirm"));
    confirm_pass->setPlaceholderText(tr("Confirm your new password"));
    pass_tips->setText(tr("At least 6 bit, include letters and digt"));
    pass_tips->setStyleSheet("font-size:14px;");
    pass_tips->setFixedHeight(16);
    valid_code->setMaxLength(4);
    stackwidget->setCurrentWidget(content);

    //控件尺寸大小设置
    del_btn->setMaximumSize(30,30);
    del_btn->setMinimumSize(30,30);
    title->adjustSize();
    del_btn->setFixedSize(30,30);
    //account->setFixedSize(338,36);
    newpass->setFixedSize(338,36);
    confirm_pass->setFixedSize(338,36);
    valid_code->setFixedSize(120,34);
    get_code->setFixedSize(198,34);
    cancel->setFixedSize(120,36);
    confirm->setFixedSize(120,36);
    setFixedSize(420,446);
    content->setFixedSize(420,446);
    success->setFixedSize(420,446);

    del_btn->setGeometry(this->width() - 46,14,30,30);
    del_btn->setFlat(true);
    //设置样式表
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
    QPixmap pixmap = svg_hd->loadSvg(":/new/image/delete.svg");
    del_btn->setIcon(pixmap);
    del_btn->setStyleSheet("QPushButton{background:transparent;border-radius:4px;}"
                           "QPushButton:hover{background:transparent;background-color:#F86457;"
                           "border-radius:4px}"
                           "QPushButton:click{background:transparent;background-color:#E44C50;border-radius:4px}");
    del_btn->installEventFilter(this);
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
    success->set_mode_text(1);
    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, valid_code );
    valid_code->setValidator( validator_code );

    //布局设置
    //account->setTextMargins(16,0,0,0);
    newpass->setTextMargins(12,0,0,0);
    valid_code->setTextMargins(12,0,0,0);
    confirm_pass->setTextMargins(12,0,0,0);
    vlayout->addWidget(title,0,Qt::AlignLeft);
    vlayout->setContentsMargins(41,55,41,36);
    vlayout->addSpacing(20);
    vlayout->setSpacing(8);
    //vlayout->addWidget(account);
    vlayout->addWidget(newpass);
    vlayout->addWidget(pass_tips);
    vlayout->addWidget(confirm_pass);
    hlayout->addWidget(valid_code);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(get_code);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
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

    //account->installEventFilter(this);
    confirm_pass->installEventFilter(this);
    newpass->installEventFilter(this);
    valid_code->installEventFilter(this);

    //控件逻辑信号连接
    connect(del_btn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(get_code,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(confirm,SIGNAL(clicked()),this,SLOT(on_edit_submit()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_start()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
    connect(success->back_login,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(success->back_login,&QPushButton::clicked,[this] () {
        emit account_changed();
    });
    connect(newpass,&ql_lineedit_pass::verify_text,[this] () {
       pass_tips->setText(tr("Your password is valid!"));
    });
    connect(newpass,&ql_lineedit_pass::false_text,[this] () {
       pass_tips->setText(tr("At least 6 bit, include letters and digt"));
    });

   // setStyleSheet("EditPassDialog{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    del_btn->raise();
    cancel->setFocusPolicy(Qt::NoFocus);
    get_code->setFocusPolicy(Qt::NoFocus);
    confirm->setFocusPolicy(Qt::NoFocus);
    confirm->setShortcut(QKeySequence::InsertParagraphSeparator);
    confirm->setShortcut(Qt::Key_Enter);
    confirm->setShortcut(Qt::Key_Return);
    //account->setFocus();

    //设置第一个输入框为聚焦
    newpass->setFocus();

    //窗口显示在屏幕中央
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

/* 设置DBUS客户端 */
void EditPassDialog::set_client(DbusHandleClient *c,QThread *t) {
    client = c;
    thread = t;

    connect(this,SIGNAL(docode(QString,QString)),client,SLOT(get_mcode_by_username(QString,QString)));
    connect(client,SIGNAL(finished_ret_code_edit(int)),this,SLOT(setret_code(int)));
    connect(this,SIGNAL(doreset(QString,QString,QString,QString)),client,SLOT(user_resetpwd(QString,QString,QString,QString)));
    connect(client,SIGNAL(finished_ret_reset_edit(int)),this,SLOT(setret_edit(int)));
    connect(this,SIGNAL(docheck()),client,SLOT(check_login()));
    connect(client,SIGNAL(finished_ret_check_edit(QString)),this,SLOT(setret_check(QString)));

    //connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_edit_code_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_mcode_by_username",this,SLOT(on_edit_code_finished(int,QString)));
    //connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_edit_submit_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_user_resetpwd",this,SLOT(on_edit_submit_finished(int,QString)));
}

/* DBUS客户端回调函数处理，处理异常 */
void EditPassDialog::setret_code(int ret) {
    if(ret != 0) {
        valid_code->setText("");
        set_code(messagebox(ret));
        tips->show();
        setshow(content);
        return ;
    } else {
    }
}

void EditPassDialog::setret_check(QString ret) {
    if (ret == "401" || ret == "201" || ret == "203" || ret == "") {
        close();
        emit dologout();
    }
}

void EditPassDialog::setret_edit(int ret) {
    if(ret == 0) {
    } else {
        set_code(messagebox(ret));
        valid_code->setText("");
        tips->show();
        setshow(content);
    }
}

/* 设置错误提示代码 */
void EditPassDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

/* 设置提示错误消息 */
void EditPassDialog::setstyleline() {
    tips->set_text(code);
}

/* 验证码发送按钮处理 */
void EditPassDialog::on_send_code() {
    char phone[32];
    emit docheck();
    get_code->setEnabled(false);
    if(newpass->check() == false){
        get_code->setEnabled(true);
        valid_code->setText("");
        set_code(tr("At least 6 bit, include letters and digt"));
        tips->show();
        setshow(content);
        return ;
    }
    //qDebug()<<name;
    if(name != "" && name != "201" && name != "203" &&confirm_pass->text()!="") {
        qstrcpy(phone,name.toStdString().c_str());
        emit docode(phone,uuid);
    }else {
        get_code->setEnabled(true);
        valid_code->setText("");
        set_code(messagebox(-1));
        tips->show();
        return ;
    }
}

/*　修改密码按钮处理 */
void EditPassDialog::on_edit_submit() {
    QString new_pass,mcode,confirm_password,acco;
    //bool ok_cur = account->text().isNull();
    bool ok_new = newpass->text().isNull();
    bool ok_code = valid_code->text().isNull();
    bool ok_confirm = confirm_pass->text().isNull();
    bool ok_acc = name.isNull();
    content->setEnabled(false);  //防止用户乱点
    if( !ok_new && !ok_code && !ok_confirm && !ok_acc) {
        //qstrcpy(cur_acc,account->text().toStdString().c_str());
        //cur_acc = account->text();
       // qstrcpy(new_pass,newpass->text().toStdString().c_str());
        new_pass = newpass->text();
        //qstrcpy(mcode,valid_code->text().toStdString().c_str());
        mcode = valid_code->text();
        //qstrcpy(confirm_password,confirm_pass->text().toStdString().c_str());
        confirm_password = confirm_pass->text();
        acco = name;
        if(newpass->check() == false) {
            content->setEnabled(true);
            set_code(tr("At least 6 bit, include letters and digt"));
            tips->show();
            setshow(content);
            return ;
        }
        if(new_pass != confirm_password ) {
            content->setEnabled(true);
            set_code(tr("Please check your password!"));
            tips->show();
            setshow(content);
            return ;
        }
        emit doreset(acco,new_pass,mcode,uuid);
    }
}

/* 计时开始 */
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

/* 修改密码成功后进行回调处理 */
void EditPassDialog::on_edit_submit_finished(int req,QString uuid) {
    if(uuid != this->uuid) {
        return ;
    }
    if(is_used == false) {
        return ;
    }

    content->setEnabled(true);
    if(req == 0) {
        //qDebug()<<"wb888";
        del_btn->hide();
        stackwidget->setCurrentWidget(success);
        success->back_login->setText(tr("Reback sign in"));
    } else {
        set_code(messagebox(req));
        tips->show();
        setshow(stackwidget);
    }
}

/* 消息盒子 */
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

/* 动态显示布局处理 */
void EditPassDialog::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

/* 验证码获取回调 */
void EditPassDialog::on_edit_code_finished(int req,QString uuid) {
    if(this->uuid != uuid) {
        return ;
    }

    if(is_used == false) {
        return ;
    }
    get_code->setEnabled(true);
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

/* 窗口重绘，阴影设置 */
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
    // rect: 绘制区域  10 圆角弧度 6
    painter.drawRoundedRect(rect, 6, 6);
}

/* 设置窗口可以动 */
void EditPassDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

/* 设置窗口可以随鼠标动 */
void EditPassDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}


/*　控件聚焦失去焦点事件处理 */
bool EditPassDialog::eventFilter(QObject *w, QEvent *e) {
//    if(w == account) {
//        if (e->type() == QEvent::FocusIn && !tips->isHidden()) {
//            tips->hide();

//            setshow(content);

//        }
//    }
    if(w == del_btn) {
        if(e->type() == QEvent::Enter) {
            QPixmap pixmap = svg_hd->loadSvg(":/new/image/delete_click.svg");
            del_btn->setIcon(pixmap);
        }
        if(e->type() == QEvent::Leave) {
            QPixmap pixmap = svg_hd->loadSvg(":/new/image/delete.svg");
            del_btn->setIcon(pixmap);
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
    if(w == confirm_pass) {
        if (e->type() == QEvent::FocusIn && !tips->isHidden()) {
            tips->hide();

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

/* 清理窗口 */
void EditPassDialog::set_clear() {
    if(!tips->isHidden()) {
        tips->hide();
    }
   // account->setText("");
    newpass->setText("");
    confirm_pass->setText("");
    valid_code->setText("");
    timerout_num = 60;
    get_code->setEnabled(true);
    get_code->setText(tr("Send"));
    timer->stop();
}

/* 关闭窗口处理函数　*/
void EditPassDialog::on_close() {
    if(del_btn->isHidden()) {
        del_btn->show();
        del_btn->raise();
    }
    newpass->get_visble()->setChecked(false);
    //account->get_visble()->setChecked(false);
    confirm_pass->get_visble()->setChecked(false);
    stackwidget->setCurrentIndex(0);
    set_clear();
    close();
}
