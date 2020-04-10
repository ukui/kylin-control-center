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
#ifndef CHANGEPWDDIALOG_H
#define CHANGEPWDDIALOG_H

#include <QDialog>
#include <QPainter>

namespace Ui {
class ChangePwdDialog;
}

class ChangePwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwdDialog(QWidget *parent = 0);
    ~ChangePwdDialog();

public:
    void setupComponent();
    void setupConnect();

    void refreshConfirmBtnStatus();

    void setFace(QString iconfile);
    void setUsername(QString username);
    void setPwdType(QString type);
    void setAccountType(QString text);

protected:
    void paintEvent(QPaintEvent *);


private:
    Ui::ChangePwdDialog *ui;

Q_SIGNALS:
    void passwd_send(QString pwd, QString username);
};

#endif // CHANGEPWDDIALOG_H
