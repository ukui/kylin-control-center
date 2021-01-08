#ifndef UPDATESOURCE_H
#define UPDATESOURCE_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDebug>
#include <QDBusPendingCall>

class UpdateSource : public QObject
{
    Q_OBJECT
public:
    explicit UpdateSource(QObject *parent = nullptr);

    void callDBusUpdateTemplate();

    void callDBusUpdateSource();

    QDBusInterface *serviceInterface;

signals:

public slots:

};

#endif // UPDATESOURCE_H
