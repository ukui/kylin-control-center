#ifndef USLIDER_H
#define USLIDER_H

#include <QSlider>
#include <QPaintEvent>
#include <QWidget>
#include <QStringList>

class Uslider : public QSlider
{

public:
    QStringList scaleList;
public:
    Uslider(QStringList list);
    void paintEvent(QPaintEvent *ev);    
};

#endif // USLIDER_H

