#ifndef CONNECTION_H
#define CONNECTION_H
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QMessageBox>
#include<QApplication>
#include<QSqlError>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

static bool CreatConnection()
{
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE","A");
    QString dirPath = QString("/var/cache/kylin-update-manager");
    QString dbPath = QString("%1/kylin-update-manager.db").arg(dirPath);
    QDir dir(dirPath);
    if(!dir.exists())
    {
      dir.mkpath(dirPath);
    }
    QFile file(dbPath);
    if(!file.exists())
    {
        QFile::copy("/usr/share/kylin-update-manager/kylin-update-manager.db", dbPath);
    }
    db1.setDatabaseName(dbPath);
    if (!db1.open()) {
           qDebug()<<"更新管理器数据库打开失败.";
           return false;
    }
    qDebug()<<"更新管理器数据库打开成功.";

    /* 获取软件商店数据库链接 */
    QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE" , "B");
    db2.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.cache/uksc/uksc.db");
    if (!db2.open()) {
        qDebug() << "Error : open software database fail";
        return false;
    }

    qDebug() << "open software center database success!";

    return true;
}

#endif // CONNECTION_H
