#ifndef USLIDER_H
#define USLIDER_H

#include <QSlider>
#include <QPaintEvent>
#include <QWidget>
#include <QStringList>

class Uslider : public QSlider
{

public:
    Uslider(QStringList list, int paintValue = 1);
    Uslider(Qt::Orientation orientation, QWidget *parent = nullptr, int paintValue = 0);
    Uslider(QWidget *parent = nullptr, int paintValue = 0);
    QStringList scaleList;
    void paintEvent(QPaintEvent *ev); 
private:
    void mousePressEvent(QMouseEvent *e);
    int paintValue;   
};

#endif // USLIDER_H

