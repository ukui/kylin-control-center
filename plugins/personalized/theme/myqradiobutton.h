#ifndef MYQRADIOBUTTON_H
#define MYQRADIOBUTTON_H

#include <QObject>
#include <QWidget>
#include <QRadioButton>

class MyQRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    MyQRadioButton(QWidget *parent = nullptr);

protected:
    virtual void leaveEvent(QEvent * event);
    virtual void enterEvent(QEvent * event);

Q_SIGNALS:
    void enterWidget();
    void leaveWidget();
};

#endif // MYQRADIOBUTTON_H
