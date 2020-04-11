#ifndef MCODE_WIDGET_H
#define MCODE_WIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTime>
#include <QPaintEvent>
#include <QPainter>

class mcode_widget : public QLabel
{
    Q_OBJECT
public:
    mcode_widget(QWidget *parent = 0);
    QChar *get_verificate_code();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *event);
private:
    int noice_point_number;
    const int letter_number = 4;
    void produceVerificationCode() const;
    //产生一个随机的字符
    QChar produceRandomLetter() const;
    //产生随机的颜色
    void produceRandomColor() const;
     QChar *verificationCode;
     QColor *colorArray;
     QList<QColor> colorList;
     bool ok = true;
     void reflushVerification();
};

#endif // MCODE_WIDGET_H
