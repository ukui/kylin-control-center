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
#include "passwordlineedit.h"
#include "dbushandleclient.h"
#include <QtDBus/QtDBus>
#include "tips.h"
#include "svghandler.h"

class EditPassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        EditPassDialog(QWidget *parent = nullptr);
    int             m_szTimerNum = 60;
    void            set_code(QString codenum);
    void            set_client(DbusHandleClient *c,QThread *t);
    void            set_clear();
    Tips*         get_tips();
    QString         messagebox(int codenum);
    void            setshow(QWidget *w);
    QString         m_szCode;
    bool            m_bIsUsed = false;
public slots:
    void            on_edit_submit();
    void            on_edit_submit_finished(int req,QString m_szUuid);
    void            on_edit_code_finished(int req,QString m_szUuid);
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
    QLabel          *m_title;
    QPushButton     *m_delBtn;
    PasswordLineEdit       *m_newPassLineEdit;
    QLineEdit       *m_mcodeLineEdit;
    QPushButton     *m_mcodeBtn;
    QPushButton     *m_cancelBtn;
    QPushButton     *m_confirmBtn;
    QVBoxLayout     *m_wokrLayout;
    PasswordLineEdit       *m_passConfirm;
    QHBoxLayout     *m_hboxLayout;
    QHBoxLayout     *m_btnLayout;
    QPoint          m_startPoint;
    DbusHandleClient   *m_dbusClient;
    QTimer          *m_cMcodeTimer;
    Tips          *m_Tips;
    QString         m_codeStatus;
    QStackedWidget  *m_stackedWidget;
    QWidget         *m_workWidget;
    SuccessDiaolog  *m_successDialog;
    QVBoxLayout     *m_vboxLayout;
    QLabel          *m_passTips;
    QThread         *m_workThread;
    QString         m_szUuid;
    SVGHandler *m_svgHandler;
signals:
    void code_changed();
    void account_changed();
    void docode(QString m_szCode,QString m_szUuid);
    void doreset(QString a,QString b,QString c,QString m_szUuid);
    void docheck();
    void dologout();
};

#endif // EDITPASSDIALOG_H
