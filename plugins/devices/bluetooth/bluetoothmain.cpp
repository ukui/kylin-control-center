#include "bluetoothmain.h"


#include <QDBusObjectPath>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>
//#include <polkit-qt5-1/PolkitQt1/Authority>

#include <QMessageLogger>

namespace BluezQt
{
    extern void bluezqt_initFakeBluezTestRun();
}

using namespace BluezQt;

BlueToothMain::BlueToothMain(QWidget *parent)
    : QMainWindow(parent)
{

//    this->setAutoFillBackground(true);
    this->setMinimumSize(582,535);

    m_manager = new BluezQt::Manager(this);
    job = m_manager->init();
    job->exec();
    Agent = new BluetoothAgent(this);
    qDebug() << m_manager->registerAgent(Agent)->errorText();
//    m_manager->setBluetoothBlocked(true);
//    m_localDevice = m_manager->usableAdapter();
    m_localDevice = m_manager->adapterForAddress("00:1A:7D:DA:71:13");
    qDebug() << (m_localDevice == nullptr);
    qDebug() << m_manager->adapters().size() << m_manager->devices().size();
    qDebug() << m_localDevice->name() << m_localDevice->isPowered() << m_localDevice->isDiscoverable() << m_localDevice->isDiscovering() << m_localDevice->address();

    connect(m_manager,&BluezQt::Manager::deviceAdded,this,&BlueToothMain::serviceDiscovered);

    main_widget = new QWidget(this);
    this->setCentralWidget(main_widget);

    main_layout = new QVBoxLayout(main_widget);
    main_layout->setSpacing(40);
    main_layout->setContentsMargins(0,25,0,0);
    frame_top    = new QWidget(main_widget);
    frame_top->setMinimumSize(582,135);
    frame_top->setMaximumSize(1000,135);
//    frame_top->setStyleSheet("background:blue;");
    frame_bottom = new QWidget(main_widget);
//    frame_bottom->setMinimumWidth(582);
//    frame_bottom->setMaximumWidth(1000);
//    frame_bottom->setMinimumHeight(340);
//    frame_bottom->setStyleSheet("background:green;");

    main_layout->addWidget(frame_top);
    main_layout->addWidget(frame_bottom);
    main_layout->addStretch();

    if(!m_localDevice->isPowered()){
        InitMainTopUI();
    }else{
        InitMainTopUI();
        InitMainbottomUI();
        this->startDiscovery();
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
                           color: rgba(0, 0, 0, 0.85);\
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
    layout->addWidget(frame_1);

    QHBoxLayout *frame_1_layout = new QHBoxLayout(frame_1);
    frame_1_layout->setSpacing(10);
    frame_1_layout->setContentsMargins(16,0,16,0);

    QLabel *label_2 = new QLabel(tr("Turn on Bluetooth"),frame_1);
    label_2->setStyleSheet("QLabel{\
                           width: 56px;\
                           height: 20px;\
                           font-size: 14px;\
                           font-family: PingFangSC-Regular, PingFang SC;\
                           font-weight: 400;\
                           color: rgba(0, 0, 0, 0.85);\
                           line-height: 20px;}");
    frame_1_layout->addWidget(label_2);
    frame_1_layout->addStretch();

    bluetooth_name = new QLabel(frame_1);
    bluetooth_name->setStyleSheet("QLabel{\
                                  width: 214px;\
                                  height: 20px;\
                                  font-size: 14px;\
                                  font-family: PingFangSC-Regular, PingFang SC;\
                                  font-weight: 400;\
                                  color: rgba(0, 0, 0, 0.85);\
                                  line-height: 20px;}");
    frame_1_layout->addWidget(bluetooth_name);

    open_bluetooth = new SwitchButton(frame_1);
    connect(open_bluetooth,SIGNAL(checkedChanged(bool)),this,SLOT(onClick_Open_Bluetooth(bool)));
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
                           color: rgba(0, 0, 0, 0.85);\
                           line-height: 20px;}");
    frame_2_layout->addWidget(label_3);
    frame_2_layout->addStretch();

    show_panel = new SwitchButton(frame_2);
    frame_2_layout->addWidget(show_panel);

//    qDebug() << m_localDevice->isValid() << m_localDevice->hostMode() /*<< open_bluetooth->isChecked*/;

    bluetooth_name->setText(tr("Can now be found as \"")+m_localDevice->name()+tr("\""));

    if(m_localDevice->isPowered()){
        open_bluetooth->setChecked(true);
        bluetooth_name->setVisible(true);
    }
    else{
        open_bluetooth->setChecked(false);
        bluetooth_name->setVisible(false);
    }

    frame_top->setLayout(top_layout);
}

