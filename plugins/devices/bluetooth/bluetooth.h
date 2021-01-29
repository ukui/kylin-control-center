#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QWidget>

#include "shell/interface.h"
#include "bluetoothmain.h"

class Bluetooth : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)
public:
    Bluetooth();
    ~Bluetooth();

    QString get_plugin_name()   Q_DECL_OVERRIDE;
    int get_plugin_type()       Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui()   Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

private:
    QString pluginName;
    int pluginType;
    BlueToothMain * pluginWidget;
    bool mFirstLoad;

};

#endif // BLUETOOTH_H
