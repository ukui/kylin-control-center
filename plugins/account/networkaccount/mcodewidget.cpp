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
#include "mcodewidget.h"

MCodeWidget::MCodeWidget(QWidget *parent): QLabel(parent)
{
    m_colorList<<QColor(Qt::black)<<QColor(Qt::red)<<QColor(Qt::darkRed)<<QColor(Qt::darkGreen)
              <<QColor(Qt::blue)<<QColor(Qt::darkBlue)<<QColor(Qt::darkCyan)<<QColor(Qt::magenta)
              <<QColor(Qt::darkMagenta)<<QColor(Qt::darkYellow);
    setFixedSize(120,36);
    qsrand(QTime::currentTime().second() * 1000 + QTime::currentTime().msec());
    m_colorArray = new QColor[m_letterNumber];
    m_verificationCode = new QChar[m_letterNumber];
    m_noicePointNumber = this->width() * 3;
    QFont defaultFont;
    defaultFont.setFamily(tr( "SongTi"));
    defaultFont.setPointSize(20);
    defaultFont.setBold(true);
    this->setFont(defaultFont);
    this->setStyleSheet("background-color:transparent;");
    this->setFocusPolicy(Qt::NoFocus);
}

/* 该函数实现了鼠标点击后验证码更新的操作 ，用ok变量控制以防止界面刷新就更新 */
void MCodeWidget::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton)
    {
        m_bIsOk = true;
        reflushVerification();
    }
}

/* 返回得到的验证码 */
QChar* MCodeWidget::get_verificate_code() {
    return m_verificationCode;
}


/* 重新绘制事件,先根据生成的数的奇偶性掰弯数字，然后再根据新生成的数字奇偶性变形 */
void MCodeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPoint p;
    painter.fillRect(this->rect(), Qt::white);
    if (m_bIsOk) {
        produceVerificationCode();
        produceRandomColor();
        m_bIsOk = false;
    }
    int charWidth = (this->width()- 14 ) / 4;
    for (int j = 0; j < m_noicePointNumber; ++j)
    {
        p.setX(qrand() % this->width());
        p.setY(qrand() % this->height());
        painter.setPen(m_colorArray[j % 4]);
        painter.drawPoint(p);
    }
    for (int i = 0; i < m_letterNumber; ++i)
    {
        painter.setPen(m_colorArray[i]);
        painter.save();
        int charSpace = (charWidth - this->fontMetrics().width("W"))/2;
        charSpace += 14/2;
        painter.translate(i*charWidth+charSpace,0);
        if (qrand()%2) {
            if (qrand()%2)
            {
                painter.rotate(qrand()% 20);
            }
            else
            {
                painter.rotate(-qrand()% 20);
            }
            double xSize = (qrand()%3+14)/10.0;
            double ySize = (qrand()%3+14)/10.0;
            painter.scale(xSize,ySize);
        } else {
            double xShear = qrand()%4/10.0;
            double yShear = qrand()%4/10.0;
            if (qrand()%2)
            {
                xShear = -xShear;
            }
            if (qrand()%2)
            {
                yShear = -yShear;
            }
            painter.shear(xShear,yShear);
        }
        painter.drawText(0,this->height()-14, QString(m_verificationCode[i]));
        painter.restore();
    }
    QLabel::paintEvent(event);
    return;
}

/* 产生一个随机数字 */
void MCodeWidget::produceVerificationCode() const
{
    for (int i = 0; i < m_letterNumber; ++i)
        m_verificationCode[i] = produceRandomLetter();
    m_verificationCode[m_letterNumber] = '\0';
    return;
}

/* 产生一个随机字母（备用）*/
QChar MCodeWidget::produceRandomLetter() const
{
    QChar c;
    c='0' + qrand() % 10;
    return c;
}

/* 产生随机颜色 */
void MCodeWidget::produceRandomColor() const
{
    for (int i = 0; i < m_letterNumber; ++i)
        m_colorArray[i] = m_colorList.at(qrand() % 4);
    return;
}

/* 控制字母是否能改变，用于外部控制 */
void MCodeWidget::set_change(const int &ok_num) {
    if (ok_num == 0) {
        m_bIsOk = false;
    }else {
        m_bIsOk = true;
    }
}

MCodeWidget::~MCodeWidget(){
    delete[] m_colorArray;
    m_colorArray = nullptr;
    delete[] m_verificationCode;
    m_verificationCode = nullptr;
}

/* 刷新验证码 ，实际上是重绘 */
void MCodeWidget::reflushVerification()
{
    repaint();
}
