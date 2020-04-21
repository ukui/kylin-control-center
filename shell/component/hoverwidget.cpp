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
#include "hoverwidget.h"

#include <QPainter>
#include <QStyleOption>

#include <QDebug>

ResHoverWidget::ResHoverWidget(QString mname, QWidget *parent) :
    QWidget(parent),
    _name(mname)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

ResHoverWidget::~ResHoverWidget()
{
}

void ResHoverWidget::enterEvent(QEvent *event){
    emit enterWidget(_name);

    QWidget::enterEvent(event);
}

void ResHoverWidget::leaveEvent(QEvent *event){
    emit leaveWidget(_name);

    QWidget::leaveEvent(event);
}

void ResHoverWidget::mousePressEvent(QMouseEvent *event){

    if (event->button() == Qt::LeftButton){
        emit widgetClicked(_name);
    }

    QWidget::mousePressEvent(event);
}

//子类化一个QWidget，为了能够使用样式表，则需要提供paintEvent事件。
//这是因为QWidget的paintEvent()是空的，而样式表要通过paint被绘制到窗口中。
void ResHoverWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
