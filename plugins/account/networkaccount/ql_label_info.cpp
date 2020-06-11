#include "ql_label_info.h"

ql_label_info::ql_label_info(QWidget *parent) : QWidget(parent)
{
    text = new QLabel(this);
    icon = new QLabel(this);
    layout = new QHBoxLayout;
    icon->setFixedSize(16,16);
    layout->addWidget(icon);
    layout->addWidget(text);
    layout->setMargin(0);
    layout->setSpacing(0);
    text->setStyleSheet("QLabel{font-size:14px;color:#F53547}");
    setLayout(layout);
    context = "";
    hide();
}

void ql_label_info::set_text(const QString text) {
    context = text;
    this->text->setText(context);
}

void ql_label_info::setpixmap(QPixmap pixmap) {
    pixmap.setDevicePixelRatio(2);
    this->icon->setPixmap(pixmap);
}
