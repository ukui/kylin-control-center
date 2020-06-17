#include "ql_animation_label.h"

ql_animation_label::ql_animation_label(QWidget *parent) : QWidget(parent)
{
    text = new QLabel(this);
    icon = new QLabel(this);
    timer = new QTimer(this);
    svg_hd = new ql_svg_handler(this);
    layout = new QHBoxLayout;

    this->setStyleSheet("ql_animation_label{background-color:#3D6BE5;border-radius:4px;}");
    text->setStyleSheet("font-size:14px;color:#ffffff;background:transparent;");
    icon->setStyleSheet("background:transparent;");
    icon->setFixedSize(24,24);
    layout->setSpacing(8);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(text);
    layout->addWidget(icon);
    setLayout(layout);
    timer->stop();
    connect(timer,&QTimer::timeout, [this] () {
        QPixmap pixmap = svg_hd->loadSvg(QString(":/new/image/loading1%1.svg").arg(cnt));
        icon->setPixmap(pixmap);
        cnt = (cnt + 8) % 7;
    });
    hide();
}

void ql_animation_label::settext(QString t) {
    text->setText(t);
}

void ql_animation_label::startmoive() {
    timer->start(140);
    show();
    adjustSize();
}

void ql_animation_label::stop() {
    cnt = 1;
    icon->setPixmap(svg_hd->loadSvg(":/new/image/loading11.svg"));
    timer->stop();
    hide();
}
