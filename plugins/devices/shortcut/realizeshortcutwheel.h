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
#ifndef REALIZESHORTCUTWHEEL_H
#define REALIZESHORTCUTWHEEL_H

#include <QGSettings>
#include <QList>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

#include <glib.h>
#include <gio/gio.h>
#include <dconf/dconf.h>

#define KEYBINDINGS_DESKTOP_SCHEMA "org.ukui.SettingsDaemon.plugins.media-keys"
#define KEYBINDINGS_SYSTEM_SCHEMA "org.gnome.desktop.wm.keybindings"

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.control-center.keybinding"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/desktop/keybindings/"

#define MAX_SHORTCUTS 1000

#define ACTION_KEY "action"
#define BINDING_KEY "binding"
#define NAME_KEY "name"
#define DEFAULT_BINDING "disable"

#define MAX_CUSTOM_SHORTCUTS 1000

QList<char *> listExistsCustomShortcutPath();

QString findFreePath();

#endif // REALIZESHORTCUTWHEEL_H
