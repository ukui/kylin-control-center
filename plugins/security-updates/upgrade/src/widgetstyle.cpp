/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "widgetstyle.h"

int WidgetStyle::themeColor = 0;
int WidgetStyle::systemStatus = 0;

//在屏幕中央显示
WidgetStyle::WidgetStyle(QWidget *parent)
{
    // 初始化组件
    this->setWidgetUi();

    // 初始化样式
    this->setWidgetStyle();
}

//在屏幕中央显示
void WidgetStyle::handleIconClickedSub()
{
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    this->move((availableGeometry.width() - this->width())/2, (availableGeometry.height() - this->height())/2);
}

// 初始化组件
void WidgetStyle::setWidgetUi()
{

}

// 初始化样式
void WidgetStyle::setWidgetStyle()
{

}

// 实现圆角阴影效果
void WidgetStyle::paintEvent(QPaintEvent *event, QWidget *widget)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QRectF rect(10, 10, widget->width()-20, widget->height()-20);
    path.addRoundRect(rect, 6, 6);

    QPainter painter(widget);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i = 0; i < 10; i++) {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundRect(10-i, 10-i, widget->width()-(10-i)*2, widget->height()-(10-i)*2,6);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}
