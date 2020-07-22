#ifndef QL_ANIMATION_LABEL_H
#define QL_ANIMATION_LABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include "svghandler.h"

class Blueeffect : public QWidget
{
    Q_OBJECT
public:
    explicit Blueeffect(QWidget *parent = nullptr);
    QLabel  *m_textLabel;
    QLabel  *m_iconLabel;
    void settext(QString t);
    void startmoive();
    void stop();
    QTimer *m_cTimer;
    QHBoxLayout *m_workLayout;
    SVGHandler *m_svgHandler;
    int m_cCnt = 1;
signals:

};

#endif // QL_ANIMATION_LABEL_H
