#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QWidget>

class SWidget : public QWidget
{
public:
    explicit SWidget(QWidget *parent = nullptr);

    QWidget *widget_bg;


};


#endif // SHADOWWIDGET_H
