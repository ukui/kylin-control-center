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
    qDebug() << m_manager->isOperational() << m_manager->isBluetoothBlocked();

    if(m_manager->adapters().size()){
        for(int i = 0; i < m_manager->adapters().size(); i++){
            if(m_manager->adapters().at(i)->address() == "00:1A:7D:DA:71:13"){
                adapter_address_list.insert(0,m_manager->adapters().at(i)->address());
                adapter_name_list.insert(0,m_manager->adapters().at(i)->name());
            }else{
                adapter_address_list << m_manager->adapters().at(i)->address();
                adapter_name_list    << m_manager->adapters().at(i)->name();
            }
        }
    }

    adapterChanged();

    if(m_manager->adapters().size() == 0){
        qDebug() << "no bluetooth adapter !!!";
        qDebug() << "Program exit !!!";
        return;
    }else if(m_manager->adapters().size() == 1){
        m_localDevice = m_manager->adapters().at(0);
        settings->set("adapter-address",QVariant::fromValue(m_localDevice->address()));
    }else{
        if(adapter_address_list.indexOf(Default_Adapter) != -1){
            m_localDevice = m_manager->adapterForAddress(Default_Adapter);
        }else{
            m_localDevice = m_manager->adapterForAddress(adapter_address_list.at(0));
        }
    }

    qDebug() << m_localDevice->name() << m_localDevice->isPowered() << m_localDevice->isDiscoverable() << m_localDevice->isDiscovering() << m_localDevice->address();

    main_widget = new QWidget(this);
    this->setCentralWidget(main_widget);

    main_layout = new QVBoxLayout(main_widget);
    main_layout->setSpacing(40);
    main_layout->setContentsMargins(0,0,20,10);
    frame_top    = new QWidget(main_widget);
    frame_top->setObjectName("frame_top");
    if(m_manager->adapters().size() > 1){
        frame_top->setMinimumSize(582,239);
        frame_top->setMaximumSize(1000,239);
    }else{
        frame_top->setMinimumSize(582,187);
        frame_top->setMaximumSize(1000,187);
    }
//    frame_top->setStyleSheet("background:blue;");
    frame_middle = new QWidget(main_widget);
    frame_middle->setObjectName("frame_middle");
//    frame_middle->setMinimumSize(582,135);
//    frame_middle->setStyleSheet("background:blue;");
    frame_bottom = new QWidget(main_widget);
    frame_bottom->setObjectName("frame_bottom");
    frame_bottom->setMinimumWidth(582);
    frame_bottom->setMaximumWidth(1000);
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
    this->setLayout(main_layout);

    updateUIWhenAdapterChanged();
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

    frame_2 = new QFrame(frame_top);
    frame_2->setMinimumWidth(582);
    frame_2->setFrameShape(QFrame::Shape::Box);
    frame_2->setFixedHeight(50);
    if(adapter_address_list.size() == 1)
        frame_2->setVisible(false);
    layout->addWidget(frame_2);

    QHBoxLayout *frame_2_layout = new QHBoxLayout(frame_2);
    frame_2_layout->setSpacing(10);
    frame_2_layout->setContentsMargins(16,0,16,0);

    QLabel *label_3 = new QLabel(tr("Bluetooth adapter"),frame_2);
    label_3->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-weight: 400;\
                           line-height: 20px;}");
    frame_2_layout->addWidget(label_3);
    frame_2_layout->addStretch();

    adapter_list = new QComboBox(frame_2);
    adapter_list->clear();
    adapter_list->addItems(adapter_name_list);
    adapter_list->setMinimumWidth(300);
    adapter_list->setCurrentIndex(adapter_address_list.indexOf(m_localDevice->address()));
    connect(adapter_list,SIGNAL(currentIndexChanged(int)),this,SLOT(adapterComboxChanged(int)));
    frame_2_layout->addWidget(adapter_list);

    QFrame *frame_3 = new QFrame(frame_top);
    frame_3->setMinimumWidth(582);
    frame_3->setFrameShape(QFrame::Shape::Box);
    frame_3->setFixedHeight(50);
    layout->addWidget(frame_3);

    QHBoxLayout *frame_3_layout = new QHBoxLayout(frame_3);
    frame_3_layout->setSpacing(10);
    frame_3_layout->setContentsMargins(16,0,16,0);

    QLabel *label_4 = new QLabel(tr("Show icon on taskbar"),frame_3);
    label_4->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-weight: 400;\
                           line-height: 20px;}");
    frame_3_layout->addWidget(label_4);
    frame_3_layout->addStretch();

    show_panel = new SwitchButton(frame_3);
    frame_3_layout->addWidget(show_panel);
    show_panel->setChecked(settings->get("tray-show").toBool());
    connect(show_panel,&SwitchButton::checkedChanged,this,&BlueToothMain::set_tray_visible);

    QFrame *frame_4 = new QFrame(frame_top);
    frame_4->setMinimumWidth(582);
    frame_4->setFrameShape(QFrame::Shape::Box);
    frame_4->setFixedHeight(50);
    layout->addWidget(frame_4);

    QHBoxLayout *frame_4_layout = new QHBoxLayout(frame_4);
    frame_4_layout->setSpacing(10);
    frame_4_layout->setContentsMargins(16,0,16,0);

    QLabel *label_5 = new QLabel(tr("Discoverable"),frame_4);
    label_5->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-weight: 400;\
                           line-height: 20px;}");
    frame_4_layout->addWidget(label_5);
    frame_4_layout->addStretch();

    switch_discover = new SwitchButton(frame_4);
    frame_4_layout->addWidget(switch_discover);
    switch_discover->setChecked(m_localDevice->isDiscoverable());
    connect(switch_discover,&SwitchButton::checkedChanged,this,&BlueToothMain::set_discoverable);
    connect(m_localDevice.data(), &BluezQt::Adapter::discoverableChanged, this, [=](bool discoverable){
        switch_discover->setChecked(discoverable);
    });

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
                                font-weight: 500;\
                                line-height: 25px;}");

    middle_layout->addWidget(middle_label,Qt::AlignTop);
    middle_layout->addLayout(paired_dev_layout,Qt::AlignTop);

    frame_middle->setLayout(middle_layout);
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

    frame_bottom->setLayout(bottom_layout);
}

