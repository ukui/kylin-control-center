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
#ifndef POWERMACRODATA_H
#define POWERMACRODATA_H

#define POWERMANAGER_SCHEMA     "org.ukui.power-manager"
#define ICONPOLICY              "icon-policy"
#define SLEEP_COMPUTER_AC_KEY   "sleep-computer-ac"
#define SLEEP_COMPUTER_BATT_KEY "sleep-computer-battery"
#define SLEEP_DISPLAY_AC_KEY    "sleep-display-ac"
#define SLEEP_DISPLAY_BATT_KEY  "sleep-display-battery"
#define BUTTON_LID_AC_KEY       "button-lid-ac"
#define BUTTON_LID_BATT_KEY     "button-lid-battery"
#define BUTTON_SUSPEND_KEY      "button-suspend"
#define BUTTON_POWER_KEY        "button-power"
#define IDLE_DIM_TIME_KEY       "idle-dim-time"
#define PER_ACTION_KEY          "percentage-action"
#define ACTION_CRI_BTY          "action-critical-battery"
#define PER_ACTION_CRI          "percentage-critical"
#define POWER_POLICY_KEY        "power-policy-current"

#define PRESENT_VALUE           "present"
#define ALWAYS_VALUE            "always"
#define CHARGE_VALUE            "charge"

#define SESSION_SCHEMA          "org.ukui.session"
#define IDLE_DELAY_KEY          "idle-delay"

#define FIXES 60

#define PERSONALSIE_SCHEMA     "org.ukui.control-center.personalise"
#define PERSONALSIE_POWER_KEY  "custompower"

#define STYLE_FONT_SCHEMA  "org.ukui.style"
#endif // POWERMACRODATA_H
