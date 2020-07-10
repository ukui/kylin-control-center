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
#ifndef UKUICUSTOMSTYLE_H
#define UKUICUSTOMSTYLE_H

#include <QObject>
#include <QPushButton>
#include <QImage>
#include <QSlider>

static QColor symbolic_color = Qt::gray;

class UkmediaVolumeSlider : public QSlider
{
    Q_OBJECT
public:
    UkmediaVolumeSlider(QWidget *parent = nullptr);
    void initStyleOption(QStyleOptionSlider *option);
    ~UkmediaVolumeSlider();

Q_SIGNALS:
    void silderPressedSignal(int);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *e)
    {
        setCursor(QCursor(Qt::OpenHandCursor));
//        m_displayLabel->move((this->width()-m_displayLabel->width())*this->value()/(this->maximum()-this->minimum()),3);
        QSlider::mouseMoveEvent(e);
    }
};

class UkuiButtonDrawSvg:public QPushButton
{
    Q_OBJECT
public:
    UkuiButtonDrawSvg(QWidget *parent = nullptr);
    ~UkuiButtonDrawSvg();
    QPixmap filledSymbolicColoredPixmap(QImage &source, QColor &baseColor);
    QRect IconGeometry();
    void draw(QPaintEvent* e);
    void init(QImage image ,QColor color);
    friend class UkmediaMainWidget;
protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *e);
private:
    QImage mImage;
    QColor mColor;
};

#endif // UKUICUSTOMSTYLE_H