void BlueToothMain::startDiscovery()
{
    if(m_localDevice->isDiscovering()){
        m_localDevice->stopDiscovery();
    }
    m_localDevice->startDiscovery();
}

void BlueToothMain::adapterChanged()
{
    connect(m_manager,&BluezQt::Manager::adapterRemoved,this,[=](BluezQt::AdapterPtr adapter){
//        qDebug() << Q_FUNC_INFO << adapter_address_list.indexOf(adapter.data()->address());
//        qDebug() << Q_FUNC_INFO << adapter_list->currentIndex() << adapter_address_list.at(adapter_list->currentIndex()) << adapter.data()->address() <<__LINE__;

        int i = adapter_address_list.indexOf(adapter.data()->address());

        adapter_name_list.removeAt(i);
        adapter_address_list.removeAll(adapter.data()->address());
        adapter_list->removeItem(i);

        if(adapter_address_list.size() == adapter_name_list.size() == 1){
            frame_2->setVisible(false);
            frame_top->setMinimumSize(582,135);
            frame_top->setMaximumSize(1000,135);
        }
    });

    connect(m_manager,&BluezQt::Manager::adapterAdded,this,[=](BluezQt::AdapterPtr adapter){

        adapter_address_list << adapter.data()->address();
        adapter_name_list << adapter.data()->name();
        qDebug() << Q_FUNC_INFO << adapter_address_list << "===" << adapter_name_list;
        adapter_list->addItem(adapter.data()->name());

        if((adapter_address_list.size() == adapter_name_list.size()) && (adapter_address_list.size() > 1)){
            if(!frame_2->isVisible())
                frame_2->setVisible(true);
            frame_top->setMinimumSize(582,187);
            frame_top->setMaximumSize(1000,187);
        }
    });
}

