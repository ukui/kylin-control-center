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
#ifndef XMLHANDLE_H
#define XMLHANDLE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDir>
#include <QString>
#include <QMap>

#define WALLPAPERDIR "/usr/share/ukui-background-properties"

class XmlHandle{

public:
    XmlHandle();
    ~XmlHandle();

public:
    void init();
    void xmlreader(QString filename);
    void xmlUpdate(QMap<QString, QMap<QString, QString>> wallpaperinfosMap);
    QMap<QString, QMap<QString, QString> > requireXmlData();

public:
    QString localconf;

private:
    QDir xmlDir;

private:
    QStringList _getXmlFiles(QString path);
    void _parseWallpaper(QXmlStreamReader &reader);
    void _xmlGenerate();

    QMap<QString, QMap<QString, QString>> wallpapersMap;

};

#endif // XMLHANDLE_H
