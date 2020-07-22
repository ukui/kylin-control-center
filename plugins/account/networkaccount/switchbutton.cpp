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
#include "switchbutton.h"

SwitchButton::SwitchButton(QWidget *parent) : QWidget(parent) {
    m_buttonColor = new QColor;
    setMaximumSize(48,24);
    setMinimumSize(48,24);
    m_fWidth = (float)width();
    m_fHeight = (float)height();
    m_cTimer = new QTimer(this);
    m_cTimer->setInterval(5);
    if(m_bIsOn == 1) {
        m_fCurrentValue = m_fWidth - 16 - 4;
    }
    else {
        m_fCurrentValue = 4;
    }
    connect(m_cTimer,SIGNAL(timeout()),this,SLOT(startAnimation()));
}

/* 绘制SwitchButton */
void SwitchButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing); //kan ju ci
    painter.setPen(Qt::NoPen);
    QColor colorActiveOn(61,107,229);
    QColor colorActiveOff(204,204,204);
    QColor colorInactive(233,233,233);
    if(m_bIsActive == 1 && m_bIsOn) {
        *m_buttonColor = colorActiveOn;
    } else if(m_bIsActive == 1 && !m_bIsOn) {
        *m_buttonColor = colorActiveOff;
    } else {
        *m_buttonColor = colorInactive;
    }
    if(m_bIsOn) {
        painter.save();
        painter.setBrush(*m_buttonColor);
        QRectF active_rect = QRectF(0,0,m_fWidth,m_fHeight);
        painter.drawRoundedRect(active_rect, 0.5 * m_fHeight, 0.5 * m_fHeight); //hua yi ge yuan
        painter.restore();
        painter.save();
        painter.setBrush(Qt::white);
        painter.drawEllipse(m_fCurrentValue,4, 16, 16);
        painter.restore();
    } else {
        painter.save();
        painter.setBrush(*m_buttonColor);
        QRectF inactive_rect = QRectF(0 ,0,m_fWidth,m_fHeight);
        painter.drawRoundedRect(inactive_rect, 0.5 * m_fHeight, 0.5 * m_fHeight);
        painter.restore();                          //kai shi shua
        painter.save();
        painter.setBrush(*m_buttonColor);
        QRectF blueRect = QRectF(m_fHeight * 0.16, m_fHeight * 0.16,m_fWidth - m_fHeight * 0.33, m_fHeight * 0.67);
        painter.drawRoundedRect(blueRect, 0.45 * m_fHeight, 0.45 * m_fHeight);
        painter.restore();
        painter.save();
        painter.setBrush(Qt::white);
        painter.drawEllipse(m_fCurrentValue,4, 16, 16);
        painter.restore();
    }
}

/* 给SwitchButton设置一个id，方便管理 */
void SwitchButton::set_id(int id) {
    this->m_buttonID = id;
}

/* 让SwitchButton处于可用状态 */
void SwitchButton::set_active(bool ok) {
    m_bIsActive = ok;
    update();
}

/* 获取按钮是否可用 */
int SwitchButton::get_active() {
    return m_bIsActive;
}

/* 播放按钮开启关闭动画 */
void SwitchButton::startAnimation() { //滑动按钮动作播放
    if(m_bIsActive == 0) {
        return ;
    }
    int pos = 4;
    int size = m_fWidth - 16;
    if(m_bIsOn) {
        m_fCurrentValue ++;                     //往右滑动
        if(m_fCurrentValue >= size - pos) {      //到达边界停下来
            m_fCurrentValue = size - pos;
            m_cTimer->stop();
        }

    } else {
        m_fCurrentValue --;
        if(m_fCurrentValue <= pos) {             //到达最小值，停止继续前进
            m_fCurrentValue = pos;
            m_cTimer->stop();
        }
    }
    update();
}

/* 按钮按下处理 */
void SwitchButton::mousePressEvent(QMouseEvent *event) {
    if(m_bIsActive == 0) {
        return ;
    }
    Q_UNUSED(event);
    m_bIsOn = !m_bIsOn;
    emit status(m_bIsOn,m_buttonID);
    m_cTimer->start();
    return QWidget::mousePressEvent(event);
}

/* 获取开关状况 */
int SwitchButton::get_swichbutton_val() {
    return this->m_bIsOn;
}

SwitchButton::~SwitchButton() {
    delete m_buttonColor;
}

/* 设置开关状态 */
void SwitchButton::set_swichbutton_val(int on) {
    this->m_bIsOn = on;
    m_cTimer->start();
}
