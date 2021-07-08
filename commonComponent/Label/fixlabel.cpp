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

#include "fixlabel.h"
#include <QFontMetrics>
#include <QDebug>

FixLabel::FixLabel(QWidget *parent):
    QLabel(parent)
{
    m_text.clear();
}

FixLabel::~FixLabel()
{

}

void FixLabel::set_fix_text(QString text)
{
    m_text = text;
}

void FixLabel::paintEvent(QPaintEvent *event)
{
    QFontMetrics fontMetrics(this->font());
//    QString mStr = this->text();
    QString mStr = m_text;
    if (mStr.isEmpty())
        mStr = this->text();
    int fontSize = fontMetrics.width(mStr);
    if (fontSize > this->width()) {
        this->setText(fontMetrics.elidedText(mStr, Qt::ElideRight, this->width()));
        this->setToolTip(mStr);
    } else {
        this->setText(mStr);
        this->setToolTip("");
    }
    QLabel::paintEvent(event);
}
