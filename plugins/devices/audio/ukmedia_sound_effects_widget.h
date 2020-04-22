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
#ifndef UKMEDIASOUNDEFFECTSWIDGET_H
#define UKMEDIASOUNDEFFECTSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include "switchbutton.h"

class UkuiMessageBox : public QMessageBox
{
public:
    explicit UkuiMessageBox();
};

class UkmediaSoundEffectsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaSoundEffectsWidget(QWidget *parent = nullptr);
    ~UkmediaSoundEffectsWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QFrame *m_pThemeWidget;
    QFrame *m_pShutDownWidget;
    QFrame *m_pLagoutWidget;
    QString qss;
    QStyledItemDelegate *itemDelegate;
    QLabel *m_pSoundEffectLabel;
    QLabel *m_pSoundThemeLabel;
    QLabel *m_pShutdownlabel;
    QLabel *m_pLagoutLabel;
    QComboBox *m_pSoundThemeCombobox;
    QComboBox *m_pShutdownCombobox;
    QComboBox *m_pLagoutCombobox;
    SwitchButton *m_pBootButton;
};

#endif // UKMEDIASOUNDEFFECTSWIDGET_H
