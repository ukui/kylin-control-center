#include "bluetooth.h"

#include <QDebug>

Bluetooth::Bluetooth() : mFirstLoad(true) {
    pluginName = tr("Bluetooth");
    pluginType = DEVICES;
}

Bluetooth::~Bluetooth() {
    if (!mFirstLoad) {
//        delete pluginWidget;
    }
}

QString Bluetooth::get_plugin_name() {
    return pluginName;
}

int Bluetooth::get_plugin_type() {
    return pluginType;
}

QWidget *Bluetooth::get_plugin_ui() {

    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new BlueToothMain;
    }
    return pluginWidget;
}

void Bluetooth::plugin_delay_control() {

}

const QString Bluetooth::name() const {
    return QStringLiteral("bluetooth");
}


