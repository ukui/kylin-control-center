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
#include "maskwidget.h"

#include <QPainter>

MaskWidget::MaskWidget(QWidget *parent) :
    QWidget(parent)
{
//    setAttribute(Qt::WA_TranslucentBackground);
    pWidth = parent->width();
    pHeigh = parent->height();
    pRadius = 6;
    pColor = QString("#ffffff");
    pBorder = 2;
}

MaskWidget::~MaskWidget()
{
}


void MaskWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QPainter painter(this);

    painter.setRenderHint(QPainter:: Antialiasing, true);  //设置渲染,启动反锯齿

    painter.setPen(QPen(QColor(palette().color(QPalette::Base)), pBorder));

    painter.drawRect(0, 0, pWidth, pHeigh);
    painter.drawRoundedRect(0, 0, pWidth, pHeigh, pRadius, pRadius);
}
