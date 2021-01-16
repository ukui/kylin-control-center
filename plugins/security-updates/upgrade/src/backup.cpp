#include "backup.h"

#define TIMESTAMP_PATH "/var/lib/kylin-software-properties/template/kylin-source-status"
#define TIMESTAMP_TAB "UpdateTime="
#define BACKINFO_STATE "0"

#define BACKUP_DBUS_SERVICE "com.kylin.backup"
#define BACKUP_DBUS_PATH "/"
#define BACKUP_DBUS_INTERFACE "com.kylin.backup.manager"

#define BACKUP_PATH "/usr/bin/kybackup"

const int needBack = 99;

BackUp::BackUp(QObject *parent) : QObject(parent)
{
    interface = new QDBusInterface(BACKUP_DBUS_SERVICE,BACKUP_DBUS_PATH, BACKUP_DBUS_INTERFACE,QDBusConnection::systemBus());
    connect(interface,SIGNAL(sendRate(int,int)),this,SLOT(sendRate(int,int)));
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
    qDebug()<<"状态码:"<<sta<<"  进度："<<pro<<"%";
    if(sta!=1&&sta!=2&&sta!=4&&sta!=5&&sta!=99)
    {//备份失败
        emit bakeupFinish(sta);
        return;
    }
    emit backupProgress(pro);
    if(pro == 100)
    {
        setProgress = false;
        emit bakeupFinish(99);
    }
}

void BackUp::startBackUp(int num)
{
    if(num==1)
    {
        QDBusReply<int> reply = interface->call("autoBackUpForSystemUpdate",timeStamp);
        emit bakeupFinish(reply.value());
        if(reply.value()==0)
        {
            setProgress = true;
        }
        return;
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
            uint time = line.toUInt(&ok);
            if(!ok)
            {
                file.close();
                qDebug()<<"源管理器配置文件不合规";
                return false;
            }
            QDateTime datetime = QDateTime::fromTime_t(time);
            QString filename = "自动备份："+datetime.toString("yyyy-MM-dd hh:mm:ss")+"("+line+")";
            timeStamp = filename;
            break;
        }
    }
    file.close();
    if(timeStamp=="")
    {
        qDebug()<<"源管理器配置文件不合规";
        return false;
    }

    qDebug()<<"读取源管理器配置文件成功";
    return true;
}

bool BackUp::readBackToolInfo()
{
    QDBusMessage msg = interface->call("getBackupCommentForSystemUpdate");
    QVariantList list = msg.arguments();
    if(list.length()<2)
    {
        qDebug()<<"备份还原接口异常";
    }
    if(list.at(0).toString()!=timeStamp)
    {
        qDebug()<<"未找到相同版本备份镜像，需要备份";
        return true;
    }
    qDebug()<<"找到相同版本镜像";
    if(list.at(1).toString() == "0")
    {
        qDebug()<<"已存在相同版本备份镜像，无需备份";
        return false;
    }
    return true;
}
