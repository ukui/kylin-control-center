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
#ifndef EDITPASSDIALOG_H
#define EDITPASSDIALOG_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QTimer>
#include <QStackedWidget>
#include "successdiaolog.h"
#include <QRegExpValidator>
#include "ql_lineedit_pass.h"
#include "dbushandleclient.h"
#include <QtDBus/QtDBus>

class EditPassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        EditPassDialog(QWidget *parent = nullptr);
    int             timerout_num = 60;
    void            set_code(QString codenum);
    void            set_client(DbusHandleClient *c,QThread *t);
    void            set_clear();
    QLabel*         get_tips();
    QString         messagebox(int codenum);
    void            setshow(QWidget *w);
    QString         name;
    bool            is_used = false;
public slots:
    void            on_edit_submit();
    void            on_edit_submit_finished(int req,QString uuid);
    void            on_edit_code_finished(int req,QString uuid);
    void            on_timer_start();
    void            on_send_code();
    void            setstyleline();
    void            on_close();
    void            setret_code(int ret);
    void            setret_check(QString ret);
    void            setret_edit(int ret);
protected:
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
    void            mouseMoveEvent(QMouseEvent *event);
    bool            eventFilter(QObject *w,QEvent *e);
private:
    QLabel          *title;
    QPushButton     *del_btn;
    ql_lineedit_pass       *newpass;
    QLineEdit       *valid_code;
    QPushButton     *get_code;
    QPushButton     *cancel;
    QPushButton     *confirm;
    QVBoxLayout     *vlayout;
    ql_lineedit_pass       *confirm_pass;
    QHBoxLayout     *hlayout;
    QHBoxLayout     *btnlayout;
    QPoint          m_startPoint;
    DbusHandleClient   *client;
    QTimer          *timer;
    QLabel          *tips;
    QString         code;
    QStackedWidget  *stackwidget;
    QWidget         *content;
    SuccessDiaolog  *success;
    QVBoxLayout     *vboxlayout;
    QLabel          *pass_tips;
    QThread         *thread;
    QString         uuid;
signals:
    void code_changed();
    void account_changed();
    void docode(QString name,QString uuid);
    void doreset(QString a,QString b,QString c,QString uuid);
    void docheck();
    void dologout();
};

#endif // EDITPASSDIALOG_H
