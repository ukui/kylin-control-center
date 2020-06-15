#ifndef QL_ANIMATION_LABEL_H
#define QL_ANIMATION_LABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include "ql_svg_handler.h"

class ql_animation_label : public QWidget
{
    Q_OBJECT
public:
    explicit ql_animation_label(QWidget *parent = nullptr);
    QLabel  *text;
    QLabel  *icon;
    void settext(QString t);
    void startmoive();
    void stop();
    QTimer *timer;
    QHBoxLayout *layout;
    ql_svg_handler *svg_hd;
    int cnt = 1;
signals:

};

#endif // QL_ANIMATION_LABEL_H
