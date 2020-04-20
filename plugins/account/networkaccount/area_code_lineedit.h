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
#ifndef AREA_CODE_LINEEDIT_H
#define AREA_CODE_LINEEDIT_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QListView>
#include "ql_combobobx.h"

class area_code_lineedit : public QLineEdit
{
    Q_OBJECT
public:
    explicit    area_code_lineedit(QWidget *parent = nullptr);
    void        InittoCountrycode();
private:
    ql_combobobx *combobox;
    QJsonArray json_code;
    QJsonArray json_array;
    QJsonDocument *json_file;
    QFrame *vertical_line;
    QFile *loadfile;
    QMap<QString,QPair<QString,QString>>country_code;
    QStandardItemModel *model;
    QHBoxLayout *layout;
signals:

};

#endif // AREA_CODE_LINEEDIT_H