void BlueToothMain::InitMainbottomUI()
{
    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->setSpacing(10);
    title_layout->setContentsMargins(0,0,0,0);

    QLabel *label_1 = new QLabel(tr("Other Devices"),frame_bottom);
    label_1->setStyleSheet("QLabel{\
                          width: 72px;\
                          height: 25px;\
                          font-size: 18px;\
                          font-family: PingFangSC-Medium, PingFang SC;\
                          font-weight: 500;\
                          color: rgba(0, 0, 0, 0.85);\
                          line-height: 25px;}");

    loadLabel = new QLabel(frame_bottom);
    loadLabel->setFixedSize(24,24);
//    loadLabel->setStyleSheet("background:blue;");
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer,&QTimer::timeout,this,&BlueToothMain::Refresh_load_Label_icon);
    m_timer->start();

    title_layout->addWidget(label_1);
    title_layout->addWidget(loadLabel);
    title_layout->addStretch();

    QVBoxLayout *bottom_layout = new QVBoxLayout(frame_bottom);
    bottom_layout->setSpacing(8);
    bottom_layout->setContentsMargins(0,0,0,0);
    bottom_layout->addLayout(title_layout);
//    bottom_layout->addStretch();

    QScrollArea *device_area = new QScrollArea(frame_bottom);
    device_area->setWidgetResizable(true);
    device_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    device_area->setMinimumHeight(290);
    device_area->setMinimumWidth(582);
    device_area->setMaximumWidth(1000);
    bottom_layout->addWidget(device_area);

    device_list = new QWidget();
//    device_list->setStyleSheet("background:blue;");
//    device_list->setMinimumHeight(290);
//    device_list->setMinimumWidth(582);
//    device_list->setMaximumWidth(1000);
    device_area->setWidget(device_list);

    device_list_layout  = new QVBoxLayout(device_list);
    device_list_layout->setSpacing(2);
    device_list_layout->setContentsMargins(0,0,0,0);
    device_list->setLayout(device_list_layout);

    for(int i = 0;i < m_manager->devices().size(); i++){
        qDebug() << m_manager->devices().at(i)->name();
        DEVICE_TYPE d_type;
        switch (m_manager->devices().at(i)->type()){
        case BluezQt::Device::Type::Uncategorized:
            d_type = DEVICE_TYPE::OTHER;
            break;
        case BluezQt::Device::Type::Computer:
            d_type = DEVICE_TYPE::PC;
            break;
        case BluezQt::Device::Type::Phone:
            d_type = DEVICE_TYPE::PHONE;
            break;
        default:
            d_type = DEVICE_TYPE::OTHER;
            break;
        }

        DeviceInfoItem *item = new DeviceInfoItem(device_list);
        connect(item,SIGNAL(sendConnectDevice(BluezQt::DevicePtr)),this,SLOT(receiveConnectsignal(BluezQt::DevicePtr)));
        item->initInfoPage(d_type, m_manager->devices().at(i)->name(), DEVICE_STATUS::UNLINK, m_manager->devices().at(i));
        device_list_layout->addWidget(item,Qt::AlignTop);
    }

    frame_bottom->setLayout(bottom_layout);
}

void BlueToothMain::startDiscovery()
{
//    qDebug() << Q_FUNC_INFO << discoveryAgent->isActive();
//    if(discoveryAgent->isActive())
//        discoveryAgent->stop();
//    discoveryAgent->start();
    if(m_localDevice->isDiscovering()){
        m_localDevice->stopDiscovery();
    }
    m_localDevice->startDiscovery();
}

