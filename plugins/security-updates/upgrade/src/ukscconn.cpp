#include "ukscconn.h"
UKSCConn::UKSCConn()
{
    ukscDb = QSqlDatabase::addDatabase("QSQLITE");

    QString UKSC_CACHE_DIR = QString(getenv("HOME")) + "/.cache/uksc/";
    QString UKSC_DATA_DIR = "/usr/share/kylin-software-center/data/";
    QString KUM_DIR = "/usr/share/kylin-update-manager/";
    QString dbFileName = "";

    dbFileName = UKSC_CACHE_DIR + "uksc.db";
    QFile file(dbFileName);

    if (!file.exists()) {
        dbFileName = UKSC_DATA_DIR + "uksc.db";
        file.setFileName(dbFileName);

        if (!file.exists()) {
            dbFileName = KUM_DIR + "uksc.db";
            file.setFileName(dbFileName);
            if (file.exists()) {
                ukscDb.setDatabaseName(dbFileName);
            }
        }
    }

    qDebug() << "dbFileName  " << dbFileName;

    ukscDb.setDatabaseName(dbFileName);
    if (!ukscDb.open())
    {
        QMessageBox::warning(0, QObject::tr("Database Error"),
                                     ukscDb.lastError().text());
        qDebug() << "Database Error";
    }

    query=QSqlQuery(ukscDb);//连接数据库

}

// 根据应用名获取应用信息
QStringList UKSCConn::getInfoByName(QString appName)
{
    QStringList appInfo;

    QString str = QString("select * from application where app_name = '%1'").arg(appName);
    query.exec(str);
    QString iconPath;       // 应用图标
    QString displayNameCN;  // 应用中文名
    QString displayName;  // 应用英文名
    QString description;    // 应用描述

    while (query.next())
    {
        iconPath = query.value(15).toString();
        displayNameCN = query.value(4).toString();
        displayName = query.value(3).toString();
        description =  query.value(7).toString();
    }
    iconPath = QString(getenv("HOME")) + "/.cache/uksc/icons/" + appName + ".png";

    appInfo.append(iconPath);
    appInfo.append(displayNameCN);
    appInfo.append(displayName);
    appInfo.append(description);

    return appInfo;
}
