#include "updatesource.h"
#include "updatesource.h"

UpdateSource::UpdateSource(QObject *parent) : QObject(parent)
{

}

void UpdateSource::startDbus()
{
    serviceInterface = new QDBusInterface("com.kylin.software.properties",
                                                          "/com/kylin/software/properties",
                                                          "com.kylin.software.properties.interface",
                                                          QDBusConnection::systemBus());
    if(!serviceInterface->isValid())
    {
        qDebug() << "源管理器：" <<"Service Interface: " << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    emit startDbusFinished();
}
/*
 * 调用源管理器更新源模版接口
*/
void UpdateSource::callDBusUpdateTemplate()
{
    QDBusPendingCall call = serviceInterface->asyncCall("updateSourceTemplate");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call,this);
    connect(watcher,&QDBusPendingCallWatcher::finished,this,&UpdateSource::getReply);
    qDebug() <<"源管理器：" << "callDBusUpdateTemplate: " << "updateSourceTemplate";
}
/*
 * 调用源管理器更新源缓存接口
*/
void UpdateSource::callDBusUpdateSource(QString symbol)
{
   QDBusPendingCall call = serviceInterface->asyncCall("updateSourcePackages",symbol);
   qDebug() << "源管理器：" <<"Call updateSourcePackages" ;

}

QString UpdateSource::getFailInfo(int statusCode)
{
    qDebug()<< "UpdateSource::getFailInfo::::"<< statusCode;
    QDBusReply<QString> replyStr;   //string类型的返回值
    replyStr = serviceInterface->call("getFailInfo",statusCode);
    if (replyStr.isValid()) {
        qDebug() <<"源管理器：" << "getFailInfo:"<<replyStr.value();
        return replyStr.value();

    }
    else{
        qDebug() << "源管理器：" <<QString("调用失败 getFailInfo");
        return QString(tr("Connection failed, please reconnect!"));
//        return QString(tr("连接失败，请重新连接！"));
    }

}
void UpdateSource::getReply(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<bool> reply = *call;
    if (!reply.isValid()) {
         qDebug() <<"源管理器：" << "getReply:" << "iserror";
    } else {
            bool status = reply.value();
            qDebug() <<"源管理器：" << "getReply:" << status;
            if (status) {
                callDBusUpdateSource(Symbol);
            }
            else
            {
                emit getReplyFalseSignal();
            }

    }
}
