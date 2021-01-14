#include "bluetoothmain.h"

#include <QDBusObjectPath>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>
#include <QDBusMessage>
//#include <polkit-qt5-1/PolkitQt1/Authority>
#include <QMessageLogger>

BlueToothMain::BlueToothMain(QWidget *parent)
    : QMainWindow(parent)
{
    if(QGSettings::isSchemaInstalled("org.ukui.bluetooth")){
        settings = new QGSettings("org.ukui.bluetooth");

        paired_device_address.clear();
        paired_device_address = settings->get("paired-device-address").toStringList();
        finally_connect_the_device = settings->get("finally-connect-the-device").toString();
        Default_Adapter = settings->get("adapter-address").toString();

        qDebug() << "GSetting Value: " << Default_Adapter << finally_connect_the_device << paired_device_address;
    }
//    this->setMinimumSize(582,500);

    m_manager = new BluezQt::Manager(this);
    job = m_manager->init();
    job->exec();

    m_localDevice = m_manager->adapterForAddress(Default_Adapter);
    connect(m_localDevice.data(),&BluezQt::Adapter::poweredChanged,this,&BlueToothMain::adapterPoweredChanged);

    qDebug() << m_manager->isOperational();
    qDebug() << m_localDevice->name() << m_localDevice->isPowered() << m_localDevice->isDiscoverable() << m_localDevice->isDiscovering() << m_localDevice->address();

    connect(m_localDevice.data(),&BluezQt::Adapter::deviceAdded,this,&BlueToothMain::serviceDiscovered);
    connect(m_localDevice.data(),&BluezQt::Adapter::nameChanged,this,[=](const QString &name){
        emit this->adapter_name_changed(name);
    });

    main_widget = new QWidget(this);
    this->setCentralWidget(main_widget);

    main_layout = new QVBoxLayout(main_widget);
    main_layout->setSpacing(40);
    main_layout->setContentsMargins(0,0,20,10);
    frame_top    = new QWidget(main_widget);
    frame_top->setObjectName("frame_top");
    frame_top->setMinimumSize(582,135);
    frame_top->setMaximumSize(1000,135);
//    frame_top->setStyleSheet("background:blue;");
    frame_middle = new QWidget(main_widget);
    frame_middle->setObjectName("frame_middle");
//    frame_middle->setMinimumSize(582,135);
//    frame_middle->setStyleSheet("background:blue;");
    frame_bottom = new QWidget(main_widget);
    frame_bottom->setObjectName("frame_bottom");
//    frame_bottom->setMinimumWidth(582);
//    frame_bottom->setMaximumWidth(1000);
//    frame_bottom->setMinimumHeight(340);
//    frame_bottom->setStyleSheet("background:green;");

    main_layout->addWidget(frame_top);
    main_layout->addWidget(frame_middle);
    main_layout->addWidget(frame_bottom);
    main_layout->addStretch();

    Discovery_device_address.clear();

    InitMainTopUI();
    InitMainMiddleUI();
    InitMainbottomUI();
    if(m_localDevice->isPowered()){
        this->startDiscovery();
//        label_2->setText(tr("Turn off Bluetooth"));
        label_2->setText(tr("Turn on Bluetooth"));
    }else{
        label_2->setText(tr("Turn on Bluetooth"));
        frame_bottom->setVisible(false);
    }
    this->setLayout(main_layout);
}

/*
 * Initialize the fixed UI in the upper half of the interface
 *
 */
