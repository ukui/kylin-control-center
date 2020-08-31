#ifndef VISBLEBUTTON_H
#define VISBLEBUTTON_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class VisbleButton : public QLabel
{
    Q_OBJECT
public:
    explicit VisbleButton(QWidget *parent = nullptr);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void setChecked(bool checked);
private:
    bool status;

signals:
    void clicked(bool checked);
    void toggled(bool checked);
};

#endif // VISBLEBUTTON_H
