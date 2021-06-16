#include "leftsiderbarwidget.h"
#include <QDebug>
#include <QGSettings>
#include <QStyleOption>
#include <QPainter>

#define PERSONALSIE_SCHEMA     "org.ukui.control-center.personalise"
#define PERSONALSIE_TRAN_KEY   "transparency"
#define CONTAIN_PERSONALSIE_TRAN_KEY   "transparency"

LeftsiderbarWidget::LeftsiderbarWidget(QWidget *parent)
    : QWidget(parent)
{
    QGSettings *personalQgsettings = nullptr;
    if (QGSettings::isSchemaInstalled(PERSONALSIE_SCHEMA)) {
        personalQgsettings = new QGSettings(PERSONALSIE_SCHEMA, QByteArray(), this);
        transparency = personalQgsettings->get(PERSONALSIE_TRAN_KEY).toDouble() * 255;
        connect(personalQgsettings,&QGSettings::changed,this,[=](QString changedKey) {  //监听透明度变化
                        if (changedKey == CONTAIN_PERSONALSIE_TRAN_KEY) {
                           transparency = personalQgsettings->get(PERSONALSIE_TRAN_KEY).toDouble() * 255;
                           this->repaint();
                        }
                });
    } else {
        personalQgsettings = nullptr;
        qDebug()<<PERSONALSIE_SCHEMA<<" not installed";
    }
}

LeftsiderbarWidget::~LeftsiderbarWidget()
{

}


void LeftsiderbarWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    QColor color = palette().color(QPalette::Window);
    color.setAlpha(transparency);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Window,QColor(color));
    this->setPalette(pal);
    QBrush brush =QBrush(color);
    p.setBrush(brush);
    p.drawRoundedRect(opt.rect,0,0);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
