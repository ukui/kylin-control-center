#ifndef UPDATESOURCE_H
#define UPDATESOURCE_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDebug>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusReply>
const QString Symbol = "control";
class UpdateSource : public QObject
{
    Q_OBJECT
public:
    explicit UpdateSource(QObject *parent = nullptr);

    void callDBusUpdateTemplate();
    void callDBusUpdateSource(QString symbol);
//    QDBusReply<QString> replyStr;   //string类型的返回值
    QDBusInterface *serviceInterface = nullptr;
    void getReply(QDBusPendingCallWatcher *call);
    QString getFailInfo(int statusCode);
signals:
    void getReplyFalseSignal();
    void startDbusFinished();
public slots:
    void startDbus();
};

#endif // UPDATESOURCE_H
