#include "netconnectwork.h"

NetconnectWork::NetconnectWork()
{

}

NetconnectWork::~NetconnectWork() {

}

void NetconnectWork::run() {
    QProcess *wifiPro = new QProcess(this);
    wifiPro->start("nmcli -f signal,security,ssid, device wifi");
    wifiPro->waitForFinished();
    QString shellOutput = "";
    QString output = wifiPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

    emit wifiGerneral(slist);
    emit workerComplete();
}
