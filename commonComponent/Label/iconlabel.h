#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class IconLabel : public QLabel
{
    Q_OBJECT
public:
    IconLabel(QWidget *parent = nullptr);


protected:
    virtual void leaveEvent(QEvent * event);
    virtual void enterEvent(QEvent * event);

Q_SIGNALS:
    void enterWidget();
    void leaveWidget();
};

#endif // ICONLABEL_H
