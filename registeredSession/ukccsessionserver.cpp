#include "ukccsessionserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ukccSessionServer::ukccSessionServer() {

}

QMap<QString, QVariant> ukccSessionServer::getJsonInfo(const QString &configFile) {
    QVariantMap moduleMap;

    QFile file(configFile);
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QByteArray readBy=file.readAll();
        QJsonParseError error;
        QJsonDocument readDoc=QJsonDocument::fromJson(readBy,&error);
        QJsonArray obj=readDoc.object().value("ukcc").toArray();

        for (int i = 0 ; i < obj.size(); i++) {
            QJsonObject faObj= obj[i].toObject();
            moduleMap.insert(faObj["name"].toString(), faObj["visible"].toVariant());
            QJsonArray childNodeAry =  faObj["childnode"].toArray();
            for (int j = 0; j < childNodeAry.size(); j++) {
                moduleMap.insert(childNodeAry.at(j).toObject().value("name").toString(),
                                 childNodeAry.at(j).toObject().value("visible").toVariant());
            }
        }
    }
    return moduleMap;
}

QString ukccSessionServer::getScreenMode()
{
    return mScreenMode;
}

void ukccSessionServer::setScreenMode(QString screenMode)
{
    mScreenMode = screenMode;
    Q_EMIT screenChanged(mScreenMode);
}

void ukccSessionServer::exitService() {
    qApp->exit();
}

void ukccSessionServer::ReloadSecurityConfig()
{
    Q_EMIT configChanged();
}

QVariantMap ukccSessionServer::getModuleHideStatus() {
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-security-config.json";

    return getJsonInfo(filename);
}

QString ukccSessionServer::GetSecurityConfigPath() {
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-security-config.json";

    return filename;
}
