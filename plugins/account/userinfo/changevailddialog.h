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
#ifndef CHANGEVAILDDIALOG_H
#define CHANGEVAILDDIALOG_H

#include <QDialog>

#include <QDate>

#include <QPainter>
#include <QPainterPath>


namespace Ui {
class ChangeVaildDialog;
}

class ChangeVaildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeVaildDialog(QString userName, QWidget *parent = 0);
    ~ChangeVaildDialog();

    void setupConnect();

    void setUserName();
    void setUserLogo(QString face);
    void setUserType(QString atype);

    void _getCurrentPwdStatus();

    void setupCurrentVaild();
    void setupComponent();

    void setupYearCombo();
    void setupMonthCombo();
    void setupDayCombo();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::ChangeVaildDialog *ui;

private:
    QString _name;

    QDate lastChangeDate;

    int delayDays;
};

#endif // CHANGEVAILDDIALOG_H
