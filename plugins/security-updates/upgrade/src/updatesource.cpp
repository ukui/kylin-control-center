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
    serviceInterface->asyncCall("updateSourcePackages");
}
