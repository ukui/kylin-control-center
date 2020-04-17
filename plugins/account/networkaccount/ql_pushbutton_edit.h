#ifndef QL_PUSHBUTTON_EDIT_H
#define QL_PUSHBUTTON_EDIT_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include "qtooltips.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QTimer>

class ql_pushbutton_edit : public QPushButton
{
    Q_OBJECT
public:
    explicit        ql_pushbutton_edit(QWidget *parent = nullptr);
    void            enterEvent(QEvent *e);
    void            leaveEvent(QEvent *e);
protected:
    QToolTips        *tips;
    QLabel          *label;
    QHBoxLayout     *layout;
    QTimer          *pTimer;
public slots:
signals:

};

#endif // QL_PUSHBUTTON_EDIT_H
