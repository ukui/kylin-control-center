#include "toolpop.h"

#include <QPaintEvent>
#include <QPainterPath>
#include <QPainter>

ToolPop::ToolPop(QWidget* parent) : QLabel (parent)
{
    this->setAlignment(Qt::AlignCenter);
    this->setStyleSheet("margin: 0 15");
    this->setStyleSheet("background-color: #3D6BE5;border-radius:4px;");

}

void ToolPop::popupSlot(QPoint point) {
    this->move(point.x() - this->width() / 2 -4,
               point.y() - this->height()-8);
    this->show();
}

void ToolPop::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing,
                           true);

    QFont font;
    font.setPixelSize(12);
    QFontMetrics fontMetrics(font);
    int labelLen = fontMetrics.width(this->text());
    painter.setPen(QPen(Qt::white));
    painter.setFont(font);
    painter.drawText((this->width() - labelLen) / 2, 16, this->text());
}


