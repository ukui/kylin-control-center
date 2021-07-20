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
#include "keyvalueconverter.h"

KeyValueConverter::KeyValueConverter()
{
    metaModule = QMetaEnum::fromType<KeyValueConverter::FunType>();
}

KeyValueConverter::~KeyValueConverter()
{
}

QString KeyValueConverter::keycodeTokeystring(int code){
    //未匹配到则返回空
    return metaModule.valueToKey(code);
}

int KeyValueConverter::keystringTokeycode(QString string){
    //QString to const char *
    QByteArray ba = string.toUpper().toLocal8Bit(); const char * str = ba.data();
    return metaModule.keyToValue(str);
}

QString KeyValueConverter::keycodeTokeyi18nstring(int code){
    QString nameString;
    switch (code) {
    case SYSTEM:
        nameString = tr("System");
        break;
    case DEVICES:
        nameString = tr("Devices");
        break;
    case NETWORK:
        nameString = tr("Network");
        break;
    case PERSONALIZED:
        nameString = tr("Personalized");
        break;
    case ACCOUNT:
        nameString = tr("Account");
        break;
    case DATETIME:
        nameString = tr("Datetime");
        break;
    case UPDATE:
        nameString = tr("Update");
        break;
    case SECURITY:
        nameString = tr("Security");
        break;
    case APPLICATION:
        nameString = tr("Application");
        break;
    case SEARCH_F:
        nameString = tr("Search");
        break;
    default:
        break;
    }
    return nameString;
}
