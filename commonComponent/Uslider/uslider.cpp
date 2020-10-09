#include "uslider.h"

#include <QStyle>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QDebug>
#include <QPalette>


Uslider::Uslider(QStringList list) : scaleList(list), QSlider(Qt::Horizontal)
{
    this->setMinimumHeight(50);
    this->setMaximumHeight(100);
}

void Uslider::paintEvent(QPaintEvent *e)
{

    QSlider::paintEvent(e);

    auto painter = new QPainter(this);
    painter->setBrush(QBrush(QColor(QPalette::Base)));

    auto rect = this->geometry();

    int numTicks = (maximum() - minimum())/tickInterval();

    QFontMetrics fontMetrics = QFontMetrics(this->font());

    if (this->orientation() == Qt::Horizontal) {
        int fontHeight = fontMetrics.height();

        for (int i=0; i<=numTicks; i++) {
            int tickNum = minimum() + (tickInterval() * i);

            int tickX = (((rect.width()/numTicks) * i) - (fontMetrics.width(QString::number(tickNum *10))/2 + 2));
            int tickY = rect.height()/2 + fontHeight + 1;

            if( 0 == i) {
                tickX += 11;
            } else if (numTicks == i){
                tickX -= 10;
            }
            painter->drawText(QPoint(tickX, tickY),
                                 this->scaleList.at(i));
           }
       }
    painter->end();
}


