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
        QString res = in.readAll();
        f.close();
        return res;
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

void ukccSessionServer::initModuleStatus(const QString &fileName) {

    QFile file(fileName);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open error";
        return;
    }

    JsonObject totalObj;

    JsonObject systemObj;
    systemObj["system"] = true;
    systemObj["display"] = true;
    systemObj["power"] = true;
    systemObj["autoboot"] = true;
    totalObj["system"] = systemObj;

    JsonObject deviceObj;
    deviceObj["device"] = true;
    deviceObj["printer"] = true;
    deviceObj["mouse"] = true;
    deviceObj["touchpad"] = true;
    deviceObj["keyboard"] = true;
    deviceObj["shortcut"] = true;
    deviceObj["audio"] = true;
    totalObj["device"] = deviceObj;

    JsonObject personlizeObj;
    personlizeObj["personalized"] = true;
    personlizeObj["background"] = true;
    personlizeObj["theme"] = true;
    personlizeObj["fonts"] = true;
    personlizeObj["screensaver"] = true;
    personlizeObj["desktop"] = true;
    totalObj["personalized"] = personlizeObj;

    JsonObject networkObj;
    networkObj["network"] = true;
    networkObj["netconnect"] = true;
    networkObj["vpn"] = true;
    networkObj["proxy"] = true;
    networkObj["vino"] = true;
    totalObj["network"] = networkObj;

    JsonObject accountObj;
    accountObj["account"] = true;
    accountObj["userinfo"] = true;
    accountObj["networkaccount"] = true;
    totalObj["account"] = accountObj;

    JsonObject datetimeObj;
    datetimeObj["datetime"] = true;
    datetimeObj["dat"] = true;
    datetimeObj["area"] = true;
    totalObj["datetime"] = datetimeObj;

    JsonObject updateObj;
    updateObj["update"] = true;
    updateObj["sercuritycenter"] = true;
    updateObj["updates"] = true;
    updateObj["backup"] = true;
    totalObj["update"] = updateObj;

    JsonObject noticeandtasksObj;
    noticeandtasksObj["noticeandtasks"] = true;
    noticeandtasksObj["notice"] = true;
    noticeandtasksObj["about"] = true;
    noticeandtasksObj["experiencepaln"] = true;
    totalObj["noticeandtasks"] = updateObj;

    file.write(QtJson::serialize(totalObj));
    file.close();
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
        initModuleStatus(filename);
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

QString ukccSessionServer::getModuleConfFile() {
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-hide.json";

    return filename;
}
