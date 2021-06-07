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
    void setTextLableName(QString text);
    void setTextLableValue(QString text);
    Uslider *slider = nullptr;
    QString outputName;
private:
    QLabel *labelName = nullptr;
    QLabel *labelValue = nullptr;
};

#endif // BRIGHTNESSFRAME_H
