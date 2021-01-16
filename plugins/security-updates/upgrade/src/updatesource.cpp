#include "updatesource.h"
#include "updatesource.h"

UpdateSource::UpdateSource(QObject *parent) : QObject(parent)
{
    serviceInterface = new QDBusInterface("com.kylin.software.properties",
                                                          "/com/kylin/software/properties",
                                                          "com.kylin.software.properties.interface",
                                                          QDBusConnection::systemBus());
    if(!serviceInterface->isValid())
    {
        qDebug() << "Service Interface: " << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
}
/*
 * 调用源管理器更新源模版接口
*/
void UpdateSource::callDBusUpdateTemplate()
{
    serviceInterface->asyncCall("updateSourceTemplate");

    qDebug() << "callDBusUpdateTemplate: " << "updateSourceTemplate";
}
/*
 * 调用源管理器更新源缓存接口
*/
void UpdateSource::callDBusUpdateSource(QString symbol)
{
   QDBusPendingCall call = serviceInterface->asyncCall("updateSourcePackages",symbol);
   qDebug() << "Call updateSourcePackages" ;
   QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call,this);
   connect(watcher,&QDBusPendingCallWatcher::finished,this,&UpdateSource::getReply);
}

QString UpdateSource::getFailInfo(int statusCode)
{
    qDebug()<< "UpdateSource::getFailInfo::::"<< statusCode;
    QDBusReply<QString> replyStr;   //string类型的返回值
    replyStr = serviceInterface->call("getFailInfo",statusCode);
    if (replyStr.isValid()) {
        qDebug() << "getFailInfo:"<<replyStr.value();
        return replyStr.value();

    }
    else{
        qDebug() << QString("Call failed getFailInfo");
    }

}
void UpdateSource::getReply(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<int> reply = *call;
    if (!reply.isValid()) {
        qDebug() <<"getReply:" << "iserror";
    } else {
        int status = reply.value();
    }
    call->deleteLater();
}
