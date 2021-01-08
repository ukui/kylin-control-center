#include "shadowwidget.h"
#include <QGraphicsDropShadowEffect>

SWidget::SWidget(QWidget *parent)
{

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置无边框
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor("#444444"));
    //设置阴影圆角
    shadow->setBlurRadius(30);

    //给嵌套QWidget设置阴影
    widget_bg = new QWidget(this);
    widget_bg->setGraphicsEffect(shadow);

    //给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
//    ui->lay_bg->setMargin(24);
}


