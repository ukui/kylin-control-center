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
#include "editpushbutton.h"

EditPushButton::EditPushButton(QWidget *parent) : QPushButton(parent)
{
    m_toolTips = new Tooltips(this);
    m_resetLabel = new QLabel(m_toolTips);
    m_workLayout = new QHBoxLayout;

    m_toolTips->setFixedSize(86,44);
    m_toolTips->setStyleSheet("QWidget{border-radius:4px;}");

    m_resetLabel->setText(tr("Reset"));
    m_resetLabel->setStyleSheet("QLabel{font-size:14px;}");

    m_workLayout->addWidget(m_resetLabel,0,Qt::AlignCenter);
    m_workLayout->setMargin(0);
    m_workLayout->setSpacing(0);
    m_toolTips->setLayout(m_workLayout);
    m_toolTips->hide();
    m_toolTips->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::NoFocus);
    m_resetLabel->setFocusPolicy(Qt::NoFocus);
}

/* 鼠标进入，弹出提示框 */
void EditPushButton::enterEvent(QEvent *e) {

    QPoint pos;
    pos.setX(this->mapToGlobal(QPoint(0, 0)).x() + 26);
    pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + 26);
    m_toolTips->move(pos);
    m_toolTips->show();
    return QPushButton::enterEvent(e);
}

/* 鼠标离开，提示框消失 */
void EditPushButton::leaveEvent(QEvent *e) {
    m_toolTips->hide();

    return QPushButton::leaveEvent(e);
}
