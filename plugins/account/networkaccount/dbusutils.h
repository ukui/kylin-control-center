#ifndef DBUSUTILS_H
#define DBUSUTILS_H

#include <QObject>
#include <QtDBus>
#include <QDBusMessage>

#define PATH "/org/kylinssoclient/path"
#define INTERFACE "org.freedesktop.kylinssoclient.interface"
#define DBUSNAME "org.kylinssoclient.dbus"
#define PROPERTYINTERFACE "org.freedesktop.DBus.Properties"
#define GETMETHOD "Get"
#define SETMETHOD "Set"

class DBusUtils : public QObject
{
    Q_OBJECT
public:
    explicit DBusUtils(QObject *parent = nullptr);
    const QString callMethod(const QString &methodName,const QList<QVariant> &argList);
    int connectSignal(const QString &signal, QObject *obejct,const char *slot) const;
    const QVariant GetProperty(const QString &dbusname,const QString &interface,const QString &path,const QString &property,const int &flag);
    const QVariant ValueRet(const QString &dbusname, const QString &interface, const QString &path, const QString &method,
                            const int &flag, const QString &value, const QList<QVariant> &argList);

Q_SIGNALS:
    void taskFinished(const QString &taskName,int ret);
    void infoFinished(const QString &ret);
    void querryFinished(const QStringList &list);
};

#endif // DBUSUTILS_H
