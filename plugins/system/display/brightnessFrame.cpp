#include "brightnessFrame.h"
#include <QHBoxLayout>

BrightnessFrame::BrightnessFrame(QWidget *parent) :
    QFrame(parent)
{
    this->setFixedHeight(50);
    this->setMinimumWidth(550);
    this->setMaximumWidth(960);
    this->setFrameShape(QFrame::Shape::Box);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(6);
    layout->setMargin(9);

    labelName = new QLabel(this);
    labelName->setFixedWidth(118);

    slider = new Uslider(Qt::Horizontal, this);
    slider->setRange(10, 100);

    labelValue = new QLabel(this);
    labelValue->setFixedWidth(30);
    labelValue->setAlignment(Qt::AlignRight);

    layout->addWidget(labelName);
    layout->addWidget(slider);
    layout->addWidget(labelValue);

}

BrightnessFrame::~BrightnessFrame()
{

}

void BrightnessFrame::setTextLableName(QString text)
{
    this->labelName->setText(text);
}

void BrightnessFrame::setTextLableValue(QString text)
{
    this->labelValue->setText(text);
}
