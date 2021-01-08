#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include "m_updatelog.h"

class LogWindow : public QMainWindow
{
    Q_OBJECT

public:
    LogWindow(QWidget *parent = nullptr);
    ~LogWindow();
    m_updatelog *m_log;
};
#endif // LOGWINDOW_H
