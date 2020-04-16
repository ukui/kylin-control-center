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
#include "leftwidgetitem.h"

#include <QPainter>
#include <QStyleOption>
#include <QDebug>

LeftWidgetItem::LeftWidgetItem(QWidget *parent) :
    QWidget(parent)
{
//    this->setStyleSheet("background: none;");
    widget = new QWidget(this);
//    widget->setFixedSize(120, 40);
//    widget->setStyleSheet("QWidget{background: #FFFFFF;}");
    widget->setFixedHeight(40);


    iconLabel = new QLabel(widget);
    QSizePolicy policy = iconLabel->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    iconLabel->setSizePolicy(policy);
    iconLabel->setFixedSize(24, 24);

    textLabel = new QLabel(widget);
    QSizePolicy policy1 = textLabel->sizePolicy();
    policy1.setHorizontalPolicy(QSizePolicy::Fixed);
    policy1.setVerticalPolicy(QSizePolicy::Fixed);
    textLabel->setSizePolicy(policy1);
    textLabel->setScaledContents(true);


    QHBoxLayout * mainlayout = new QHBoxLayout(widget);
    mainlayout->setSpacing(8);
    mainlayout->setContentsMargins(8, 0, 0, 0);
    mainlayout->addWidget(iconLabel, Qt::AlignVCenter);
    mainlayout->addWidget(textLabel, Qt::AlignVCenter);
    mainlayout->addStretch();

    widget->setLayout(mainlayout);


    QVBoxLayout * baseVerLayout = new QVBoxLayout(this);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setMargin(0);

    baseVerLayout->addWidget(widget);
    baseVerLayout->addStretch();

    setLayout(baseVerLayout);
}

LeftWidgetItem::~LeftWidgetItem()
{
}

void LeftWidgetItem::setLabelPixmap(QString filename, QString icoName){
    this->icoName = icoName;
    iconLabel->setPixmap(QPixmap(filename));
}

void LeftWidgetItem::isSetLabelPixmapWhite(bool selected) {
    QString fileName;
    if(selected) {
        fileName = "://img/secondaryleftmenu/"+this->icoName+"White.png";
    } else {
        fileName = "://img/secondaryleftmenu/"+this->icoName+".png";
    }
//    qDebug()<<"file name is-------->"<<fileName<<endl;
    iconLabel->setPixmap(QPixmap(fileName));
}

void LeftWidgetItem::setLabelText(QString text){

    textLabel->setText(text);
}

void LeftWidgetItem::setLabelTextIsWhite(bool selected) {
    if(selected) {
        textLabel->setStyleSheet("color: palette(highlighted-text);");
    } else {
        textLabel->setStyleSheet("color: palette(windowText);");
    }
}

void LeftWidgetItem::setSelected(bool selected){
    if (selected) {
        widget->setStyleSheet("QWidget{background: #3D6BE5; border-radius: 4px;}");
    } else {
        widget->setStyleSheet("QWidget{background: palette(base);}");
    }

}

QString LeftWidgetItem::text(){
    return textLabel->text();
}
