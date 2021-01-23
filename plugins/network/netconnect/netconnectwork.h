#ifndef NETCONNECTWORK_H
#define NETCONNECTWORK_H

#include <QObject>
#include <QStringList>
#include <QProcess>

class NetconnectWork : public QObject
{
    Q_OBJECT
public:
    explicit NetconnectWork();
    ~NetconnectWork();

public:
    void run();

Q_SIGNALS:
    void wifiGerneral(QStringList wifiList);
    void workerComplete();
};

#endif // NETCONNECTWORK_H
