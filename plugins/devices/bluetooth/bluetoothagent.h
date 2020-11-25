#ifndef BLUETOOTHAGENT_H
#define BLUETOOTHAGENT_H

#include "./pincodewidget.h"

#include <QObject>
#include <KF5/BluezQt/bluezqt/agent.h>
#include <KF5/BluezQt/bluezqt/adapter.h>
#include <KF5/BluezQt/bluezqt/device.h>
#include <KF5/BluezQt/bluezqt/request.h>

#include <QDBusObjectPath>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>
#include <QMessageBox>

class PinCodeWidget;

class BluetoothAgent : public BluezQt::Agent
{
    Q_OBJECT

public:
    explicit BluetoothAgent(QObject *parent = nullptr);

    QDBusObjectPath objectPath() const override;

    void requestPinCode(BluezQt::DevicePtr device, const BluezQt::Request<QString> &request) override;
    void displayPinCode(BluezQt::DevicePtr device, const QString &pinCode) override;
    void requestPasskey(BluezQt::DevicePtr device, const BluezQt::Request<quint32> &request) override;
    void displayPasskey(BluezQt::DevicePtr device, const QString &passkey, const QString &entered) override;
    void requestConfirmation(BluezQt::DevicePtr device, const QString &passkey, const BluezQt::Request<> &request) override;
    void requestAuthorization(BluezQt::DevicePtr device, const BluezQt::Request<> &request) override;
    void authorizeService(BluezQt::DevicePtr device, const QString &uuid, const BluezQt::Request<> &request) override;

    void cancel() override;
    void release() override;

    int daemonIsNotRunning();

signals:
    void requestAccept();
    void requestReject();
private:

    BluezQt::DevicePtr m_device;
    // requestPinCode
    bool m_pinRequested;
    // displayPinCode
    QString m_displayedPinCode;
    // requestPasskey
    bool m_passkeyRequested;
    // displayPasskey
    QString m_displayedPasskey;
    QString m_enteredPasskey;
    // requestConfirmation
    QString m_requestedPasskey;
    // requestAuthorization
    bool m_authorizationRequested;
    // authorizeService
    QString m_authorizedUuid;
    // cancel
    bool m_cancelCalled;
    // release
    bool m_releaseCalled;

    PinCodeWidget *pincodewidget;
};

#endif // BLUETOOTHAGENT_H
