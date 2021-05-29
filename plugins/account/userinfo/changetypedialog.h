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
#ifndef CHANGETYPEDIALOG_H
#define CHANGETYPEDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>

namespace Ui {
class ChangeTypeDialog;
}

class ChangeTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeTypeDialog(QWidget *parent = 0);
    ~ChangeTypeDialog();

public:
    void setupComonpent();

    void setFace(QString faceFile);
    void setUsername(QString name);
    void setCurrentAccountTypeLabel(QString aType);
    void setCurrentAccountTypeBtn(int id);

    void forbidenChange(int can);

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

private:
    Ui::ChangeTypeDialog *ui;

    int currenttype;
    bool currentloginstatus;

Q_SIGNALS:
    void type_send(int type);
};

#endif // CHANGETYPEDIALOG_H
