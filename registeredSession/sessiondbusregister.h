#ifndef SESSIONDBUSREGISTER_H
#define SESSIONDBUSREGISTER_H

#include <QObject>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QMap>
#include <QCoreApplication>
#include <QDBusContext>
#include <QScopedPointer>

#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class SessionDbusRegister : public QObject,  protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.control.center.qt.sessiondbus")
public:

    explicit SessionDbusRegister(QObject *parent);
    ~SessionDbusRegister();

private:
    QString readFile(const QString &filename);
    QMap<QString, QVariant> getJsonInfo(const JsonObject &result, const QStringList &keies);

public slots:
    Q_SCRIPTABLE void exitService();
    Q_SCRIPTABLE QMap<QString, QVariant> getModuleHideStatus();
};

#endif // SESSIONDBUSREGISTER_H