void BlueToothMain::InitMainTopUI()
{
    QLabel *label_1 = new QLabel(tr("Bluetooth"),frame_top);
    label_1->setFixedSize(100,25);
    label_1->setStyleSheet("QLabel{\
                           font-size: 18px;\
                           font-family: PingFangSC-Medium, PingFang SC;\
                           font-weight: 500;\
                           line-height: 25px;}");

    QVBoxLayout *top_layout = new QVBoxLayout();
    top_layout->setSpacing(8);
    top_layout->setContentsMargins(0,0,0,0);
    top_layout->addWidget(label_1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(0,0,0,0);
    top_layout->addLayout(layout);

    QFrame *frame_1 = new QFrame(frame_top);
    frame_1->setMinimumWidth(582);
    frame_1->setFrameShape(QFrame::Shape::Box);
    frame_1->setFixedHeight(50);
    frame_1->setAutoFillBackground(true);
    layout->addWidget(frame_1);

    QHBoxLayout *frame_1_layout = new QHBoxLayout(frame_1);
    frame_1_layout->setSpacing(10);
    frame_1_layout->setContentsMargins(16,0,16,0);

    label_2 = new QLabel(frame_1);
    label_2->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-family: PingFangSC-Regular, PingFang SC;\
                           font-weight: 400;\
                           line-height: 20px;}");
    frame_1_layout->addWidget(label_2);
    frame_1_layout->addStretch();

    bluetooth_name = new BluetoothNameLabel(frame_1,300,38);
    connect(bluetooth_name,&BluetoothNameLabel::send_adapter_name,this,&BlueToothMain::change_adapter_name);
    connect(this,&BlueToothMain::adapter_name_changed,bluetooth_name,&BluetoothNameLabel::set_label_text);
    frame_1_layout->addWidget(bluetooth_name);

    open_bluetooth = new SwitchButton(frame_1);

    frame_1_layout->addWidget(open_bluetooth);

    QFrame *frame_2 = new QFrame(frame_top);
    frame_2->setMinimumWidth(582);
    frame_2->setFrameShape(QFrame::Shape::Box);
    frame_2->setFixedHeight(50);
    layout->addWidget(frame_2);

    QHBoxLayout *frame_2_layout = new QHBoxLayout(frame_2);
    frame_2_layout->setSpacing(10);
    frame_2_layout->setContentsMargins(16,0,16,0);

    QLabel *label_3 = new QLabel(tr("Show icon on taskbar"),frame_2);
    label_3->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-family: PingFangSC-Regular, PingFang SC;\
                           font-weight: 400;\
                           line-height: 20px;}");
    frame_2_layout->addWidget(label_3);
    frame_2_layout->addStretch();

    show_panel = new SwitchButton(frame_2);
    frame_2_layout->addWidget(show_panel);
    show_panel->setChecked(settings->get("tray-show").toBool());
    connect(show_panel,&SwitchButton::checkedChanged,this,&BlueToothMain::set_tray_visible);

//    qDebug() << m_localDevice->isValid() << m_localDevice->hostMode() /*<< open_bluetooth->isChecked*/;

//    bluetooth_name->set_dev_name(tr("Can now be found as ")+"\""+m_localDevice->name()+"\"");
    bluetooth_name->set_dev_name(m_localDevice->name());

    if(m_localDevice->isPowered()){
        open_bluetooth->setChecked(true);
        bluetooth_name->setVisible(true);
    }
    else{
        open_bluetooth->setChecked(false);
        bluetooth_name->setVisible(false);
    }

    connect(open_bluetooth,SIGNAL(checkedChanged(bool)),this,SLOT(onClick_Open_Bluetooth(bool)));
    frame_top->setLayout(top_layout);
}

void BlueToothMain::InitMainMiddleUI()
{
    QVBoxLayout *middle_layout = new QVBoxLayout(frame_middle);
    middle_layout->setSpacing(8);
    middle_layout->setContentsMargins(0,0,0,0);

    paired_dev_layout = new QVBoxLayout();
    paired_dev_layout->setSpacing(2);
    paired_dev_layout->setContentsMargins(0,0,0,0);

    QLabel *middle_label = new QLabel(tr("My Devices"),frame_middle);
    middle_label->setStyleSheet("QLabel{width: 72px;\
                                height: 25px;\
                                font-size: 18px;\
                                font-family: PingFangSC-Medium, PingFang SC;\
                                font-weight: 500;\
                                line-height: 25px;}");

    middle_layout->addWidget(middle_label,Qt::AlignTop);
    middle_layout->addLayout(paired_dev_layout,Qt::AlignTop);

    bool show_flag = false;
    for(int i = 0;i < m_localDevice->devices().size(); i++){
//        qDebug() << m_localDevice->devices().at(i)->name() << m_localDevice->devices().at(i)->type();
        if(!m_localDevice->devices().at(i)->isPaired())
            continue;

        show_flag = true;

        DeviceInfoItem *item = new DeviceInfoItem();
        connect(item,SIGNAL(sendConnectDevice(QString)),this,SLOT(receiveConnectsignal(QString)));
        connect(item,SIGNAL(sendDisconnectDeviceAddress(QString)),this,SLOT(receiveDisConnectSignal(QString)));
        connect(item,SIGNAL(sendDeleteDeviceAddress(QString)),this,SLOT(receiveRemoveSignal(QString)));
        connect(item,SIGNAL(sendPairedAddress(QString)),this,SLOT(change_device_parent(QString)));
        if(m_localDevice->devices().at(i)->isConnected())
            item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::LINK, m_localDevice->devices().at(i));
        else
            item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::UNLINK, m_localDevice->devices().at(i));

        paired_dev_layout->addWidget(item,Qt::AlignTop);
    }

    frame_middle->setLayout(middle_layout);

    qDebug() << Q_FUNC_INFO << m_localDevice->devices().size() << show_flag;
    if(!show_flag){
        frame_middle->setVisible(false);
    }
}

