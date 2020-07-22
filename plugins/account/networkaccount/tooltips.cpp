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
#include "tooltips.h"

Tooltips::Tooltips(QWidget *parent) : QWidget(parent)
{
    m_postionX = 6;
    m_postionY = 6;
    m_alphaValue = 10;
    m_borderRadius = 4;

    setStyleSheet("ql_popup{border-radius:4px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
}

/* 重绘加上边框阴影 */
void Tooltips::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    QColor m_defaultBackgroundColor = qRgb(0, 0, 0);
    QPainterPath path1;
    path1.setFillRule(Qt::WindingFill);
    path1.addRoundedRect(m_postionX, m_postionY, this->width() - (m_postionX * 2), this->height() - (m_postionY * 2), m_borderRadius, m_borderRadius);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path1, QBrush(QColor(m_defaultBackgroundColor.red(),
                                          m_defaultBackgroundColor.green(),
                                          m_defaultBackgroundColor.blue())));

    QColor color(0, 0, 0, m_alphaValue);
    for (int i = 0; i < 5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(m_postionX - i, m_postionY - i, this->width() - (m_postionX - i) * 2, this->height() - (m_postionY - i) * 2, m_borderRadius, m_borderRadius);
        color.setAlpha(80 - qSqrt(i) * 40);
        painter.setPen(color);
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(palette().color(QPalette::Base)));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setX(m_postionX);
    rect.setY(m_postionY);
    rect.setWidth(rect.width() - m_postionY);
    rect.setHeight(rect.height() - m_postionX);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);
}
