#include "ql_swichbutton.h"

QL_SwichButton::QL_SwichButton(QWidget *parent) : QWidget(parent) {
    setMaximumSize(48,24);
    setMinimumSize(48,24);
    ql_width = (float)width();
    ql_height = (float)height();
    timer = new QTimer(this);
    timer->setInterval(5);
    if(on == 1) {
        cur_val = ql_width - 16 - 4;
    }
    else {
        cur_val = 4;
    }
    connect(timer,SIGNAL(timeout()),this,SLOT(startAnimation()));
}

void QL_SwichButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter ql_painter(this);
    ql_painter.setRenderHint(QPainter::SmoothPixmapTransform);
    ql_painter.setRenderHint(QPainter::Antialiasing); //kan ju ci
    ql_painter.setPen(Qt::NoPen);
    if(on) {
        ql_painter.save();
        QColor ql_color(61,107,229);
        ql_painter.setBrush(ql_color);
        QRectF active_rect = QRectF(0,0,ql_width,ql_height);
        ql_painter.drawRoundedRect(active_rect, 0.5 * ql_height, 0.5 * ql_height); //hua yi ge yuan
        ql_painter.restore();
        ql_painter.save();
        ql_painter.setBrush(Qt::white);
        ql_painter.drawEllipse(cur_val,4, 16, 16);
        ql_painter.restore();
    } else {
        ql_painter.save();
        QColor ql_color(204,204,204);
        ql_painter.setBrush(ql_color);
        QRectF inactive_rect = QRectF(0 ,0,ql_width,ql_height);
        ql_painter.drawRoundedRect(inactive_rect, 0.5 * ql_height, 0.5 * ql_height);
        ql_painter.restore();                          //kai shi shua
        ql_painter.save();
        ql_painter.setBrush(ql_color);
        QRectF blueRect = QRectF(ql_height * 0.16, ql_height * 0.16,ql_width - ql_height * 0.33, ql_height * 0.67);
        ql_painter.drawRoundedRect(blueRect, 0.45 * ql_height, 0.45 * ql_height);
        ql_painter.restore();
        ql_painter.save();
        ql_painter.setBrush(Qt::white);
        ql_painter.drawEllipse(cur_val,4, 16, 16);
        ql_painter.restore();
    }
}

void QL_SwichButton::set_id(int id) {
    this->id = id;
}

void QL_SwichButton::startAnimation() { //hua dong dong zuo
    int pos = 4;
    int size = ql_width - 16;
    if(on) {
        cur_val ++;                     //wang you hua dong
        if(cur_val > size - pos) {      //dao da zui da zhi ting xia lai
            timer->stop();
        }

    } else {
        cur_val --;
        if(cur_val < pos) {             //dao da zui xiao zhi ting xia lai
            timer->stop();
        }
    }
    update();
}

void QL_SwichButton::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    on = !on;
    emit status(on,id);
    timer->start(1);
    update();
}

int QL_SwichButton::get_swichbutton_val() {
    return this->on;
}

void QL_SwichButton::set_swichbutton_val(int on) {
    this->on = on;
    if(on == 1) {
        cur_val = ql_width - 16 - 4;
    }
    else {
        cur_val = 4;
    }
}
