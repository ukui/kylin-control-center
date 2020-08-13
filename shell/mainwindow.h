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
#include <QMainWindow>
#include <QPaintEvent>
#include <QShowEvent>
#include <QMoveEvent>
#include <QHideEvent>
#include <QPainter>

#include "interface.h"
#include "homepagewidget.h"
#include "modulepagewidget.h"
#include "searchwidget.h"

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

    void bootOptionsFilter(QString opt);
    void bootOptionsSwitch(int moduleNum, int funcNum);

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MainWindow *ui;

private:
    HomePageWidget * homepageWidget;
    ModulePageWidget * modulepageWidget;

    QButtonGroup * leftBtnGroup;
    QButtonGroup * leftMicBtnGroup; //
    bool bIsFullScreen;

    QDir pluginsDir;
//    QStringList modulesStringList;
    QList<int> moduleIndexList;
    QList<QMap<QString, QObject *>> modulesList;

    KeyValueConverter * kvConverter;
    SearchWidget * m_searchWidget;

private:
    void setBtnLayout(QPushButton * &pBtn);
    void loadPlugins();
    void initLeftsideBar();
    QPushButton * buildLeftsideBtn(QString bname, QString tipName);
    bool isExitsCloudAccount();

    // load svg picture
    const QPixmap loadSvg(const QString &fileName, QString color);
    //Render icon from theme
    const QPixmap renderSvg(const QIcon &icon, QString color);
    // chang svg picture's color
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source, QString color);

    bool dblOnEdge(QMouseEvent *event);
    void initStyleSheet();

public slots:
    void functionBtnClicked(QObject * plugin);
    void sltMessageReceived(const QString &msg);
};

#endif // MAINWINDOW_H
