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
#ifndef SHOWALLSHORTCUT_H
#define SHOWALLSHORTCUT_H

#include <QDialog>
#include <QPainter>
#include <QPushButton>

namespace Ui {
class ShowAllShortcut;
}

class ClickWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClickWidget(QString name);
    ~ClickWidget(){}

public:
    bool checked();

protected:
    void mousePressEvent(QMouseEvent * e);
    void paintEvent(QPaintEvent * e);

private:
    QPushButton * directionBtn;

Q_SIGNALS:
    void widgetClicked(bool checked);

};

class ShowAllShortcut : public QDialog
{
    Q_OBJECT

public:
    explicit ShowAllShortcut(QWidget *parent = nullptr);
    ~ShowAllShortcut();

public:
    void buildComponent(QMap<QString, QMap<QString, QString>> shortcutsMap);

    QWidget * buildTitleWidget(QString tName);
    QWidget * buildGeneralWidget(QString schema, QMap<QString, QString> subShortcutsMap);

protected:
    void paintEvent(QPaintEvent *);


private:
    Ui::ShowAllShortcut *ui;
};

#endif // SHOWALLSHORTCUT_H
