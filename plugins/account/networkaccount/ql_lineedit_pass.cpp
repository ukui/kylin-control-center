#include "ql_lineedit_pass.h"

ql_lineedit_pass::ql_lineedit_pass(QWidget *parent) : QLineEdit(parent)
{
    setEchoMode(QLineEdit::Password);
    visble = new QPushButton(this);
    layout = new QHBoxLayout;

    visble->setCursor(Qt::PointingHandCursor);
    visble->setCheckable(true);
    connect(visble,&QPushButton::toggled,[this] (bool checked) {
        if(checked) {
            setEchoMode(QLineEdit::Normal);
        } else {
            setEchoMode(QLineEdit::Password);
        }
    });
    visble->setStyleSheet("QPushButton{width:  16px;height: 16px;qproperty-flat: true;"
    "margin-right: 4px;border: none;border-width: 0;"
    "border-image: url(:/invisible.png) 0 0 0 0 stretch stretch;"
    "background: transparent;}"
    "QPushButton:checked{border-image: url(:/visible.png) 0 0 0 0 stretch stretch;}");

    layout->addStretch();
    layout->addWidget(visble);
    layout->setMargin(0);
    visble->setFocusPolicy(Qt::NoFocus);
    setLayout(layout);

}

QPushButton* ql_lineedit_pass::get_visble() {
    return visble;
}

bool ql_lineedit_pass::check() {
    bool uper = false;
    bool normal = false;
    bool number = false;
    bool line = false;
    if(this->text() != "") {
        QString str = this->text();
        for(QChar c:str) {
            if(c>='A' && c <= 'Z') {
                uper = true;
                continue;
            }
            if(c>='a' && c <='z') {
                normal = true;
                continue;
            }
            if(c>='0' && c<='9') {
                number = true;
                continue;
            }
        }
        if(text().length() >= 6) {
            line = true;
        }
    } else {
        return false;
    }
    return uper && normal && number && line;
}
