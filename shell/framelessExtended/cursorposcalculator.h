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
#ifndef CURSORPOSCALCULATOR_H
#define CURSORPOSCALCULATOR_H

#include <QPoint>
#include <QRect>

class CursorPosCalculator
{

public:
    explicit CursorPosCalculator();

public:
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);
    void recalculateMove(const QPoint &globalMousePos, const QRect &frameRect);

public:
    bool onEdges : true;
    bool moveEdges : true;

    bool onLeftEdges : true;
    bool onTopEdges : true;
    bool onRightEdges : true;
    bool onBottomEdges : true;

    bool onLeftTopEdges :  true;
    bool onRightTopEdges : true;
    bool onRightBottomEdges : true;
    bool onLeftBottomEdges : true;

    static int borderWidth;
    static int moveWidth;
};

#endif // CURSORPOSCALCULATOR_H
