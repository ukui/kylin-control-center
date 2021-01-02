#include "hoverbtn.h"

#include <QDebug>

HoverBtn::HoverBtn(QString mname, QWidget *parent) :
    mName(mname), QFrame(parent)
{
    this->setFrameShape(QFrame::Shape::Box);
    this->setMaximumSize(960, 50);
    this->setMinimumSize(550, 50);
    initUI();
}

HoverBtn::~HoverBtn() {

}

void HoverBtn::initUI() {
    mHLayout  = new QHBoxLayout(this);

    mAbtBtn   = new QPushButton(this);

    mPitIcon  = new QLabel(this);
    mPitLabel = new QLabel(this);

    mHLayout->addWidget(mPitIcon);
    mHLayout->addWidget(mPitLabel);
    mHLayout->addStretch();
    mHLayout->addWidget(mAbtBtn);

    mAbtBtn->setVisible(false);
    this->setLayout(mHLayout);
}

void HoverBtn::enterEvent(QEvent *event) {
    mAbtBtn->setVisible(true);
    emit enterWidget(mName);
    QFrame::enterEvent(event);
}

void HoverBtn::leaveEvent(QEvent *event) {
    mAbtBtn->setVisible(false);
    emit leaveWidget(mName);
    QFrame::leaveEvent(event);
}

void HoverBtn::mousePressEvent(QMouseEvent *event) {

    emit widgetClicked(mName);
    QFrame::mousePressEvent(event);
}
