#ifndef UKCCSESSIONSERVER_H
#define UKCCSESSIONSERVER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QVariantMap>
#include <QCoreApplication>
#include <QDBusContext>
#include <QScopedPointer>

#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class ukccSessionServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.ukcc.session.interface")
public:
    explicit ukccSessionServer();

private:
    QString readFile(const QString &filename);
    QMap<QString, QVariant> getJsonInfo(const JsonObject &result, const QStringList &keies);
    void initModuleStatus(const QString &fileName);

public slots:
    void exitService();
    QVariantMap getModuleHideStatus();
    QString     getModuleConfFile();
};

#endif // UKCCSESSIONSERVER_H
