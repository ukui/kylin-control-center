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

ComboBox::ComboBox(QWidget *parent) : QWidget(parent)
{
    //Allocate the memory
    m_mainLineEdit = new QLineEdit(this);
    m_pushButton = new QPushButton(this);
    m_popupWidget = new PopupWidget(this);
    m_listWidget = new QListWidget(this);
    m_lineeditLayout = new QHBoxLayout;
    m_popupLayout = new QVBoxLayout;
    m_comboboxLayout = new QVBoxLayout;
    m_svgHandler = new SVGHandler(this);

    //Size
    m_mainLineEdit->setFixedSize(66,34);
    m_pushButton->setFixedSize(14,14);
    m_listWidget->setMinimumSize(188,36);
    m_listWidget->resize(188,36);
    m_popupWidget->setFixedSize(216,196);
    setMaximumSize(66,34);
    resize(66,34);
    idx = 0;
    setContentsMargins(0,0,0,0);

    //Configuration
    m_listWidget->setContentsMargins(0,0,0,0);
    m_mainLineEdit->setTextMargins(16,0,0,0);
    m_mainLineEdit->setFocusPolicy(Qt::NoFocus);
    m_pushButton->setFocusPolicy(Qt::NoFocus);

    //Style
    //lineedit->setStyleSheet("QLineEdit{border:none;}");
    QPixmap pixmap = m_svgHandler->loadSvg(":/new/image/arrow_down.svg");
    m_pushButton->setIcon(pixmap);
    m_pushButton->setStyleSheet("QPushButton{"
                              "background-repeat:no-repeat;background-position :center;background-color:transparent;border:none}");
    m_listWidget->setStyleSheet("QListView{border:1px solid #CFCFCF;border-color:#F4F4F4;border-radius:4px;}"
                              "QListView::item{background: #FFF;border-radius:4px;}"
                              "QListView::item:selected{background: #ffffffff;}"
                              "QListView::item:hover {background: #3D6BE5;}");

    //Layout
    m_lineeditLayout->setMargin(0);
    m_lineeditLayout->setSpacing(0);
    m_lineeditLayout->addWidget(m_pushButton,0,Qt::AlignRight);
    m_mainLineEdit->setLayout(m_lineeditLayout);

    m_popupLayout->setMargin(0);
    m_popupLayout->setSpacing(0);
    m_popupLayout->addWidget(m_listWidget);
    m_popupWidget->setLayout(m_popupLayout);
    m_popupWidget->setContentsMargins(6,6,6,6);
    m_popupWidget->hide();

    m_comboboxLayout->setMargin(0);
    m_comboboxLayout->setSpacing(0);
    m_comboboxLayout->addWidget(m_mainLineEdit);
    m_comboboxLayout->addWidget(m_popupWidget);
    m_comboboxLayout->setAlignment(Qt::AlignLeft);
    setLayout(m_comboboxLayout);
    m_mainLineEdit->setText("+86");
    m_pushButton->setEnabled(false);

    //Connect slots
    connect(m_listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(closepopup(QListWidgetItem *)));
    connect(m_pushButton, SIGNAL(clicked()),this,SLOT(showpopup()));
}

/* 显示下拉框 */
void ComboBox::showpopup() {
    if (m_popupWidget->isHidden()) {
        QPoint pos;
        pos.setX(this->mapToGlobal(QPoint(0, 0)).x() - 6);
        pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + this->height());
        m_popupWidget->move(pos);
        m_popupWidget->show();
    } else {
        m_popupWidget->hide();
    }
}

/* 关闭下拉框，并记录鼠标点击的位置相对应的国家代码输入到代码框 */
void ComboBox::closepopup(QListWidgetItem *item) {
    m_popupWidget->hide();
    QWidget *wgt = m_listWidget->itemWidget(item);
    if (wgt != NULL) {
        QLabel *code = wgt->findChild<QLabel *>("code");
        m_mainLineEdit->setText(code->text());
    }
}

/* 增加新的国家以及代码，不常用 */
void ComboBox::addItem(const QString &country, const QString &code) {
    QListWidgetItem* widgetItem = new QListWidgetItem(m_listWidget);
    BoxItem *wgt = new BoxItem(this);

    wgt->set_code(country);
    wgt->set_country_code(code);
    m_listWidget->insertItem(idx ++,widgetItem);
    widgetItem->setSizeHint(QSize(188,36));
    widgetItem->setHidden(false);
    m_listWidget->setItemWidget(widgetItem, wgt);
}


