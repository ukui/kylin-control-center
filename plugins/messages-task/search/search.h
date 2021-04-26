#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QGSettings>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <SwitchButton/switchbutton.h>
#include <QGSettings>
#include "shell/interface.h"
#include "ComboxFrame/comboxframe.h"

#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define SEARCH_METHOD_KEY "indexSearch"
#define WEB_ENGINE_KEY "webEngine"

namespace Ui {
class Search;
}

class Search : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    explicit Search();
    ~Search();

    QString get_plugin_name()   Q_DECL_OVERRIDE;
    int get_plugin_type()       Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui()   Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

private:
    Ui::Search *ui;

    QWidget * m_plugin_widget = nullptr;
    QString m_plugin_name = "";
    int m_plugin_type = 0;

    QGSettings * m_gsettings = nullptr;

    void initUi();
    QVBoxLayout * m_mainLyt = nullptr;
    QLabel * m_methodTitleLabel = nullptr;
    QLabel * m_descLabel = nullptr;
    QFrame * m_searchMethodFrame = nullptr;
    QHBoxLayout * m_searchMethodLyt = nullptr;
    QLabel * m_searchMethodLabel = nullptr;
    SwitchButton * m_searchMethodBtn = nullptr;

    QLabel * m_webEngineLabel = nullptr;
    ComboxFrame * m_webEngineFrame = nullptr;
};

#endif // SEARCH_H
