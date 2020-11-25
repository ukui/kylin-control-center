#include "pincodewidget.h"

PinCodeWidget::PinCodeWidget(QString name, QString pin)
   : dev_name(name),
     PINCode(pin)
{
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(420,330);
    this->setStyleSheet("QDialog{background:white;}");

    QString top_text = tr("Is it paired with \"")+dev_name+tr("\"");
    QString tip_text = tr("Please make sure the number displayed on \"")+dev_name+tr("\" matches the number below. Please do not enter this code on any other accessories.");
    QLabel *top_label = new QLabel(top_text,this);
    top_label->setStyleSheet("QLabel{\
                             font-size: 18px;\
                             font-family: PingFangSC-Medium, PingFang SC;\
                             font-weight: 500;\
                             color: rgba(0, 0, 0, 0.85);\
                             line-height: 25px;}");
    top_label->setGeometry(32,48,334,25);

    QLabel *tip_label = new QLabel(tip_text,this);
    tip_label->setStyleSheet("QLabel{\
                             font-size: 14px;\
                             font-family: PingFangSC-Regular, PingFang SC;\
                             font-weight: 400;\
                             color: rgba(0, 0, 0, 0.85);\
                             line-height: 20px;}");
    tip_label->setGeometry(32,89,359,60);
    tip_label->setWordWrap(true);

    QLabel *PIN_label = new QLabel(PINCode,this);
    PIN_label->setStyleSheet("QLabel{\
                             font-size: 36px;\
                             font-family: ArialMT;\
                             color: rgba(0, 0, 0, 0.85);\
                             line-height: 42px;}");
    PIN_label->setGeometry(151,166,120,40);

    accept_btn = new QPushButton(tr("Accept"),this);
    accept_btn->setStyleSheet("QPushButton{\
                              background: #E7E7E7;\
                              border-radius: 6px;\
                              border: 0px solid #979797;}\
                              QPushButton:hover{background:rgba(67,127,240,1);\
                              border-radius:6px;color:white;border: 0px solid #979797;}");
    accept_btn->setGeometry(60,255,120,36);
    connect(accept_btn,&QPushButton::clicked,this,&PinCodeWidget::onClick_accept_btn);

    refuse_btn = new QPushButton(tr("Refush"),this);
    refuse_btn->setStyleSheet("QPushButton{\
                              background: #E7E7E7;\
                              border-radius: 6px;\
                              border: 0px solid #979797;}\
                              QPushButton:hover{background:rgba(67,127,240,1);\
                              border-radius:6px;color:white;border: 0px solid #979797;}");
    refuse_btn->setGeometry(250,255,120,36);
    connect(refuse_btn,&QPushButton::clicked,this,&PinCodeWidget::onClick_refuse_btn);

    close_btn = new QPushButton(this);
    QIcon icon = QIcon::fromTheme("window-close-symbolic");
    close_btn->setIcon(icon);
    close_btn->setIconSize(QSize(13,13));
    close_btn->setGeometry(393,14,13,13);
    connect(close_btn,&QPushButton::clicked,this,&PinCodeWidget::onClick_close_btn);
}

PinCodeWidget::~PinCodeWidget()
{

}

void PinCodeWidget::onClick_close_btn(bool)
{
    this->close();
    emit this->rejected();
}

void PinCodeWidget::onClick_accept_btn(bool)
{
    this->close();
    emit this->accepted();
}

void PinCodeWidget::onClick_refuse_btn(bool)
{
    this->close();
    emit this->rejected();
}
