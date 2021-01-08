
#ifndef XINPUTMANAGER_H
#define XINPUTMANAGER_H
#include "monitorinputtask.h"
#include <QObject>
#include <QThread>




class XinputManager : public QObject
{
    Q_OBJECT
public:
    XinputManager(QObject *parent = nullptr);

    void start();
    void stop();

Q_SIGNALS:
    void sigStartThread();
    void xinputSlaveAdded(int device_id);
    void xinputSlaveRemoved(int device_id);

private:
    void init();

private:
    QThread *m_pManagerThread;
    QMutex m_runningMutex;
    MonitorInputTask *m_pMonitorInputTask;

private Q_SLOTS:
    void onSlaveAdded(int device_id);
    void onSlaveRemoved(int device_id);

private:
    void SetPenRotation(int device_id);

};


#endif // XINPUTMANAGER_H

