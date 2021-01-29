#ifndef LOADINGLABEL_H
#define LOADINGLABEL_H

#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QPaintEvent>

class LoadingLabel : public QLabel
{
public:
    explicit LoadingLabel(QObject *parent = nullptr);
    ~LoadingLabel();
    void setTimerStop();
    void setTimerStart();
    void setTimeReresh(int);

private slots:
    void Refresh_icon();
private:
    QTimer *m_timer;
    int i;
};

#endif // LOADINGLABEL_H
