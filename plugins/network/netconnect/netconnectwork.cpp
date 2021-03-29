#include "netconnectwork.h"

#include <QDBusInterface>
#include <QDBusReply>

NetconnectWork::NetconnectWork()
{

}

NetconnectWork::~NetconnectWork() {

}

void NetconnectWork::run() {
    if (!getWifiIsOpen()) {
        emit wifiGerneral(QStringList());
        return ;
    }
    QProcess *wifiPro = new QProcess(this);
    wifiPro->start("nmcli -f signal,security,ssid, device wifi");
    wifiPro->waitForFinished();
    QString shellOutput = "";
    QString output = wifiPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

    emit wifiGerneral(slist);
}

bool NetconnectWork::getWifiIsOpen() {

    QDBusInterface interface( "org.freedesktop.NetworkManager",
                              "/org/freedesktop/NetworkManager",
                              "org.freedesktop.DBus.Properties",
                              QDBusConnection::systemBus() );
    //　获取当前wifi是否打开
    QDBusReply<QVariant> m_result = interface.call("Get", "org.freedesktop.NetworkManager", "WirelessEnabled");

    if (m_result.isValid()) {
        bool status = m_result.value().toBool();
        return status;
    } else {
        qDebug()<<"org.freedesktop.NetworkManager get invalid"<<endl;
        return false;
    }
}