void BlueToothMain::updateUIWhenAdapterChanged()
{
    connect(m_localDevice.data(),&BluezQt::Adapter::poweredChanged,this,&BlueToothMain::adapterPoweredChanged);
    connect(m_localDevice.data(),&BluezQt::Adapter::deviceAdded,this,&BlueToothMain::serviceDiscovered);
    connect(m_localDevice.data(),&BluezQt::Adapter::deviceChanged,this,&BlueToothMain::serviceDiscoveredChange);
    connect(m_localDevice.data(),&BluezQt::Adapter::nameChanged,this,&BlueToothMain::adapterNameChanged);
    connect(m_localDevice.data(),&BluezQt::Adapter::deviceRemoved,this,&BlueToothMain::adapterDeviceRemove);
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
    //==============初始化蓝牙信息和基础信息====================================
     bluetooth_name->set_dev_name(m_localDevice->name());

     if(m_localDevice->isPowered()){
         open_bluetooth->setChecked(true);
         bluetooth_name->setVisible(true);
 //        label_2->setText(tr("Turn off Bluetooth"));
         label_2->setText(tr("Turn on Bluetooth"));
         if(!frame_bottom->isVisible())
             frame_bottom->setVisible(true);
     }else{
         qDebug() << Q_FUNC_INFO << m_manager->isBluetoothBlocked() << __LINE__;
         open_bluetooth->setChecked(false);
         bluetooth_name->setVisible(false);
         label_2->setText(tr("Turn on Bluetooth"));
         frame_bottom->setVisible(false);
         frame_middle->setVisible(false);
     }
     //===========================END==========================================

     // =============清空我的设备和蓝牙发现设备栏布局下的所有设备item=================
     QLayoutItem *child;
     while ((child = paired_dev_layout->takeAt(0)) != 0)
     {
         qDebug() << Q_FUNC_INFO << __LINE__;
         if(child->widget())
         {
             child->widget()->setParent(NULL);
         }
         delete child;
         child = nullptr;
     }
     while ((child = device_list_layout->takeAt(0)) != 0)
     {
         qDebug() << Q_FUNC_INFO << __LINE__;
         if(child->widget())
         {
             child->widget()->setParent(NULL);
         }
         delete child;
         child = nullptr;
     }
     // ========================END===========================================
     qDebug() << Q_FUNC_INFO <<m_localDevice->devices().size();
     show_flag = false;
     Discovery_device_address.clear();
     for(int i = 0;i < m_localDevice->devices().size(); i++){
//         qDebug() << m_localDevice->devices().at(i)->name() << m_localDevice->devices().at(i)->type();

         DeviceInfoItem *item = new DeviceInfoItem();
         connect(item,SIGNAL(sendConnectDevice(QString)),this,SLOT(receiveConnectsignal(QString)));
         connect(item,SIGNAL(sendDisconnectDeviceAddress(QString)),this,SLOT(receiveDisConnectSignal(QString)));
         connect(item,SIGNAL(sendDeleteDeviceAddress(QString)),this,SLOT(receiveRemoveSignal(QString)));
         connect(item,SIGNAL(sendPairedAddress(QString)),this,SLOT(change_device_parent(QString)));
         if(m_localDevice->devices().at(i)->isConnected())
             item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::LINK, m_localDevice->devices().at(i));
         else
             item->initInfoPage(m_localDevice->devices().at(i)->name(), DEVICE_STATUS::UNLINK, m_localDevice->devices().at(i));

         if(m_localDevice->devices().at(i)->isPaired()){
              show_flag = true;
              paired_dev_layout->addWidget(item,Qt::AlignTop);
         }else{
             if(!Discovery_device_address.isEmpty()){
                 if(Discovery_device_address.contains(m_localDevice->devices().at(i)->address())){
                     continue;
                 }
             }
             device_list_layout->addWidget(item);

             Discovery_device_address << m_localDevice->devices().at(i)->address();
         }
     }
     device_list_layout->addStretch();

     qDebug() << Q_FUNC_INFO << m_localDevice->devices().size() << show_flag;
     if(m_localDevice->isPowered()){
         if(show_flag)
             frame_middle->setVisible(true);
         else
             frame_middle->setVisible(false);
     }

     //当适配器查找设备状态改变时，改变加载动画和刷新按钮的状态
//     connect(m_localDevice.data(),&BluezQt::Adapter::discoveringChanged,this,[=](bool discover){
//        if(discover){
//            loadLabel->setVisible(true);
//            m_timer->start();
//            discover_refresh->setEnabled(false);
//            discovering_timer->start();
//        }else{
//            loadLabel->setVisible(false);
//            m_timer->stop();
//            discover_refresh->setEnabled(true);
//            discovering_timer->stop();
//        }
//     });

     if(m_localDevice->isPowered())
         m_localDevice->startDiscovery();
}

void BlueToothMain::removeDeviceItemUI(QString address)
{
    qDebug() << Q_FUNC_INFO << address <<__LINE__;
    if(Discovery_device_address.indexOf(address) != -1){
        DeviceInfoItem *item = device_list->findChild<DeviceInfoItem *>(address);
        if(item){
            device_list_layout->removeWidget(item);
            item->setParent(NULL);
            delete item;
            item = nullptr;
            Discovery_device_address.removeAll(address);
        }else{
            return;
        }
    }else{
        DeviceInfoItem *item = frame_middle->findChild<DeviceInfoItem *>(address);
        if(item){
            paired_dev_layout->removeWidget(item);
            item->setParent(NULL);
            delete item;
            item = nullptr;

            if(frame_middle->children().size() == 2){
                frame_middle->setVisible(false);
            }
        }else{
            return;
        }
    }
}

