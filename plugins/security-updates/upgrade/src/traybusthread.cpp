#include "traybusthread.h"
#include <QSqlDatabase>
#include <QDateTime>

void traybusthread::run()
{

//    QTime dieTime = QTime::currentTime().addMSecs(2000);
//    while( QTime::currentTime() < dieTime )
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    m_pServiceInterface = new QDBusInterface("com.scorpio.test",
                                             "/test/objectsd",
                                             "com.scorpio.test.value",
                                             QDBusConnection::sessionBus());

    QDBusConnection::sessionBus().connect(QString("com.scorpio.test"), QString("/test/objectsd"),
                                          QString("com.scorpio.test.value"),
                                          QString("ready"), this, SLOT(getInameAndCnameList(QString)));
    QDBusReply<QStringList> reply1 = m_pServiceInterface->call("connectSuccessslots");
}

void traybusthread::getInameAndCnameList(QString arg)
{
    qDebug()<<"getsignal";
    qDebug()<<arg;
    //调用远程的value方法

    QDBusReply<QStringList> reply = m_pServiceInterface->call("getImportant");

//    QSqlDatabase db3 = QSqlDatabase::addDatabase("QSQLITE","C");
//    QString dirPath = QString("%1/.cache/kylin-update-manager/").arg(QDir::homePath());
//    QString dbPath = QString("%1/kylin-update-manager.db").arg(dirPath);
//    db3.setDatabaseName(dbPath);
//    QDateTime nowtime = QDateTime::currentDateTime();
//    QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
//    QSqlQuery query(QSqlDatabase::database("C"));
//    query.exec(QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次检测时间"));
    if (reply.isValid()) {
        inameList = reply.value();
        qDebug() << inameList;

    } else {
        qDebug() << "value method called failed!";
    }
    emit result(inameList);
    QDBusReply<QString> rep = m_pServiceInterface->call("quitslots");

}
