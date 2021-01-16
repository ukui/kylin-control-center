#include "checkbutton.h"
#include <QDebug>

m_button::m_button(QWidget *parent) : QPushButton(parent)
{
    m_cTimer = new QTimer(this);
    states =true;
    m_cTimer->setInterval(140);//设置动画速度
    buttonshow();

}

void m_button::buttonshow()
{
    //通过定时器实现循环插入载入图片
    connect(m_cTimer,&QTimer::timeout, [this] () {
            QIcon icon;
            QString str = QString(":/img/plugins/upgrade/loading%1.svg").arg(i);
            icon.addFile(str);
            this->setIcon(icon);
            i--;
            if(i==9){
                i=17;
            }
        });
}

void m_button::start()
{
    this->setText("");
    m_cTimer->start();
    states = false;
}

void m_button::stop()
{
    QIcon icon;
    this->setIcon(icon);
    m_cTimer->stop();
    states = true;
}

void m_button::buttonswitch()
{
    //两态开关，实现一个按钮开始暂停功能
    qDebug()<<states;
    if(states){
        start();
    }else{
        stop();
    }
}
