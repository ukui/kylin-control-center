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

ItemList::ItemList(QWidget *parent,int itemssize) : QWidget(parent) {
    this->parent();
    m_vboxLayout = new QVBoxLayout;
    m_cItemCnt = m_szItemNameList.size();
    for(int cur_ptr = 0; cur_ptr < m_cItemCnt; cur_ptr ++) {
        m_itemWidget[cur_ptr] = new FrameItem(this);
        m_itemWidget[cur_ptr]->set_itemname(m_szItemNameList[cur_ptr]);
        m_itemWidget[cur_ptr]->get_swbtn()->set_id(cur_ptr);
        m_vboxLayout->addWidget(m_itemWidget[cur_ptr]);
    }
    //customize the script on/off area
    m_vboxLayout->setSpacing(1);
    m_vboxLayout->setMargin(0);
    m_vboxLayout->addStretch();
    m_vboxLayout->addSpacing(48);
    this->setLayout(m_vboxLayout);
    //decorate the widget
    adjustSize();
}

/* 获取列表物品，如麒麟天气、壁纸等选单列表
 * Get a item of list, for example: Getting Kylin-Weather, Wallpaper etc.. */
FrameItem*  ItemList::get_item(const int &cur) {
    return m_itemWidget[cur];
}

FrameItem* ItemList::get_item_by_name(const QString &name) {
    m_cItemCnt = m_szItemNameList.size();
    for(int cur_ptr = 0; cur_ptr < m_cItemCnt; cur_ptr ++) {
        if (m_itemWidget[cur_ptr]->get_itemname() == name) {
            return m_itemWidget[cur_ptr];
        }
    }
    return 0;
}

/* 读取列表
 * Read the list */
QStringList ItemList::get_list() const {
    return m_szItemNameList;
}
