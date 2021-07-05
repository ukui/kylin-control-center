#include "brightnessFrame.h"
#include <QHBoxLayout>
#include <QtConcurrent>
#include <QGSettings>
#include <QDBusConnection>
#include <QDBusMessage>
#include <unistd.h>
#include <QDBusReply>
#include <QDBusInterface>

#define POWER_SCHMES                     "org.ukui.power-manager"
#define POWER_KEY                        "brightness-ac"

BrightnessFrame::BrightnessFrame(const QString &name, const bool &isBattery, const QString &serialNum, QWidget *parent) :
    QFrame(parent)
{
    this->setFixedHeight(50);
    this->setMinimumWidth(550);
    this->setMaximumWidth(960);
    this->setFrameShape(QFrame::Shape::Box);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(6);
    layout->setMargin(9);

    labelName = new QLabel(this);
    labelName->setFixedWidth(198);

    slider = new Uslider(Qt::Horizontal, this);
    slider->setRange(10, 100);

    labelValue = new QLabel(this);
    labelValue->setFixedWidth(35);
    labelValue->setAlignment(Qt::AlignRight);

    layout->addWidget(labelName);
    layout->addWidget(slider);
    layout->addWidget(labelValue);

    this->outputEnable = true;
    this->connectFlag = true;
    this->busType = "";
    this->exitFlag = false;
    this->isBattery = isBattery;
    this->outputName = name;
    this->serialNum  = serialNum;

    labelValue->setText("0"); //最低亮度10,获取前显示为0
    slider->setEnabled(false); //成功连接了再改为true，否则表示无法修改亮度
}

BrightnessFrame::~BrightnessFrame()
{
    busType = "";
    exitFlag = true;
    threadRun.waitForFinished();
}

void BrightnessFrame::setTextLabelName(QString text)
{
    this->labelName->setText(text);
}

void BrightnessFrame::setTextLabelValue(QString text)
{
    this->labelValue->setText(text);
}

void BrightnessFrame::runConnectThread(const bool &openFlag)
{
    outputEnable = openFlag;
    if (false == isBattery) {
        threadRun = QtConcurrent::run([=]{
            if (serialNum != "" && "" == this->busType) {
                getDDCtype();
            }

            if ("" == this->busType) {
                return;
            }

            int brightnessValue = getDDCBrighthess();
            if (brightnessValue == -1 || !slider || exitFlag)
                return;
            if (brightnessValue > 100) {
                brightnessValue = 100;
            }
            slider->setValue(brightnessValue);
            setTextLabelValue(QString::number(brightnessValue));
            slider->setEnabled(true);
            connect(slider, &QSlider::valueChanged, this, [=](){
                 qDebug()<<outputName<<"brightness"<<" is changed, value = "<<slider->value();
                 setTextLabelValue(QString::number(slider->value()));
                 setDDCBrightness(slider->value());
            });
        });
    } else {
        QByteArray powerId(POWER_SCHMES);
        if (QGSettings::isSchemaInstalled(powerId)) {
            QGSettings *mPowerGSettings = new QGSettings(powerId, QByteArray(), this);

            int brightnessValue = mPowerGSettings->get(POWER_KEY).toInt();
            setTextLabelValue(QString::number(brightnessValue));
            slider->setValue(brightnessValue);
            slider->setEnabled(true);
            connect(slider, &QSlider::valueChanged, this, [=](){
                qDebug()<<outputName<<"brightness"<<" is changed, value = "<<slider->value();
                mPowerGSettings->set(POWER_KEY, slider->value());
                setTextLabelValue(QString::number(mPowerGSettings->get(POWER_KEY).toInt()));
            });
        }
    }
}

void BrightnessFrame::setOutputEnable(const bool &enable)
{
    outputEnable = enable;
}

bool BrightnessFrame::getSliderEnable()
{
    return slider->isEnabled();
}

bool BrightnessFrame::getOutputEnable()
{
    return outputEnable;
}

QString BrightnessFrame::getOutputName()
{
    return outputName;
}

int BrightnessFrame::getDDCBrighthess()
{
    int times = 100;
    QDBusInterface ukccIfc("com.control.center.qt.systemdbus",
                           "/",
                           "com.control.center.interface",
                           QDBusConnection::systemBus());
    QDBusReply<int> reply;
    while (--times) {
        if (busType == "" || exitFlag)
            return -1;
        reply = ukccIfc.call("getDDCBrightness", busType);
        if (reply.isValid() && reply.value() >= 0) {
            return reply.value();
        }
        usleep(80000);
    }
    return -1;
}


void BrightnessFrame::getDDCtype()
{
    QDBusInterface ukccIfc("com.control.center.qt.systemdbus",
                           "/",
                           "com.control.center.interface",
                           QDBusConnection::systemBus());
    qDebug()<<"11111111111111111111111111";
    QDBusReply<QVariantMap> replyMap = ukccIfc.call("getBusMap");
    qDebug()<<"2222222222222222222222222222222";
    QMap<QString, QVariant>::const_iterator replay_i = replyMap.value().constBegin();
    while (replay_i != replyMap.value().constEnd()) {
        if (serialNum == replay_i.key()) {
            QVariant reply = replay_i.value();
            QString serial = reply.toString();
            busType = serial.split("-").at(1);
            return;
        }
        replay_i++;
    }

    busType = "";
    return;
}

void BrightnessFrame::setDDCBrightness(const int &value)
{
    QDBusInterface ukccIfc("com.control.center.qt.systemdbus",
                           "/",
                           "com.control.center.interface",
                           QDBusConnection::systemBus());


    if (mLock.tryLock()) {
        ukccIfc.call("setDDCBrightness", QString::number(value), this->busType);
        mLock.unlock();
    }
}
