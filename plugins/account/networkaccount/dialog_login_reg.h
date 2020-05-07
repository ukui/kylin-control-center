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
#ifndef DIALOG_LOGIN_REG_H
#define DIALOG_LOGIN_REG_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QtMath>
#include <QDebug>
#include <QStackedWidget>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "logindialog.h"
#include "regdialog.h"
#include "successdiaolog.h"
#include "passdialog.h"
#include <QTimer>
#include <QSizePolicy>
#include "ql_lineedit_pass.h"
#include "dbushandleclient.h"
#include <QShortcut>
#include "bindphonedialog.h"
#include <QMovie>
#include <QtDBus/QtDBus>

class Dialog_login_reg : public QWidget
{
    Q_OBJECT
public:
    explicit        Dialog_login_reg(QWidget *parent = nullptr);
    QString         status = tr("Sign in Cloud");
    int             timerout_num = 60;
    int             timerout_num_reg = 60;
    int             timerout_num_log = 60;
    int             timerout_num_bind = 60;
    QString         messagebox(int code);
    void            set_client(DbusHandleClient *c,QThread *t);
    QPushButton    *get_login_submit();
    bool            retok = true;
    void            set_clear();
    void            setshow(QWidget *w);
    bool            is_used = false;

public slots:
    void linked_forget_btn();
    void linked_register_btn();
    void back_login_btn();
    void back_normal();
    void on_login_btn();
    void on_pass_btn();
    void on_reg_btn();
    void on_login_finished(int ret,QString uuid);
    void on_pass_finished(int ret,QString uuid);
    void on_reg_finished(int ret,QString uuid);
    void on_get_mcode_by_name(int ret,QString uuid);
    void on_get_mcode_by_phone(int ret,QString uuid);
    void on_timer_timeout();
    void on_send_code();
    void on_send_code_reg();
    void on_send_code_log();
    void on_send_code_bind();
    void on_timer_reg_out();
    void on_timer_log_out();
    void on_timer_bind_out();
    void on_close();
    void on_bind_finished(int ret,QString uuid);
    void on_bind_btn();
    void cleanconfirm(QString str);
    void setret_login(int ret);
    void setret_phone_login(int ret);
    void setret_rest(int ret);
    void setret_reg(int ret);
    void setret_bind(int ret);
    void setret_code_phone_login(int ret);
    void setret_code_phone_reg(int ret);
    void setret_code_user_pass(int ret);
    void setret_code_user_bind(int ret);
    void set_back();
protected:
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
    void            mouseMoveEvent(QMouseEvent *event);
    bool            eventFilter(QObject *w,QEvent *e);

private:
    QString         account;
    QString         passwd;
    LoginDialog     *box_login;
    QLabel          *title;
    QPushButton     *login_submit;
    QPushButton     *register_account;
    QStackedWidget  *stack_box;
    QVBoxLayout     *vboxlayout;
    QHBoxLayout     *hboxlayout;
    QPoint          m_startPoint;
    QPushButton     *del_btn;
    PassDialog      *box_pass;
    RegDialog       *box_reg;
    QLabel          *user_tip;
    QLabel          *pass_tip;
    QLabel          *pass_tips;
    QLabel          *reg_tips;
    ql_lineedit_pass       *reg_pass;
    QLineEdit       *reg_user;
    ql_lineedit_pass       *login_pass;
    QLineEdit       *valid_code;
    QLineEdit       *phone_user;
    QLineEdit       *pass_user;
    ql_lineedit_pass       *pass_confirm;
    ql_lineedit_pass       *pass_pwd;
    QLineEdit       *pass_code;
    QLineEdit       *login_mcode;
    QLineEdit       *login_user;
    ql_lineedit_pass       *reg_confirm;
    QLabel          *passlabel;
    QLabel          *passtips;
    QLineEdit       *login_code;
    QLabel          *codelable;
    QPushButton     *send_btn_reg;
    QPushButton     *send_btn_fgt;
    QPushButton     *send_btn_log;
    QString         *name_fgt;
    QTimer          *timer;
    QTimer          *timer_reg;
    QTimer          *timer_log;
    QTimer          *timer_bind;
    DbusHandleClient   *client;
    QWidget         *log_reg;
    QStackedWidget  *basewidegt;
    SuccessDiaolog  *succ;
    BindPhoneDialog *box_bind;
    QLabel          *gif;
    QMovie          *pm;
    QThread         *thread;
    bool            send_is_ok = false;
    bool            send_is_ok_log = false;
    bool            send_is_ok_reg = false;
    QString         uuid;

signals:
    void on_login_success();
    void on_allow_send();
    void dologin(QString username,QString pwd,QString uuid);
    void dogetmcode_phone_log(QString phonenumb,QString uuid);
    void dogetmcode_phone_reg(QString phonenumb,QString uuid);
    void dogetmcode_number_bind(QString username,QString uuid);
    void dogetmcode_number_pass(QString username,QString uuid);
    void dorest(QString username, QString newpwd, QString mCode,QString uuid);
    void doreg(QString username, QString pwd, QString phonenumb, QString mcode,QString uuid);
    void dophonelogin(QString phone, QString mCode,QString uuid);
    void dobind(QString username, QString pwd, QString phone, QString mCode,QString uuid);

};

#endif // DIALOG_LOGIN_REG_H
