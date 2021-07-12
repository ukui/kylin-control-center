#ifndef KSC_MAIN_PAGE_WIDGET_H
#define KSC_MAIN_PAGE_WIDGET_H

#include <QWidget>
#include "defender_interface.h"
#include "FlowLayout/flowlayout.h"

class ksc_module_func_widget;

namespace Ui {
class ksc_main_page_widget;
}

class ksc_main_page_widget : public QWidget
{
    Q_OBJECT

public:
    explicit ksc_main_page_widget(QWidget *parent = nullptr);
    ~ksc_main_page_widget();

private:
    void init_list_widget();
    void auto_set_main_icon(ksc_defender_module &module);

private slots:
    void slot_recv_ksc_defender_module_change(ksc_defender_module module);

    void on_pushButton_clicked();

private:
    Ui::ksc_main_page_widget *ui;

    defender_interface *m_pInterface;

    QMap<int, ksc_module_func_widget*> m_map;

    FlowLayout * flowLayout;
};

#endif // KSC_MAIN_PAGE_WIDGET_H
