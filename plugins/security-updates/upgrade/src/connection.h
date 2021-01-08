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

static bool CreatConnection()
{
    qDebug() << "connect db";
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE","A");
    QString dirPath = QString("%1/.cache/kylin-update-manager/").arg(QDir::homePath());
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

    qDebug() << "set db";
    db1.setDatabaseName(dbPath);
    if (!db1.open()) {
           qDebug()<<"open sql error";
           return false;
    }
    qDebug() << "open db";

    qDebug() << "over";
    return true;
}

#endif // CONNECTION_H
