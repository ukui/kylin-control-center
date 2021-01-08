#ifndef LISTWIG_H
#define LISTWIG_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "changelogwidget.h"

class listwig : public QWidget
{
    Q_OBJECT
public:
    explicit listwig(QWidget *parent = nullptr);
    QLabel *m_Appnamelabel;
    QLabel *m_Versionlabel;
    QLabel *m_Timelabel;
    QLabel *m_Descriptionlabel;
    QLabel *m_statuelabel;

    QHBoxLayout  *m_entryHlayout   = nullptr;
    QVBoxLayout  *m_entryVlayout   = nullptr;
    QWidget      *namewidget;
    changelogwidget *changeLogWidget;
    void mousePressEvent(QMouseEvent *e);//鼠标点击事件

protected:
    void paintEvent(QPaintEvent *e);

signals:

};

class headerwidget : public QWidget
{
    Q_OBJECT
public:
    explicit headerwidget(QWidget *parent = nullptr);
    QLabel *firstlabel;
    QLabel *secondlabel;
    QLabel *thirdlabel;

    QHBoxLayout  *m_entryHlayout   = nullptr;


protected:
    void paintEvent(QPaintEvent *e);



signals:

};

#endif // LISTWIG_H
