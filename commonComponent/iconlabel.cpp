#include "iconlabel.h"
#include <QEvent>

IconLabel::IconLabel(QWidget *parent):
    QLabel(parent)
{

}

void IconLabel::enterEvent(QEvent *event){
    Q_EMIT enterWidget();

    QLabel::enterEvent(event);
}

void IconLabel::leaveEvent(QEvent *event){
    Q_EMIT leaveWidget();

    QLabel::leaveEvent(event);
}
