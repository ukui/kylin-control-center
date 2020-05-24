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
#ifndef FUNCTIONSELECT_H
#define FUNCTIONSELECT_H

#include <QList>
#include <QStack>
//#include <QStringList>
#include <QObject>


typedef struct _FuncInfo
{
    int type;
    int index;
    bool mainShow;
    QString nameString;
    QString namei18nString;
}FuncInfo;

typedef struct _RecordFunc
{
    int type;
    QString namei18nString;

}RecordFunc;

Q_DECLARE_METATYPE(_RecordFunc)
Q_DECLARE_METATYPE(_FuncInfo)

class FunctionSelect
{
public:
    explicit FunctionSelect();
    ~FunctionSelect();

public:
    static QList<QList<FuncInfo>> funcinfoList;
    static QStack<RecordFunc> recordFuncStack;

//    static FuncInfo displayStruct;

    static void initValue();
    static void pushRecordValue(int type, QString name);
    static void popRecordValue();
};

#endif // FUNCTIONSELECT_H
