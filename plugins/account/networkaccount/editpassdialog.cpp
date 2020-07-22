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
    m_szUuid = QUuid::createUuid().toString();
    //内存分配
    m_stackedWidget = new QStackedWidget(this);     //切换成功页面与业务逻辑页面
    m_title = new QLabel(this);                   //标题
    m_delBtn = new QPushButton(this);            //关闭按钮，定点摆放
    //account = new ql_lineedit_pass(this);
    m_newPassLineEdit = new PasswordLineEdit(this);       //新密码框
    m_passConfirm = new PasswordLineEdit(this);  //确认密码框
    m_passTips = new QLabel(this);               //密码提示
    m_mcodeLineEdit = new QLineEdit(this);           //验证码框
    m_mcodeBtn = new QPushButton(this);           //发送验证码按钮
    m_cancelBtn = new QPushButton(this);             //取消按钮
    m_confirmBtn = new QPushButton(this);            //确认按钮
    m_successDialog = new SuccessDiaolog(this);         //成功页面
    m_workWidget = new QWidget(this);                //业务逻辑页面

    m_wokrLayout = new QVBoxLayout;                  //输入框布局
    m_hboxLayout = new QHBoxLayout;                  //验证码布局
    m_btnLayout = new QHBoxLayout;                //按钮布局
    m_cMcodeTimer = new QTimer(this);                   //验证码计时器
    m_Tips = new Tips(this);                    //错误提示
    m_vboxLayout = new QVBoxLayout;               //主窗口界面布局
    m_svgHandler = new SVGHandler(this);          //SVG控制器



    //控件初始化设置
    m_stackedWidget->addWidget(m_workWidget);
    m_stackedWidget->addWidget(m_successDialog);
    m_cMcodeTimer->stop();
    m_title->setText(tr("Edit Password"));
    //account->setPlaceholderText(tr("Your password here"));
    m_newPassLineEdit->setPlaceholderText(tr("Your new password here"));
    m_mcodeLineEdit->setPlaceholderText(tr("Your code"));
    m_mcodeBtn->setText(tr("Get phone code"));
    m_cancelBtn->setText(tr("Cancel"));
    m_confirmBtn->setText(tr("Confirm"));
    m_passConfirm->setPlaceholderText(tr("Confirm your new password"));
    m_passTips->setText(tr("At least 6 bit, include letters and digt"));
    m_passTips->setStyleSheet("font-size:14px;");
    m_passTips->setFixedHeight(16);
    m_mcodeLineEdit->setMaxLength(4);
    m_stackedWidget->setCurrentWidget(m_workWidget);

    //控件尺寸大小设置
    m_delBtn->setMaximumSize(30,30);
    m_delBtn->setMinimumSize(30,30);
    m_title->adjustSize();
    m_delBtn->setFixedSize(30,30);
    //account->setFixedSize(338,36);
    m_newPassLineEdit->setFixedSize(338,36);
    m_passConfirm->setFixedSize(338,36);
    m_mcodeLineEdit->setFixedSize(120,34);
    m_mcodeBtn->setFixedSize(198,34);
    m_cancelBtn->setFixedSize(120,36);
    m_confirmBtn->setFixedSize(120,36);
    setFixedSize(420,446);
    m_workWidget->setFixedSize(420,446);
    m_successDialog->setFixedSize(420,446);

    m_delBtn->setGeometry(this->width() - 46,14,30,30);
    m_delBtn->setFlat(true);
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
    QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/delete.svg");
    m_delBtn->setIcon(pixmap);
    m_delBtn->setStyleSheet("QPushButton{background:transparent;border-radius:4px;}"
                           "QPushButton:hover{background:transparent;background-color:#F86457;"
                           "border-radius:4px}"
                           "QPushButton:click{background:transparent;background-color:#E44C50;border-radius:4px}");
    m_delBtn->installEventFilter(this);
    m_title->setStyleSheet(labelss);
    //account->setStyleSheet(liness);
    //newpass->setStyleSheet(liness);
    //valid_code->setStyleSheet(liness);
    //confirm_pass->setStyleSheet(liness);
    //get_code->setStyleSheet(btnss);
    //cancel->setStyleSheet(btns);
    m_confirmBtn->setStyleSheet(confirmbtnss);
    m_delBtn->setFocusPolicy(Qt::NoFocus);

    m_mcodeLineEdit->setMaxLength(4);
    m_successDialog->set_mode_text(1);
    QRegExp regx_code("[0-9]+$");
    QValidator *validator_code = new QRegExpValidator(regx_code, m_mcodeLineEdit );
    m_mcodeLineEdit->setValidator( validator_code );

    //布局设置
    //account->setTextMargins(16,0,0,0);
    m_newPassLineEdit->setTextMargins(12,0,0,0);
    m_mcodeLineEdit->setTextMargins(12,0,0,0);
    m_passConfirm->setTextMargins(12,0,0,0);
    m_wokrLayout->addWidget(m_title,0,Qt::AlignLeft);
    m_wokrLayout->setContentsMargins(41,55,41,36);
    m_wokrLayout->addSpacing(20);
    m_wokrLayout->setSpacing(8);
    //vlayout->addWidget(account);
    m_wokrLayout->addWidget(m_newPassLineEdit);
    m_wokrLayout->addWidget(m_passTips);
    m_wokrLayout->addWidget(m_passConfirm);
    m_hboxLayout->addWidget(m_mcodeLineEdit);
    m_hboxLayout->setMargin(0);
    m_hboxLayout->setSpacing(16);
    m_hboxLayout->addWidget(m_mcodeBtn);
    m_hboxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_wokrLayout->addLayout(m_hboxLayout);
    m_wokrLayout->addWidget(m_Tips);
    m_wokrLayout->addStretch();
    m_wokrLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_btnLayout->addStretch();
    m_btnLayout->setMargin(0);
    m_btnLayout->setSpacing(16);
    m_btnLayout->addWidget(m_cancelBtn);
    m_btnLayout->addWidget(m_confirmBtn);
    m_btnLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_wokrLayout->addLayout(m_btnLayout,0);
    m_workWidget->setLayout(m_wokrLayout);

    m_vboxLayout->setMargin(0);
    m_vboxLayout->setAlignment(Qt::AlignCenter);
    m_vboxLayout->setSpacing(0);
    m_vboxLayout->addWidget(m_stackedWidget);
    setLayout(m_vboxLayout);
    
    m_passTips->setContentsMargins(16,0,0,0);
    m_Tips->hide();
    m_Tips->setAttribute(Qt::WA_DontShowOnScreen);
    m_passTips->hide();
    m_passTips->setAttribute(Qt::WA_DontShowOnScreen);

    //account->installEventFilter(this);
    m_passConfirm->installEventFilter(this);
    m_newPassLineEdit->installEventFilter(this);
    m_mcodeLineEdit->installEventFilter(this);

    //控件逻辑信号连接
    connect(m_delBtn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(m_mcodeBtn,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(m_confirmBtn,SIGNAL(clicked()),this,SLOT(on_edit_submit()));
    connect(m_cMcodeTimer,SIGNAL(timeout()),this,SLOT(on_timer_start()));
    connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
    connect(m_successDialog->m_backloginBtn,SIGNAL(clicked()),this,SLOT(on_close()));
    connect(m_successDialog->m_backloginBtn,&QPushButton::clicked,[this] () {
        emit account_changed();
    });
    connect(m_newPassLineEdit,&PasswordLineEdit::verify_text,[this] () {
       m_passTips->setText(tr("Your password is valid!"));
    });
    connect(m_newPassLineEdit,&PasswordLineEdit::false_text,[this] () {
       m_passTips->setText(tr("At least 6 bit, include letters and digt"));
    });

   // setStyleSheet("EditPassDialog{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    m_delBtn->raise();
    m_cancelBtn->setFocusPolicy(Qt::NoFocus);
    m_mcodeBtn->setFocusPolicy(Qt::NoFocus);
    m_confirmBtn->setFocusPolicy(Qt::NoFocus);
    m_confirmBtn->setShortcut(QKeySequence::InsertParagraphSeparator);
    m_confirmBtn->setShortcut(Qt::Key_Enter);
    m_confirmBtn->setShortcut(Qt::Key_Return);
    //account->setFocus();

    //设置第一个输入框为聚焦
    m_newPassLineEdit->setFocus();

    //窗口显示在屏幕中央
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

/* 设置DBUS客户端 */
void EditPassDialog::set_client(DbusHandleClient *c,QThread *t) {
    m_dbusClient = c;
    m_workThread = t;

    connect(this,SIGNAL(docode(QString,QString)),m_dbusClient,SLOT(get_mcode_by_username(QString,QString)));
    connect(m_dbusClient,SIGNAL(finished_ret_code_edit(int)),this,SLOT(setret_code(int)));
    connect(this,SIGNAL(doreset(QString,QString,QString,QString)),m_dbusClient,SLOT(user_resetpwd(QString,QString,QString,QString)));
    connect(m_dbusClient,SIGNAL(finished_ret_reset_edit(int)),this,SLOT(setret_edit(int)));
    connect(this,SIGNAL(docheck()),m_dbusClient,SLOT(check_login()));
    connect(m_dbusClient,SIGNAL(finished_ret_check_edit(QString)),this,SLOT(setret_check(QString)));

    //connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_edit_code_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_mcode_by_username",this,SLOT(on_edit_code_finished(int,QString)));
    //connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_edit_submit_finished(int)));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface","finished_user_resetpwd",this,SLOT(on_edit_submit_finished(int,QString)));
}

/* DBUS客户端回调函数处理，处理异常 */
void EditPassDialog::setret_code(int ret) {
    if(ret != 0) {
        m_mcodeLineEdit->setText("");
        set_code(messagebox(ret));
        m_Tips->show();
        setshow(m_workWidget);
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
        m_mcodeLineEdit->setText("");
        m_Tips->show();
        setshow(m_workWidget);
    }
}

/* 设置错误提示代码 */
void EditPassDialog::set_code(QString codenum) {
    m_codeStatus = codenum;
    emit code_changed();
}

/* 设置提示错误消息 */
void EditPassDialog::setstyleline() {
    m_Tips->set_text(m_codeStatus);
}

/* 验证码发送按钮处理 */
void EditPassDialog::on_send_code() {
    char phone[32];
    emit docheck();
    m_mcodeBtn->setEnabled(false);
    if(m_newPassLineEdit->check() == false){
        m_mcodeBtn->setEnabled(true);
        m_mcodeLineEdit->setText("");
        set_code(tr("At least 6 bit, include letters and digt"));
        m_Tips->show();
        setshow(m_workWidget);
        return ;
    }
    //qDebug()<<name;
    if(m_szCode != "" && m_szCode != "201" && m_szCode != "203" &&m_passConfirm->text()!="") {
        qstrcpy(phone,m_szCode.toStdString().c_str());
        emit docode(phone,m_szUuid);
    }else {
        m_mcodeBtn->setEnabled(true);
        m_mcodeLineEdit->setText("");
        set_code(messagebox(-1));
        m_Tips->show();
        return ;
    }
}

/*　修改密码按钮处理 */
void EditPassDialog::on_edit_submit() {
    QString new_pass,mcode,confirm_password,acco;
    //bool ok_cur = account->text().isNull();
    bool ok_new = m_newPassLineEdit->text().isNull();
    bool ok_code = m_mcodeLineEdit->text().isNull();
    bool ok_confirm = m_passConfirm->text().isNull();
    bool ok_acc = m_szCode.isNull();
    m_workWidget->setEnabled(false);  //防止用户乱点
    if( !ok_new && !ok_code && !ok_confirm && !ok_acc) {
        //qstrcpy(cur_acc,account->text().toStdString().c_str());
        //cur_acc = account->text();
       // qstrcpy(new_pass,newpass->text().toStdString().c_str());
        new_pass = m_newPassLineEdit->text();
        //qstrcpy(mcode,valid_code->text().toStdString().c_str());
        mcode = m_mcodeLineEdit->text();
        //qstrcpy(confirm_password,confirm_pass->text().toStdString().c_str());
        confirm_password = m_passConfirm->text();
        acco = m_szCode;
        if(m_newPassLineEdit->check() == false) {
            m_workWidget->setEnabled(true);
            set_code(tr("At least 6 bit, include letters and digt"));
            m_Tips->show();
            setshow(m_workWidget);
            return ;
        }
        if(new_pass != confirm_password ) {
            m_workWidget->setEnabled(true);
            set_code(tr("Please check your password!"));
            m_Tips->show();
            setshow(m_workWidget);
            return ;
        }
        emit doreset(acco,new_pass,mcode,m_szUuid);
    }
}

/* 计时开始 */
void EditPassDialog::on_timer_start() {
    if(m_szTimerNum > 0) {
        QString str = tr("Resend(") + QString::number(m_szTimerNum,10) + tr(")");
        m_mcodeBtn->setText(tr(str.toStdString().c_str()));
        m_szTimerNum --;
    }else if(m_szTimerNum == 0) {
        m_szTimerNum = 60;
        m_mcodeBtn->setEnabled(true);
        m_mcodeBtn->setText(tr("Send"));
        m_cMcodeTimer->stop();
    }
}

/* 修改密码成功后进行回调处理 */
void EditPassDialog::on_edit_submit_finished(int req,QString uuid) {
    if(uuid != this->m_szUuid) {
        return ;
    }
    if(m_bIsUsed == false) {
        return ;
    }

    m_workWidget->setEnabled(true);
    if(req == 0) {
        //qDebug()<<"wb888";
        m_delBtn->hide();
        m_stackedWidget->setCurrentWidget(m_successDialog);
        m_successDialog->m_backloginBtn->setText(tr("Reback sign in"));
    } else {
        set_code(messagebox(req));
        m_Tips->show();
        setshow(m_stackedWidget);
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
    if(this->m_szUuid != uuid) {
        return ;
    }

    if(m_bIsUsed == false) {
        return ;
    }
    m_mcodeBtn->setEnabled(true);
    if(req != 0) {
        set_code(messagebox(req));
        m_Tips->show();
        setshow(m_stackedWidget);
    } else if(req == 0) {
        m_cMcodeTimer->start();
        m_cMcodeTimer->setInterval(1000);
        m_mcodeBtn->setEnabled(false);
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
    if(w == m_newPassLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_Tips->isHidden()) {
            m_Tips->hide();

            setshow(m_workWidget);

        }
        if (e->type() == QEvent::FocusIn && m_passTips->isHidden()) {
            m_passTips->show();

            setshow(m_workWidget);

        } else if (e->type() == QEvent::FocusOut && !m_passTips->isHidden()) {
            m_passTips->hide();

            setshow(m_workWidget);
        }
    }
    if(w == m_passConfirm) {
        if (e->type() == QEvent::FocusIn && !m_Tips->isHidden()) {
            m_Tips->hide();

            setshow(m_workWidget);

        }
    }
    if(w == m_mcodeLineEdit) {
        if (e->type() == QEvent::FocusIn && !m_Tips->isHidden()) {
            m_Tips->hide();

            setshow(m_workWidget);

        }
    }
    return QWidget::eventFilter(w,e);
}

/* 清理窗口 */
void EditPassDialog::set_clear() {
    if(!m_Tips->isHidden()) {
        m_Tips->hide();
    }
   // account->setText("");
    m_newPassLineEdit->setText("");
    m_passConfirm->setText("");
    m_mcodeLineEdit->setText("");
    m_szTimerNum = 60;
    m_mcodeBtn->setEnabled(true);
    m_mcodeBtn->setText(tr("Send"));
    m_cMcodeTimer->stop();
}

/* 关闭窗口处理函数　*/
void EditPassDialog::on_close() {
    if(m_delBtn->isHidden()) {
        m_delBtn->show();
        m_delBtn->raise();
    }
    m_newPassLineEdit->get_visble()->setChecked(false);
    //account->get_visble()->setChecked(false);
    m_passConfirm->get_visble()->setChecked(false);
    m_stackedWidget->setCurrentIndex(0);
    set_clear();
    close();
}
