#ifndef CHECKBUTTON_H
#define CHECKBUTTON_H

#include "picturetowhite.h"
#include <QObject>
#include <QPushButton>
#include <QTimer>
#include <QGSettings>

class m_button : public QPushButton
{
    Q_OBJECT
public:
     m_button(QWidget *parent = nullptr);
     QTimer *m_cTimer;
     QGSettings *mGsetting = NULL;
     PictureToWhite *mPictureRecerse = NULL;

     int i = 17;//初始化从17图片开始
     bool states;

     void start();

     void stop();

     void buttonshow();

     void buttonswitch();


signals:

};
#endif // CHECKBUTTON_H