void BlueToothMain::InitMainbottomUI()
{
    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->setSpacing(10);
    title_layout->setContentsMargins(0,0,10,0);

    QLabel *label_1 = new QLabel(tr("Other Devices"),frame_bottom);
    label_1->setStyleSheet("QLabel{\
                          width: 72px;\
                          height: 25px;\
                          font-size: 18px;\
                          font-family: PingFangSC-Medium, PingFang SC;\
                          font-weight: 500;\
                          line-height: 25px;}");

    loadLabel = new QLabel(frame_bottom);
    loadLabel->setFixedSize(24,24);

    discover_refresh = new QPushButton(tr("Refresh"),frame_bottom);
    discover_refresh->setFixedSize(80,32);

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    discovering_timer = new QTimer(this);
    discovering_timer->setInterval(30000);
    connect(discovering_timer,&QTimer::timeout,this,[=]{
        discovering_timer->stop();
        if(m_localDevice->isDiscovering()){
            m_localDevice->stopDiscovery();
        }
    });

    connect(m_timer,&QTimer::timeout,this,&BlueToothMain::Refresh_load_Label_icon);

    //当适配器查找设备状态改变时，改变加载动画和刷新按钮的状态
    connect(m_localDevice.data(),&BluezQt::Adapter::discoveringChanged,this,[=](bool discover){
       if(discover){
           loadLabel->setVisible(true);
           m_timer->start();
           discover_refresh->setEnabled(false);
           discovering_timer->start();
       }else{
           loadLabel->setVisible(false);
           m_timer->stop();
           discover_refresh->setEnabled(true);
           discovering_timer->stop();
       }
    });

    //点击刷新按钮，开启适配器查找周围的蓝牙设备
    connect(discover_refresh,&QPushButton::clicked,this,[=]{
        discovering_timer->start();
        startDiscovery();
    });

    title_layout->addWidget(label_1);
    title_layout->addWidget(loadLabel);
    title_layout->addStretch();
    title_layout->addWidget(discover_refresh,Qt::AlignVCenter);

    QVBoxLayout *bottom_layout = new QVBoxLayout(frame_bottom);
    bottom_layout->setSpacing(8);
    bottom_layout->setContentsMargins(0,0,0,0);
    bottom_layout->addLayout(title_layout);

    device_list = new QWidget();

    bottom_layout->addWidget(device_list);

    device_list_layout  = new QVBoxLayout(device_list);
    device_list_layout->setSpacing(2);
    device_list_layout->setContentsMargins(0,0,0,0);
    device_list->setLayout(device_list_layout);

    for(int i = 0;i < m_localDevice->devices().size(); i++){
        qDebug() << m_localDevice->devices().at(i)->name() << m_localDevice->devices().at(i)->type()<<m_localDevice->devices().at(i)->isPaired();
        if(!Discovery_device_address.isEmpty()){
            if(Discovery_device_address.contains(m_localDevice->devices().at(i)->address())){
                continue;
            }
        }

        if(m_localDevice->devices().at(i)->isPaired())
            continue;

        DeviceInfoItem *item = new DeviceInfoItem();
        connect(item,SIGNAL(sendConnectDevice(QString)),this,SLOT(receiveConnectsignal(QString)));
        connect(item,SIGNAL(sendDisconnectDeviceAddress(QString)),this,SLOT(receiveDisConnectSignal(QString)));
        connect(item,SIGNAL(sendDeleteDeviceAddress(QString)),this,SLOT(receiveRemoveSignal(QString)));
        connect(item,SIGNAL(sendPairedAddress(QString)),this,SLOT(change_device_parent(QString)));

        if(m_localDevice->devices().at(i)->isConnected())
            item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::LINK, m_localDevice->devices().at(i));
        else
            item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::UNLINK, m_localDevice->devices().at(i));

        device_list_layout->addWidget(item);

        Discovery_device_address << m_localDevice->devices().at(i)->address();
    }

    device_list_layout->addStretch();

    frame_bottom->setLayout(bottom_layout);
}

