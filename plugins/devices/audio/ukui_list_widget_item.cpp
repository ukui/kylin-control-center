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
#include "ukui_list_widget_item.h"

#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QVBoxLayout>
//#include <QSvgRenderer>
#include <QApplication>
bool isCheckBluetoothInput;
UkuiListWidget::UkuiListWidget(QWidget *parent) :
    QListWidget(parent)
{

}

UkuiListWidget::~UkuiListWidget()
{

}

UkuiListWidgetItem::UkuiListWidgetItem(QWidget *parent) :
    QWidget(parent)
{
    this->setFixedSize(500,50);

    QVBoxLayout *vLayout = new QVBoxLayout;

    portLabel = new QLabel(this);
    deviceLabel = new QLabel(this);

    portLabel->setFixedSize(600,20);
    deviceLabel->setFixedSize(600,20);
    vLayout->addWidget(portLabel);
    vLayout->addWidget(deviceLabel);
    this->setLayout(vLayout);

    this->show();
}

UkuiListWidgetItem::~UkuiListWidgetItem()
{
}

void UkuiListWidgetItem::setSelected(bool selected){
    if (selected) {
        widget->setStyleSheet("QWidget{background: #3D6BE5; border-radius: 4px;}");
    } else {
        widget->setStyleSheet("QListWidget::Item:hover{background:#FF3D6BE5;border-radius: 4px;}");
    }
}


void UkuiListWidgetItem::setLabelText(QString portLabel, QString deviceLabel){

    this->portLabel->setText(portLabel);
    this->deviceLabel->setText(deviceLabel);
}

void UkuiListWidgetItem::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    qDebug() << "Mouse Press Event" << this->portLabel->text() << this->deviceLabel->text() << isCheckBluetoothInput;
    //蓝牙输入去除勾选
    if (this->deviceLabel->text().contains("bluez_card")) {
        if (isCheckBluetoothInput == false)
            isCheckBluetoothInput = true;
        else  {
            isCheckBluetoothInput = false;
            QString cmd = "pactl set-card-profile "+this->deviceLabel->text()+" a2dp_sink";
            system(cmd.toLocal8Bit().data());
        }
    }
}

//void UkuiListWidgetItem::paintEvent(QPaintEvent *event)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
////    double transparence = transparency * 255;
//    QColor color = palette().color(QPalette::Base);
////    color.setAlpha(transparence);
//    QBrush brush = QBrush(color);
//    p.setBrush(brush);
//    p.setPen(Qt::NoPen);
//    QPainterPath path;
//    opt.rect.adjust(0,0,0,0);
//    path.addRoundedRect(opt.rect,6,6);
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.drawRoundedRect(opt.rect,6,6);
//    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
////    QWidget::paintEvent(event);
//}