BlueToothMain::~BlueToothMain()
{
    delete settings;
    settings = nullptr;
    delete device_list;
    device_list = nullptr;
}

void BlueToothMain::onClick_Open_Bluetooth(bool ischeck)
{
    qDebug() << Q_FUNC_INFO << ischeck << m_localDevice->isPowered() << show_flag <<__LINE__;
    if(ischeck){
        if(m_manager->isBluetoothBlocked())
            m_manager->setBluetoothBlocked(false);
        BluezQt::PendingCall *call = m_localDevice->setPowered(true);
        connect(call,&BluezQt::PendingCall::finished,this,[=](BluezQt::PendingCall *v){
            if(v->error() == 0){
                bluetooth_name->set_dev_name(m_localDevice->name());
                bluetooth_name->setVisible(true);
                frame_bottom->setVisible(true);

                if(show_flag)
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

                if(frame_middle->isVisible())
                    frame_middle->setVisible(false);

                label_2->setText(tr("Turn on Bluetooth"));
            }
        });
    }
}

void BlueToothMain::serviceDiscovered(BluezQt::DevicePtr device)
{
    qDebug() << Q_FUNC_INFO << device->type() << device->name() << device->address();
    if(device->type() == BluezQt::Device::Uncategorized){
        return;
    }
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

void BlueToothMain::serviceDiscoveredChange(BluezQt::DevicePtr device)
{
    qDebug() << Q_FUNC_INFO << device->type() << device->name() << device->address();
    if(device->type() == BluezQt::Device::Uncategorized){
        return;
    }
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

    m_localDevice->removeDevice(m_localDevice->deviceForAddress(address));
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

void BlueToothMain::set_discoverable(bool value)
{
    m_localDevice->setDiscoverable(value);
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
    if(item){
        device_list_layout->removeWidget(item);
        item->setParent(frame_middle);
        paired_dev_layout->addWidget(item);
        Discovery_device_address.removeAll(address);
    }else{
        return;
    }
}

void BlueToothMain::adapterPoweredChanged(bool value)
{
    qDebug() << Q_FUNC_INFO <<show_flag;
    settings->set("switch",QVariant::fromValue(value));
    if(value){
        bluetooth_name->set_dev_name(m_localDevice->name());
        bluetooth_name->setVisible(true);
        frame_bottom->setVisible(true);

        if(show_flag)
            frame_middle->setVisible(true);

        open_bluetooth->setChecked(true);
        this->startDiscovery();
        qDebug() << discovering_timer->isActive();
    }else{
        bluetooth_name->setVisible(false);
        open_bluetooth->setChecked(false);
        frame_bottom->setVisible(false);

        if(frame_middle->isVisible())
            frame_middle->setVisible(false);

        label_2->setText(tr("Turn on Bluetooth"));
        if(m_localDevice->isDiscovering()){
            m_localDevice->stopDiscovery();
        }
        qDebug() << discovering_timer->isActive();
    }
}

void BlueToothMain::adapterComboxChanged(int i)
{
//    qDebug() << Q_FUNC_INFO << i << adapter_address_list.at(i) << adapter_name_list.at(i) << adapter_address_list << adapter_name_list;
    if(i != -1){
        m_localDevice = m_manager->adapterForAddress(adapter_address_list.at(i));
        m_localDevice->stopDiscovery();
        updateUIWhenAdapterChanged();
        settings->set("adapter-address",QVariant::fromValue(adapter_address_list.at(i)));
    }else{
        if(open_bluetooth->isChecked()){
            open_bluetooth->setChecked(false);
            open_bluetooth->setDisabledFlag(false);
        }
        if(frame_middle->isVisible()){
            frame_middle->setVisible(false);
        }
    }
}

void BlueToothMain::adapterNameChanged(const QString &name)
{
    emit this->adapter_name_changed(name);

    //设备名字改变，同时改变combox的当前设备名字
     int index;
    index = adapter_address_list.indexOf(m_localDevice->address());
    adapter_name_list.removeAt(index);
    adapter_name_list.insert(index,name);
//    qDebug() << Q_FUNC_INFO << adapter_name_list << adapter_address_list;
    adapter_list->setItemText(index,name);
}

void BlueToothMain::adapterDeviceRemove(BluezQt::DevicePtr ptr)
{
//    qDebug() << Q_FUNC_INFO << ptr.data()->address() << ptr.data()->name();
    removeDeviceItemUI(ptr.data()->address());
}

