#ifndef QL_SWICHBUTTON_H
#define QL_SWICHBUTTON_H

#include <QWidget>
#include <QTimer>
#include <QPainter>

class QL_SwichButton : public QWidget
{
    Q_OBJECT
public:
    explicit        QL_SwichButton(QWidget *parent = nullptr);
    void            set_swichbutton_val(int on);
    int             get_swichbutton_val();
    void            set_id(int id);
    int             get_id();
private:
    int             on = 1;
    QTimer          *timer;
    float           ql_width;
    float           ql_height;
    float           cur_val;
    int             id;
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
signals:
    void            status(int on,int id);
private slots:
    void startAnimation();
};

#endif // QL_SWICHBUTTON_H
