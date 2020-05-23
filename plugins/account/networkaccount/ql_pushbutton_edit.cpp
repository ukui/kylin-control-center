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
#include "ql_pushbutton_edit.h"

ql_pushbutton_edit::ql_pushbutton_edit(QWidget *parent) : QPushButton(parent)
{
    tips = new QToolTips(this);
    label = new QLabel(tips);
    layout = new QHBoxLayout;

    tips->setFixedSize(86,44);
    tips->setStyleSheet("QWidget{border-radius:4px;}");

    label->setText(tr("Reset"));
    label->setStyleSheet("QLabel{font-size:14px;}");

    layout->addWidget(label,0,Qt::AlignCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    tips->setLayout(layout);
    tips->hide();
    tips->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::NoFocus);
    label->setFocusPolicy(Qt::NoFocus);
}

/* 鼠标进入，弹出提示框 */
void ql_pushbutton_edit::enterEvent(QEvent *e) {

    QPoint pos;
    pos.setX(this->mapToGlobal(QPoint(0, 0)).x() + 26);
    pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + 26);
    tips->move(pos);
    tips->show();
    return QPushButton::enterEvent(e);
}

/* 鼠标离开，提示框消失 */
void ql_pushbutton_edit::leaveEvent(QEvent *e) {
    tips->hide();

    return QPushButton::leaveEvent(e);
}
