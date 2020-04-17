#include "network_item.h"

network_item::network_item(QWidget *parent)
{
    this->parent();
    setMaximumSize(1080,50);
    setMinimumSize(0,50);
    widget = new QWidget;
    label_item = new QLabel(widget);
    label_item->setStyleSheet("font-size: 14px;color: rgba(0,0,0,85%)");
    switch_btn = new QL_SwichButton(widget);
    switch_btn->setStyleSheet("margin-right: 16px");
    layout = new QHBoxLayout;
    layout->addWidget(label_item);
    QHBoxLayout *qhb = new QHBoxLayout;
    qhb->addStretch(2);
    layout->addLayout(qhb);
    layout->addWidget(switch_btn);
    layout->setMargin(16);
    widget->setAttribute(Qt::WA_StyledBackground,true);
    widget->setStyleSheet("background-color: rgba(244,244,244,85%);border-radius:4px;");
    widget->setLayout(layout);

}

QL_SwichButton* network_item::get_swbtn() {
    return switch_btn;
}

QWidget* network_item::get_widget() {
    return widget;
}

QString network_item::get_itemname() {
    return label_item->text();
}

QHBoxLayout* network_item::get_layout() {
    return layout;
}

void network_item::set_itemname(QString name) {
    label_item->setText(name);
}

void network_item::make_itemon() {
    if(switch_btn != nullptr) {
        if(switch_btn->get_swichbutton_val() != 1) {
            switch_btn->set_swichbutton_val(1);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}

void network_item::make_itemoff() {
    if(switch_btn != nullptr) {
        if(switch_btn->get_swichbutton_val() != 0) {
            switch_btn->set_swichbutton_val(0);
            //switch_btn->update();
        }
    } else {
        qDebug() <<"switch button is null ptr";
    }
}
