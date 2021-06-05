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

    label_1 = new QLabel(this);
    label_1->setFixedWidth(118);

    slider = new Uslider(Qt::Horizontal, this);
    slider->setRange(10, 100);

    label_2 = new QLabel(this);
    label_2->setFixedWidth(30);
    label_2->setAlignment(Qt::AlignRight);

    layout->addWidget(label_1);
    layout->addWidget(slider);
    layout->addWidget(label_2);

}

BrightnessFrame::~BrightnessFrame()
{

}

void BrightnessFrame::setTextLable_1(QString text)
{
    this->label_1->setText(text);
}

void BrightnessFrame::setTextLable_2(QString text)
{
    this->label_2->setText(text);
}
