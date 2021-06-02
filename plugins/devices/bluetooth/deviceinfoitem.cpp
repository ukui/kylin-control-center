#include "deviceinfoitem.h"

DeviceInfoItem::DeviceInfoItem(QWidget *parent) : QWidget(parent)
{
    if(QGSettings::isSchemaInstalled("org.ukui.style")){
        item_gsettings = new QGSettings("org.ukui.style");
        connect(item_gsettings,&QGSettings::changed,this,&DeviceInfoItem::GSettingsChanges);
    }

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
    connect_btn->setVisible(false);
    connect(connect_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Connect_Btn(bool)));

    disconnect_btn = new QPushButton(tr("Disconnect"),this);
    disconnect_btn->setVisible(false);
    connect(disconnect_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Disconnect_Btn(bool)));

    del_btn = new QPushButton(tr("Remove"),this);
    del_btn->setVisible(false);
    connect(del_btn,SIGNAL(clicked(bool)),this,SLOT(onClick_Delete_Btn(bool)));
}

DeviceInfoItem::~DeviceInfoItem()
{

}

void DeviceInfoItem::initInfoPage(QString d_name, DEVICE_STATUS status, BluezQt::DevicePtr device)
{
    this->setObjectName(device->address());

    connect(device.data(),&BluezQt::Device::pairedChanged,this,[=](bool paird){
        qDebug() << Q_FUNC_INFO  << "pairedChanged" << paird;
        changeDevStatus(paird);
    });
    connect(device.data(),&BluezQt::Device::connectedChanged,this,[=](bool connected){
        qDebug() << Q_FUNC_INFO << "connectedChanged" << connected;
        setDevConnectedIcon(connected);
    });
    connect(device.data(),&BluezQt::Device::nameChanged,this,[=](QString name){
        qDebug() << Q_FUNC_INFO << "nameChanged" << name;
        device_name->setText(name);
    });

    QIcon icon_device,icon_status;
    if(device->type() == BluezQt::Device::Computer){
        icon_device = QIcon::fromTheme("computer-symbolic");
    }else if(device->type() == BluezQt::Device::Phone){
        icon_device = QIcon::fromTheme("phone-apple-iphone-symbolic");
    }else if((device->type() == BluezQt::Device::Headset)||(device->type() == BluezQt::Device::Headphones)){
        icon_device = QIcon::fromTheme("audio-headphones-symbolic");
    }else if(device->type() == BluezQt::Device::Mouse){
        icon_device = QIcon::fromTheme("input-mouse-symbolic");
    }else if(device->type() == BluezQt::Device::Keyboard){
        icon_device = QIcon::fromTheme("input-keyboard-symbolic");
    }else{
        icon_device = QIcon::fromTheme("bluetooth-symbolic");
    }
    device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));

    if(d_name.isEmpty()){
        return;
    }
    device_name->setText(d_name);

    d_status = status;
    device_item = device;

    if(d_status == DEVICE_STATUS::LINK){
        icon_status = QIcon::fromTheme("ukui-dialog-success");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }
//    else if(status == DEVICE_STATUS::UNLINK){
//        icon_status = QIcon::fromTheme("software-update-available-symbolic");
//        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
//    }

    if(item_gsettings->get("style-name").toString() == "ukui-black" || item_gsettings->get("style-name").toString() == "ukui-dark")
    {
        device_icon->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
        device_icon->setProperty("useIconHighlightEffect", 0x10);
        device_status->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
        device_status->setProperty("useIconHighlightEffect", 0x10);
    }

    AnimationInit();
}
void DeviceInfoItem::refresh_device_icon(BluezQt::Device::Type changeType)
{
    QIcon icon_device;
    if(changeType == BluezQt::Device::Computer){
        icon_device = QIcon::fromTheme("computer-symbolic");
    }else if(changeType == BluezQt::Device::Phone){
        icon_device = QIcon::fromTheme("phone-apple-iphone-symbolic");
    }else if((changeType == BluezQt::Device::Headset)||(changeType == BluezQt::Device::Headphones)){
        icon_device = QIcon::fromTheme("audio-headphones-symbolic");
    }else if(changeType == BluezQt::Device::Mouse){
        icon_device = QIcon::fromTheme("input-mouse-symbolic");
    }else if(changeType == BluezQt::Device::Keyboard){
        icon_device = QIcon::fromTheme("input-keyboard-symbolic");
    }else{
        icon_device = QIcon::fromTheme("bluetooth-symbolic");
    }

    device_icon->setPixmap(icon_device.pixmap(QSize(24,24)));
    device_icon->update();

}


