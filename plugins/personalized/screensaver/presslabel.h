#ifndef PRESSLABEL_H
#define PRESSLABEL_H

#include <QLabel>

class PressLabel : public QLabel
{
    Q_OBJECT
public:
    PressLabel(QWidget *parent = nullptr);
    ~PressLabel();
    void mousePressEvent(QMouseEvent *event);
};
#endif // PRESSLABEL_H
