#ifndef MONITORINPUTTASK_H
#define MONITORINPUTTASK_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QDBusConnection>

#include <QDebug>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>

class MonitorInputTask : public QObject
{
    Q_OBJECT
public:
    bool m_running;

public:
    static MonitorInputTask* instance(QObject *parent = nullptr);

public Q_SLOTS:
    void StartManager();

Q_SIGNALS:
    /*!
     * \brief slaveAdded 从设备添加
     * \param device_id
     */
    void slaveAdded(int device_id);
    /*!
     * \brief slaveRemoved 从设备移除
     * \param device_id
     */
    void slaveRemoved(int device_id);
    /*!
     * \brief masterAdded 主分支添加
     * \param device_id
     */
    void masterAdded(int device_id);
    /*!
     * \brief masterRemoved 主分支移除
     * \param device_id
     */
    void masterRemoved(int device_id);
    /*!
     * \brief deviceEnable 设备启用
     * \param device_id
     */
    void deviceEnable(int device_id);
    /*!
     * \brief deviceDisable 设备禁用
     * \param device_id
     */
    void deviceDisable(int device_id);
    /*!
     * \brief slaveAttached 从设备附加
     * \param device_id
     */
    void slaveAttached(int device_id);
    /*!
     * \brief slaveDetached 从设备分离
     * \param device_id
     */
    void slaveDetached(int device_id);

private:
    MonitorInputTask(QObject *parent = nullptr);
    void initConnect();
    /*!
     * \brief ListeningToInputEvent 监听所有输入设备的事件
     */
    void ListeningToInputEvent();
    /*!
     * \brief EventSift 筛选出发生事件的设备ID
     * \param event 所有设备的事件信息
     * \param flag 当前发生的事件
     * \return 查找失败  return -1;   查找成功 return device_id;
     */
    int EventSift(XIHierarchyEvent *event, int flag);

};

#endif // MONITORINPUTTASK_H
