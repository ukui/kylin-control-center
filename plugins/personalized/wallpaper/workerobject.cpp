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
#include "workerobject.h"

#include "xmlhandle.h"
#include <QMetaType>
#include <QDebug>

WorkerObject::WorkerObject()
{
    //自定义结构提，线程传递时无法放入列表，注册解决
    qRegisterMetaType<QMap<QString, QMap<QString, QString>>>("QMap<QString, QMap<QString, QString>>");
}

WorkerObject::~WorkerObject()
{
    delete xmlHandleObj;
}

void WorkerObject::run(){
    //构建xmlhandle对象
    xmlHandleObj = new XmlHandle();

    //解析壁纸数据，如果本地xml文件不存在则自动构建
    xmlHandleObj->init();

    //获取壁纸数据
    wallpaperinfosMap = xmlHandleObj->requireXmlData();

    //压缩壁纸
    QSize IMAGE_SIZE(166, 110);
    QMap<QString, QMap<QString, QString> >::iterator iters = wallpaperinfosMap.begin();
    for (int row = 0; iters != wallpaperinfosMap.end(); iters++, row++){
        //跳过xml的头部信息
        if (QString(iters.key()) == "head")
            continue;

        //跳过被删除的壁纸
        QMap<QString, QString> wpMap = (QMap<QString, QString>)iters.value();
        QString delstatus = QString(wpMap.find("deleted").value());
        if (delstatus == "true")
            continue;

        QString filename = QString(iters.key());
        QPixmap pixmap = QPixmap(filename)/*.scaled(IMAGE_SIZE)*/;

        emit pixmapGenerate(pixmap, filename);
    }

    emit workComplete(wallpaperinfosMap);

}
