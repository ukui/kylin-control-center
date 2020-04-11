#ifndef CONFIG_LIST_WIDGET_H
#define CONFIG_LIST_WIDGET_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QMovie>
#include "item_list.h"
#include "network_item.h"
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include "ql_pushbutton_edit.h"
#include "dialog_login_reg.h"
#include <QMessageBox>
#include "editpassdialog.h"
#include "config_file.h"
#include <libkylinssoclient/libkylinssoclient.h>

class config_list_widget : public QWidget
{
    Q_OBJECT
public:
    explicit        config_list_widget(QWidget *parent = nullptr);
    ~config_list_widget();
    QLabel*         get_title();
    QLabel*         get_info();
    QWidget*        get_login_dialog();
    void            setshow(QWidget *w);
    void            set_client(libkylinssoclient *c);
    void            init_gui();
    void            handle_conf();
    bool            judge_item(QString enable,int cur);
protected:
private:
    item_list       *list;
    QString         qss_btn_str = "QPushButton#status[is_on=false]{font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:#FFFFFF;}"
                          "QPushButton#status[is_on=true] {font-size:14px;position:relative;color:#FFFFFF;border-radius:4px;background-color: #3D6BE5}"
                          "QPushButton#status[is_on=false]:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:#FFFFFF;}"
                          "QPushButton#status[is_on=false]:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:#FFFFFF;}";
    network_item    *auto_syn;
    QLabel          *title;
    QLabel          *info;
    QLabel          *gif;
    QLabel          *gif_step;
    QPushButton     *exit_page;
    LoginDialog     *ld;
    QWidget         *container;
    QWidget         *tab;
    QMovie          *pm;
    QMovie          *pm_step;
    QListWidgetItem *items[6];
    QVBoxLayout     *vboxlayout;
    QPushButton     *edit;
    QGraphicsDropShadowEffect *shadow;
    EditPassDialog      *edit_dialog;
    QStackedWidget  *stacked_widget;
    QWidget         *null_widget;
    libkylinssoclient   *client;
    QPushButton         *login;
    QLabel              *logout;
    QLabel              *title2;
    QVBoxLayout         *vlayout;
    QHBoxLayout         *hlayout;
    QVBoxLayout         *cvlayout;
    QString             code;
    QString             home;
    QStringList         mapid = {tr("ukui-menu"),tr("ukui-panel"),tr("ukui-panel2"),tr("ukui-control-center"),tr("indicator-china-weather"),tr("kylin-video")};

public slots:
    void            neweditdialog();
    void            on_login_out();
    void            set_login_process();
    void            on_login();
    void            open_cloud();
    void            finished_load(int ret);
    void            on_switch_button(int on,int id);
    void            on_auto_syn(int on,int id);
signals:

};

#endif // CONFIG_LIST_WIDGET_H
