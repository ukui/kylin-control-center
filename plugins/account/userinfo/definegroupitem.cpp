/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2020 KYLINOS Information Technology Co., Ltd.
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

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>

#include "definegroupitem.h"

DefineGroupItem::DefineGroupItem(QString groupName)
{
//    setAttribute(Qt::WA_DeleteOnClose);

    _deleteable = false;
    _updateable = false;
    _editable   = false;

    QHBoxLayout * baseHorLayout = new QHBoxLayout(this);
    baseHorLayout->setSpacing(16);
    baseHorLayout->setMargin(0);

    pWidget = new QWidget(this);

    QHBoxLayout * mainHorLayout = new QHBoxLayout(pWidget);
    mainHorLayout->setSpacing(0);
    //mainHorLayout->setContentsMargins(16, 0, 16, 0);

    pWidget->setLayout(mainHorLayout);

    pLabel = new QLabel(pWidget);
    pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pLabel->setText(groupName);

    pEditBtn = new QPushButton(this);
    pEditBtn->setText(tr("Edit"));
    pEditBtn->setFixedSize(60,36);
    pEditBtn->hide();

    QSizePolicy btnSizePolicy = pEditBtn->sizePolicy();
    btnSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    pEditBtn->setSizePolicy(btnSizePolicy);

    pDelBtn = new QPushButton(this);
    pDelBtn->setText(tr("Delete"));
    pDelBtn->setFixedSize(60,36);
    pDelBtn->hide();

    QSizePolicy btnSizePolicy2 = pDelBtn->sizePolicy();
    btnSizePolicy2.setVerticalPolicy(QSizePolicy::Expanding);
    pDelBtn->setSizePolicy(btnSizePolicy2);

    QPalette palette = pEditBtn->palette();
    QColor ColorPlaceholderText(255,255,255,0);
    QBrush brush;
    brush.setColor(ColorPlaceholderText);
    palette.setBrush(QPalette::Button, Qt::white);
    //palette.setBrush(QPalette::ButtonText, brush);
    pEditBtn->setPalette(palette);
    pDelBtn->setPalette(palette);

    mainHorLayout->addWidget(pLabel);

    baseHorLayout->addWidget(pWidget);
    baseHorLayout->addWidget(pEditBtn);
    baseHorLayout->addWidget(pDelBtn);
    baseHorLayout->addSpacing(16);

    setLayout(baseHorLayout);
}

DefineGroupItem::~DefineGroupItem()
{
}

QWidget * DefineGroupItem::widgetComponent(){
    return pWidget;
}

QLabel * DefineGroupItem::labelComponent(){
    return pLabel;
}

QPushButton * DefineGroupItem::editBtnComponent(){
    return pEditBtn;
}

QPushButton * DefineGroupItem::delBtnComponent(){
    return pDelBtn;
}

void DefineGroupItem::setDeleteable(bool deleteable){
    _deleteable = deleteable;
}

void DefineGroupItem::setUpdateable(bool updateable){
    _updateable = updateable;
}

void DefineGroupItem::setEditable(bool editable){
    _editable = editable;
}

void DefineGroupItem::setShortcutName(QString newName){
    pLabel->setText(newName);
}

void DefineGroupItem::mousePressEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton && _deleteable && _editable){
        pEditBtn->show();
        pDelBtn->show();
    }

    QWidget::mousePressEvent(e);
}

void DefineGroupItem::mouseDoubleClickEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton && _updateable){
        emit updateShortcutSignal();
    }
    QWidget::mouseDoubleClickEvent(e);
}
