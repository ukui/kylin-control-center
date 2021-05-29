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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QTimer>
#include <QMainWindow>
#include <QPaintEvent>
#include <QShowEvent>
#include <QMoveEvent>
#include <QHideEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QVariantMap>
#include <QGSettings/QGSettings>

#include "interface.h"
#include "homepagewidget.h"
#include "modulepagewidget.h"
#include "searchwidget.h"

class QLabel;
class QPushButton;
class QButtonGroup;
class KeyValueConverter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    QMap<QString, QObject *> exportModule(int);
    void setModuleBtnHightLight(int id);
    static bool isExitBluetooth();
    static bool isExitBattery();
    void bootOptionsFilter(QString opt);
    void bootOptionsSwitch(int moduleNum, int funcNum);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MainWindow *ui;

private:
    HomePageWidget * homepageWidget;
    ModulePageWidget * modulepageWidget;

    QButtonGroup * leftBtnGroup;
    QButtonGroup * leftMicBtnGroup; //

    QDir pluginsDir;
    QList<int> moduleIndexList;
    QList<QMap<QString, QObject *>> modulesList;

    KeyValueConverter * kvConverter;
    SearchWidget      * m_searchWidget;

    QPushButton *backBtn;
    QPushButton *mOptionBtn;
    QPushButton *minBtn;
    QPushButton *maxBtn;
    QPushButton *closeBtn;
    QLabel      *titleLabel;
    QLabel      *mTitleIcon;
    QTimer      *timer;
    QLabel      *logoLabel;
    QLabel            *m_queryIcon;
    QLabel            *m_queryText  = nullptr;
    QPropertyAnimation *m_animation = nullptr;
    QWidget           *m_queryWid   = nullptr;
    bool              m_isSearching = false;
    QString           m_searchKeyWords;
    QVariantMap       m_ModuleMap;
    QGSettings       *m_fontSetting;


private:
    void initUI();
    void initTileBar();
    void setBtnLayout(QPushButton * &pBtn);
    void loadPlugins();
    void initLeftsideBar();
    QPushButton * buildLeftsideBtn(QString bname, QString tipName);
    bool isExitsCloudAccount();

    bool dblOnEdge(QMouseEvent *event);
    void initStyleSheet();
    void changeSearchSlot();

public slots:
    void functionBtnClicked(QObject * plugin);
    void sltMessageReceived(const QString &msg);
    void switchPage(QString moduleName, QString jumpMoudle);
    void animationFinishedSlot();
    void showUkccAboutSlot();
    void onF1ButtonClicked();

Q_SIGNALS:
    void posChanged();

};

#endif // MAINWINDOW_H
