#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "ukccsessionserver.h"
#include "session_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName("Kylin Team");
    app.setApplicationName("ukcc-session-service");

    ukccSessionServer service;
    InterfaceAdaptor *iftApt = new InterfaceAdaptor(&service);

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService("org.ukui.ukcc.session")) {
        qCritical() << "QDbus register service failed reason:" << sessionBus.lastError();
        exit(1);
    }

    if (!sessionBus.registerObject("/", &service)) {
        qCritical() << "QDbus register object failed reason:" << sessionBus.lastError();
        exit(2);
    }
    delete iftApt;
    return app.exec();
}
