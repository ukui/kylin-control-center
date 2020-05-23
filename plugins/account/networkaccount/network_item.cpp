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
#include "network_item.h"

network_item::network_item(QWidget *parent)
{
    this->parent();
    setMaximumSize(1080,50);
    setMinimumSize(0,50);
    widget = new QFrame(this);
    widget->setFrameShape(QFrame::Shape::Box);
    label_item = new QLabel(widget);
    label_item->setStyleSheet("font-size: 14px;");
    switch_btn = new QL_SwichButton(widget);
    switch_btn->setStyleSheet("margin-right: 16px");
    layout = new QHBoxLayout;
    layout->addWidget(label_item);
    QHBoxLayout *qhb = new QHBoxLayout;
    qhb->addStretch(2);
    layout->addLayout(qhb);
    layout->addWidget(switch_btn);
    layout->setMargin(16);
    widget->setAttribute(Qt::WA_StyledBackground,true);
    //widget->setStyleSheet("background-color: rgba(244,244,244,85%);border-radius:4px;");
    widget->setLayout(layout);

}

/* 获取子部件SwicthButton */ 
QL_SwichButton* network_item::get_swbtn() {
    return switch_btn;
}

/* 获取背景部件 */
QWidget* network_item::get_widget() {
    return widget;
}

/* 获取项目名字 */
QString network_item::get_itemname() {
    return label_item->text();
}

/* 获取布局*/
QHBoxLayout* network_item::get_layout() {
    return layout;
}

/* 设置项目名字 */
void network_item::set_itemname(QString name) {
    label_item->setText(name);
}

/* 让SwitchButton播放打开动画 */
void network_item::make_itemon() {
    if(switch_btn != nullptr) {
        if(switch_btn->get_swichbutton_val() != 1) {
            switch_btn->set_swichbutton_val(1);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}

/* 让SwitchButton播放关闭动画 */
void network_item::make_itemoff() {
    if(switch_btn != nullptr) {
        if(switch_btn->get_swichbutton_val() != 0) {
            switch_btn->set_swichbutton_val(0);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}

/* 让列表的SwitchButton可用或者不可用，调用一次就是取反 */
void network_item::set_active(bool ok) {
    switch_btn->set_active(ok);
}
