#include "qtooltips.h"

QToolTips::QToolTips(QWidget *parent) : QWidget(parent)
{
    xpos = 6;
    ypos = 6;
    alpha = 10;
    radius = 4;

    setStyleSheet("ql_popup{border-radius:4px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
}


void QToolTips::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    QColor m_defaultBackgroundColor = qRgb(0, 0, 0);
    QPainterPath path1;
    path1.setFillRule(Qt::WindingFill);
    path1.addRoundedRect(xpos, ypos, this->width() - (xpos * 2), this->height() - (ypos * 2), radius, radius);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path1, QBrush(QColor(m_defaultBackgroundColor.red(),
                                          m_defaultBackgroundColor.green(),
                                          m_defaultBackgroundColor.blue())));

    QColor color(0, 0, 0, alpha);
    for (int i = 0; i < 5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(xpos - i, ypos - i, this->width() - (xpos - i) * 2, this->height() - (ypos - i) * 2, radius, radius);
        color.setAlpha(80 - qSqrt(i) * 40);
        painter.setPen(color);
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setX(xpos);
    rect.setY(ypos);
    rect.setWidth(rect.width() - ypos);
    rect.setHeight(rect.height() - xpos);
    // rect: 绘制区域  15：圆角弧度
    painter.drawRoundedRect(rect, radius, radius);
}
