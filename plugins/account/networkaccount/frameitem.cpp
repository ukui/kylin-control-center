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
#include "frameitem.h"

FrameItem::FrameItem(QWidget *parent)
{
    this->parent();
    setMaximumSize(1080,50);
    setMinimumSize(0,50);
    m_itemFrame = new QFrame(this);
    m_itemFrame->setFrameShape(QFrame::Shape::Box);
    m_itemName = new QLabel(m_itemFrame);
    m_itemName->setStyleSheet("font-size: 14px;");
    m_switchBtn = new SwitchButton(m_itemFrame);
    m_switchBtn->setStyleSheet("margin-right: 16px");
    m_workLayout = new QHBoxLayout;
    m_workLayout->addWidget(m_itemName);
    QHBoxLayout *qhb = new QHBoxLayout;
    qhb->addStretch(2);
    m_workLayout->addLayout(qhb);
    m_workLayout->addWidget(m_switchBtn);
    m_workLayout->setMargin(16);
    m_itemFrame->setAttribute(Qt::WA_StyledBackground,true);
    //widget->setStyleSheet("background-color: rgba(244,244,244,85%);border-radius:4px;");
    m_itemFrame->setLayout(m_workLayout);

}

/* 获取子部件SwicthButton */ 
SwitchButton* FrameItem::get_swbtn() {
    return m_switchBtn;
}

/* 获取背景部件 */
QWidget* FrameItem::get_widget() {
    return m_itemFrame;
}

/* 获取项目名字 */
QString FrameItem::get_itemname() {
    return m_itemName->text();
}

/* 获取布局*/
QHBoxLayout* FrameItem::get_layout() {
    return m_workLayout;
}

/* 设置项目名字 */
void FrameItem::set_itemname(QString name) {
    m_itemName->setText(name);
}

/* 让SwitchButton播放打开动画 */
void FrameItem::make_itemon() {
    if(m_switchBtn != nullptr) {
        if(m_switchBtn->get_swichbutton_val() != 1) {
            m_switchBtn->set_swichbutton_val(1);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}

/* 让SwitchButton播放关闭动画 */
void FrameItem::make_itemoff() {
    if(m_switchBtn != nullptr) {
        if(m_switchBtn->get_swichbutton_val() != 0) {
            m_switchBtn->set_swichbutton_val(0);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}

/* 让列表的SwitchButton可用或者不可用，调用一次就是取反 */
void FrameItem::set_active(bool ok) {
    m_switchBtn->set_active(ok);
}
