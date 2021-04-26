#ifndef UPDATELOG_H
#define UPDATELOG_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>

#include "widgetstyle.h"


class UpdateLog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateLog(QDialog *parent = nullptr);

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
    QFrame *contentFrame;


    void paintEvent(QPaintEvent *);


};



#endif // UPDATELOG_H
