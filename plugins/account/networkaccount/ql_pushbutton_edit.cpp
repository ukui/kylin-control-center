#include "ql_pushbutton_edit.h"

ql_pushbutton_edit::ql_pushbutton_edit(QWidget *parent) : QPushButton(parent)
{
    tips = new QToolTips(this);
    label = new QLabel(tips);
    layout = new QHBoxLayout;

    tips->setFixedSize(86,44);
    tips->setStyleSheet("QWidget{border-radius:4px;}");

    label->setText(tr("重置密码"));
    label->setStyleSheet("QLabel{color:rgba(0,0,0,0.85);font-size:14px;}");

    layout->addWidget(label,0,Qt::AlignCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    tips->setLayout(layout);
    tips->hide();
    tips->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::NoFocus);
    label->setFocusPolicy(Qt::NoFocus);
}

void ql_pushbutton_edit::enterEvent(QEvent *e) {

    QPoint pos;
    pos.setX(this->mapToGlobal(QPoint(0, 0)).x() + 26);
    pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + 26);
    tips->move(pos);
    tips->show();
    return QPushButton::enterEvent(e);
}

void ql_pushbutton_edit::leaveEvent(QEvent *e) {
    tips->hide();

    return QPushButton::leaveEvent(e);
}
