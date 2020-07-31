#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include "svghandler.h"
#include "tooltips.h"

class InfoLabel : public QLabel
{
    Q_OBJECT
public:
    explicit InfoLabel(QWidget *parent = nullptr);
    void setTipText(QString text);

    void            leaveEvent(QEvent *e);
    void enterEvent(QEvent *e);
private:
    SVGHandler *m_svgHandler;
    Tooltips *m_toolTips;
    QLabel  *m_textLabel;
signals:

};

#endif // INFOLABEL_H
