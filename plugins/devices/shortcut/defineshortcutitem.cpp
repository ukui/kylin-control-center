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
#include "defineshortcutitem.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>

#include "customlineedit.h"

DefineShortcutItem::DefineShortcutItem(QString name, QString binding)
{
//    setAttribute(Qt::WA_DeleteOnClose);

    _deleteable = false;
    _updateable = false;

    QHBoxLayout * baseHorLayout = new QHBoxLayout(this);
    baseHorLayout->setSpacing(16);
    baseHorLayout->setMargin(0);

    pWidget = new QWidget(this);

    QHBoxLayout * mainHorLayout = new QHBoxLayout(pWidget);
    mainHorLayout->setSpacing(0);
    mainHorLayout->setContentsMargins(16, 0, 24, 0);

    pWidget->setLayout(mainHorLayout);

    pLabel = new QLabel(pWidget);
    pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pLabel->setText(name);

    pLineEdit = new CustomLineEdit(binding, pWidget);
//    pLineEdit->setStyleSheet("QLineEdit{border: none;}");
    pLineEdit->setAlignment(Qt::AlignRight);
    pLineEdit->setText(binding);
    pLineEdit->setReadOnly(true);
    pLineEdit->setFixedWidth(200);

    pButton = new QPushButton(this);
    pButton->setText(tr("Delete"));
    pButton->setFixedWidth(64);
    pButton->hide();

    QSizePolicy btnSizePolicy = pButton->sizePolicy();
    btnSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    pButton->setSizePolicy(btnSizePolicy);

    mainHorLayout->addWidget(pLabel);
    mainHorLayout->addStretch();
    mainHorLayout->addWidget(pLineEdit);

    baseHorLayout->addWidget(pWidget);
    baseHorLayout->addWidget(pButton);

    setLayout(baseHorLayout);
}

DefineShortcutItem::~DefineShortcutItem()
{
}

QWidget * DefineShortcutItem::widgetComponent(){
    return pWidget;
}

QLabel * DefineShortcutItem::labelComponent(){
    return pLabel;
}

CustomLineEdit * DefineShortcutItem::lineeditComponent(){
    return pLineEdit;
}

QPushButton * DefineShortcutItem::btnComponent(){
    return pButton;
}

void DefineShortcutItem::setDeleteable(bool deleteable){
    _deleteable = deleteable;
}

void DefineShortcutItem::setUpdateable(bool updateable){
    _updateable = updateable;
}

void DefineShortcutItem::setShortcutName(QString newName){
    pLabel->setText(newName);
}

void DefineShortcutItem::setShortcutBinding(QString newBinding){
    pLineEdit->setText(newBinding);
    pLineEdit->updateOldShow(newBinding);
}

void DefineShortcutItem::mousePressEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton && _deleteable){
        pButton->show();
    }

    QWidget::mousePressEvent(e);
}

void DefineShortcutItem::mouseDoubleClickEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton && _updateable){
        emit updateShortcutSignal();
    }
    QWidget::mouseDoubleClickEvent(e);
}
