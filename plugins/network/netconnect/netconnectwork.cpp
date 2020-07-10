#include "netconnectwork.h"

NetconnectWork::NetconnectWork()
{

}

NetconnectWork::~NetconnectWork() {

}

void NetconnectWork::run() {
    QProcess *wifiPro = new QProcess();
    wifiPro->start("nmcli -f signal,ssid device wifi");
    wifiPro->waitForFinished();
    QString shellOutput = "";
    QString output = wifiPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

//    qDebug()<<"slist is--------->"<<slist<<endl;
    emit wifiGerneral(slist);
    emit workerComplete();
}
