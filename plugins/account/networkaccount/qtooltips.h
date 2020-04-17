#ifndef QTOOLTIPS_H
#define QTOOLTIPS_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QtMath>

class QToolTips : public QWidget
{
    Q_OBJECT
public:
    QToolTips(QWidget *parent = nullptr);
    int             xpos;
    int             ypos;
    int             radius;
    int             alpha;
protected:
    void            paintEvent(QPaintEvent *event);
};

#endif // QTOOLTIPS_H