void BlueToothMain::InitTray()
{
    pSystemTray = new QSystemTrayIcon(this);
    QMenu *traymenu = new QMenu(this);

    traymenu->addAction(tr("Turn on bluetooth"));
    traymenu->addSeparator();
    traymenu->addAction(tr("Bluetooth settings"));

    pSystemTray->setContextMenu(traymenu);
    pSystemTray->setToolTip(tr("Bluetooth"));
    pSystemTray->setIcon(QIcon::fromTheme("bluetooth-symbolic"));
//    pSystemTray->setVisible(true);
    pSystemTray->show();

    connect(pSystemTray,
            &QSystemTrayIcon::activated,
            [=](QSystemTrayIcon::ActivationReason reason){
                switch (reason)
                {
                    case QSystemTrayIcon::DoubleClick: /* 来自于双击激活。 */
                    case QSystemTrayIcon::Trigger: /* 来自于单击激活。 */
                        traymenu->move(pSystemTray->geometry().x()+16,pSystemTray->geometry().y()-traymenu->height());
                        traymenu->exec();
                        break;
                }
            });
}

BlueToothMain::~BlueToothMain()
{
}

void BlueToothMain::onClick_Open_Bluetooth(bool ischeck)
{
//    qDebug() << Q_FUNC_INFO << m_localDevice->isValid() << ischeck;
    if(ischeck){
        m_localDevice->setPowered(true);
        qDebug() << m_localDevice->isPowered();
        bluetooth_name->setText(tr("Can now be found as \"")+m_localDevice->name()+tr("\""));
        bluetooth_name->setVisible(true);
//            startDiscovery();
    }else{
        m_localDevice->setPowered(false);
        bluetooth_name->setVisible(false);
    }
}

void BlueToothMain::serviceDiscovered(BluezQt::DevicePtr device)
{
//    qDebug() << device->name();
//    QLabel *label = new QLabel(service.name(),device_list);
//    device_list_layout->addWidget(label);

    DEVICE_TYPE d_type;
    switch (device->type()){
    case BluezQt::Device::Type::Uncategorized:
        d_type = DEVICE_TYPE::OTHER;
        break;
    case BluezQt::Device::Type::Computer:
        d_type = DEVICE_TYPE::PC;
        break;
    case BluezQt::Device::Type::Phone:
        d_type = DEVICE_TYPE::PHONE;
        break;
    default:
        d_type = DEVICE_TYPE::OTHER;
        break;
    }

    DeviceInfoItem *item = new DeviceInfoItem(device_list);
    connect(item,SIGNAL(sendConnectDevice(BluezQt::DevicePtr)),this,SLOT(receiveConnectsignal(BluezQt::DevicePtr)));
    item->initInfoPage(d_type, device->name(), DEVICE_STATUS::UNLINK, device);
    device_list_layout->addWidget(item,Qt::AlignTop);
}

void BlueToothMain::receiveConnectsignal(BluezQt::DevicePtr device)
{
    qDebug() << Q_FUNC_INFO << device->name();
    bool bRet  = false;
//    BluezQt::PendingCall *callback = device->pair();
//    qDebug() << Q_FUNC_INFO << callback->error();
//    if(callback->error() == 6){
//        m_timer->stop();
//    }
//    device->connectToDevice();
    QDBusMessage m = QDBusMessage::createMethodCall("com.ukui.bluetooth","/","com.ukui.bluetooth","connectToDevice");
    m << device->address();
    qDebug() << Q_FUNC_INFO << __LINE__;
    // 发送Message
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
//    // 判断Method是否被正确返回
//    if (response.type()== QDBusMessage::ReplyMessage)
//    {
//        // QDBusMessage的arguments不仅可以用来存储发送的参数，也用来存储返回值;
//        bRet = response.arguments().at(0).toBool();
//    }
//    else {
//        qDebug()<<"showGuide In fail!\n";
//    }

//    qDebug()<<"bRet:"<<bRet;
}

void BlueToothMain::Refresh_load_Label_icon()
{
    if(i == 0)
        i = 7;
    loadLabel->setPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(i,10)).pixmap(24,24));
    loadLabel->update();
    i--;
}

