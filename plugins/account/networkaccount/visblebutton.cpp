#include "visblebutton.h"

VisbleButton::VisbleButton(QWidget *parent) : QLabel(parent)
{
    status = false;
    setFixedSize(16,16);
}

void VisbleButton::enterEvent(QEvent *event) {

}

void VisbleButton::leaveEvent(QEvent *event) {

}

void VisbleButton::mousePressEvent(QMouseEvent *event) {
    status = !status;
    emit clicked(status);
    emit toggled(status);
}

void VisbleButton::setChecked(bool checked) {
    status = checked;
}
