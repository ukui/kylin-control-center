#include "myqradiobutton.h"

MyQRadioButton::MyQRadioButton(QWidget *parent):
    QRadioButton(parent)
{

}

void MyQRadioButton::leaveEvent(QEvent *event)
{
    Q_EMIT leaveWidget();

    QRadioButton::enterEvent(event);
}

void MyQRadioButton::enterEvent(QEvent *event)
{
     Q_EMIT enterWidget();

    QRadioButton::leaveEvent(event);
}
