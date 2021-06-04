#ifndef BRIGHTNESSFRAME_H
#define BRIGHTNESSFRAME_H

#include <QFrame>
#include <QLabel>
#include <QWidget>
#include <QLabel>
#include "Uslider/uslider.h"

class BrightnessFrame : public QFrame
{
    Q_OBJECT
public:
    BrightnessFrame(QWidget *parent = nullptr);
    ~BrightnessFrame();
    void setTextLable_1(QString text);
    void setTextLable_2(QString text);
    Uslider *slider = nullptr;
    QString outputName;
private:
    QLabel *label_1 = nullptr;
    QLabel *label_2 = nullptr;
};

#endif // BRIGHTNESSFRAME_H
