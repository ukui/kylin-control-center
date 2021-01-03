#include "upgrade.h"

#include <QWidget>
#include <QMainWindow>

Upgrade::Upgrade() :mFirstLoad(true) {
    //~ contents_path /upgrade/Upgrade
    pluginName = tr("Upgrade");
    pluginType = UPDATE;
}

Upgrade::~Upgrade() {

}

QString Upgrade::get_plugin_name() {
    return pluginName;
}

int Upgrade::get_plugin_type() {
    return pluginType;
}

QWidget *Upgrade::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        // will delete by takewidget
        pluginWidget = new MainWindow("");
    }

    return pluginWidget;
}

void Upgrade::plugin_delay_control() {

}

const QString Upgrade::name() const {
    return QStringLiteral("upgrade");
}