QString DeviceInfoItem::get_dev_name()
{
    return device_item->name();
}

void DeviceInfoItem::resizeEvent(QResizeEvent *event)
{
//    this->resize(event->size());
    info_page->resize(event->size());
}

void DeviceInfoItem::enterEvent(QEvent *event)
{
    AnimationFlag = true;

    if (device_status->isVisible())
    {
        if (LINK == d_status)
        {
            device_status->setToolTip(tr("Device connected"));
        }
        else
        {
            device_status->setToolTip(tr("Device not connected"));
        }
        //else //正在连接状态为做对一切换，暂时不加入
        //{
        //    device_status->setToolTip(tr("Connecting device"));
        //}

    }

    mouse_timer->start();
}

void DeviceInfoItem::leaveEvent(QEvent *event)
{
//    QDateTime current_date_time = QDateTime::currentDateTime();
//    QString current_time = current_date_time.toString("hh:mm:ss.zzz ");
//    qDebug() << Q_FUNC_INFO << current_time;
    AnimationFlag = false;

    disconnect_btn->setVisible(false);
    connect_btn->setVisible(false);
    del_btn->setVisible(false);

    leave_action->setStartValue(QRect(0, 0, info_page->width(), info_page->height()));
    leave_action->setEndValue(QRect(0, 0, this->width(), info_page->height()));
    leave_action->start();
}

void DeviceInfoItem::onClick_Connect_Btn(bool isclicked)
{
    if(!icon_timer&&!connect_timer){
        icon_timer = new QTimer(this);
        icon_timer->setInterval(100);

        connect_timer = new QTimer(this);
        connect_timer->setInterval(10000);

        connect(connect_timer,&QTimer::timeout,this,[=]{
            if(icon_timer->isActive()){
                icon_timer->stop();
                device_status->setPixmap(QIcon::fromTheme("emblem-danger").pixmap(QSize(24,24)));
                device_status->update();
            }
            connect_timer->stop();
        });

        emit sendConnectDevice(device_item->address());

        i = 7;

        if(!device_status->isVisible())
            device_status->setVisible(true);

        connect(icon_timer,&QTimer::timeout,this,[=]{
            if(i == -1)
                i = 7;
            device_status->setPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(i,10)).pixmap(24,24));
            device_status->update();
            i--;
        });
        connect_timer->start(10000);
        icon_timer->start(100);

    }else{
        emit sendConnectDevice(device_item->address());
        connect_timer->start(10000);
        icon_timer->start(100);
        if(!device_status->isVisible())
            device_status->setVisible(true);
    }
}

void DeviceInfoItem::onClick_Disconnect_Btn(bool isclicked)
{
//    qDebug() << Q_FUNC_INFO;
    emit sendDisconnectDeviceAddress(device_item->address());
}

void DeviceInfoItem::onClick_Delete_Btn(bool isclicked)
{
//    qDebug() << Q_FUNC_INFO;
//    this->setVisible(false);
    emit sendDeleteDeviceAddress(device_item->address());
}

void DeviceInfoItem::changeDevStatus(bool pair)
{
    if(icon_timer && icon_timer->isActive())
        icon_timer->stop();

    if(pair){
        if (!device_item->isConnected()){
            device_status->setVisible(false);
            d_status = DEVICE_STATUS::UNLINK;
        }else{
            device_status->setVisible(true);
            d_status = DEVICE_STATUS::LINK;
            QIcon icon_status = QIcon::fromTheme("ukui-dialog-success");
            device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
        }
        emit sendPairedAddress(device_item->address());
    }else{
//        QIcon icon_status = QIcon::fromTheme("software-installed-symbolic");
//        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));
    }
}

