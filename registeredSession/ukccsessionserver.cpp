#include "ukccsessionserver.h"

ukccSessionServer::ukccSessionServer()
{

}

QString ukccSessionServer::readFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

QMap<QString, QVariant> ukccSessionServer::getJsonInfo(const QtJson::JsonObject &result, const QStringList &keys)
{
    QVariantMap moduleMap;

    for (int i = 0; i < keys.length(); i++) {
        QVariantMap childMap = result[keys.at(i)].toMap();
        QMapIterator<QString, QVariant> it(childMap);
        while (it.hasNext()) {
            it.next();
            moduleMap.insert(it.key(), it.value());
        }
    }
    return moduleMap;
}

void ukccSessionServer::exitService()
{
    qApp->exit();
}

QVariantMap ukccSessionServer::getModuleHideStatus()
{
    QVariantMap moduleRes;
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-hide.json";

    QString json = readFile(filename);
    if (json.isEmpty()) {
        qWarning("Could not read JSON file!");
        return moduleRes;
    }

    bool ok;
    JsonObject result = QtJson::parse(json, ok).toMap();
    if (!ok) {
        qWarning("An error occurred during parsing");
        return moduleRes;
    }
    return getJsonInfo(result, result.keys());
}
