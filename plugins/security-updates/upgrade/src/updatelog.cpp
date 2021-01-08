#include "updatelog.h"


UpdateLog::UpdateLog(QWidget *parent) : QWidget(parent)
{
    logUi();
    logStyle();

}
void UpdateLog::logUi()
{
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->setFixedSize(460,560);

    logVLayout = new QVBoxLayout();
    logMainLayout = new QVBoxLayout();

    logAppHLayout = new QHBoxLayout();
    logAppVLayout = new QVBoxLayout();

    logAppIcon = new QLabel(this);
    logAppName = new QLabel(this);
    logAppVerson = new QLabel(this);

    logWidget = new QWidget(this);
    logAppWidget = new QWidget(this);

    logContent = new QTextEdit(this);

    logAppName->setText("麒麟音乐更新日志");
    logAppVerson->setText("当前版本 1.0.2");
    logAppIcon->setFixedSize(48,48);

    //    logContent->setFixedSize(396,411);


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
    logVLayout->addWidget(logContent);
    logVLayout->setContentsMargins(24,0,24,24);
//    logVLayout->setMargin(0);
    logVLayout->setSpacing(0);







    logWidget->setLayout(logVLayout);


    logMainLayout->addWidget(logWidget);
    logMainLayout->setMargin(5);
    logMainLayout->setSpacing(0);


    this->setLayout(logMainLayout);
}

void UpdateLog::logStyle()
{

}

// 实现圆角阴影效果
void UpdateLog::paintEvent(QPaintEvent *event)
{
//    WidgetStyle::paintEvent(event, this);
}

//void UpdateLog::closeUpdateLog()
//{
//    this->close();
//}
