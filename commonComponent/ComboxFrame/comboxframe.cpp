#include "comboxframe.h"

ComboxFrame:: ComboxFrame(QString labelStr, QWidget *parent) : QFrame(parent), mTitleName(labelStr) {

    this->setMinimumSize(550, 50);
    this->setMaximumSize(16777215, 50);
    this->setFrameShape(QFrame::Shape::Box);

    mTitleLabel = new QLabel(mTitleName, this);
    mCombox = new QComboBox(this);

    mHLayout = new QHBoxLayout(this);
    mHLayout->addWidget(mTitleLabel);
    mHLayout->addWidget(mCombox);

    this->setLayout(mHLayout);
}

ComboxFrame::ComboxFrame(bool isNum, QString labelStr, QWidget *parent) : QFrame(parent), mTitleName(labelStr)
{
    Q_UNUSED(isNum)
    this->setMinimumSize(550, 50);
    this->setMaximumSize(16777215, 50);
    this->setFrameShape(QFrame::Shape::Box);

    mTitleLabel = new QLabel(mTitleName, this);
    mNumCombox  = new QComboBox(this);
    mCombox     = new QComboBox(this);

    mHLayout = new QHBoxLayout(this);
    mHLayout->addWidget(mTitleLabel);
    mHLayout->addWidget(mNumCombox);
    mHLayout->addWidget(mCombox);

    this->setLayout(mHLayout);
}

ComboxFrame::~ComboxFrame() {

}
