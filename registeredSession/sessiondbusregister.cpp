#include "sessiondbusregister.h"

SessionDbusRegister::SessionDbusRegister(QObject *parent) : QObject(parent)
{

}

SessionDbusRegister::~SessionDbusRegister()
{

}

QString SessionDbusRegister::readFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

QMap<QString, QVariant> SessionDbusRegister::getJsonInfo(const JsonObject &result, const QStringList &keys)
{
    QMap<QString, QVariant> moduleMap;

    for (int i = 0; i < keys.length(); i++) {
        QMap<QString, QVariant> childMap = result[keys.at(i)].toMap();
        QMapIterator<QString, QVariant> it(childMap);
        while (it.hasNext()) {
            it.next();
            moduleMap.insert(it.key(), it.value());
        }
    }
    return moduleMap;
}

void SessionDbusRegister::exitService() {
    this->exitService();
}

QMap<QString, QVariant> SessionDbusRegister::getModuleHideStatus()
{
    qDebug() << Q_FUNC_INFO;
    QMap<QString, QVariant> moudleRes;
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }
    QString filename = QDir::homePath() + "/.config/ukui-control-center-hide.json";

    QString json = readFile(filename);
    if (json.isEmpty()) {
        qFatal("Could not read JSON file!");
        return moudleRes;
    }

    bool ok;
    JsonObject result = QtJson::parse(json, ok).toMap();
    QMap<QString, QVariant> keyMap = QtJson::parse(json, ok).toMap();
    if (!ok) {
        qFatal("An error occurred during parsing");
        return moudleRes;
    }

    return getJsonInfo(result, keyMap.keys());
}
