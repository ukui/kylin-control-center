#ifndef KSC_MODULE_FUNC_WIDGET_H
#define KSC_MODULE_FUNC_WIDGET_H
#include<QGSettings>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>

#include "ksc_business_def.h"

namespace Ui {
class ksc_module_func_widget;
}

class ksc_module_func_widget : public QWidget
{
    Q_OBJECT

public:
    explicit ksc_module_func_widget(QWidget *parent = nullptr);
    ~ksc_module_func_widget();

    void set_module_data(ksc_defender_module module);

    void update_module_data(ksc_defender_module module);

private:
    void runExternalApp(QString cmd) {
        QProcess process(this);
        process.startDetached(cmd);
    }

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::ksc_module_func_widget *ui;

    ksc_defender_module m_module;

    QString m_interval_icon_hover;
    QString m_interval_icon_normal;
};

#endif // KSC_MODULE_FUNC_WIDGET_H
