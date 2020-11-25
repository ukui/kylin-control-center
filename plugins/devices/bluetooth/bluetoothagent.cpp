#include "bluetoothagent.h"


BluetoothAgent::BluetoothAgent(QObject *parent)
    : Agent(parent)
    , m_pinRequested(false)
    , m_passkeyRequested(false)
    , m_authorizationRequested(false)
    , m_cancelCalled(false)
    , m_releaseCalled(false)
{
    qDebug() << Q_FUNC_INFO;
    if(daemonIsNotRunning()){
        QDBusConnection bus = QDBusConnection::systemBus();
        // 在session bus上注册名为"com.kylin_user_guide.hotel"的service

        if (!bus.registerService("org.bluez.Agent1")) {  //注意命名规则-和_
                qDebug() << bus.lastError().message();
                exit(1);
        }
        // "QDBusConnection::ExportAllSlots"表示把类Hotel的所有Slot都导出为这个Object的method
        bus.registerObject("/", this ,QDBusConnection::ExportAllContents);
    }
}

QDBusObjectPath BluetoothAgent::objectPath() const
{
    return QDBusObjectPath(QStringLiteral("/BluetoothAgent"));
}

void BluetoothAgent::requestPinCode(BluezQt::DevicePtr device, const BluezQt::Request<QString> &request)
{
    qDebug() << Q_FUNC_INFO;
    m_device = device;
    m_pinRequested = true;

    request.accept(QString());

}

void BluetoothAgent::displayPinCode(BluezQt::DevicePtr device, const QString &pinCode)
{
    m_device = device;
    m_displayedPinCode = pinCode;
    qDebug() << Q_FUNC_INFO;
}

void BluetoothAgent::requestPasskey(BluezQt::DevicePtr device, const BluezQt::Request<quint32> &request)
{
    qDebug() << Q_FUNC_INFO;
    m_device = device;
    m_passkeyRequested = true;

    request.accept(0);
}

void BluetoothAgent::displayPasskey(BluezQt::DevicePtr device, const QString &passkey, const QString &entered)
{
    qDebug() << Q_FUNC_INFO;
    m_device = device;
    m_displayedPasskey = passkey;
    m_enteredPasskey = entered;
}

void BluetoothAgent::requestConfirmation(BluezQt::DevicePtr device, const QString &passkey, const BluezQt::Request<> &request)
{
    qDebug() << Q_FUNC_INFO << device->name() << passkey;
    m_device = device;
    m_requestedPasskey = passkey;

    pincodewidget = new PinCodeWidget(device->name(),passkey);

    connect(pincodewidget,&PinCodeWidget::accepted,this,[=]{
        request.accept();
    });

    connect(pincodewidget,&PinCodeWidget::rejected,this,[=]{
        request.reject();
    });

    pincodewidget->exec();

//    QMessageBox msgBox;
//    msgBox.setText(device->name()+"------"+passkey);
//    msgBox.exec();

//    request.accept();
}

void BluetoothAgent::requestAuthorization(BluezQt::DevicePtr device, const BluezQt::Request<> &request)
{
    qDebug() << Q_FUNC_INFO;
    m_device = device;
    m_authorizationRequested = true;

    request.accept();
}

void BluetoothAgent::authorizeService(BluezQt::DevicePtr device, const QString &uuid, const BluezQt::Request<> &request)
{
    qDebug() << Q_FUNC_INFO;
    m_device = device;
    m_authorizedUuid = uuid;

    request.accept();
}

void BluetoothAgent::cancel()
{
    qDebug() << Q_FUNC_INFO;
    m_cancelCalled = true;
}

void BluetoothAgent::release()
{
    qDebug() << Q_FUNC_INFO;
    m_releaseCalled = true;
}

int BluetoothAgent::daemonIsNotRunning()
{
    QDBusConnection conn = QDBusConnection::systemBus();
    if (!conn.isConnected())
        return 0;

    QDBusReply<QString> reply = conn.interface()->call("GetNameOwner", "org.bluez.Agent1");
    return reply.value() == "";
}
