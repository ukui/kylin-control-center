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
#include "ql_lineedit_pass.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "area_code_lineedit.h"
#include <QLabel>
#include "ql_label_info.h"
#include "ql_svg_handler.h"

class BindPhoneDialog : public QWidget
{
    Q_OBJECT
public:
    explicit BindPhoneDialog(QWidget *parent = nullptr);
    void    setclear();
    void    set_code(QString code);
    QPushButton *get_send_code();
    ql_label_info  *get_tips();
    QString get_phone();
    QString get_code();
    area_code_lineedit* get_phone_lineedit();
    QLineEdit* get_code_lineedit();
    void set_staus(bool ok);
public slots:
    void setstyleline();
signals:
    void code_changed();
private:
    QString             code;
    area_code_lineedit  *phone;
    QLineEdit           *valid_code;
    QPushButton         *send_code;
    QVBoxLayout         *layout;
    QHBoxLayout         *sublayout;
    ql_label_info              *tips;
    ql_svg_handler      *svg_hd;

};

#endif // BINDPHONEDIALOG_H

