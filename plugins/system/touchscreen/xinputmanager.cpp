

#include "xinputmanager.h"
#include <QProcess>
#include <QQueue>


XinputManager::XinputManager(QObject *parent):
    QObject(parent)
{
    init();
}

void XinputManager::init()
{
    m_pMonitorInputTask = MonitorInputTask::instance();
    connect(this, &XinputManager::sigStartThread, m_pMonitorInputTask, &MonitorInputTask::StartManager);
    connect(m_pMonitorInputTask, &MonitorInputTask::slaveAdded, this, &XinputManager::onSlaveAdded);
    connect(m_pMonitorInputTask, &MonitorInputTask::slaveRemoved, this, &XinputManager::onSlaveRemoved);

    m_pManagerThread = new QThread(this);
    m_pMonitorInputTask->moveToThread(m_pManagerThread);
}

void XinputManager::start()
{
    qDebug() << "info: [XinputManager][start]: thread id = " << QThread::currentThreadId();
    m_runningMutex.lock();
    m_pMonitorInputTask->m_running = true;
    m_runningMutex.unlock();

    m_pManagerThread->start();
    Q_EMIT sigStartThread();
}

void XinputManager::stop()
{
    if(m_pManagerThread->isRunning())
    {
        m_runningMutex.lock();
        m_pMonitorInputTask->m_running = false;
        m_runningMutex.unlock();

        m_pManagerThread->quit();
    }
}

void XinputManager::onSlaveAdded(int device_id)
{
    qDebug() << "info: [XinputManager][onSlaveAdded]: Slave Device(id =" << device_id << ") Added!";
    Q_EMIT xinputSlaveAdded(device_id);
}

void XinputManager::onSlaveRemoved(int device_id)
{
    qDebug() << "info: [XinputManager][onslaveRemoved]: Slave Device(id =" << device_id << ") Removed!";
    Q_EMIT xinputSlaveRemoved(device_id);
}

