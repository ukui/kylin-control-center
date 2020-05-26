#ifndef KYLIN_NETWORK_INTERFACE_H
#define KYLIN_NETWORK_INTERFACE_H

#include<stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
    char *con_name;//活动网络连接名称
    char *type;//活动网络连接类型
    char *dev;//活动网络所属设备
}activecon;//存放当前活动网络连接

activecon *kylin_network_get_activecon_info(char *path);

#ifdef __cplusplus
}

#endif // KYLIN_NETWORK_INTERFACE_H

#endif

