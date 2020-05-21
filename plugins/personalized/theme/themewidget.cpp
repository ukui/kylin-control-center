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
#include "themewidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <QPainter>
#include <QStyleOption>

#include <QDebug>

ThemeWidget::ThemeWidget(QSize iSize, QString name, QStringList iStringList, QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(66);
    setMinimumWidth(550);
    setMaximumWidth(960);
    setAttribute(Qt::WA_DeleteOnClose);

//    setStyleSheet("background: palette(button); border-radius: 6px;");

    pValue = "";


    QHBoxLayout * mainHorLayout = new QHBoxLayout(this);
    mainHorLayout->setSpacing(16);
    mainHorLayout->setContentsMargins(16, 0, 16, 0);

    //占位Label，解决隐藏选中图标后文字Label位置的变化
    placeHolderLabel = new QLabel(this);
    QSizePolicy phSizePolicy = placeHolderLabel->sizePolicy();
    phSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    phSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    placeHolderLabel->setSizePolicy(phSizePolicy);
    placeHolderLabel->setFixedSize(QSize(16, 16)); //选中图标的大小为16*16

    selectedLabel = new QLabel(this);
    QSizePolicy sSizePolicy = selectedLabel->sizePolicy();
    sSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    selectedLabel->setSizePolicy(sSizePolicy);
    selectedLabel->setScaledContents(true);
    selectedLabel->setPixmap(QPixmap("://img/plugins/theme/selected.png"));

    QLabel * nameLabel = new QLabel(this);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setFixedWidth(102);
    nameLabel->setText(name);

    QHBoxLayout * iconHorLayout = new QHBoxLayout;
    iconHorLayout->setSpacing(16);
    iconHorLayout->setMargin(0);
    for (QString icon : iStringList){
        QLabel * label = new QLabel(this);
        label->setFixedSize(iSize);
        label->setPixmap(QPixmap(icon));
        iconHorLayout->addWidget(label);
    }

    mainHorLayout->addWidget(placeHolderLabel);
    mainHorLayout->addWidget(selectedLabel);
    mainHorLayout->addWidget(nameLabel);
    mainHorLayout->addStretch(1);
    mainHorLayout->addLayout(iconHorLayout);
    mainHorLayout->addStretch(2);

    setLayout(mainHorLayout);

}

ThemeWidget::ThemeWidget(QSize iSize, QString name, const QList<QPixmap> &listMap, QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(66);
    setMinimumWidth(550);
    setMaximumWidth(960);
    setAttribute(Qt::WA_DeleteOnClose);

//    setStyleSheet("background: palette(button); border-radius: 6px;");

    pValue = "";


    QHBoxLayout * mainHorLayout = new QHBoxLayout(this);
    mainHorLayout->setSpacing(16);
    mainHorLayout->setContentsMargins(16, 0, 16, 0);

    //占位Label，解决隐藏选中图标后文字Label位置的变化
    placeHolderLabel = new QLabel(this);
    QSizePolicy phSizePolicy = placeHolderLabel->sizePolicy();
    phSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    phSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    placeHolderLabel->setSizePolicy(phSizePolicy);
    placeHolderLabel->setFixedSize(QSize(16, 16)); //选中图标的大小为16*16

    selectedLabel = new QLabel(this);
    QSizePolicy sSizePolicy = selectedLabel->sizePolicy();
    sSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    selectedLabel->setSizePolicy(sSizePolicy);
    selectedLabel->setScaledContents(true);
    selectedLabel->setPixmap(QPixmap("://img/plugins/theme/selected.png"));

    QLabel * nameLabel = new QLabel(this);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setFixedWidth(102);
    nameLabel->setText(name);

    QHBoxLayout * iconHorLayout = new QHBoxLayout;
    iconHorLayout->setSpacing(25);
    iconHorLayout->setMargin(0);
    for (QPixmap icon : listMap){
        QLabel * label = new QLabel(this);
        label->setFixedSize(iSize);
        label->setPixmap(icon);
        iconHorLayout->addWidget(label);
    }


    mainHorLayout->addWidget(placeHolderLabel);
    mainHorLayout->addWidget(selectedLabel);
    mainHorLayout->addWidget(nameLabel);
    mainHorLayout->addStretch(1);
    mainHorLayout->addLayout(iconHorLayout);
    mainHorLayout->addStretch(2);


    setLayout(mainHorLayout);

}

ThemeWidget::~ThemeWidget()
{
}

void ThemeWidget::setSelectedStatus(bool status){
    placeHolderLabel->setHidden(status);
    selectedLabel->setVisible(status);
}

void ThemeWidget::setValue(QString value){
    pValue = value;
}

QString ThemeWidget::getValue(){
    return pValue;
}

void ThemeWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}

//子类化一个QWidget，为了能够使用样式表，则需要提供paintEvent事件。
//这是因为QWidget的paintEvent()是空的，而样式表要通过paint被绘制到窗口中。
void ThemeWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.save();
    p.setBrush(opt.palette.color(QPalette::Button));
    p.setPen(Qt::transparent);
    p.setOpacity(0.6);
    p.drawRoundedRect(this->rect(), 6, 6);
    p.restore();
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

