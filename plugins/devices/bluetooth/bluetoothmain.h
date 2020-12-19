#ifndef BLUETOOTHMAIN_H
#define BLUETOOTHMAIN_H

#include "SwitchButton/switchbutton.h"

#include <polkit-qt5-1/PolkitQt1/Authority>

#include <KF5/BluezQt/bluezqt/adapter.h>
#include <KF5/BluezQt/bluezqt/manager.h>
#include <KF5/BluezQt/bluezqt/initmanagerjob.h>
#include <KF5/BluezQt/bluezqt/device.h>
#include <KF5/BluezQt/bluezqt/agent.h>
#include <KF5/BluezQt/bluezqt/pendingcall.h>

#include <QGSettings/QGSettings>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QString>
#include <QDebug>
#include <QLabel>
#include <QFrame>
#include <QSystemTrayIcon>
#include <QFont>
#include <QMetaType>
#include <QSizePolicy>
#include <QWidget>
#include <QScrollArea>
#include <QMenu>
#include <QTimer>
#include <QVariant>


#include "deviceinfoitem.h"
#include "bluetoothagent.h"
#include "bluetoothnamelabel.h"

class BlueToothMain : public QMainWindow
{
    Q_OBJECT
public:
    BlueToothMain(QWidget *parent = nullptr);
    void InitMainTopUI();
    void InitMainbottomUI();
    void startDiscovery();
    ~BlueToothMain();

signals:
    void adapter_name_changed(const QString &name);

private slots:
    void onClick_Open_Bluetooth(bool);
    void serviceDiscovered(BluezQt::DevicePtr);
    void receiveConnectsignal(QString);
    void receiveDisConnectSignal(QString);
    void receiveRemoveSignal(QString);
    void Refresh_load_Label_icon();
    void GSetting_value_chanage(const QString &key);
//    void get_pair_item();
    void set_tray_visible(bool);
    void change_adapter_name(const QString &name);
private:
    QGSettings *settings;
    QString Default_Adapter;
    QStringList paired_device_address;
    QString finally_connect_the_device;

    QVBoxLayout *main_layout;
    QSystemTrayIcon * tray;
    QWidget *main_widget;
    QWidget *frame_top;
    QWidget *frame_bottom;
//    QLabel *bluetooth_name;
    BluetoothNameLabel *bluetooth_name = nullptr;

    QVBoxLayout *bottom_layout;
    QScrollArea *device_area;
    QWidget *device_list;
    QVBoxLayout *device_list_layout;

//    QBluetoothLocalDevice *m_localDevice;
    BluetoothAgent *Agent;
    BluezQt::Manager *m_manager;
    BluezQt::InitManagerJob  *job;
    BluezQt::AdapterPtr m_localDevice;

//    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

    SwitchButton *open_bluetooth;
    SwitchButton *show_panel;

    QSystemTrayIcon *pSystemTray;

    QLabel *label_2;
    QLabel *loadLabel;
    QPushButton *discover_refresh;
    QTimer *m_timer;
    QTimer *discovering_timer;
    int i = 7;
};

#endif // BLUETOOTHMAIN_H
