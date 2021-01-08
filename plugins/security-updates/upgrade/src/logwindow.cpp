#include "logwindow.h"

LogWindow::LogWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags (Qt::FramelessWindowHint);
    this->setFixedSize(500,600);
    m_log = new m_updatelog(this);
    connect(m_log,&m_updatelog::closesignal,[=](){
        this->close();
    });

}

LogWindow::~LogWindow()
{
}


