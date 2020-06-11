#include "ql_label_info.h"

ql_label_info::ql_label_info(QWidget *parent) : QWidget(parent)
{
    text = new QLabel(this);
    icon = new QSvgWidget(":/new/image/_.svg");
    layout = new QHBoxLayout;
    icon->setFixedSize(16,16);
    layout->addWidget(icon);
    layout->addWidget(text);
    layout->setContentsMargins(12,0,0,0);
    layout->setSpacing(8);
    text->setStyleSheet("QLabel{font-size:14px;color:#F53547}");
    setLayout(layout);
    context = "";
    hide();
}

void ql_label_info::set_text(const QString text) {
    context = text;
    this->text->setText(context);
}

ql_label_info::~ql_label_info() {
    delete icon;
}
