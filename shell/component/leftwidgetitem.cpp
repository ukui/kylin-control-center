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
#include <QSvgRenderer>
#include <QApplication>

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

void LeftWidgetItem::setLabelPixmap(QString filename, QString icoName, QString color) {
    this->icoName = icoName;

    QPixmap pix = loadSvg(filename, color);
    iconLabel->setPixmap(pix);
}

void LeftWidgetItem::isSetLabelPixmapWhite(bool selected) {
    QString fileName;
    if(selected) {
        fileName = "://img/secondaryleftmenu/"+this->icoName+"White.svg";
    } else {
        fileName = "://img/secondaryleftmenu/"+this->icoName+".svg";
    }
//    qDebug()<<"file name is-------->"<<fileName<<endl;
    QPixmap pix =  loadSvg(fileName, "blue");
    iconLabel->setPixmap(pix);
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
        widget->setStyleSheet("QListWidget::Item:hover{background:#FF3D6BE5;border-radius: 4px;}");
    }
}

QString LeftWidgetItem::text(){
    return textLabel->text();
}

const QPixmap LeftWidgetItem::loadSvg(const QString &fileName, QString color)
{
    int size = 24;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size = 48;
    } else if (3 == ratio) {
        size = 96;
    }
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);
    return drawSymbolicColoredPixmap(pixmap, color);
}

QPixmap LeftWidgetItem::drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor)
{
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if ("white" == cgColor) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                } else if ("black" == cgColor) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                } else if ("gray" == cgColor) {
                    color.setRed(152);
                    color.setGreen(163);
                    color.setBlue(164);
                    img.setPixelColor(x, y, color);
                } else {
                    return source;
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

