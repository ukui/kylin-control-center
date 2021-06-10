#include "updatelog.h"


UpdateLog::UpdateLog(QWidget *parent) : QDialog(parent)
{
    logUi();

}
void UpdateLog::logUi()
{
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->setFixedSize(460,560);
    if(QLocale::system().name()!="zh_CN")
    {
        this->setWindowTitle(tr("Update log"));
    }
    else
    {
        this->setWindowTitle("更新日志");
    }
//    this->setWindowTitle("更新日志");

    logVLayout = new QVBoxLayout();
    logMainLayout = new QVBoxLayout();

    logAppHLayout = new QHBoxLayout();
    logAppVLayout = new QVBoxLayout();

    logAppIcon = new QLabel(this);
    logAppIcon->setFixedSize(48,48);

    logAppName = new QLabel(this);
    logAppVerson = new QLabel(this);

    logWidget = new QWidget(this);
    logAppWidget = new QWidget(this);


    logContent = new QTextEdit(this);
    QPalette pl = logContent->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    logContent->setPalette(pl);
    contentFrame = new QFrame(this);
    contentFrame->setFrameShape(QFrame::Box);
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentFrame->setLayout(contentLayout);
    contentLayout->addWidget(logContent);
    contentLayout->setMargin(0);
    contentLayout->setSpacing(0);
    logContent->setReadOnly(true);

    logAppVLayout->addWidget(logAppName);
    logAppVLayout->addWidget(logAppVerson);
    logAppVLayout->setMargin(0);
    logAppVLayout->setSpacing(0);
    logAppHLayout->addWidget(logAppIcon);
    logAppHLayout->addLayout(logAppVLayout);
    logAppHLayout->setMargin(0);
    logAppHLayout->setSpacing(8);
    logAppWidget->setLayout(logAppHLayout);

    logVLayout->addWidget(logAppWidget);
    logVLayout->addSpacing(12);
    logVLayout->addWidget(contentFrame);
    logVLayout->setContentsMargins(24,0,24,24);
    logVLayout->setSpacing(0);
    logWidget->setLayout(logVLayout);


    logMainLayout->addWidget(logWidget);
    logMainLayout->setMargin(5);
    logMainLayout->setSpacing(0);


    this->setLayout(logMainLayout);
}

// 实现圆角阴影效果
void UpdateLog::paintEvent(QPaintEvent *event)
{
//    WidgetStyle::paintEvent(event, this);
}