void BlueToothMain::startDiscovery()
{
    if(m_localDevice->isDiscovering()){
        m_localDevice->stopDiscovery();
    }
    m_localDevice->startDiscovery();
}

BlueToothMain::~BlueToothMain()
{
    delete settings;
    delete device_list;
}

void BlueToothMain::onClick_Open_Bluetooth(bool ischeck)
{
    qDebug() << Q_FUNC_INFO << ischeck << m_localDevice->isPowered() << __LINE__;
    if(ischeck){
        BluezQt::PendingCall *call = m_localDevice->setPowered(true);
        connect(call,&BluezQt::PendingCall::finished,this,[=](BluezQt::PendingCall *v){
            if(v->error() == 0){
                bluetooth_name->set_dev_name(m_localDevice->name());
                bluetooth_name->setVisible(true);
                frame_bottom->setVisible(true);
                frame_middle->setVisible(true);
//                label_2->setText(tr("Turn off Bluetooth"));
                qDebug() << Q_FUNC_INFO << m_localDevice->isPowered() <<__LINE__;
                this->startDiscovery();
            }
        });
    }else{
        qDebug() << Q_FUNC_INFO << __LINE__;
        if(m_localDevice->isDiscovering()){
            m_localDevice->stopDiscovery();
        }
        BluezQt::PendingCall *call = m_localDevice->setPowered(false);
        connect(call,&BluezQt::PendingCall::finished,this,[=](BluezQt::PendingCall *v){
            if(v->error() == 0){
                bluetooth_name->setVisible(false);
                qDebug() << Q_FUNC_INFO << !m_localDevice->isPowered() << __LINE__;
                frame_bottom->setVisible(false);
                frame_middle->setVisible(false);
                label_2->setText(tr("Turn on Bluetooth"));
            }
        });
    }
}

void BlueToothMain::serviceDiscovered(BluezQt::DevicePtr device)
{
    qDebug() << Q_FUNC_INFO << device->type() << device->name();
    if(Discovery_device_address.contains(device->address())){
        return;
    }

    DeviceInfoItem *item = new DeviceInfoItem(device_list);
    connect(item,SIGNAL(sendConnectDevice(QString)),this,SLOT(receiveConnectsignal(QString)));
    connect(item,SIGNAL(sendDisconnectDeviceAddress(QString)),this,SLOT(receiveDisConnectSignal(QString)));
    connect(item,SIGNAL(sendDeleteDeviceAddress(QString)),this,SLOT(receiveRemoveSignal(QString)));
    connect(item,SIGNAL(sendPairedAddress(QString)),this,SLOT(change_device_parent(QString)));

    item->initInfoPage(device->name(), DEVICE_STATUS::UNLINK, device);

    device_list_layout->addWidget(item,Qt::AlignTop);

    Discovery_device_address << device->address();
}

void BlueToothMain::receiveConnectsignal(QString device)
{
    QDBusMessage m = QDBusMessage::createMethodCall("org.ukui.bluetooth","/org/ukui/bluetooth","org.ukui.bluetooth","connectToDevice");
    m << device;
    qDebug() << Q_FUNC_INFO << m.arguments().at(0).value<QString>() <<__LINE__;
    // 发送Message
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    // 判断Method是否被正确返回
//    if (response.type()== QDBusMessage::ReplyMessage)
//    {
//        // QDBusMessage的arguments不仅可以用来存储发送的参数，也用来存储返回值;
//        bRet = response.arguments().at(0).toBool();
//    }
//    else {
//        qDebug()<<"Message In fail!\n";
//    }
//    qDebug() << Q_FUNC_INFO << __LINE__;
    //    qDebug()<<"bRet:"<<bRet;
}

