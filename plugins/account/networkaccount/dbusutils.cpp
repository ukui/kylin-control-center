#include "dbusutils.h"

DBusUtils::DBusUtils(QObject *parent) : QObject(parent)
{

}

const QString DBusUtils::callMethod(const QString &methodName, const QList<QVariant> &argList) {
    QVariant ret;
    QDBusMessage message = QDBusMessage::createMethodCall(QString(DBUSNAME),
                           QString(PATH),
                           QString(INTERFACE),
                           methodName);
    if (argList.isEmpty() == false) {
        message.setArguments(argList);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if (response.arguments().isEmpty() == false) {
            ret = response.arguments().takeFirst();;
        }
    } else {
        qDebug()<<methodName<<"called failed";
    }

    //qDebug() << methodName;
    if (methodName == "checkLogin") {
        emit infoFinished(ret.toString());
    } else if (methodName == "querryUploaded") {
        emit querryFinished(ret.toStringList());
    } else {
        emit taskFinished(methodName,ret.toInt());
    }
    return ret.toString();
}

int DBusUtils::connectSignal(const QString &signal, QObject *object,const char *slot) const {
    QDBusConnection::sessionBus().connect(QString(), QString(PATH), QString(INTERFACE), signal, object, slot);
    return 0;
}

const QVariant DBusUtils::GetProperty(const QString &dbusname,const QString &interface, const QString &path, const QString &property,const int &flag) {
    QVariant variant = "qwer";

    QDBusMessage message = QDBusMessage::createMethodCall(dbusname,path,
                                                          QString(PROPERTYINTERFACE),
                                                          QString(GETMETHOD));
    message << interface << property;
    QDBusMessage response = flag == 1 ? QDBusConnection::sessionBus().call(message) : QDBusConnection::systemBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage) {
        if (response.arguments().isEmpty() == false) {
            QDBusVariant value = qvariant_cast<QDBusVariant>(response.arguments().takeFirst());
            variant = value.variant();
        }
    } else {
        qDebug() <<interface << property << "call failed";
    }
    return variant;
}

const QVariant DBusUtils::ValueRet(const QString &dbusname, const QString &interface, const QString &path, const QString &method,
                                   const int &flag, const QString &value, const QList<QVariant> &argList) {
    QVariant variant = "qwer";
    QDBusMessage message = QDBusMessage::createMethodCall(dbusname,path,
                                                          interface,
                                                          method);
    QList<QVariant> list ;
    if (value != "")
        list << value;
    if (argList.isEmpty() == false) {
        list.append(argList);
    }
    if (list.isEmpty() == false) {
        message.setArguments(list);
    }
    //qDebug() << dbusname <<interface << method << "  ==========================";
    QDBusMessage response = flag == 1 ? QDBusConnection::sessionBus().call(message) : QDBusConnection::systemBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage) {
        if (response.arguments().isEmpty() == false) {
            QDBusVariant value = qvariant_cast<QDBusVariant>(response.arguments().takeFirst());
            variant = value.variant();
            if (variant.isValid() == false) {
                variant = response.arguments().takeFirst();
            }
        }
    } else {
        qDebug() << "call failed";
    }
    //qDebug()<<variant<<"=============";
    return variant;
}
