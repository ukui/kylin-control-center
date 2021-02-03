#include "updatedbus.h"
#include "connection.h"
#include <QMutexLocker>
#define PROGRAM_NAME "control-upgrade"
#define PATH_MAX_LEN 1024
#define PID_STRING_LEN 64

UpdateDbus* UpdateDbus::updateMutual = nullptr;
using namespace std;

UpdateDbus* UpdateDbus::getInstance(QObject *parent)
{
    static QMutex mutex;
    if(nullptr == updateMutual)
    {
        QMutexLocker locker(&mutex);
        updateMutual = new UpdateDbus(parent);
    }
    return updateMutual;
}

UpdateDbus::UpdateDbus(QObject *parent)
    :QObject(parent)
{
    qRegisterMetaType<AppMsg>("AppMsg"); //注册信号槽类型
    qRegisterMetaType<AppAllMsg>("AppAllMsg"); //注册信号槽类型
    interface = new QDBusInterface(KYLIN_UPDATE_MANAGER_SERVICE,
                                   KYLIN_UPDATE_MANAGER_PATH,
                                   KYLIN_UPDATE_MANAGER_INTERFACE,
                                   QDBusConnection::systemBus());

    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("kum_apt_signal"), this, SLOT(getAptSignal(QString, QMap<QString, QVariant>)));

    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("important_app_message_signal"), this, SLOT(getAppMessageSignal(QMap<QString, QVariant>, QStringList, QStringList, QStringList, QStringList, QString, bool)));

    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("get_message_finished_signal"), this, SLOT(slotFinishGetMessage(QString)));

    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("copy_finish"), this, SLOT(slotCopyFinished(QString)));

    cleanUpdateList();
    setImportantStatus(true);

}

void UpdateDbus::onRequestSendDesktopNotify(QString message)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(tr("ukui-control-center"))
       <<((unsigned int) 0)
      <<("ukui-control-center")
     <<tr("ukui-control-center-update") //显示的是什么类型的信息  控制面板-更新提示
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

bool UpdateDbus::fileLock()
{
    int uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    QString nameuid = QString("%1(%2)\n").arg(pwd->pw_name).arg(uid);
    char*  charnameuid;
    QByteArray ba = nameuid.toLatin1(); // must
    charnameuid=ba.data();
    qDebug()<<"文件锁打开 用户ID：" << charnameuid;

    QDir dir("/tmp/lock/");
    if(dir.exists()){
        char name_string[20] = {"ukui-control-center"};
        umask(0000);
        int fd = open(lockPath.toUtf8().data(), O_RDWR | O_CREAT | O_TRUNC,0666);
        if(fd < 0)
        {
            qDebug() << "文件锁的文件不存在，程序退出。";
            return false;
        }
        else{
            write(fd, charnameuid, strlen(charnameuid));
            write(fd, name_string, strlen(name_string));
        }
        return lockf(fd, F_TLOCK, 0);
    }else {
        dir.mkdir("/tmp/lock/");//只创建一级子目录，即必须保证上级目录存在
        chmod("/tmp/lock/",0777);

        //初始化运行程序名称
        char name_string[20] = {"ukui-control-center"};
        umask(0000);
        int fd = open(lockPath.toUtf8().data(), O_RDWR | O_CREAT | O_TRUNC,0666);
        if (fd < 0) {
            qDebug()<<"文件锁打开异常";
            return false;
        } else {
            write(fd, charnameuid, strlen(charnameuid));
            write(fd, name_string, strlen(name_string));
        }
        return lockf(fd, F_TLOCK, 0);
    }

}

void UpdateDbus::fileUnLock()
{
    QDir dir("/tmp/lock/");
    if(dir.exists()) {
        umask(0000);
        int fd = open(lockPath.toUtf8().data(), O_RDWR | O_CREAT | O_TRUNC,0666);
        if (fd < 0) {
            qDebug()<<"解锁时文件锁打开异常";
            return;
        } else {
            ftruncate(fd, 0);
        }
        lockf(fd, F_ULOCK, 0);

    } else {
        dir.mkdir("/tmp/lock/");//只创建一级子目录，即必须保证上级目录存在
        chmod("/tmp/lock/",0777);
        umask(0000);
        int fd = open(lockPath.toUtf8().data(), O_RDWR | O_CREAT | O_TRUNC,0666);
        if (fd < 0) {
            qDebug()<<"解锁时文件锁打开异常";
            return;
        } else {
            ftruncate(fd, 0);
        }
        lockf(fd, F_ULOCK, 0);
    }
}

void UpdateDbus::slotFinishGetMessage(QString num)
{
    int inum = num.toInt();
    emit sendFinishGetMsgSignal(inum);
}


//拷贝软件包到安装目录，拷贝之前需要判断是否存在archives目录
void UpdateDbus::copyFinsh(QStringList srcPath, QString appName)
{
    QDir dir(QString("/var/cache/apt/archives/"));
    if(!dir.exists())
    {
        makeDirs(QString("/var/cache/apt/archives/"));
    }
    replyStr = interface->call("copy_file_to_install",srcPath,appName);
    qDebug() << "拷贝软件包到安装目录，调用接口copy_file_to_install";
}

