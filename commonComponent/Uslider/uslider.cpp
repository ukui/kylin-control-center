#include "uslider.h"

#include <QStyle>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QDebug>


Uslider::Uslider(QStringList list) : scaleList(list), QSlider(Qt::Horizontal)
{
    this->setMinimumHeight(50);
}

void Uslider::paintEvent(QPaintEvent *e)
{

    QSlider::paintEvent(e);

    auto painter = new QPainter(this);
    painter->setPen(QPen(Qt::black));

    auto rect = this->geometry();

    int numTicks = (maximum() - minimum())/tickInterval();

    QFontMetrics fontMetrics = QFontMetrics(this->font());

    if (this->orientation() == Qt::Horizontal) {
        int fontHeight = fontMetrics.height();

        for (int i=0; i<=numTicks; i++) {
            int tickNum = minimum() + (tickInterval() * i);

            int tickX = (((rect.width()/numTicks) * i) - (fontMetrics.width(QString::number(tickNum))/2));
            int tickY = rect.height()/2 + fontHeight + 2;

            if( 0 == i) {
                tickX += 3;
            } else if (numTicks == i){
                tickX -= 10;
            }
            painter->drawText(QPoint(tickX, tickY),
                                 this->scaleList.at(i));
           }
       }
    painter->end();
}


