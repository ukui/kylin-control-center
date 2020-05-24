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
#include "area_code_lineedit.h"


/* 读取国家代码JSon文件并写入ComboBox */
void area_code_lineedit::InittoCountrycode() {
    loadfile = new QFile(":/country.json",this);
    if(!loadfile->open(QIODevice::ReadOnly)) {
        qDebug() <<"Open fail!";
        return ;
    }
    QByteArray Data = loadfile->readAll();
    loadfile->close();
    QJsonParseError json_error;
    json_file = new QJsonDocument(QJsonDocument::fromJson(Data,&json_error));

    if(json_error.error != QJsonParseError::NoError) {
        qDebug() << "Json Error!";
        return ;
    }

    json_code = json_file->array();
    for(int itempos = 0;itempos < json_code.count();itempos ++) {
        QJsonObject json = json_code.at(itempos).toObject();
        QJsonArray jar = json.value("items").toArray();
        for(int country = 0;country < jar.count();country ++) {
            QString c_code = jar.at(country).toObject().value("label").toString();
            QString code = jar.at(country).toObject().value("code").toString();
            QString c_en = jar.at(country).toObject().value("label_en").toString();
            country_code.insert(c_en,QPair<QString,QString>(c_code,code));
        }
    }
}

area_code_lineedit::area_code_lineedit(QWidget *parent) : QLineEdit(parent)
{
    //Allocate the memories
    combobox = new ql_combobobx(this);
    vertical_line = new QFrame(this);
    layout = new QHBoxLayout;

    //Resize
    resize(338,36);
    setMinimumSize(338,36);
    vertical_line->setParent(this);
    setMaximumSize(338,36);
   // setMaximumSize(82,36);
    vertical_line->setMaximumSize(2,14);
    vertical_line->setMinimumSize(2,14);
    vertical_line->resize(2,14);
    setTextMargins(98,0,0,0);
    //Configuration
    InittoCountrycode();
    QMapIterator<QString,QPair<QString,QString>> Iter(country_code);
    while(Iter.hasNext())
    {
        Iter.next();
        combobox->addItem(Iter.value().second,Iter.value().first);
    }
    vertical_line->setFrameShape(QFrame::VLine);
    vertical_line->setFrameShadow(QFrame::Plain);
    vertical_line->setLineWidth(1);
    vertical_line->setFixedHeight(14);
    vertical_line->setStyleSheet("color:#CCCCCC");
    setPlaceholderText(tr("Sign up by Phone"));

    //Set Layout

    layout->setContentsMargins(2,0,0,0);
    layout->addWidget(combobox);
    layout->addSpacing(8);
    layout->addWidget(vertical_line);
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);
    setContentsMargins(0,0,0,0);

}
