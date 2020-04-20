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
#include "mcode_widget.h"

mcode_widget::mcode_widget(QWidget *parent): QLabel(parent)
{
    colorList<<QColor(Qt::black)<<QColor(Qt::red)<<QColor(Qt::darkRed)<<QColor(Qt::darkGreen)
              <<QColor(Qt::blue)<<QColor(Qt::darkBlue)<<QColor(Qt::darkCyan)<<QColor(Qt::magenta)
              <<QColor(Qt::darkMagenta)<<QColor(Qt::darkYellow);
    setFixedSize(120,36);
    qsrand(QTime::currentTime().second() * 1000 + QTime::currentTime().msec());
    colorArray = new QColor[letter_number];
    verificationCode = new QChar[letter_number];
    noice_point_number = this->width()*4;
    QFont defaultFont;
    defaultFont.setFamily(tr( "SongTi"));
    defaultFont.setPointSize(20);
    defaultFont.setBold(true);
    this->setFont(defaultFont);
    this->setStyleSheet("background-color:transparent;");
    this->setFocusPolicy(Qt::NoFocus);
}

void mcode_widget::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton)
    {
        ok = true;
        reflushVerification();
    }
}

QChar* mcode_widget::get_verificate_code() {
    return verificationCode;
}


void mcode_widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPoint p;
    painter.fillRect(this->rect(), Qt::white);
    if(ok) {
        produceVerificationCode();
        produceRandomColor();
        ok = false;
    }
    int charWidth = (this->width()- 12 ) / 4;
    for (int j = 0; j < noice_point_number; ++j)
    {
        p.setX(qrand() % this->width());
        p.setY(qrand() % this->height());
        painter.setPen(colorArray[j % 4]);
        painter.drawPoint(p);
    }
    for (int i = 0; i < letter_number; ++i)
    {
        painter.setPen(colorArray[i]);
        painter.save();
        int charSpace = (charWidth - this->fontMetrics().horizontalAdvance(verificationCode[i]))/2;
        charSpace += 12/2;
        painter.translate(i*charWidth+charSpace,0);
        if(qrand()%2) {
            if(qrand()%2)
            {
                painter.rotate(qrand()% 20);
            }
            else
            {
                painter.rotate(-qrand()% 20);
            }
            double xSize = (qrand()%3+9)/10.0;
            double ySize = (qrand()%3+9)/10.0;
            painter.scale(xSize,ySize);
        } else {
            double xShear = qrand()%4/10.0;
            double yShear = qrand()%4/10.0;
            if(qrand()%2)
            {
                xShear = -xShear;
            }
            if(qrand()%2)
            {
                yShear = -yShear;
            }
            painter.shear(xShear,yShear);
        }
        painter.drawText(0,this->height()-12, QString(verificationCode[i]));
        painter.restore();
    }
    QLabel::paintEvent(event);
    return;
}

void mcode_widget::produceVerificationCode() const
{
    for (int i = 0; i < letter_number; ++i)
        verificationCode[i] = produceRandomLetter();
    verificationCode[letter_number] = '\0';
    return;
}

QChar mcode_widget::produceRandomLetter() const
{
    QChar c;
    c='0' + qrand() % 10;
    return c;
}

void mcode_widget::produceRandomColor() const
{
    for (int i = 0; i < letter_number; ++i)
        colorArray[i] = colorList.at(qrand() % 4);
    return;
}

void mcode_widget::set_change(int ok_num) {
    if(ok_num == 0) {
        ok = false;
    }else {
        ok = true;
    }
}


void mcode_widget::reflushVerification()
{
    repaint();
}