void BlueToothMain::receiveDisConnectSignal(QString address)
{
    QDBusMessage m = QDBusMessage::createMethodCall("org.ukui.bluetooth","/org/ukui/bluetooth","org.ukui.bluetooth","disConnectToDevice");
    m << address;
    qDebug() << Q_FUNC_INFO << m.arguments().at(0).value<QString>() <<__LINE__;
    // 发送Message
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
}

void BlueToothMain::receiveRemoveSignal(QString address)
{
//    QDBusMessage m = QDBusMessage::createMethodCall("org.ukui.bluetooth","/org/ukui/bluetooth","org.ukui.bluetooth","removeDevice");
//    m << address;
//    qDebug() << Q_FUNC_INFO << m.arguments().at(0).value<QString>() <<__LINE__;
//    // 发送Message
//    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    qDebug() << Q_FUNC_INFO << address;

    if(m_localDevice->deviceForAddress(address).isNull()){
        qDebug() << Q_FUNC_INFO << "No this address " << address;
        DeviceInfoItem *item = device_list->findChild<DeviceInfoItem *>(address);
        device_list_layout->removeWidget(item);
        item->setParent(NULL);
        delete item;
        return;
    }

    //根据设备是否配对，判断在我的设备栏还是设备栏
    bool layout_flag = false;
    if(m_localDevice->deviceForAddress(address)->isPaired()){
        layout_flag = true;
    }

    //如果点击移除设备，则将界面的item移除；如果是在我的设备下的设备被移除完后，隐藏我的设备一栏
    BluezQt::PendingCall *call = m_localDevice->removeDevice(m_localDevice->deviceForAddress(address));
    connect(call,&BluezQt::PendingCall::finished,this,[=](BluezQt::PendingCall *value){
        if(value->error() == 0){
            if(layout_flag){
                DeviceInfoItem *item = frame_middle->findChild<DeviceInfoItem *>(address);
                paired_dev_layout->removeWidget(item);
                item->setParent(NULL);
                delete item;

                if(frame_middle->children().size() == 2){
                    frame_middle->setVisible(false);
                }
            }else{
                DeviceInfoItem *item = device_list->findChild<DeviceInfoItem *>(address);
                device_list_layout->removeWidget(item);
                item->setParent(NULL);
                delete item;
            }
        }else{
            qDebug() << Q_FUNC_INFO << "Device Remove failed!!!";
        }
    });
}

void BlueToothMain::Refresh_load_Label_icon()
{
    if(i == 0)
        i = 7;
    loadLabel->setPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(i,10)).pixmap(24,24));
    loadLabel->update();
    i--;
}

void BlueToothMain::set_tray_visible(bool value)
{
    settings->set("tray-show",QVariant::fromValue(value));
}

void BlueToothMain::change_adapter_name(const QString &name)
{
    m_localDevice->setName(name);
}

void BlueToothMain::change_device_parent(const QString &address)
{
    qDebug() << Q_FUNC_INFO ;
    if(!frame_middle->isVisible()){
        frame_middle->setVisible(true);
    }

    DeviceInfoItem *item = device_list->findChild<DeviceInfoItem *>(address);
    device_list_layout->removeWidget(item);
    item->setParent(frame_middle);
    paired_dev_layout->addWidget(item);
}

void BlueToothMain::adapterPoweredChanged(bool value)
{
    settings->set("switch",QVariant::fromValue(value));
    if(value){
        bluetooth_name->set_dev_name(m_localDevice->name());
        bluetooth_name->setVisible(true);
        frame_bottom->setVisible(true);
        frame_middle->setVisible(true);
        open_bluetooth->setChecked(true);
//                label_2->setText(tr("Turn off Bluetooth"));
        this->startDiscovery();
        qDebug() << discovering_timer->isActive();
    }else{
        bluetooth_name->setVisible(false);
        open_bluetooth->setChecked(false);
        frame_bottom->setVisible(false);
        frame_middle->setVisible(false);
        label_2->setText(tr("Turn on Bluetooth"));
        if(m_localDevice->isDiscovering()){
            m_localDevice->stopDiscovery();
        }
        qDebug() << discovering_timer->isActive();
    }
}

