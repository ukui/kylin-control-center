extern "C"{
#include <libudev.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XInput.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
}

static int find_event_from_touchId(int pId ,char *_event,char *devnode,int max_len)
{
    Display *_dpy = XOpenDisplay(NULL);
    int ret = -1;
    if(NULL == _dpy || NULL == _event)
    {
        printf("[%s%d] NULL ptr. \n", __FUNCTION__, __LINE__);
        return ret;
    }
    int         	i           = 0;
    int         	j           = 0;
    int         	num_devices = 0;
    XDeviceInfo 	*pXDevs_info = NULL;
    XDevice         *pXDev       = NULL;
    unsigned char 	*cNode      = NULL;
    const char  	cName[]     = "event";
    const char      *cEvent     = NULL;
    int             nprops       = 0;
    Atom            *props       = NULL;
    char            *name;
    Atom            act_type;
    int             act_format;
    unsigned long   nitems, bytes_after;
    unsigned char   *data;


    pXDevs_info = XListInputDevices(_dpy, &num_devices);
    for(i = 0; i < num_devices; i++)
    {
        pXDev = XOpenDevice(_dpy, pXDevs_info[i].id);
        if (!pXDev)
        {
            printf("unable to open device '%s'\n", pXDevs_info[i].name);
            continue;
        }

        props = XListDeviceProperties(_dpy, pXDev, &nprops);
        if (!props)
        {
            printf("Device '%s' does not report any properties.\n", pXDevs_info[i].name);
            continue;
        }
        //printf("pId=%d, pXDevs_info[i].id=%d \n",pId,pXDevs_info[i].id);
        if(pId == pXDevs_info[i].id)
        {
            for(j = 0; j < nprops; j++)
            {
                name = XGetAtomName(_dpy, props[j]);
                if(0 != strcmp(name, "Device Node"))
                {
                    continue;
                }
                XGetDeviceProperty(_dpy, pXDev, props[j], 0, 1000, False,
                                    AnyPropertyType, &act_type, &act_format,
                                    &nitems, &bytes_after, &data);
                cNode = data;
            }
            if(NULL == cNode)
            {
                continue;
            }
            cEvent = strstr((const char*)cNode, cName);
            if(NULL == cEvent)
            {
                continue;
            }
            strcpy(devnode,(const char*)cNode);
            strncpy(_event, cEvent, max_len>0?(max_len-1):max_len);
            //printf("cNode=%s,cEvent=%s,_event=%s\n",cNode,cEvent,_event);
            ret = Success;
            break;
        }

    }

    return ret;
}

static int find_serial_from_event(char *_name, char *_event, char *_serial, int max_len)
{
    int ret = -1;
    if((NULL == _name) || (NULL == _event))
    {
        printf("[%s%d] NULL ptr. \n", __FUNCTION__, __LINE__);
        return ret;
    }

    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    udev = udev_new();
    enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *pPath;
        const char *pEvent;
        const char cName[] = "event";
        pPath = udev_list_entry_get_name(dev_list_entry);
        //printf("[%s%d] path: %s\n",__FUNCTION__, __LINE__, pPath);
        dev = udev_device_new_from_syspath(udev, pPath);
        //touchScreen is usb_device
        dev = udev_device_get_parent_with_subsystem_devtype(
                dev,
                "usb",
                "usb_device");
        if (!dev)
        {
            //printf("Unable to find parent usb device. \n");
            continue;
        }

        const char *pProduct = udev_device_get_sysattr_value(dev,"product");
        pEvent = strstr(pPath, cName);
        if(NULL == pEvent || (NULL == pProduct))
        {
            continue;
        }
        //printf("pEvent=%s,_event=%s\n",pEvent,_event);
        //printf("_name=%s,pProduct=%s\n",_name,pProduct);
        char *ret=strstr(_name, pProduct);
        if((NULL!=ret) && (0 == strcmp(_event, pEvent)))
        {
            const char *pSerial = udev_device_get_sysattr_value(dev, "serial");
            //printf(" _serial:%s\n  pSerial: %s\n",_serial, pSerial);
            if(NULL == pSerial)
            {
                continue;
            }
            strncpy(_serial, pSerial, max_len>0?(max_len-1):max_len);
            ret = Success;
            //printf(" _serial:%s\n  pSerial: %s\n",_serial, pSerial);
            break;
        }

        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return ret;
}

int findSerialFromId(int touchid,char *touchname,char *_touchserial,char *devnode,int maxlen)
{
    char event[32]={0};
    int ret=find_event_from_touchId(touchid, event,devnode, 32);
    ret=find_serial_from_event(touchname, event,_touchserial,maxlen);
    if(!strcmp(_touchserial,""))
        strncpy(_touchserial,"kydefault",maxlen>0?(maxlen-1):maxlen);
    return ret;
}
