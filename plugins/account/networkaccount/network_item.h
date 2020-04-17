#ifndef NETWORK_ITEM_H
#define NETWORK_ITEM_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include "ql_swichbutton.h"

class network_item : public QWidget
{
    Q_OBJECT
public:
    explicit        network_item(QWidget *parent = nullptr);
    void            set_itemname(QString name);
    QString         get_itemname();
    void            make_itemoff();
    void            make_itemon();
    QHBoxLayout*    get_layout();
    QL_SwichButton* get_swbtn();
    QWidget*        get_widget();
private:
    QLabel          *label_item;
    QHBoxLayout     *layout;
    QWidget         *widget;
    int             on;
    QL_SwichButton  *switch_btn;
signals:

};

#endif // NETWORK_ITEM_H
