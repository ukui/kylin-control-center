#include "monitorinputtask.h"

//#include "xinputmanager.h"

MonitorInputTask* MonitorInputTask::instance(QObject *parent)
{
    static MonitorInputTask *_instance = nullptr;
    QMutex mutex;
    mutex.lock();
    if(!_instance)
            _instance = new MonitorInputTask(parent);
    mutex.unlock();
    return _instance;
}

MonitorInputTask::MonitorInputTask(QObject *parent):
    QObject(parent),
    m_running(false)
{
    initConnect();
}

void MonitorInputTask::initConnect()
{

}

void MonitorInputTask::StartManager()
{
    qDebug() << "info: [MonitorInputTask][StartManager]: thread id = " << QThread::currentThreadId();
    QTimer::singleShot(0, this, &MonitorInputTask::ListeningToInputEvent);
}

int MonitorInputTask::EventSift(XIHierarchyEvent *event, int flag)
{
    int device_id = -1;
    int cnt = event->num_info;
    XIHierarchyInfo *event_list = event->info;
    for(int i = 0;i < cnt;i++)
    {
        if(event_list[i].flags & flag)
        {
            device_id = event_list[i].deviceid;
        }
    }
    return device_id;
}

void MonitorInputTask::ListeningToInputEvent()
{
    qDebug() << "info: [MonitorInputTask][ListeningToInputEvent]: Start ListeningToInputEvent!";

    Display *display = NULL;
    // open display
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        qDebug() << "info: [MonitorInputTask][ListeningToInputEvent]: Failed to open display.";
        return;
    }
    XIEventMask mask[2];
    XIEventMask *m;
    Window win;

    win = DefaultRootWindow(display);

    m = &mask[0];
    m->deviceid = XIAllDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = (unsigned char*)calloc(m->mask_len, sizeof(char));
    XISetMask(m->mask, XI_HierarchyChanged);

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = (unsigned char*)calloc(m->mask_len, sizeof(char));

    XISelectEvents(display, win, &mask[0], 2);
    XSync(display, False);

    free(mask[0].mask);
    free(mask[1].mask);

    while(true)
    {
        XEvent ev;
        XGenericEventCookie *cookie = (XGenericEventCookie*)&ev.xcookie;
        XNextEvent(display, (XEvent*)&ev);
        // 判断当前事件监听是否还要继续
        // 保证效率 m_running[*bool] 的访问不需要加锁
        if(!m_running) break;

        if (XGetEventData(display, cookie) &&
            cookie->type == GenericEvent)
        {
            if(XI_HierarchyChanged == cookie->evtype)
            {

                XIHierarchyEvent *hev = (XIHierarchyEvent*)cookie->data;
                if(hev->flags & XIMasterRemoved)
                {
                    Q_EMIT masterRemoved(EventSift(hev, XIMasterRemoved));
                }
                else if(hev->flags & XISlaveAdded)
                {
                    Q_EMIT slaveAdded(EventSift(hev, XISlaveAdded));
                }
                else if(hev->flags & XISlaveRemoved)
                {
                    Q_EMIT slaveRemoved(EventSift(hev, XISlaveRemoved));
                }
                else if(hev->flags & XISlaveAttached)
                {
                    Q_EMIT slaveAttached(EventSift(hev, XISlaveAttached));
                }
                else if(hev->flags & XISlaveDetached)
                {
                    Q_EMIT slaveDetached(EventSift(hev, XISlaveDetached));
                }
                else if(hev->flags & XIDeviceEnabled)
                {
                    Q_EMIT deviceEnable(EventSift(hev, XIDeviceEnabled));
                }
                else if(hev->flags & XIDeviceDisabled)
                {
                    Q_EMIT deviceDisable(EventSift(hev, XIDeviceDisabled));
                }
                else if(hev->flags & XIMasterAdded)
                {
                    Q_EMIT masterAdded(EventSift(hev, XIMasterAdded));
                }
            }
        }
        XFreeEventData(display, cookie);
    }

    XDestroyWindow(display, win);
}
