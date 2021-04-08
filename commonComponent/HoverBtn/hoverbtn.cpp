#include "hoverbtn.h"

#include <QDebug>

HoverBtn::HoverBtn(QString mname, bool isHide, QWidget *parent) :
   mName(mname), mIsHide(isHide), QWidget(parent)
{
    this->setMaximumSize(960, 50);
    this->setMinimumSize(550, 50);
    initUI();
}

HoverBtn::HoverBtn(QString mname, QString detailName, QWidget *parent) :
    mName(mname), mDetailName(detailName), QWidget(parent)
{
    this->setMaximumSize(960, 50);
    this->setMinimumSize(550, 50);
    initUI();
}

HoverBtn::~HoverBtn() {

}

void HoverBtn::initUI() {

    mIsHide ? (mHideWidth = 0) : (mHideWidth = 102);

    mInfoItem = new QFrame(this);
    mInfoItem->setFrameShape(QFrame::Shape::Box);
    mInfoItem->setGeometry(0, 0, this->width(), this->height());

    mHLayout = new QHBoxLayout(mInfoItem);
    mHLayout->setSpacing(8);

    mPitIcon  = new QLabel(mInfoItem);
    mHLayout->addWidget(mPitIcon);

    mPitLabel = new QLabel(mInfoItem);
    mHLayout->addWidget(mPitLabel);

    mDetailLabel = new QLabel(mInfoItem);
    mHLayout->addWidget(mDetailLabel);

    mHLayout->addStretch();

    mAbtBtn   = new QPushButton(this);
    mAbtBtn->setVisible(false);

    initAnimation();
}

void HoverBtn::initAnimation() {
    mMouseTimer = new QTimer(this);
    mMouseTimer->setInterval(1);

    connect(mMouseTimer, &QTimer::timeout, this, [=] {
        if (mAnimationFlag) {
            if(mLeaveAction->state() != QAbstractAnimation::Running){
                mEnterAction->setStartValue(QRect(0, 0, mInfoItem->width(), mInfoItem->height()));
                mEnterAction->setEndValue(QRect(0, 0, mInfoItem->width() - mHideWidth, mInfoItem->height()));
                mEnterAction->start();
            }
        }
        mMouseTimer->stop();
    });

    mEnterAction = new QPropertyAnimation(mInfoItem, "geometry");
    mEnterAction->setDuration(1);
    mEnterAction->setEasingCurve(QEasingCurve::OutQuad);

    connect(mEnterAction, &QPropertyAnimation::finished, this, [=] {
        mAbtBtn->setGeometry(this->width()-100, 2, 80, 45);
        mAbtBtn->setVisible(!mIsHide);
    });

    mLeaveAction = new QPropertyAnimation(mInfoItem,"geometry");
    mLeaveAction->setDuration(1);
    mLeaveAction->setEasingCurve(QEasingCurve::InQuad);
}

void HoverBtn::resizeEvent(QResizeEvent *event) {
    mInfoItem->resize(event->size());
}

void HoverBtn::enterEvent(QEvent *event) {
    Q_UNUSED(event);
    mAnimationFlag = true;
    mMouseTimer->start();
}

void HoverBtn::leaveEvent(QEvent *event) {
    Q_UNUSED(event);

    mAnimationFlag = false;

    mAbtBtn->setVisible(false);

    mLeaveAction->setStartValue(QRect(0, 0, mInfoItem->width(), mInfoItem->height()));
    mLeaveAction->setEndValue(QRect(0, 0, this->width(), mInfoItem->height()));
    mLeaveAction->start();
}

void HoverBtn::mousePressEvent(QMouseEvent *event) {

    emit widgetClicked(mName);
    QWidget::mousePressEvent(event);
}
