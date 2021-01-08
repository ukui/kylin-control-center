#ifndef UPDATELOG_H
#define UPDATELOG_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "widgetstyle.h"
#include "shadowwidget.h"


class UpdateLog : public QWidget
{

public:
    explicit UpdateLog(QWidget *parent = nullptr);

    void logUi();

    QLabel *logAppIcon;
    QLabel *logAppName;
    QLabel *logAppVerson;

    QWidget *logWidget;
    QWidget *logAppWidget;

    QHBoxLayout *logAppHLayout;
    QVBoxLayout *logAppVLayout;

    QVBoxLayout *logVLayout;

    QVBoxLayout *logMainLayout;

    QTextEdit *logContent;

    void logStyle();


    void paintEvent(QPaintEvent *);


};



#endif // UPDATELOG_H
