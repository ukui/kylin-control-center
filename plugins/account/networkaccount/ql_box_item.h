#ifndef QL_BOX_ITEM_H
#define QL_BOX_ITEM_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>


class ql_box_item : public QWidget
{
    Q_OBJECT
public:
    explicit ql_box_item(QWidget *parent = nullptr);
    void set_country_code(QString str);
    void set_code(QString str);
protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
private:
    QLabel *cd;
    QLabel *ct;
signals:

};

#endif // QL_BOX_ITEM_H
