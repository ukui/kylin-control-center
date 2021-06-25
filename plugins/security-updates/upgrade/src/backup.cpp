#include "backup.h"
#include <kybackup/backuptools-define.h>
#include <unistd.h>
#include <sys/types.h>

#define TIMESTAMP_PATH "/var/lib/kylin-software-properties/template/kylin-source-status"
#define TIMESTAMP_TAB "UpdateTime="
#define BACKINFO_STATE "0"

#define BACKUP_DBUS_SERVICE "com.kylin.backup"
#define BACKUP_DBUS_PATH "/"
#define BACKUP_DBUS_INTERFACE "com.kylin.backup.manager"

#define BACKUP_PATH "/usr/bin/kybackup"

const int needBack = int(backuptools::backup_state::BACKUP_STATE_INIT);

BackUp::BackUp(QObject *parent) : QObject(parent)
{
//    interface = new QDBusInterface(BACKUP_DBUS_SERVICE,BACKUP_DBUS_PATH, BACKUP_DBUS_INTERFACE,QDBusConnection::systemBus());
//    connect(interface,SIGNAL(sendRate(int,int)),this,SLOT(sendRate(int,int)));
//    watcher = new QDBusServiceWatcher(BACKUP_DBUS_SERVICE,QDBusConnection::systemBus(),QDBusServiceWatcher::WatchForOwnerChange,this);
//    connect(watcher,  &QDBusServiceWatcher::serviceOwnerChanged,this, &BackUp::onDBusNameOwnerChanged);
}

void BackUp::onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);

    if (name == BACKUP_DBUS_SERVICE) {
        if (newOwner.isEmpty()) {
            qWarning() << "麒麟备份还原工具被中断";
            emit bakeupFinish(-20);
        } else {
            qWarning() << "麒麟备份还原工具被重启";
        }
        //Q_EMIT serviceStatusChanged(!newOwner.isEmpty());
    }
}

void BackUp::creatInterface()
{
    interface->deleteLater();
    interface = new QDBusInterface(BACKUP_DBUS_SERVICE,BACKUP_DBUS_PATH, BACKUP_DBUS_INTERFACE,QDBusConnection::systemBus());
    connect(interface,SIGNAL(sendRate(int,int)),this,SLOT(sendRate(int,int)));
#if 0
    connect(interface, SIGNAL(sendStartBackupResult(int)), this, SIGNAL(bakeupFinish(int)));
#else
    connect(interface, SIGNAL(sendStartBackupResult(int)), this, SLOT(receiveStartBackupResult(int)));
#endif

    watcher->deleteLater();
    watcher = new QDBusServiceWatcher(BACKUP_DBUS_SERVICE,QDBusConnection::systemBus(),QDBusServiceWatcher::WatchForOwnerChange,this);
    connect(watcher,  &QDBusServiceWatcher::serviceOwnerChanged,this, &BackUp::onDBusNameOwnerChanged);
}

int BackUp::needBacdUp()
{
    //构造dbus对象
    creatInterface();
    //备份工具是否存在
    QFileInfo file(BACKUP_PATH);
    if(!file.exists())
        return -9;
    //备份工具是否可用
    if(!haveBackTool())
        return -1;
    if (m_isActive) {
        //正在备份
        if(bakeupState==int(backuptools::backup_state::FULL_BACKUP_SYSUPDATE) || bakeupState==int(backuptools::backup_state::INC_BACKUP_SYSUPDATE))
            return 1;
        //备份工具占用
        if(bakeupState!=needBack)
            return -2;
    }
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
    qDebug() << "-------------------正在备份！！！！";
    qDebug()<<"状态码:"<<sta<<"  进度："<<pro<<"%";

    if (sta == int(backuptools::backup_state::DU_ING)) {
        emit calCapacity();
        return;
    }
    if(!setProgress)
        return;
    if(sta!=int(backuptools::backup_state::FULL_BACKUP_SYS)
            &&sta!=int(backuptools::backup_state::FULL_BACKUP_SYSUPDATE)
            &&sta!=int(backuptools::backup_state::INC_BACKUP_SYS)
            &&sta!=int(backuptools::backup_state::INC_BACKUP_SYSUPDATE)
            &&sta!=int(backuptools::backup_state::BACKUP_STATE_INIT)
            )
    {//备份失败
        emit bakeupFinish(sta);
        return;
    }
#if 1
    //rsync进程未正常完成(比如被强制中断)
    if (pro == -1) {
        setProgress = false;
        emit bakeupFinish(-20);
        return;
    }
#endif
    emit backupProgress(pro);
    if(pro == 100)
    {
        setProgress = false;
        emit bakeupFinish(int(backuptools::backup_state::BACKUP_STATE_INIT));
    }
}

void BackUp::receiveStartBackupResult(int result)
{

    if (result == int(backuptools::backup_result::BACKUP_START_SUCCESS))
        setProgress = true;
    emit backupStartRestult(result);
}

void BackUp::startBackUp(int num)
{
    if(num==1)
    {

        QString create_note = QObject::tr("system upgrade new backup");
        QString inc_note = QObject::tr("system upgrade increment backup");
        QString userName = qgetenv("USER");
        int uid = getuid();

        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(timeStamp) << QVariant::fromValue(create_note) << QVariant::fromValue(inc_note)
                     << QVariant::fromValue(userName) << QVariant::fromValue(uid);
        qDebug() << argumentList;
        interface->asyncCallWithArgumentList(QStringLiteral("autoBackUpForSystemUpdate_noreturn"), argumentList);
    }
}

bool BackUp::haveBackTool()
{
    if(interface==nullptr)
    {
        qDebug()<<"dbus对象未实例化";
        return false;
    }
//    if(!interface->isValid())
//    {
//        qDebug()<<"未创建备份还原分区";
//        return false;
//    }
    QDBusPendingReply<int> reply = interface->call("getBackupState");
    if(!reply.isValid())
    {
        qDebug()<<"备份还原接口异常";
        return false;
    }
    bakeupState = reply.argumentAt(0).toInt();
    m_isActive = reply.argumentAt(1).toBool();
    return true;
}

bool BackUp::readSourceManagerInfo()
{
    int i_ret = access(TIMESTAMP_PATH , F_OK);
    if (i_ret) {
        qDebug() << "源管理器配置文件不存在 ， 采用默认时间戳";
        timeStamp = "自动备份:0000-00-00 00:00:00(00000000000000)";
        return true;
    }

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
    if(list.at(0).toString()!= timeStamp)
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
