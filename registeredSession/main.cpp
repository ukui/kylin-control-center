#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "sessiondbusregister.h"
#include "./conf/SessionAdaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName("Kylin Team");
    app.setApplicationName("ukcc-serriosn-service");

    SessionDbusRegister service;
    new SessionAdaptor(&service);

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService("com.control.center.qt.sessiondbus")){
        qCritical() << "QDbus register service failed reason:" << sessionBus.lastError();
        exit(1);
    }

    if (!sessionBus.registerObject("/com/control/center/qt/sessiondbus",
                                   &service,
                                   QDBusConnection::ExportAllSlots |
                                   QDBusConnection::ExportAllSignals)) {
        qCritical() << "QDbus register object failed reason:" << sessionBus.lastError();
        exit(2);
    }           

//    SessionDbusRegister *dbus = new SessionDbusRegister;
//    dbus->getModuleHideStatus();

    return app.exec();
}
