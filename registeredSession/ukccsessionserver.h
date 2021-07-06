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
#include <QDBusVariant>

#include "json.h"
#include "screenStruct.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

class ukccSessionServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.ukcc.session.interface")
public:
    explicit ukccSessionServer();

private:
    QMap<QString, QVariant> getJsonInfo(const  QString &confFile);

private:
    QString mScreenMode;
    QVariantList mPreScreenCfg;
    QString mFilePath;

Q_SIGNALS:
    void configChanged();
    void screenChanged(QString status);

public Q_SLOTS:
    void exitService();
    void        ReloadSecurityConfig();
    QVariantMap getModuleHideStatus();
    QString     GetSecurityConfigPath();
    QString getScreenMode();
    void setScreenMode(QString screenMode);
    void setPreScreenCfg(QVariantList preScreenCfg);
    QVariantList getPreScreenCfg();
};

#endif // UKCCSESSIONSERVER_H
