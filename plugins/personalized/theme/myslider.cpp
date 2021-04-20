#include "myslider.h"
#include <QMouseEvent>
#include <QDebug>


MySlider::MySlider(QWidget *parent):QSlider (parent){}
MySlider::~MySlider(){}

//重写鼠标点击事件
void MySlider::mousePressEvent(QMouseEvent *ev)
{
    QSlider::mousePressEvent(ev);
    int currentx = ev->pos().x();
    int value = (currentx /(double)this->width())*(this->maximum() - this->minimum()) + this->minimum();
    this->setValue(value);
}



