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
#ifndef QL_COMBOBOBX_H
#define QL_COMBOBOBX_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtMath>
#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QScrollBar>
#include "boxitem.h"
#include "popupwidget.h"
#include <QScrollArea>
#include "svghandler.h"

class ComboBox : public QWidget
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = nullptr);
    void addItem(QString country,QString code);
public slots:
    void showpopup();
    void closepopup(QListWidgetItem *item);
protected:
    int idx;
private:
    QLineEdit       *m_mainLineEdit;
    QPushButton     *m_pushButton;
    QListWidget     *m_listWidget;
    QWidget         *m_popupWidget;
    QHBoxLayout     *m_lineeditLayout;
    QVBoxLayout     *m_popupLayout;
    QVBoxLayout     *m_comboboxLayout;
    SVGHandler  *m_svgHandler;
signals:
    void currentIndexChanged(int index);
};

#endif // QL_COMBOBOBX_H
