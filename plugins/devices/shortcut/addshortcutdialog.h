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
#ifndef ADDSHORTCUTDIALOG_H
#define ADDSHORTCUTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include <QRegExpValidator>

namespace Ui {
class addShortcutDialog;
}

class addShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addShortcutDialog(QWidget *parent = nullptr);
    ~addShortcutDialog();

public:
    void setTitleText(QString text);
    void openProgramFileDialog();

    void setUpdateEnv(QString path, QString name, QString exec);

    void refreshCertainChecked();
    void limitInput();

protected:
    void paintEvent(QPaintEvent *);


private:
    Ui::addShortcutDialog *ui;

private:
    QString gsPath;
    QString selectedfile;


Q_SIGNALS:
    void shortcutInfoSignal(QString path, QString name, QString exec);
};

#endif // ADDSHORTCUTDIALOG_H
