#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <QWidget>
#include <QObject>
#include <QtPlugin>

#include "shell/interface.h"
#include "widget.h"


namespace Ui { class TouchScreen; }


class TouchScreen : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    TouchScreen();
    ~TouchScreen();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

private:
    Ui::TouchScreen *ui;
    QString pluginName;
    int pluginType;
    Widget * pluginWidget;

    bool mFirstLoad;
};
#endif // TOUCHSCREEN_H
