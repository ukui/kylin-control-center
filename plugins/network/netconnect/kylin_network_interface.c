#include "kylin_network_interface.h"

#include <syslog.h>

//获取所有网络连接
//获取当前活动网络连接

activecon *kylin_network_get_activecon_info(char *path)
{
    char *chr = "nmcli connection show -active > ";
    char *cmd = (char *) malloc(strlen(chr) + strlen(path) + 1);
    strcpy(cmd, chr);
    strcat(cmd, path);
    int status = system(cmd);
    //int status = system("nmcli connection show -active > /tmp/activecon.txt");
    if (status != 0){ syslog(LOG_ERR, "execute 'nmcli connection show -active' in function 'kylin_network_get_activecon_info' failed");}
    char *filename = path;

    FILE *activefp;
    int activenum=0;
    char activeStrLine[1024];
    if((activefp=fopen(filename,"r"))==NULL)
    {
        printf("error!");

    }
    fgets(activeStrLine,1024,activefp);
    while(!feof(activefp))
    {
        fgets(activeStrLine,1024,activefp);
        activenum++;
    }
    // printf("%d\n",activenum);
    fclose(activefp);
    activecon *activelist=(activecon *)malloc(sizeof(activecon)*activenum);

    int count=0;
    FILE *fp;
    char StrLine[1024];
    if((fp=fopen(filename,"r"))==NULL)
    {
        printf("error!");

    }
    fgets(StrLine,1024,fp);
    while(!feof(fp))
    {
        if(count==activenum-1)break;

        fgets(StrLine,1024,fp);

        char *index=StrLine;
        char conname[100];

        //截取连接名称
        int num=0;
        for(index;*index!='\n';index++)
        {
            if(*index==' ')
            {
                if(*(index+1)==' ')
                    break;
            }
            num++;
        }

        // printf("连接名称长度：%d\n",num);
        activelist[count].con_name=(char *)malloc(sizeof(char)*(num+1));
        strncpy(conname,StrLine,num+1);
        conname[num]='\0';
        strncpy(activelist[count].con_name,conname,num+1);
        // printf("%s\n",activelist[count].con_name);

        //截取连接类型
        char type[100];
        for(index;*index!='\n';index++)
        {
            if(*index==' ')
            {
                if(*(index+1)=' ')
                    if(*(index+2)!=' ')
                        break;

            }
        }
        char *index1=index+2;
        for(index1;*index1!='\n';index1++)
        {
            if(*index1==' ')
            {
                if(*(index1+1)==' ')
                    break;
            }
        }
        int num1=0;
        char *index2=index1+2;
        for(index2;*index2!='\n';index2++)
        {
            if(*index2==' ')break;
            num1++;
        }
        activelist[count].type=(char *)malloc(sizeof(char)*(num1+1));
        strncpy(type,index1+2,num1+1);
        type[num1]='\0';
        strncpy(activelist[count].type,type,num1+1);
        // printf("%s\n",activelist[count].type);

        //截取连接所属设备
        char *index3=index2;
        char dev[100];
        for(index3;*index3!='\n';index3++)
        {
            if(*index3==' ')
            {
                if(*(index3+1)!=' ')
                    break;
            }
        }
        int num2=0;
        char *index4=index3+1;
        for(index4;*index4!='\n';index4++)
        {
            if(*index4==' ')break;
            num2++;
        }
        activelist[count].dev=(char *)malloc(sizeof(char)*(num2+1));
        strncpy(dev,index3+1,num2+1);
        dev[num2]='\0';
        strncpy(activelist[count].dev,dev,num2+1);
        // printf("%s\n",activelist[count].dev);
        count++;
    }
    fclose(fp);

    activelist[count].con_name=NULL;
    activelist[count].type=NULL;
    activelist[count].dev=NULL;

    return activelist;
}
