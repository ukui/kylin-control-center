#ifndef UTILS_H
#define UTILS_H
#include "updatedbus.h"
#include <QDir>
#define DOWN_CACHE_PATH QString("%1/.cache/kylin-update-manager/").arg(QDir::homePath())
#define DOWN_TEMP_PATH QString("%1/.cache/kylin-update-manager/kylin-update-manager-deb/deb/").arg(QDir::homePath())

struct UrlMsg //记录单包信息：包名、全名、链接、大小
{
    QString name = "";
    QString fullname = "";
    QString url = "";
    int size;
};

struct AppMsg //记录当前包信息：包名、依赖列表、总大小、获取依赖状态
{
    QVector<UrlMsg> depList;
    long allSize = 0;
    bool getDepends = true;
};

struct AppAllMsg
{
    QString name;
    QString section;
    QString origin;
    int installedSize;
    QString maintainer;
    QString source;
    QString version;
    int packageSize;
    QString shortDescription;
    QString longDescription;

    QString changedPackages;
    QString packageCount;

    QString changelogUrl;
    QString screenshotUrl;
    QString availableVersion;

    bool isInstalled;
    bool upgradeable;
    AppMsg msg;

};
#endif // UTILS_H
