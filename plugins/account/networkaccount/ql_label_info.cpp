#include "ql_label_info.h"

ql_label_info::ql_label_info(QWidget *parent) : QWidget(parent)
{
    text = new QLabel(this);
    icon = new QLabel(this);
    layout = new QHBoxLayout;
    layout->addWidget(icon);
    layout->addWidget(text);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    context = "";
    hide();
}

void ql_label_info::set_text(const QString text) {
    context = text;
    this->text->setText(context);
}

void ql_label_info::setpixmap(const QPixmap pixmap) {
    this->icon->setPixmap(pixmap);
}
