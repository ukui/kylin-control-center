#include "backup.h"

#define TIMESTAMP_PATH "/var/lib/kylin-software-properties/template/kylin-source-status"
#define TIMESTAMP_TAB "UpdateTime="
#define BACKINFO_PATH "/backup/snapshots/backuplists.xml"
#define BACKINFO_TAB1 "<Comment>"
#define BACKINFO_TAB2 "</Comment>"
#define BACKINFO_TAB3 "<State>"
#define BACKINFO_TAB4 "</State>"
#define BACKINFO_STATE "backup finished"

#define BACKUP_DBUS_SERVICE "com.kylin.backup"
#define BACKUP_DBUS_PATH "/"
#define BACKUP_DBUS_INTERFACE "com.kylin.backup.manager"

#define BACKUP_PATH "/usr/bin/kybackup"

const int needBack = 99;

BackUp::BackUp(QObject *parent) : QObject(parent)
{
    interface = new QDBusInterface(BACKUP_DBUS_SERVICE,BACKUP_DBUS_PATH, BACKUP_DBUS_INTERFACE,QDBusConnection::systemBus());
    connect(interface,SIGNAL(sendRate(int,int)),this,SLOT(sendRate(int,int)));
//    QDBusConnection::connect()
}


int BackUp::needBacdUp()
{
    //备份工具是否存在
    QFileInfo file(BACKUP_PATH);
    if(!file.exists())
        return -9;
    //备份工具是否可用
    if(!haveBackTool())
        return -1;
    //正在备份
    if(bakeupState==1||bakeupState==2||bakeupState==4||bakeupState==5)
        return 1;
    //备份工具占用
    if(bakeupState!=needBack)
        return -2;
    //获取时间戳
    if(!readSourceManagerInfo())
        return -3;
    //比对上次备份时间戳
    if(!readBackToolInfo())
        return -4;
    return needBack;
}

void BackUp::sendRate(int sta,int pro)
{
    if(!setProgress)
        return;
    emit backupProgress(pro);
    qDebug()<<"sta:"<<sta;
}

void BackUp::startBackUp(int num)
{
    if(num==1)
    {
        QDBusReply<int> reply = interface->call("autoBackUpForSystemUpdate",timeStamp);
        emit bakeupFinish(reply.value());
    }
    emit bakeupFinish(0);
    setProgress = true;
}

bool BackUp::haveBackTool()
{
    if(interface==nullptr)
    {
        qDebug()<<"dbus对象未实例化";
        return false;
    }
    if(!interface->isValid())
    {
        qDebug()<<"未创建备份还原分区";
        return false;
    }
    QDBusReply<int> reply = interface->call("getBackupState");
    if(!reply.isValid())
    {
        qDebug()<<"备份还原接口异常";
        return false;
    }
    bakeupState = reply.value();
//    if(bakeupState==0)
//    {
//        qDebug()<<"未安装新版麒麟备份还原工具";
//        return false;
//    }
    qDebug()<<"bakeupState:"<<bakeupState;
    return true;
}

bool BackUp::readSourceManagerInfo()
{
    QFile file(TIMESTAMP_PATH);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"读取源管理器配置文件失败";
        return false;
    }
    while (!file.atEnd()) {
        QString line = file.readLine();
        if(line.contains(TIMESTAMP_TAB,Qt::CaseSensitive))
        {
            line=line.replace("\n","");
            line=line.replace(TIMESTAMP_TAB,"");
            bool ok;
            line.toInt(&ok);
            if(!ok)
            {
                file.close();
                qDebug()<<"源管理器配置文件不合规";
                return false;
            }
            timeStamp = line;
            break;
        }
    }
    file.close();
    if(timeStamp=="")
    {
        qDebug()<<"源管理器配置文件不合规";
        return false;
    }
    return true;
}

bool BackUp::readBackToolInfo()
{
    QFile file(BACKINFO_PATH);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"读取备份还原工具配置文件失败";
        return true;
    }
    bool tag = false;
    while (!file.atEnd()) {
        QString line = file.readLine();
        if(line.contains(BACKINFO_TAB1,Qt::CaseSensitive))
        {
            line=line.replace("\n","");
            line=line.replace(BACKINFO_TAB1,"");
            line=line.replace(BACKINFO_TAB2,"");
            if(line == timeStamp)
            {
                tag = true;
            }
        }
        if(line.contains(BACKINFO_TAB3,Qt::CaseSensitive))
        {
            if(!tag)
                continue;
            tag = false;
            line=line.replace("\n","");
            line=line.replace(BACKINFO_TAB3,"");
            line=line.replace(BACKINFO_TAB4,"");
            if(line == BACKINFO_STATE)
            {
                file.close();
                return false;
            }
        }
    }
    file.close();
    return true;
}
