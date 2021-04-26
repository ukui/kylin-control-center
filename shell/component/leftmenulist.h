#ifndef LEFTMENULIST_H
#define LEFTMENULIST_H

#include <QListWidget>
#include <QResizeEvent>

class LeftMenuList : public QListWidget
{
public:
    LeftMenuList(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // LEFTMENULIST_H
