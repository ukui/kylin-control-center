#include "loadinglabel.h"

LoadingLabel::LoadingLabel(QObject *parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(Refresh_icon()));
    this->setPixmap(QIcon::fromTheme("ukui-loading-"+QString("%1").arg(i)).pixmap(this->width(),this->height()));
}

LoadingLabel::~LoadingLabel()
{
//    delete m_timer;
}

void LoadingLabel::setTimerStop()
{
    m_timer->start();
}

void LoadingLabel::setTimerStart()
{
    m_timer->stop();
}

void LoadingLabel::setTimeReresh(int m)
{
    m_timer->setInterval(m);
}

void LoadingLabel::Refresh_icon()
{
    qDebug() << Q_FUNC_INFO;
    if(i == 8)
        i = 0;
    this->setPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(i,10)).pixmap(this->width(),this->height()));
    this->update();
    i++;
}
