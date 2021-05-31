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
#ifndef SYNCDIALOG_H
#define SYNCDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainterPath>
#include "configfile.h"

class SyncDialog : public QDialog
{
    Q_OBJECT
public:
    explicit        SyncDialog(QString name,QString path,QWidget *parent = nullptr);
    void initUI();

    QStringList     m_szItemNameList = {tr("Wallpaper"),tr("ScreenSaver"),tr("Font"),tr("Avatar"),tr("Menu"),tr("Tab"),tr("Quick Start"),
                                        tr("Themes"),tr("Mouse"),tr("TouchPad"),tr("KeyBoard"),tr("ShortCut"),
                                        tr("Area"),tr("Date/Time"),tr("Default Open"),tr("Notice"),tr("Option"),tr("Peony"),
                                        tr("Boot"),tr("Power"),tr("Editor"),tr("Terminal"),tr("Weather"),tr("Media")};

    QStringList         m_szItemlist = {"wallpaper","ukui-screensaver","font","avatar","ukui-menu","ukui-panel","ukui-panel2",
                                        "themes","mouse","touchpad","keyboard","shortcut","area","datetime","default-open",
                                        "notice","option","peony","boot","power","editor","terminal",
                                        "indicator-china-weather","kylin-video"};

    QStringList m_List;
    void checkOpt();

protected:
    void paintEvent(QPaintEvent * event);

private:
    TitleLabel * mTitle;
    QLabel * mTips;
    QPushButton * mSyncButton;
    QPushButton * mCancelButton;

    QString mDate;

    QListWidget * mListWidget;

    QVBoxLayout * mMainLayout;
    QHBoxLayout * mHBoxLayout;

signals:
    void sendKeyMap(const QStringList &list);
    void coverMode();
};

#endif // SYNCDIALOG_H
