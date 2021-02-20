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
#include "framelesshandle.h"

#include <QEvent>

#include "framelesshandleprivate.h"
#include "widgethandlerealize.h"

FramelessHandle::FramelessHandle(QObject *parent) :
    QObject(parent),
    fpri(new FramelessHandlePrivate())
{
    fpri->widgetResizable = true;
    fpri->widgetMovable = true;
}

FramelessHandle::~FramelessHandle()
{
    QList<QWidget *> widgets = fpri->widgethandleHash.keys();
    for (int i = 0; i < widgets.size(); i++){
        delete fpri->widgethandleHash.take(widgets[i]);
    }
    delete fpri;
    fpri = nullptr;
}

bool FramelessHandle::eventFilter(QObject *watched, QEvent *event){
    switch (event->type()){

    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Leave:{
        WidgetHandleRealize * widgethandleobj = fpri->widgethandleHash.value(static_cast<QWidget *>(watched));
        if (widgethandleobj){
            widgethandleobj->handleWidgetEvent(event);
            return true;
        }
    }
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

void FramelessHandle::activateOn(QWidget *topLevelWidget){
    if (!fpri->widgethandleHash.contains(topLevelWidget)){
        WidgetHandleRealize * widgethandleobj = new WidgetHandleRealize(fpri, topLevelWidget);
        fpri->widgethandleHash.insert(topLevelWidget, widgethandleobj);

        topLevelWidget->installEventFilter(this);
    }
}

void FramelessHandle::setWidgetResizable(bool resizable){
    fpri->widgetResizable = resizable;
}

void FramelessHandle::setWidgetMovable(bool movable){
    fpri->widgetMovable = movable;
}

void FramelessHandle::setBorderWidget(uint width){
    if (width > 0)
        CursorPosCalculator::borderWidth = width;
}

bool FramelessHandle::currentWidgetResizable(){
    return fpri->widgetResizable;
}

bool FramelessHandle::currentWidgetMovable(){
    return fpri->widgetMovable;
}

uint FramelessHandle::currentBorderWidth(){
    return CursorPosCalculator::borderWidth;
}
