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

#include <QDebug>

SwitchButton::SwitchButton(QWidget *parent) :
    QWidget(parent)
{
//    this->resize(QSize(52, 24));
    this->setFixedSize(QSize(48, 24));

    checked = false;

    borderColorOff = QColor("#cccccc");

    bgColorOff = QColor("#cccccc");
    bgColorOn = QColor("#3790FA");

    sliderColorOff = QColor("#ffffff");
    sliderColorOn = QColor("#ffffff");

    space = 4;
//    rectRadius = 5;

    step = width() / 40;
    startX = 0;
    endX= 0;

    timer = new QTimer(this);
    timer->setInterval(5);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatevalue()));
}

SwitchButton::~SwitchButton()
{
}

void SwitchButton::paintEvent(QPaintEvent *){
    //启用反锯齿
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawBg(&painter);
    drawSlider(&painter);
}

void SwitchButton::drawBg(QPainter *painter){
    painter->save();
//    painter->setPen(Qt::NoPen);

    if (!checked){
        painter->setPen(Qt::NoPen);
        painter->setBrush(bgColorOff);
    }
    else{
        painter->setPen(Qt::NoPen);
        painter->setBrush(bgColorOn);
    }
    //circle out
//    QRect rect(space, space, width() - space * 2, height() - space * 2);
//    painter->drawRoundedRect(rect, rectRadius, rectRadius);

    //circle in
    QRect rect(0, 0, width(), height());
    //半径为高度的一半
    int radius = rect.height() / 2;
    //圆的宽度为高度
    int circleWidth = rect.height();

    QPainterPath path;
    path.moveTo(radius, rect.left());
    path.arcTo(QRectF(rect.left(), rect.top(), circleWidth, circleWidth), 90, 180);
    path.lineTo(rect.width() - radius, rect.height());
    path.arcTo(QRectF(rect.width() - rect.height(), rect.top(), circleWidth, circleWidth), 270, 180);
    path.lineTo(radius, rect.top());

    painter->drawPath(path);

    painter->restore();
}

void SwitchButton::drawSlider(QPainter *painter){
    painter->save();
    painter->setPen(Qt::NoPen);

    if (!checked){
        painter->setBrush(sliderColorOff);
    }
    else
        painter->setBrush(sliderColorOn);
    //circle out
//    QRect rect(0, 0, width() - space, height() - space);
//    int sliderwidth = rect.height();
//    QRect sliderRect(startX, space / 2, sliderwidth, sliderwidth);
//    painter->drawEllipse(sliderRect);

    //circle in
    QRect rect(0, 0, width(), height());
    int sliderWidth = rect.height() - space * 2;
    QRect sliderRect(startX + space, space, sliderWidth, sliderWidth);
    painter->drawEllipse(sliderRect);

    painter->restore();
}

void SwitchButton::mousePressEvent(QMouseEvent *){
    checked = !checked;
    emit checkedChanged(checked);

    step = width() / 40;

    if (checked){
        //circle out
//        endX = width() - height() + space;
        //circle in
        endX = width() - height();
    }
    else{
        endX = 0;
    }
    timer->start();
}

void SwitchButton::resizeEvent(QResizeEvent *){
    //
    step = width() / 40;

    if (checked){
        //circle out
//        startX = width() - height() + space;
        //circle in
        startX = width() - height();
    }
    else
        startX = 0;

    update();
}

void SwitchButton::updatevalue(){
    if (checked)
        if (startX < endX){
            startX = startX + step;
        }
        else{
            startX = endX;
            timer->stop();
        }
    else{
        if (startX > endX){
            startX = startX - step;
        }
        else{
            startX = endX;
            timer->stop();
        }
    }
    update();
}

void SwitchButton::setChecked(bool checked){
    if (this->checked != checked){
        this->checked = checked;
        emit checkedChanged(checked);
        update();
    }

    step = width() / 40;

    if (checked){
        //circle out
//        endX = width() - height() + space;
        //circle in
        endX = width() - height();
    }
    else{
        endX = 0;
    }
    timer->start();
}

bool SwitchButton::isChecked(){
    return this->checked;
}


