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
#include "ql_swichbutton.h"

QL_SwichButton::QL_SwichButton(QWidget *parent) : QWidget(parent) {
    ql_color = new QColor;
    setMaximumSize(48,24);
    setMinimumSize(48,24);
    ql_width = (float)width();
    ql_height = (float)height();
    timer = new QTimer(this);
    timer->setInterval(5);
    if(on == 1) {
        cur_val = ql_width - 16 - 4;
    }
    else {
        cur_val = 4;
    }
    connect(timer,SIGNAL(timeout()),this,SLOT(startAnimation()));
}

/* 绘制SwitchButton */
void QL_SwichButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter ql_painter(this);
    ql_painter.setRenderHint(QPainter::SmoothPixmapTransform);
    ql_painter.setRenderHint(QPainter::Antialiasing); //kan ju ci
    ql_painter.setPen(Qt::NoPen);
    QColor ql_color_active_on(61,107,229);
    QColor ql_color_active_off(204,204,204);
    QColor ql_color_inactive(233,233,233);
    if(active == 1 && on) {
        *ql_color = ql_color_active_on;
    } else if(active == 1 && !on) {
        *ql_color = ql_color_active_off;
    } else {
        *ql_color = ql_color_inactive;
    }
    if(on) {
        ql_painter.save();
        ql_painter.setBrush(*ql_color);
        QRectF active_rect = QRectF(0,0,ql_width,ql_height);
        ql_painter.drawRoundedRect(active_rect, 0.5 * ql_height, 0.5 * ql_height); //hua yi ge yuan
        ql_painter.restore();
        ql_painter.save();
        ql_painter.setBrush(Qt::white);
        ql_painter.drawEllipse(cur_val,4, 16, 16);
        ql_painter.restore();
    } else {
        ql_painter.save();
        ql_painter.setBrush(*ql_color);
        QRectF inactive_rect = QRectF(0 ,0,ql_width,ql_height);
        ql_painter.drawRoundedRect(inactive_rect, 0.5 * ql_height, 0.5 * ql_height);
        ql_painter.restore();                          //kai shi shua
        ql_painter.save();
        ql_painter.setBrush(*ql_color);
        QRectF blueRect = QRectF(ql_height * 0.16, ql_height * 0.16,ql_width - ql_height * 0.33, ql_height * 0.67);
        ql_painter.drawRoundedRect(blueRect, 0.45 * ql_height, 0.45 * ql_height);
        ql_painter.restore();
        ql_painter.save();
        ql_painter.setBrush(Qt::white);
        ql_painter.drawEllipse(cur_val,4, 16, 16);
        ql_painter.restore();
    }
}

/* 给SwitchButton设置一个id，方便管理 */
void QL_SwichButton::set_id(int id) {
    this->id = id;
}

/* 让SwitchButton处于可用状态 */
void QL_SwichButton::set_active(bool ok) {
    active = ok;
    update();
}

/* 获取按钮是否可用 */
int QL_SwichButton::get_active() {
    return active;
}

/* 播放按钮开启关闭动画 */
void QL_SwichButton::startAnimation() { //滑动按钮动作播放
    if(active == 0) {
        return ;
    }
    int pos = 4;
    int size = ql_width - 16;
    if(on) {
        cur_val ++;                     //往右滑动
        if(cur_val > size - pos) {      //到达边界停下来
            timer->stop();
        }

    } else {
        cur_val --;
        if(cur_val < pos) {             //到达最小值，停止继续前进
            timer->stop();
        }
    }
    update();
}

/* 按钮按下处理 */
void QL_SwichButton::mousePressEvent(QMouseEvent *event) {
    if(active == 0) {
        return ;
    }
    Q_UNUSED(event);
    on = !on;
    emit status(on,id);
    timer->start();
    return QWidget::mousePressEvent(event);
}

/* 获取开关状况 */
int QL_SwichButton::get_swichbutton_val() {
    return this->on;
}

QL_SwichButton::~QL_SwichButton() {
    delete ql_color;
}

/* 设置开关状态 */
void QL_SwichButton::set_swichbutton_val(int on) {
    this->on = on;
    timer->start();
    update();
}
