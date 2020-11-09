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
#ifndef MODULEPAGEWIDGET_H
#define MODULEPAGEWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVariantMap>

class MainWindow;
class CommonInterface;
class KeyValueConverter;

class QListWidgetItem;

namespace Ui {
class ModulePageWidget;
}

class ModulePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModulePageWidget(QWidget *parent = 0);
    ~ModulePageWidget();

public:
    void initUI();
    void switchPage(QObject * plugin, bool recorded = true);
    void refreshPluginWidget(CommonInterface * plu);
    void highlightItem(QString text);

private:
    void getModuleStatus();

private:
    Ui::ModulePageWidget *ui;

private:
    MainWindow * pmainWindow;

    KeyValueConverter * mkvConverter;

    QVariantMap mModuleMap;

private:
    QMap<QString, CommonInterface*> pluginInstanceMap;
    // 存储功能名与二级菜单item的Map,为了实现高亮
    QMultiMap<QString, QListWidgetItem*> strItemsMap;

    bool flagBit;

public slots:
    void currentLeftitemChanged(QListWidgetItem * cur, QListWidgetItem * pre);

};

#endif // MODULEPAGEWIDGET_H
