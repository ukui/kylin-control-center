#include "netconnectwork.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QTime>

NetconnectWork::NetconnectWork()
{

}

NetconnectWork::~NetconnectWork() {

}

void NetconnectWork::run() {
    QTime dieTime = QTime::currentTime().addMSecs(3500);
    while( QTime::currentTime() < dieTime ) {
    }
    emit success();
}

bool NetconnectWork::getWifiIsOpen() {

    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );
    // 获取当前wifi是否打开
    QDBusReply<QVariant> m_result = interface.call("Get", "org.freedesktop.NetworkManager", "WirelessEnabled");

    if (m_result.isValid()) {
        bool status = m_result.value().toBool();
        return status;
    } else {
        qDebug()<<"org.freedesktop.NetworkManager get invalid"<<endl;
        return false;
    }
}
