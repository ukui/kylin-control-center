#include "uslider.h"

#include <QStyle>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QDebug>
#include <QPalette>


Uslider::Uslider(QStringList list, int paintValue) : QSlider(Qt::Horizontal), scaleList(list)
{
    this->setMinimumHeight(50);
    this->setMaximumHeight(100);
    this->paintValue = paintValue;
    this->setPageStep(0); //防止qslider的mousePressEvent对坐标造成影响
}

Uslider::Uslider(Qt::Orientation orientation, QWidget *parent, int paintValue) :
    QSlider(orientation, parent)
{
    this->paintValue = paintValue;
    this->setPageStep(0); //防止qslider的mousePressEvent对坐标造成影响
}

Uslider::Uslider(QWidget *parent, int paintValue) : QSlider(parent)
{
    this->paintValue = paintValue;
    this->setPageStep(0); //防止qslider的mousePressEvent对坐标造成影响
}

void Uslider::paintEvent(QPaintEvent *e)
{
    QSlider::paintEvent(e);
    if (paintValue == 0)   //不需要刻度值显示
            return;

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
            if (i == numTicks) {
                tickX -= 3;
            }

            painter->drawText(QPoint(tickX, tickY),
                              this->scaleList.at(i));
            tickX += fontRect.width();
            tickX += interval;
        }
    }
    painter->end();
}

void Uslider::mousePressEvent(QMouseEvent *e)
{
    int value = 0;
    int currentX = e->pos().x();
    double per = currentX * 1.0 / this->width();
    if ((this->maximum() - this->minimum()) >= 50) { //减小鼠标点击像素的影响
        value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
        if (value <= (this->maximum() / 2 - this->maximum() / 10 + this->minimum() / 10)) {
            value = qRound(per*(this->maximum() - this->minimum() - 1)) + this->minimum();
        } else if (value > (this->maximum() / 2 + this->maximum() / 10 + this->minimum() / 10)) {
            value = qRound(per*(this->maximum() - this->minimum() + 1)) + this->minimum();
        } else {
            value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
        }
    } else {
        value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
    }
    this->setValue(value);

    QSlider::mousePressEvent(e);  //必须放在后面，否则点击拖动无法使用(待优化)
}
