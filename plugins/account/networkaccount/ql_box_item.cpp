#include "ql_box_item.h"

ql_box_item::ql_box_item(QWidget *parent) : QWidget(parent)
{
    cd= new QLabel;
    ct = new QLabel;
    cd->setStyleSheet("QLabel{color:rgba(0,0,0,0.85);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    ct->setStyleSheet("QLabel{color:rgba(0,0,0,0.65);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    QHBoxLayout *layout = new QHBoxLayout;
    cd->setObjectName("code");
    ct->setObjectName("country");

    layout->setContentsMargins(16,0,16,0);
    layout->setSpacing(0);
    layout->addWidget(cd,0,Qt::AlignLeft);
    layout->addWidget(ct,0,Qt::AlignRight);
    setLayout(layout);
    //setFixedSize(200,36);
}

void ql_box_item::set_code(QString str) {
    cd->setText(str);
}

void ql_box_item::set_country_code(QString str) {
    ct->setText(str);
}

void ql_box_item::enterEvent(QEvent *e) {
    QString str = "color:#FFFFFF;font-size:14px";
    cd->setStyleSheet(str);
    ct->setStyleSheet(str);
}

void ql_box_item::leaveEvent(QEvent *e) {
    cd->setStyleSheet("color:rgba(0,0,0,0.85);font-size:14px");
    ct->setStyleSheet("color:rgba(0,0,0,0.65);font-size:14px");
}
