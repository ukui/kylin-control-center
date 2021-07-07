#ifndef CONFIG_H
#define CONFIG_H

enum DEVICE_TYPE{
    PC = 0,
    PHONE,
    HEADSET,
    OTHER,
    Mouse,
};

enum DEVICE_STATUS{
    LINK = 0,
    UNLINK,
    MATCHED,
    ERROR,
    NOT,
};

#endif // CONFIG_H
