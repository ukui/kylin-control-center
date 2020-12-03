#include "comboxframe.h"

ComboxFrame::ComboxFrame(QString labelStr, QWidget *parent) : mTitleName(labelStr), QFrame(parent) {

    this->setMinimumSize(550, 50);
    this->setMaximumSize(960, 50);
    this->setFrameShape(QFrame::Shape::Box);

    mTitleLabel = new QLabel(mTitleName, this);
    mCombox = new QComboBox(this);

    mHLayout = new QHBoxLayout(this);
    mHLayout->addWidget(mTitleLabel);
    mHLayout->addWidget(mCombox);

    this->setLayout(mHLayout);
}

ComboxFrame::~ComboxFrame() {

}
