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
#ifndef REALIZENOTICE_H
#define REALIZENOTICE_H


#include <QGSettings>
#include <QList>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <dconf/dconf.h>
}

#define NOTICE_SCHEMA           "org.ukui.control-center.notice"
#define NEW_FEATURE_KEY         "show-new-feature"
#define ENABLE_NOTICE_KEY       "enable-notice"
#define SHOWON_LOCKSCREEN_KEY   "show-on-lockscreen"

#define NOTICE_ORIGIN_SCHEMA    "org.ukui.control-center.noticeorigin"
#define NOTICE_ORIGIN_PATH      "/org/ukui/control-center/noticeorigin/"

#define MAX_SHORTCUTS           1000

#define MESSAGES_KEY            "messages"
#define VOICE_KEY               "voice"
#define MAXIMINE_KEY            "maximize"
#define NAME_KEY_US             "name-us"
#define NAME_KEY_CN             "name-cn"


#define MAX_CUSTOM_SHORTCUTS 1000

QList<char *> listExistsCustomNoticePath();

QString findFreePath();


#endif // REALIZENOTICE_H
