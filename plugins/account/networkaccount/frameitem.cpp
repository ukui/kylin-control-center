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

FrameItem::FrameItem(QWidget *parent) :QFrame(parent)
{
    this->parent();
    setMaximumSize(1080,50);
    setMinimumSize(0,50);

    this->setFrameShape(QFrame::Shape::Box);
    m_itemName = new QLabel(this);
    m_itemName->setStyleSheet("font-size: 14px;");
    m_switchBtn = new SwitchButton(this);
    m_workLayout = new QHBoxLayout;
    m_workLayout->addWidget(m_itemName);
    m_hboxLayout = new QHBoxLayout;
    m_run = new QLabel(this);
    m_errorStatusLabel = new InfoLabel(this);
    m_cTimer = new QTimer(this);
    m_svgHandler = new SVGHandler(this);
    m_stackedWidget = new QStackedWidget(this);
    m_nullWidget = new QWidget(this);

    m_stackedWidget->addWidget(m_run);
    m_stackedWidget->addWidget(m_errorStatusLabel);
    m_stackedWidget->addWidget(m_nullWidget);


    m_nullWidget->setStyleSheet("background:transparent;");

    m_stackedWidget->setCurrentWidget(m_nullWidget);

    m_cTimer->stop();

    m_workLayout->addStretch();
    m_hboxLayout->setMargin(0);
    m_hboxLayout->setSpacing(16);
    m_hboxLayout->addWidget(m_stackedWidget,0,Qt::AlignRight);
    m_hboxLayout->addWidget(m_switchBtn,0,Qt::AlignRight);
    m_workLayout->addLayout(m_hboxLayout);
    m_workLayout->setMargin(16);
    this->setAttribute(Qt::WA_StyledBackground,true);
    //widget->setStyleSheet("background-color: rgba(244,244,244,85%);border-radius:4px;");
    this->setLayout(m_workLayout);

    m_stackedWidget->adjustSize();


    connect(m_cTimer,&QTimer::timeout, [this] () {
        QPixmap pixmap = m_svgHandler->loadSvgColor(QString(":/new/image/loading1%1.svg").arg(m_cCnt),"black",16);
        m_run->setPixmap(pixmap);
        m_cCnt = (m_cCnt + 9) % 8;
    });
}

/* 获取子部件SwicthButton */ 
SwitchButton* FrameItem::get_swbtn() {
    return m_switchBtn;
}

/* 获取背景部件 */
QWidget* FrameItem::get_widget() {
    return this;
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

void FrameItem::set_change(int status,QString code) {
    if(status == 1) {
        m_stackedWidget->setCurrentWidget(m_run);
        bIsStart = true;
        m_cTimer->start(140);
    } else if (status == 0) {
        m_cTimer->stop();
        bIsStart = false;
        m_stackedWidget->setCurrentWidget(m_nullWidget);
    } else {
        m_cTimer->stop();
        bIsStart = false;
        if(code == "Failed!") {
            m_errorStatusLabel->setTipText(tr("Sync failed, please login out to retry!"));
        }
        if(code == "Change conf file failed!") {
            m_errorStatusLabel->setTipText(tr("Change configuration file failed, please login out to retry!"));
        }
        if(code == "Config file not exist!") {
            m_errorStatusLabel->setTipText(tr("Configuration file not exist, please login out to retry!"));
        }
        if(code == "Cloud verifyed file download failed!") {
            m_errorStatusLabel->setTipText(tr("Cloud verifyed file download failed, please login out to retry!"));
        }
        if(code == "OSS access failed!") {
            m_errorStatusLabel->setTipText(tr("OSS access failed, please login out to retry!"));
        }
        else if(code != "Upload" && code != "Download") {
            m_errorStatusLabel->setTipText(tr("Sync failed, please retry or login out to get a better experience!"));
        }
        m_stackedWidget->setCurrentWidget(m_errorStatusLabel);
    }
    m_stackedWidget->adjustSize();
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

FrameItem::~FrameItem() {
    if(m_cTimer->isActive())
        m_cTimer->stop();
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