void DeviceInfoItem::setDevConnectedIcon(bool connected)
{
    if(icon_timer && icon_timer->isActive())
        icon_timer->stop();

    if(connected && device_item->isPaired()){
        d_status = DEVICE_STATUS::LINK;
        device_status->setVisible(true);
        QIcon icon_status = QIcon::fromTheme("ukui-dialog-success");
        device_status->setPixmap(icon_status.pixmap(QSize(24,24)));

        if(connect_btn->isVisible()){
            connect_btn->setVisible(false);
            disconnect_btn->setGeometry(this->width()-BTN_1_X,2,BTN_1_WIDTH,45);
            disconnect_btn->setVisible(true);
        }

    }else{
        if(disconnect_btn->isVisible()){
            disconnect_btn->setVisible(false);
            connect_btn->setGeometry(this->width()-BTN_1_X,2,BTN_1_WIDTH,45);
            connect_btn->setVisible(true);
        }

        d_status = DEVICE_STATUS::UNLINK;
        device_status->setVisible(false);
    }
}

void DeviceInfoItem::AnimationInit()
{
    mouse_timer = new QTimer(this);
    mouse_timer->setInterval(300);

    connect(mouse_timer,&QTimer::timeout,this,[=]{
        if(AnimationFlag){
            if(leave_action->state() != QAbstractAnimation::Running){
                enter_action->setStartValue(QRect(0, 0, info_page->width(), info_page->height()));
                enter_action->setEndValue(QRect(0, 0, info_page->width()-((device_item->isPaired() && device_item->type() != BluezQt::Device::Mouse && device_item->type() != BluezQt::Device::Keyboard)?ITEM_WIDTH:ITEM_WIDTH1), info_page->height()));
                enter_action->start();
            }
        }
        mouse_timer->stop();
    });

    enter_action = new QPropertyAnimation(info_page,"geometry");
    enter_action->setDuration(0);
    enter_action->setEasingCurve(QEasingCurve::OutQuad);

    connect(enter_action,&QPropertyAnimation::finished,this,[=]{
        if (device_item->isPaired()) {
            if (device_item->type() != BluezQt::Device::Mouse && device_item->type() != BluezQt::Device::Keyboard) {
                if (d_status == DEVICE_STATUS::LINK){
                    disconnect_btn->setGeometry(this->width()-BTN_1_X,2,BTN_1_WIDTH,45);
                    disconnect_btn->setVisible(true);
                }else if (d_status == DEVICE_STATUS::UNLINK){
                    connect_btn->setGeometry(this->width()-BTN_1_X,2,BTN_1_WIDTH,45);
                    connect_btn->setVisible(true);
                }
                del_btn->setGeometry(this->width()-BTN_2_X,2,BTN_2_WIDTH,45);
                del_btn->setVisible(true);
            }else{
                del_btn->setGeometry(this->width()-125,2,BTN_1_WIDTH,45);
                del_btn->setVisible(true);
            }
        } else {
            connect_btn->setGeometry(this->width()-125,2,BTN_1_WIDTH,45);
            connect_btn->setVisible(true);
        }
    });


    leave_action = new QPropertyAnimation(info_page,"geometry");
    leave_action->setDuration(0);
    leave_action->setEasingCurve(QEasingCurve::InQuad);
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

void DeviceInfoItem::GSettingsChanges(const QString &key)
{
    qDebug() << Q_FUNC_INFO << key;
    if(key == "styleName"){
        if(item_gsettings->get("style-name").toString() == "ukui-black" || item_gsettings->get("style-name").toString() == "ukui-dark")
        {
            device_icon->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
            device_icon->setProperty("useIconHighlightEffect", 0x10);
            device_status->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
            device_status->setProperty("useIconHighlightEffect", 0x10);
        }else{
            device_icon->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::black));
            device_icon->setProperty("useIconHighlightEffect", 0x10);
            device_status->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
            device_status->setProperty("useIconHighlightEffect", 0x10);
        }
    }
}
