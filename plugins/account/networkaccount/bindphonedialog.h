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
#ifndef BINDPHONEDIALOG_H
#define BINDPHONEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include "passwordlineedit.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "areacodelineedit.h"
#include <QLabel>
#include "tips.h"
#include "svghandler.h"

class BindPhoneDialog : public QWidget
{
    Q_OBJECT
public:
    explicit BindPhoneDialog(QWidget *parent = nullptr);
    void    setclear();
    void    set_code(QString m_countryCode);
    QPushButton *get_send_code();
    Tips  *get_tips();
    QString get_phone();
    QString get_code();
    AreaCodeLineEdit* get_phone_lineedit();
    QLineEdit* get_code_lineedit();
    void set_staus(bool ok);
public slots:
    void setstyleline();
signals:
    void code_changed();
private:
    QString             m_countryCode;
    AreaCodeLineEdit  *m_phoneLineEdit;
    QLineEdit           *m_mCodeLineEdit;
    QPushButton         *m_sendCode_btn;
    QVBoxLayout         *m_vboxLayout;
    QHBoxLayout         *m_subHBoxLayout;
    Tips              *m_tips;
    SVGHandler      *m_svgHandler;

};

#endif // BINDPHONEDIALOG_H

