#ifndef NETCONNECTWORK_H
#define NETCONNECTWORK_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QDebug>

class NetconnectWork : public QObject
{
    Q_OBJECT
public:
    explicit NetconnectWork();
    ~NetconnectWork();

public:
    void run();

private:
    bool getWifiIsOpen();

Q_SIGNALS:
    void wifiGerneral(QStringList wifiList);
};

#endif // NETCONNECTWORK_H
