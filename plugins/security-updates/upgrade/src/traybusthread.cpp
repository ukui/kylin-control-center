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
    if (reply.isValid()) {
        inameList = reply.value();
        qDebug() << inameList;

    } else {
        qDebug() << "value method called failed!";
    }
    emit result(inameList);
    QDBusReply<QString> rep = m_pServiceInterface->call("quitslots");

}
