#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include "svghandler.h"

class InfoLabel : public QLabel
{
    Q_OBJECT
public:
    explicit InfoLabel(QWidget *parent = nullptr);

    void            leaveEvent(QEvent *e);
    void            enterEvent(QEvent *e);
private:
    SVGHandler *m_svgHandler;
signals:

};

#endif // INFOLABEL_H
