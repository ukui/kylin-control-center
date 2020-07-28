

#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#include "colorpreview.h"

ColorPreview::ColorPreview(QWidget *parent) :
    QWidget(parent)
  , col(Qt::red)
  , back( Qt::darkGray, Qt::DiagCrossPattern )
  , alpha_mode(NoAlpha), colorPrevious(Qt::blue)
{
    back.setTexture(QPixmap(QString(":/images/alphaback.png")));
}

QSize ColorPreview::sizeHint() const
{
    return QSize(48,48);
}

void ColorPreview::setColor(QColor c)
{
    col = c;
    update();
    emit colorChanged(c);
}

void ColorPreview::setPreviousColor(QColor colorPre)
{
    colorPrevious = colorPre;
    update();
}

void ColorPreview::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    QStyleOption opt;
    opt.init(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(col);
    painter.drawRoundedRect(opt.rect,4,4);
}
