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
#include "itemlist.h"

ItemList::ItemList(QListWidget *parent,int itemssize) : QListWidget(parent) {
    this->parent();
    m_cItemCnt = m_szItemNameList.size();
    for(int cur_ptr = 0; cur_ptr < m_cItemCnt; cur_ptr ++) {
        m_itemWidget[cur_ptr] = new FrameItem(this);
        m_listwidgetItem[cur_ptr] = new QListWidgetItem(this);
        m_listwidgetItem[cur_ptr]->setSizeHint(QSize(200, 50));
        m_listwidgetItem[cur_ptr]->setFlags(m_listwidgetItem[cur_ptr]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        m_itemWidget[cur_ptr]->set_itemname(m_szItemNameList[cur_ptr]);
        m_itemWidget[cur_ptr]->get_swbtn()->set_id(cur_ptr);
        this->addItem(m_listwidgetItem[cur_ptr]);
        setItemWidget(m_listwidgetItem[cur_ptr], m_itemWidget[cur_ptr]->get_widget());
    }
    setFrameShape(QListWidget::NoFrame);
    //customize the script on/off area
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //decorate the widget
    this->setSpacing(1);
    adjustSize();
}

/* 获取列表物品，如麒麟天气、壁纸等选单列表
 * Get a item of list, for example: Getting Kylin-Weather, Wallpaper etc.. */
FrameItem*  ItemList::get_item(int cur) {
    return m_itemWidget[cur];
}

/* 添加物品选单，方便以后扩展 *
 * Add item for list to make convienience for future application */
void ItemList::add_item(QString item_name) {
    m_cItemCnt = m_cItemCnt + 1;
    m_itemWidget[m_cItemCnt - 1] = new FrameItem(this);
    m_listwidgetItem[m_cItemCnt - 1] = new QListWidgetItem(this);
    m_listwidgetItem[m_cItemCnt - 1]->setSizeHint(QSize(200, 50));
    m_listwidgetItem[m_cItemCnt - 1]->setFlags(m_listwidgetItem[m_cItemCnt - 1]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    m_itemWidget[m_cItemCnt - 1]->set_itemname(item_name);
    this->addItem(m_listwidgetItem[m_cItemCnt - 1]);
    setItemWidget(m_listwidgetItem[m_cItemCnt - 1], m_itemWidget[m_cItemCnt - 1]->get_widget());
}

/* 读取列表
 * Read the list */
QStringList ItemList::get_list() {
    return m_szItemNameList;
}
