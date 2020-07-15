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
#include "cursorposcalculator.h"
#include <QDebug>

int CursorPosCalculator::borderWidth = 5;
int CursorPosCalculator::moveWidth = 50;

CursorPosCalculator::CursorPosCalculator()
{
    reset();
}

void CursorPosCalculator::reset(){
    onEdges = false;
    moveEdges = false;

    onLeftEdges = false;
    onTopEdges = false;
    onRightEdges = false;
    onBottomEdges = false;

    onLeftTopEdges =false;
    onRightTopEdges = false;
    onRightBottomEdges = false;
    onLeftBottomEdges = false;
}

void CursorPosCalculator::recalculate(const QPoint &globalMousePos, const QRect &frameRect){
    int globalMouseX = globalMousePos.x();
    int globalMouseY = globalMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    onLeftEdges = (globalMouseX >= frameX &&
                   globalMouseX <= frameX + borderWidth);

    onRightEdges = (globalMouseX >= frameX + frameWidth - borderWidth &&
                    globalMouseX <= frameX + frameWidth);

    onTopEdges = (globalMouseY >= frameY &&
                  globalMouseY <= frameY + borderWidth);

    onBottomEdges = (globalMouseY >= frameY + frameHeight - borderWidth &&
                     globalMouseY <= frameY + frameHeight);

    onLeftTopEdges = onTopEdges && onLeftEdges;
    onRightTopEdges = onRightEdges && onTopEdges;
    onRightBottomEdges = onRightEdges && onBottomEdges;
    onLeftBottomEdges = onLeftEdges && onBottomEdges;

    onEdges = onLeftEdges || onRightEdges || onTopEdges || onBottomEdges;
}


void CursorPosCalculator::recalculateMove(const QPoint &globalMousePos, const QRect &frameRect){
    int globalMouseY = globalMousePos.y();
    int frameY = frameRect.y();

    onTopEdges = (globalMouseY >= frameY &&
                  globalMouseY <= frameY + moveWidth);

    moveEdges =  onTopEdges;
}
