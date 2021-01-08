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
//    connect(serviceInterface,SIGNAL(updateSourceProgress(QString )),this,SLOT(aaaa(QString )));
}
/*
 * 调用源管理器更新源模版接口
*/
void UpdateSource::callDBusUpdateTemplate()
{
    serviceInterface->asyncCall("updateSourceTemplate");

}
/*
 * 调用源管理器更新源缓存接口
*/
void UpdateSource::callDBusUpdateSource()
{
   QDBusPendingCall call = serviceInterface->asyncCall("updateSourcePackages");
   qDebug() << "调用完了" ;
   QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call,this);
   connect(watcher,&QDBusPendingCallWatcher::finished,this,&UpdateSource::getReply);
}

void UpdateSource::getReply(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString> reply = *call;
    if (!reply.isValid()) {
        qDebug() << "iserror";
    } else {
        QString text = reply.argumentAt<0>();
        qDebug() << "QString::" << text <<"111111111111111111111";
    }
    call->deleteLater();
}
