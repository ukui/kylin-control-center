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
#include "ukui_custom_style.h"
#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QDebug>

UkuiCustomStyle::UkuiCustomStyle()
{

}

void UkuiButtonDrawSvg::init(QImage img, QColor color)
{
    mImage = img;
    mColor = color;
}

void UkuiButtonDrawSvg::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x13,0x14,0x00)));
    p.setPen(Qt::NoPen);
    QPainterPath path;
    opt.rect.adjust(0,0,0,0);
    path.addRoundedRect(opt.rect,6,6);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QRect UkuiButtonDrawSvg::IconGeometry()
{
    QRect res = QRect(QPoint(0,0),QSize(24,24));
    res.moveCenter(QRect(0,0,width(),height()).center());
    return res;
}

void UkuiButtonDrawSvg::draw(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    QRect iconRect = IconGeometry();
    if (mImage.size() != iconRect.size())
    {
        mImage = mImage.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect r = mImage.rect();
        r.moveCenter(iconRect.center());
        iconRect = r;
    }

    this->setProperty("fillIconSymbolicColor", true);
    filledSymbolicColoredPixmap(mImage,mColor);
    painter.drawImage(iconRect, mImage);
}

bool UkuiButtonDrawSvg::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Paint:
        draw(static_cast<QPaintEvent*>(event));
        break;

    case QEvent::Move:
    case QEvent::Resize:
    {
        QRect rect = IconGeometry();
    }
        break;

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        event->accept();
        break;

    default:
        break;
    }

    return QPushButton::event(event);
}


UkuiButtonDrawSvg::UkuiButtonDrawSvg(QWidget *parent)
{
    Q_UNUSED(parent);
}
UkuiButtonDrawSvg::~UkuiButtonDrawSvg()
{

}

QPixmap UkuiButtonDrawSvg::filledSymbolicColoredPixmap(QImage &img, QColor &baseColor)
{

    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                int hue = color.hue();
                if (!qAbs(hue - symbolic_color.hue()) < 10) {
                    color.setRed(baseColor.red());
                    color.setGreen(baseColor.green());
                    color.setBlue(baseColor.blue());
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }

    return QPixmap::fromImage(img);
}

