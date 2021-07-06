#include "touchscreen.h"
#include "ui_touchscreen.h"


#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/output.h>
#include <QDebug>
#include <QThread>

TouchScreen::TouchScreen() : mFirstLoad(true)
{
    pluginName = tr("TouchScreen");
    pluginType = SYSTEM;
}

TouchScreen::~TouchScreen() {
}

QWidget *TouchScreen::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new Widget;
        QObject::connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
                         [&](KScreen::ConfigOperation *op) {
            QThread::usleep(20000);
            pluginWidget->setConfig(qobject_cast<KScreen::GetConfigOperation*>(op)->config());
        });
    }
    return pluginWidget;
}

QString TouchScreen::get_plugin_name() {
    return pluginName;
}

int TouchScreen::get_plugin_type() {
    return pluginType;
}

void TouchScreen::plugin_delay_control() {

}

const QString TouchScreen::name() const {

    return QStringLiteral("touchscreen");
}
