#include "uslider.h"

#include <QStyle>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QDebug>
#include <QPalette>


Uslider::Uslider(QStringList list) : QSlider(Qt::Horizontal), scaleList(list)
{
    this->setMinimumHeight(50);
    this->setMaximumHeight(100);
    this->setTickPosition(QSlider::TicksAbove);
}

void Uslider::paintEvent(QPaintEvent *e)
{
    QSlider::paintEvent(e);

    auto painter = new QPainter(this);
    painter->setBrush(QBrush(QColor(QPalette::Base)));

    auto rect = this->geometry();
    int numTicks = (maximum() - minimum()) / tickInterval();
    int total = 0;
    QFontMetrics fontMetrics = QFontMetrics(this->font());

    for (int i=0; i <= numTicks; i++) {
        QRect fontRect = fontMetrics.boundingRect(scaleList.at(i));
        total += fontRect.width();
    }
    const int interval = (rect.width() - total) / numTicks;

    if (this->orientation() == Qt::Horizontal) {
        int fontHeight = fontMetrics.height();
        int tickX = 1;
        int tickY = rect.height() / 2 + fontHeight + 5;
        for (int i=0; i <= numTicks; i++) {
            QRect fontRect = fontMetrics.boundingRect(scaleList.at(i));

            painter->drawText(QPoint(tickX, tickY),
                              this->scaleList.at(i));
            tickX += fontRect.width();
            tickX += interval;
        }
    }
    painter->end();
}


