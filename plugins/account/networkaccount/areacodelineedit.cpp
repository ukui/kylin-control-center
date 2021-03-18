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
#include "areacodelineedit.h"


/* 读取国家代码JSon文件并写入ComboBox */
void AreaCodeLineEdit::InittoCountrycode() {
    m_loadFile = new QFile(":/country.json",this);
    if (!m_loadFile->open(QIODevice::ReadOnly)) {
        //qDebug() <<"Open fail!";
        return ;
    }
    QByteArray Data = m_loadFile->readAll();
    m_loadFile->close();
    QJsonParseError json_error;
    m_jsonFile = new QJsonDocument(QJsonDocument::fromJson(Data,&json_error));

    if (json_error.error != QJsonParseError::NoError) {
        //qDebug() << "Json Error!";
        return ;
    }

    m_jsonCode = m_jsonFile->array();
    for(int itempos = 0;itempos < m_jsonCode.count();itempos ++) {
        QJsonObject json = m_jsonCode.at(itempos).toObject();
        QJsonArray jar = json.value("items").toArray();
        for(int country = 0;country < jar.count();country ++) {
            QString c_code = jar.at(country).toObject().value("label").toString();
            QString code = jar.at(country).toObject().value("code").toString();
            QString c_en = jar.at(country).toObject().value("label_en").toString();
            m_countryCode.insert(c_en,QPair<QString,QString>(c_code,code));
        }
    }
}

AreaCodeLineEdit::AreaCodeLineEdit(QWidget *parent) : QLineEdit(parent)
{
    //Allocate the memories
    m_countryComboBox = new ComboBox(this);
    m_verticalLine = new QFrame(this);
    m_hboxLayout = new QHBoxLayout;

    //Resize
    resize(338,36);
    setMinimumSize(338,36);
    m_verticalLine->setParent(this);
    setMaximumSize(338,36);
   // setMaximumSize(82,36);
    m_verticalLine->setMaximumSize(2,14);
    m_verticalLine->setMinimumSize(2,14);
    m_verticalLine->resize(2,14);
    setTextMargins(98,0,0,0);
    //Configuration
    InittoCountrycode();
    QMapIterator<QString,QPair<QString,QString>> Iter(m_countryCode);
    while(Iter.hasNext())
    {
        Iter.next();
        m_countryComboBox->addItem(Iter.value().second,Iter.value().first);
    }
    m_verticalLine->setFrameShape(QFrame::VLine);
    m_verticalLine->setFrameShadow(QFrame::Plain);
    m_verticalLine->setLineWidth(1);
    m_verticalLine->setFixedHeight(14);
    m_verticalLine->setStyleSheet("color:#CCCCCC");
    setPlaceholderText(tr("Sign up by Phone"));

    //Set Layout

    m_hboxLayout->setContentsMargins(2,0,0,0);
    m_hboxLayout->addWidget(m_countryComboBox);
    m_hboxLayout->addSpacing(8);
    m_hboxLayout->addWidget(m_verticalLine);
    m_hboxLayout->setAlignment(Qt::AlignLeft);
    setLayout(m_hboxLayout);
    setContentsMargins(0,0,0,0);

}

AreaCodeLineEdit::~AreaCodeLineEdit() {
    delete m_jsonFile;
    m_jsonFile = nullptr;
}
