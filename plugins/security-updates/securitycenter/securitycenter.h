#ifndef SECURITYCENTER_H
#define SECURITYCENTER_H

#include <QWidget>
#include <QObject>
#include <QtPlugin>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QList>

#include <QMouseEvent>
#include <QLabel>
#include <QTimer>

#include "shell/interface.h"
#include "FlowLayout/flowlayout.h"
#include "ksc_main_page_widget.h"

class SecurityCenter : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    SecurityCenter();
    ~SecurityCenter();

public:
    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

private:
    QString pluginName;
    int pluginType;
    ksc_main_page_widget * pluginWidget;

    bool mFirstLoad;

};
#endif // SECURITYCENTER_H
