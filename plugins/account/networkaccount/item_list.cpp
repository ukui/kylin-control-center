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
#include "item_list.h"

item_list::item_list(QListWidget *parent,int itemssize) : QListWidget(parent) {
    this->parent();
    itemsize = varmap.size();
    for(int cur_ptr = 0; cur_ptr < itemsize; cur_ptr ++) {
        itempack[cur_ptr] = new network_item(this);
        items[cur_ptr] = new QListWidgetItem(this);
        items[cur_ptr]->setSizeHint(QSize(200, 50));
        items[cur_ptr]->setFlags(items[cur_ptr]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        itempack[cur_ptr]->set_itemname(varmap[cur_ptr]);
        itempack[cur_ptr]->get_swbtn()->set_id(cur_ptr);
        QSize size = items[cur_ptr]->sizeHint();
        this->addItem(items[cur_ptr]);
        itempack[cur_ptr]->get_widget()->setSizeIncrement(size.width(), 56);
        setItemWidget(items[cur_ptr], itempack[cur_ptr]->get_widget());
    }
    setFrameShape(QListWidget::NoFrame);
    //customize the script on/off area
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //decorate the widget
    this->setSpacing(1);
}

network_item*  item_list::get_item(int cur) {
    return itempack[cur];
}

void item_list::add_item(QString item_name) {
    itemsize = itemsize + 1;
    itempack[itemsize - 1] = new network_item(this);
    items[itemsize - 1] = new QListWidgetItem(this);
    items[itemsize - 1]->setSizeHint(QSize(200, 50));
    items[itemsize - 1]->setFlags(items[itemsize - 1]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    itempack[itemsize - 1]->set_itemname(item_name);
    QSize size = items[itemsize - 1]->sizeHint();
    this->addItem(items[itemsize - 1]);
    itempack[itemsize  - 1]->get_widget()->setSizeIncrement(size.width(), 56);
    setItemWidget(items[itemsize - 1], itempack[itemsize - 1]->get_widget());
}


QStringList item_list::get_list() {
    return varmap;
}
