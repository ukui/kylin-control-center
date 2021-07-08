#include "securitycenter.h"
#include "ui_securitycenter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QProcess>
#include <QDebug>
#include <QFontMetrics>

SecurityCenter::SecurityCenter() : mFirstLoad(true)
{
    pluginName = tr("Security Center");
    pluginType = UPDATE;
}

SecurityCenter::~SecurityCenter()
{
    if (!mFirstLoad) {

    }
}

QString SecurityCenter::get_plugin_name(){
    return pluginName;
}

int SecurityCenter::get_plugin_type(){
    return pluginType;
}

QWidget * SecurityCenter::get_plugin_ui(){
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new ksc_main_page_widget();
//        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
//        ui->setupUi(pluginWidget);

//        initSearchText();
//        initComponent();

//        connect(ui->pushButton, &QPushButton::clicked, [=]{
//            QString cmd = "/usr/sbin/ksc-defender";
//            runExternalApp(cmd);
//        });
    }
    return pluginWidget;
}

void SecurityCenter::plugin_delay_control(){

}

const QString SecurityCenter::name() const {

    return QStringLiteral("securitycenter");
}
