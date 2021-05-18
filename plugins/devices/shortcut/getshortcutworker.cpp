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
#include "getshortcutworker.h"

#include <QDebug>

#include "realizeshortcutwheel.h"

GetShortcutWorker::GetShortcutWorker()
{
}

GetShortcutWorker::~GetShortcutWorker()
{
}

void GetShortcutWorker::run()
{
    // list system shortcut
    QByteArray id(KEYBINDINGS_SYSTEM_SCHEMA);
    GSettings *systemgsettings;
    if (QGSettings::isSchemaInstalled(id)) {
        systemgsettings = g_settings_new(KEYBINDINGS_SYSTEM_SCHEMA);
    } else {
        return;
    }

    char **skeys = g_settings_list_keys(systemgsettings);
    for (int i = 0; skeys[i] != NULL; i++) {
        // 切换为mutter后，原先为string的变为字符串数组，这块只取了字符串数组的第一个元素
        GVariant *variant = g_settings_get_value(systemgsettings, skeys[i]);
        gsize size = g_variant_get_size(variant);
        char **tmp = const_cast<char **>(g_variant_get_strv(variant, &size));
        char *str = tmp[0];

        // 保存系统快捷键
        QString key = QString(skeys[i]);
        QString value = QString(str);
        if (value != "") {
            generalShortcutGenerate(KEYBINDINGS_SYSTEM_SCHEMA, key, value);
        }
    }
    g_strfreev(skeys);
    g_object_unref(systemgsettings);

    // list desktop shortcut
    GSettings *desktopsettings = NULL;
    if (QGSettings::isSchemaInstalled(KEYBINDINGS_DESKTOP_SCHEMA)) {
        desktopsettings = g_settings_new(KEYBINDINGS_DESKTOP_SCHEMA);
        char **dkeys = g_settings_list_keys(desktopsettings);
        for (int i = 0; dkeys[i] != NULL; i++) {
            // 跳过非快捷键
            if (!g_strcmp0(dkeys[i], "active") || !g_strcmp0(dkeys[i], "volume-step")
                || !g_strcmp0(dkeys[i], "priority") || !g_strcmp0(dkeys[i], "enable-osd"))
                continue;

            GVariant *variant = g_settings_get_value(desktopsettings, dkeys[i]);
            gsize size = g_variant_get_size(variant);
            char *str = const_cast<char *>(g_variant_get_string(variant, &size));

            // 保存桌面快捷键
            QString key = QString(dkeys[i]);
            QString value = QString(str);
            if (value.contains("KP_Delete")) {
                value = "<Ctrl><Alt>Del";
                generalShortcutGenerate(KEYBINDINGS_DESKTOP_SCHEMA, key, value);
            }
            if (value != "" && !value.contains("XF86") && !value.contains("KP_")) {
                generalShortcutGenerate(KEYBINDINGS_DESKTOP_SCHEMA, key, value);
            }
        }
        g_strfreev(dkeys);
        g_object_unref(desktopsettings);
    }

    // list custdom shortcut
    QList<char *> existsPath = listExistsCustomShortcutPath();

    for (char *path : existsPath) {
        QString strFullPath = QString(KEYBINDINGS_CUSTOM_DIR);
        strFullPath.append(path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(strFullPath.toLatin1().data());
        QGSettings *settings = new QGSettings(ba, bba, this);

        QString pathStr = strFullPath;
        QString actionStr = settings->get(ACTION_KEY).toString();
        QString bindingStr = settings->get(BINDING_KEY).toString();
        QString nameStr = settings->get(NAME_KEY).toString();

        customShortcutGenerate(pathStr, nameStr, bindingStr, actionStr);
    }

    emit workerComplete();
}
