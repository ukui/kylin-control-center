#ifndef QL_POPUP_H
#define QL_POPUP_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QVBoxLayout>

class ql_popup : public QWidget
{
    Q_OBJECT
public:
    explicit        ql_popup(QWidget *parent = nullptr);
    int             xpos;
    int             ypos;
    int             radius;
    int             alpha;
protected:
    void            paintEvent(QPaintEvent *event);
signals:

};

#endif // QL_POPUP_H
