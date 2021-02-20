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
#include "combobox.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>

#include <QDebug>


ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent)
{
    this->setStyleSheet("QComboBox{border: 1px solid #cccccc; padding: 1px 2px 1px 2px; background-color: #eeeeee;}"
                        "QComboBox QAbstractItemView::item{height: 30px}"
                        "QListView::item{background: white}"
                        "QListView::item:hover{background: #BDD7FD}");

    partListWidget = new QListWidget(this);

    partListWidget->setItemDelegate(new NoFocusFrameDelegate(this));
    partListWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width: 10px}");
    partListWidget->setFixedHeight(15 * 30);

    this->setModel(partListWidget->model());
    this->setView(partListWidget);
    this->setEditable(true);
}

ComboBox::~ComboBox()
{
}

void ComboBox::onChooseItem(QString text){
    this->setEditText(text);
    this->hidePopup();
}

void ComboBox::addwidgetItem(QString text){

    ComboboxItem * item = new ComboboxItem(this);
    item->setLabelContent(text);
    connect(item, SIGNAL(chooseItem(QString)), this, SLOT(onChooseItem(QString)));
    QListWidgetItem * widgetItem = new QListWidgetItem(partListWidget);
    partListWidget->setItemWidget(widgetItem, item);
}

void ComboBox::setcurrentwidgetIndex(int index){
    QListWidgetItem * item = partListWidget->takeItem(index);
    this->setEditText(item->text());
}

void ComboBox::removewidgetItems(){
    for (int i = partListWidget->count() - 1; i >= 0; i--){
        QListWidgetItem * item = partListWidget->item(i);
        partListWidget->removeItemWidget(item);
        delete item;
        item = nullptr;
    }
}

