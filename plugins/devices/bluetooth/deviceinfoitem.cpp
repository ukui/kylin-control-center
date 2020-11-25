#include "deviceinfoitem.h"

DeviceInfoItem::DeviceInfoItem(QWidget *parent) : QWidget(parent)
{
    this->setMinimumSize(580,50);
    this->setMaximumSize(1000,50);
//    this->setStyleSheet("background:white;");

    info_page = new QFrame(this);
    info_page->setFrameShape(QFrame::Shape::Box);
//    info_page->setStyleSheet("background:blue;");
    info_page->setGeometry(0,0,this->width(),this->height());

    QHBoxLayout *info_page_layout = new QHBoxLayout(info_page);
    info_page_layout->setSpacing(8);
    info_page_layout->setContentsMargins(16,0,16,0);

    device_icon = new QLabel(info_page);
    info_page_layout->addWidget(device_icon);

    device_name = new QLabel(info_page);
    info_page_layout->addWidget(device_name);
    info_page_layout->addStretch();

    device_status = new QLabel(info_page);
    info_page_layout->addWidget(device_status);

    connect_btn = new QPushButton(tr("Connect"),this);
    connect_btn->setStyleSheet("QPushButton{background: #E7E7E7;border-radius: 6px;}");
    connect_btn->setVisible(false);
    connect(connect_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Connect_Btn(bool)));

    disconnect_btn = new QPushButton(tr("Disconnect"),this);
    disconnect_btn->setStyleSheet("QPushButton{background: #E7E7E7;border-radius: 6px;}");
    disconnect_btn->setVisible(false);
    connect(disconnect_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Disconnect_Btn(bool)));

    del_btn = new QPushButton(tr("Remove"),this);
    del_btn->setStyleSheet("QPushButton{background: #E7E7E7;border-radius: 6px;}");
    del_btn->setVisible(false);
    connect(del_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Delete_Btn(bool)));

    icon_timer = new QTimer(this);
    icon_timer->setInterval(200);
}

DeviceInfoItem::~DeviceInfoItem()
{

}

void DeviceInfoItem::initInfoPage(DEVICE_TYPE icon_type, QString d_name, DEVICE_STATUS status, BluezQt::DevicePtr device)
{
    QIcon icon_device,icon_status;
    if(icon_type == DEVICE_TYPE::PC){
        icon_device = QIcon::fromTheme("video-display-symbolic");
        device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));
    }else if(icon_type == DEVICE_TYPE::PHONE){
        icon_device = QIcon::fromTheme("phone-apple-iphone-symbolic");
        device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));
    }else if(icon_type == DEVICE_TYPE::HEADSET){
        icon_device = QIcon::fromTheme("audio-headphones-symbolic");
        device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));
    }else{
        icon_device = QIcon::fromTheme("bluetooth-symbolic");
        device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));
    }

    if(d_name.isEmpty()){
        return;
    }
    device_name->setText(d_name);

    d_status = status;
    device_item = device;

    if(status == DEVICE_STATUS::LINK){
        icon_status = QIcon::fromTheme("emblem-ok-symbolic");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }else if(status == DEVICE_STATUS::UNLINK){
        icon_status = QIcon::fromTheme("emblem-danger");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }
}

void DeviceInfoItem::resizeEvent(QResizeEvent *event)
{
//    this->resize(event->size());
    info_page->resize(event->size());
}

void DeviceInfoItem::enterEvent(QEvent *event)
{
//    qDebug() << Q_FUNC_INFO << device_name->text();
    QPropertyAnimation *enter_action = new QPropertyAnimation(info_page,"geometry");
    enter_action->setDuration(300);
    enter_action->setStartValue(QRect(0, 0, info_page->width(), info_page->height()));
    enter_action->setEndValue(QRect(0, 0, info_page->width()-170, info_page->height()));
    enter_action->start();

    connect(enter_action,&QPropertyAnimation::finished,this,[=]{
        if (d_status == DEVICE_STATUS::LINK){
            disconnect_btn->setGeometry(this->width()-155,2,80,45);
            disconnect_btn->setVisible(true);
        }else if (d_status == DEVICE_STATUS::UNLINK){
            connect_btn->setGeometry(this->width()-155,2,80,45);
            connect_btn->setVisible(true);
        }

        del_btn->setGeometry(this->width()-65,2,65,45);
        del_btn->setVisible(true);
    });
}

void DeviceInfoItem::leaveEvent(QEvent *event)
{

    if (d_status == DEVICE_STATUS::LINK){
        disconnect_btn->setVisible(false);
    }else if (d_status == DEVICE_STATUS::UNLINK){
        connect_btn->setVisible(false);
    }

    del_btn->setVisible(false);

    QPropertyAnimation *leave_action = new QPropertyAnimation(info_page,"geometry");
    leave_action->setDuration(300);
    leave_action->setStartValue(QRect(0, 0, info_page->width(), info_page->height()));
    leave_action->setEndValue(QRect(0, 0, this->width(), info_page->height()));
    leave_action->start();

}

void DeviceInfoItem::onClick_Connect_Btn(bool isclicked)
{
    qDebug() << Q_FUNC_INFO;
    emit sendConnectDevice(device_item);

    connect(icon_timer,&QTimer::timeout,this,[=]{
        if(i == 0)
            i = 7;
        device_status->setPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(i,10)).pixmap(24,24));
        device_status->update();
        i--;
    });
    icon_timer->start();
}

void DeviceInfoItem::onClick_Disconnect_Btn(bool isclicked)
{
    qDebug() << Q_FUNC_INFO;
}

void DeviceInfoItem::onClick_Delete_Btn(bool isclicked)
{
    qDebug() << Q_FUNC_INFO;
}

void DeviceInfoItem::setTimerStop(QString dev_name)
{
    if(dev_name == device_item->name() || dev_name == "")
    {
        icon_timer->stop();
    }
}

void DeviceInfoItem::setTimerStart(QString dev_name)
{
    if(dev_name == device_item->name() || dev_name == "")
    {
        icon_timer->start();
    }
}

void DeviceInfoItem::updateDeviceStatus(DEVICE_STATUS status)
{
    QIcon icon_status;
    if(status == DEVICE_STATUS::LINK){
        icon_status = QIcon::fromTheme("emblem-default");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }else if(status == DEVICE_STATUS::UNLINK){
        icon_status = QIcon::fromTheme("emblem-important");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }
}
