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
#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include <QDebug>
#include <QWidget>

class ColorPreview : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged DESIGNABLE true)
    Q_PROPERTY(Alpha_Mode alpha_mode READ alphaMode WRITE setAlphaMode DESIGNABLE true)
    Q_PROPERTY(QBrush background READ getBackground WRITE setBackground DESIGNABLE true)
    Q_ENUMS(Alpha_Mode)
public:
    enum Alpha_Mode
    {
        NoAlpha,
        SplitAlpha,
    };

private:
    QColor      col;
    QBrush      back;
    Alpha_Mode  alpha_mode;
    QColor      colorPrevious;

public:
    explicit ColorPreview(QWidget *parent = 0);

    void setPreviousColor(QColor colorPre);

    void setBackground(QBrush bk)
    {
        back = bk;
        update();
    }

    QBrush getBackground() const
    {
        return back;
    }

    Alpha_Mode alphaMode() const
    {
        return alpha_mode;
    }

    void setAlphaMode(Alpha_Mode am)
    {
        alpha_mode = am;
        update();
    }


    QColor color() const
    {
        return col;
    }

    QSize sizeHint () const;

public slots:
    void setColor(QColor c);

signals:
    void colorChanged(QColor);

protected:
    void paintEvent(QPaintEvent *);

};

#endif // COLORPREVIEW_H
