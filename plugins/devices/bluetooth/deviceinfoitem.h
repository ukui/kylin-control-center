#ifndef DEVICEINFOITEM_H
#define DEVICEINFOITEM_H

#include "config.h"


#include <KF5/BluezQt/bluezqt/device.h>

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QPixmap>
#include <QString>
#include <QIcon>
#include <QDebug>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDateTime>
#include <QString>

class DeviceInfoItem : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceInfoItem(QWidget *parent = nullptr);
    ~DeviceInfoItem();
    void initInfoPage(DEVICE_TYPE icon_type = DEVICE_TYPE::OTHER,QString d_name = "",DEVICE_STATUS status = DEVICE_STATUS::NOT,BluezQt::DevicePtr device = nullptr);
    QString get_dev_name();
    void changeDevStatus(bool);
    void setDevConnectedIcon(bool);
    void AnimationInit();
protected:
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
signals:
    void sendConnectDevice(QString);
    void sendDisconnectDeviceAddress(QString);
    void sendDeleteDeviceAddress(QString);
    void send_this_item_is_pair();
    void sendPairedAddress(QString);
private slots:
    void onClick_Connect_Btn(bool);
    void onClick_Disconnect_Btn(bool);
    void onClick_Delete_Btn(bool);
    void updateDeviceStatus(DEVICE_STATUS status = DEVICE_STATUS::NOT);
private:
    QWidget *parent_widget = nullptr;
    QLabel *device_icon = nullptr;
    QLabel *device_name = nullptr;
    QLabel *device_status = nullptr;

    BluezQt::DevicePtr device_item;

    QPushButton *connect_btn = nullptr;
    QPushButton *disconnect_btn = nullptr;
    QPushButton *del_btn = nullptr;

    DEVICE_STATUS d_status;

    QFrame *info_page = nullptr;
    QTimer *icon_timer = nullptr;
    int i = 7;

    QPropertyAnimation *enter_action = nullptr;
    QPropertyAnimation *leave_action = nullptr;

    bool AnimationFlag = false;
    QTimer *mouse_timer = nullptr;
};

#endif // DEVICEINFOITEM_H
