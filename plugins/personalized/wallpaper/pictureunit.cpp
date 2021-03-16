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
#include "pictureunit.h"

#include "MaskWidget/maskwidget.h"

PictureUnit::PictureUnit()
{
    _filename = "";

    QColor highLightColor = palette().color(QPalette::Highlight);
    QString stringColor = QString("rgb(%1,%2,%3)")
            .arg(highLightColor.red())
            .arg(highLightColor.green())
            .arg(highLightColor.blue());
    hoverStyleSheet = QString("border-width: 3px;border-style: solid;border-color: %1;").arg(stringColor);
    clickedStyleSheet = QString("border-width: 6px;border-style: solid;border-color: %1;").arg(stringColor);
    clickedFlag = false;
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(166, 110));
    setScaledContents(true);

    MaskWidget * maskWidget = new MaskWidget(this);
    maskWidget->setGeometry(0, 0, this->width(), this->height());
}

PictureUnit::~PictureUnit()
{
}

void PictureUnit::setFilenameText(QString fn){
    _filename = fn;
}

QString PictureUnit::filenameText(){
    return _filename;
}

void PictureUnit::mousePressEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton)
        emit clicked(_filename);
//    QLabel::mousePressEvent(event);
}

void PictureUnit::enterEvent(QEvent *e)
{
    if(getClickedFlag() == false)
    {
        setFrameShape (QFrame::Box);
        setStyleSheet(hoverStyleSheet);
    }
}
void PictureUnit::leaveEvent(QEvent *e)
{
    if(getClickedFlag() == false)
        setStyleSheet("border-width: 0px;");
}

bool PictureUnit::getClickedFlag()
{
    return clickedFlag;
}

void PictureUnit::changeClickedFlag(bool flag)
{   
    clickedFlag = flag;
}