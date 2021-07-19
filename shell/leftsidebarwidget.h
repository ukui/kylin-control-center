#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <QWidget>
#include <QObject>


class LeftsidebarWidget : public QWidget
{
    Q_OBJECT
public:
    LeftsidebarWidget(QWidget *parent = nullptr);
    ~LeftsidebarWidget();
    void paintEvent(QPaintEvent *event);
private:
    int transparency = 0;


};

#endif // LEFTSIDEBARWIDGET_H
