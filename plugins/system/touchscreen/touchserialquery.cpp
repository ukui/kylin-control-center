extern "C"{
#include <libudev.h>
#include <X11/extensions/XInput2.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <xorg/xserver-properties.h>
}
/* Get device node for gudev
   node like "/dev/input/event6"
 */
unsigned char *
get_device_node (XIDeviceInfo devinfo)
{
    Atom  prop;
    Atom act_type;
    int  act_format;
    unsigned long nitems, bytes_after;
    unsigned char *data;


    prop = XInternAtom(GDK_DISPLAY_XDISPLAY (gdk_display_get_default()), XI_PROP_DEVICE_NODE, False);
    if (!prop)
        return NULL;

    gdk_x11_display_error_trap_push (gdk_display_get_default ());
    if (XIGetProperty(GDK_DISPLAY_XDISPLAY (gdk_display_get_default()), devinfo.deviceid, prop, 0, 1000, False,
                      AnyPropertyType, &act_type, &act_format, &nitems, &bytes_after, &data) == Success)
    {
        gdk_x11_display_error_trap_pop (gdk_display_get_default ());
        return data;
    }
    gdk_x11_display_error_trap_pop (gdk_display_get_default ());

    XFree(data);
    return NULL;
}

static int find_event_from_touchId(int pId ,char *_event, int max_len)
{
    Display *_dpy = XOpenDisplay(NULL);
    int ret = -1;
    if(NULL == _dpy || NULL == _event)
    {
        printf("[%s%d] NULL ptr. \n", __FUNCTION__, __LINE__);
        return ret;
    }
    int         	i           = 0;
    int         	num_devices = 0;
    XIDeviceInfo 	*devs_info;
    unsigned char 	*cNode      = NULL;
    const char  	cName[]     = "event";
    const char      *cEvent     = NULL;

    devs_info = XIQueryDevice(_dpy, XIAllDevices, &num_devices);

    for(i = 0; i < num_devices; i++)
    {
        cNode = get_device_node(devs_info[i]);
        if(NULL == cNode)
        {
            continue;
        }
        printf("[%s%d] cNode:%s ptr:%s\n",__FUNCTION__, __LINE__, cNode, cEvent);

        if( pId == devs_info[i].deviceid)
        {
            cNode = get_device_node(devs_info[i]);
            if(NULL == cNode)
            {
                continue;
            }
            cEvent = strstr((const char*)cNode, cName);
            if(NULL == cEvent)
            {
                continue;
            }

            strncpy(_event, cEvent, max_len);
            //printf("cEvent=%s,_event=%s\n",cEvent,_event);
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
            strncpy(_serial, pSerial, max_len);
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

int findSerialFromId(int touchid,char *touchname,char *_touchserial,int maxlen)
{
    char event[32]={0};
    int ret=find_event_from_touchId(touchid, event, 32);
    ret=find_serial_from_event(touchname, event,_touchserial,maxlen);
    if(!strcmp(_touchserial,""))
        strncpy(_touchserial,"kydefault",maxlen);
    return ret;
}