bool UpdateDbus::makeDirs(QString path)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("makedirs", path);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << "makeDirs" << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("调用失败： makeDirs");
        return false;
    }
}

// setImportantStatus
void UpdateDbus::setImportantStatus(bool status)
{
    interface->asyncCall("set_important_status", status);
    qDebug() <<"更新管理器-设置状态值" <<"setImportantStatus:"<<status;
}

//安装和升级
bool UpdateDbus::installAndUpgrade(QString pkgName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    interface->asyncCall("install_and_upgrade",pkgName);

    // 将reply.value()作为返回值
    return true;
}



void UpdateDbus::modifyConf(QString path, QString group, QString key, QString value)
{
    replyStr = interface->call("modify_conf",path,group,key,value);
    qDebug() << QString("调用 modify_conf接口");

}

void UpdateDbus::cleanUpdateList()
{
    qDebug() << "更新管理器清除列表：cleanUpdateList";
    interface->asyncCall("clear_install_list");
}

//初始化cache
void UpdateDbus::init_cache()
{

    interface->call("init_cache");
    qDebug() << " call init_cache";
}

void UpdateDbus::getInameAndCnameList(QString arg)
{

    qDebug()<<"getsignal";
    qDebug()<<arg;
    //调用远程的value方法
    QDBusReply<QStringList> reply = interface1->call("getImportant");
    if (reply.isValid()) {
        inameList = reply.value();
        qDebug() << inameList;
//        qDebug() <<  value;
    } else {
        qDebug() << "value method 调用失败!";
    }
//    emit sendImportant();

}

void UpdateDbus::getAppMessageSignal(QMap<QString, QVariant> map, QStringList urlList, QStringList nameList,QStringList fullnameList,QStringList sizeList, QString allSize, bool dependState)
{
    QVariant dateQVariant;
    AppAllMsg appAllMsg;
    QVariantMap::Iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        if (it.key() == "appname")
        {
            dateQVariant = it.value();
            appAllMsg.name = dateQVariant.toString();
        }
        if(it.key() == "current_version")
        {
            dateQVariant = it.value();
            appAllMsg.version = dateQVariant.toString();
        }
        if(it.key() == "source_version")
        {
            dateQVariant = it.value();
            appAllMsg.availableVersion = dateQVariant.toString();
        }
        if(it.key() == "size")
        {
            dateQVariant = it.value();
            appAllMsg.packageSize = dateQVariant.toInt();
        }
        if(it.key() == "description")
        {
            dateQVariant = it.value();
            appAllMsg.longDescription = dateQVariant.toString();
        }
    }
    if(urlList.length() != 0)
    {
        for(int i = 0; i < urlList.length(); i++)
        {
            UrlMsg msg;
            msg.url = urlList.at(i);
            msg.name = nameList.at(i);
            msg.fullname = fullnameList.at(i);
            QString size = sizeList.at(i);
            msg.size = size.toInt();
            appAllMsg.msg.depList.append(msg);
        }
    }
    appAllMsg.msg.allSize = allSize.toLong();
    appAllMsg.msg.getDepends = dependState;
//    qDebug() << "获取信息" << appAllMsg.name << appAllMsg.longDescription;
    emit sendAppMessageSignal(appAllMsg);
}

void UpdateDbus::insertInstallStates(QString item,QString info)
{
    interface->asyncCall("insert_install_state",item,info);
}

QStringList UpdateDbus::checkInstallOrUpgrade(QStringList list)
{
    replyStrList = interface->call("check_installed_or_upgrade",list);
    if (replyStrList.isValid()) {
        return replyStrList.value();
    }
    else{
        QStringList error;
        qDebug() << QString("check_installed_or_upgrade接口调用失败！");
        return error;
    }
}

void UpdateDbus::getAppMessage(QStringList list)
{
    interface->asyncCall("get_app_message",list);
    qDebug() << "调用 get_app_message";
}


void UpdateDbus::getAptSignal(QString arg, QMap<QString, QVariant> map)
{

    QString aptStatus;
    QString aptAppName;
    QString errorMessage;
    float aptPercent;

    QVariant dateQVariant;
    aptStatus = arg;

    qDebug() << "安装状态" << arg;


    QVariantMap::Iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        if (it.key() == "apt_appname")
        {
            dateQVariant = it.value();
            aptAppName = dateQVariant.toString();
        }
        if(it.key() == "apt_percent")
        {
            dateQVariant = it.value();
            aptPercent = dateQVariant.toFloat();
        }
        if(it.key() == "error_message")
        {
            dateQVariant = it.value();
            errorMessage = dateQVariant.toString();
        }
    }

    qDebug() << "aptAppName:" << aptAppName;
    qDebug() << "aptPercent:" << aptPercent;

    emit transferAptProgress(aptStatus,aptAppName,aptPercent,errorMessage);

}

void UpdateDbus::slotCopyFinished(QString appName)
{
    emit copyFinish(appName);
}


UpdateDbus::~UpdateDbus()
{
//    qDebug() << "update quit------>";
    cleanUpdateList();
    setImportantStatus(false);
    fileUnLock();
}
