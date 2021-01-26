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
#include "passdialog.h"
#include <QTimer>
#include <QSizePolicy>
#include "passwordlineedit.h"
#include "dbusutils.h"
#include <QShortcut>
#include <QMovie>
#include <QtDBus/QtDBus>
#include "tips.h"
#include "svghandler.h"
#include "blueeffect.h"
#include "logindialog.h"
#include <QDialog>
#include "CloseButton/closebutton.h"

class MainDialog : public QDialog
{
    Q_OBJECT
public:
    explicit        MainDialog(QWidget *parent = nullptr);
    QString         status = tr("Sign in Cloud");
    int             timerout_num = 60;
    QString         messagebox(const int &code) const;
    void            set_client(DBusUtils *c,QThread *t);
    QPushButton    *get_login_submit();
    QString replace_blank(QString &str);
    LoginDialog     *get_dialog();
    bool            retok = true;
    void            set_clear();
    void            setshow(QWidget *w);
    bool            is_used = false;
    void            set_staus(const bool &ok);
    void            closedialog();
    void            setnormal();
    ~MainDialog();

public slots:
    void on_login_btn();
    void on_pass_btn();
    void on_reg_btn();
    void on_login_finished(int ret);
    //void on_reg_finished(int ret,QString m_uuid);
    void on_get_mcode_by_phone(int ret);
    void on_timer_timeout();
    void on_send_code_log();
    //void on_send_code_bind();
    void on_close();
    //void on_bind_finished(int ret,QString m_uuid);
    //void on_bind_btn();
    void cleanconfirm(QString str);
    void setret_login(int ret);
    void setret_phone_login(int ret);
    //void setret_rest(int ret);
    //void setret_bind(int ret);
    void setret_code_phone_login(int ret);
    //void setret_code_user_bind(int ret);
    void set_back();
protected:
    void            paintEvent(QPaintEvent *event);
    bool            eventFilter(QObject *w,QEvent *e);

private:
    bool            m_bAutoLogin = false;
    QString         m_szRegAccount;
    QString         m_szRegPass;
    QString         m_szAccount;
    QString         m_szPass;
    LoginDialog     *m_loginDialog;
    QLabel          *m_titleLable;
    QPushButton     *m_submitBtn;
    QPushButton     *m_regBtn;
    QStackedWidget  *m_stackedWidget;
    QVBoxLayout     *m_workLayout;
    QHBoxLayout     *m_subLayout;
    QPoint          m_startPoint;
    CloseButton     *m_delBtn;
    PasswordLineEdit       *m_loginPassLineEdit;

    QLineEdit       *m_loginLineEdit;
    QLineEdit       *m_loginAccountLineEdit;
    Tips          *m_loginTips;
    //QLabel          *m_regTips;
    QLineEdit       *m_loginMCodeLineEdit;
    Tips          *m_loginCodeStatusTips;
    //QPushButton     *m_regSendCodeBtn;
    //QPushButton     *m_forgetpassBtn;
    QPushButton     *m_forgetpassSendBtn;
    QString         *m_szPassName;
    QTimer          *m_timer;
    DBusUtils   *m_dbusClient;
    QWidget         *m_containerWidget;
    QStackedWidget  *m_baseWidget;
    //BindPhoneDialog *m_BindDialog;
    Blueeffect          *m_blueEffect;
    QThread         *m_workThread;
    bool            m_bIsSendOk = false;
    bool            m_bIsLogOk = false;
    bool            m_bIsRegOk = false;
    QString         m_uuid;
    SVGHandler *m_svgHandler;
    QHBoxLayout    *m_animateLayout;

    QString        m_PhoneLogin;
    QString        m_NameLogin;

signals:
    void on_close_event();
    void on_login_failed();
    void on_login_success();
    void on_allow_send();
    void dologin(QString username,QString pwd);
    void dogetmcode_phone_log(QString phonenumb);
    void dophonelogin(QString phone,QString mCode);

};

#endif // DIALOG_LOGIN_REG_H
