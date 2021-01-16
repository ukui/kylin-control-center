#include "updatedbus.h"
#include "connection.h"
#include <QMutexLocker>
#define PROGRAM_NAME "control-upgrade"
#define PATH_MAX_LEN 1024
#define PID_STRING_LEN 64

UpdateDbus* UpdateDbus::updateMutual = nullptr;
using namespace std;

UpdateDbus* UpdateDbus::getInstance()
{
    static QMutex mutex;
    if(nullptr == updateMutual)
    {
        QMutexLocker locker(&mutex);
        updateMutual = new UpdateDbus;
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

    init_cache();
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
    args<<(QCoreApplication::applicationName())
       <<((unsigned int) 0)
      <<QString("qweq")
     <<tr("控制面板-更新提示") //显示的是什么类型的信息
    <<message //显示的具体信息
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

bool UpdateDbus::fileLock()
{
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd < 0)
    {
        qDebug() << "文件锁的文件不存在，程序退出。";
        exit(0);
    }
    return lockf(fd, F_TLOCK, 0);
}

void UpdateDbus::fileUnLock()
{
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd < 0)
    {
        qDebug() << "解锁文件不存在，程序退出。";
        exit(0);
    }
    lockf(fd, F_ULOCK, 0);
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
    qDebug() << "copy_file_to_install";
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
        qDebug() << QString("Call failed makeDirs");
    }
}

// setImportantStatus
bool UpdateDbus::setImportantStatus(bool status)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("set_important_status", status);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() <<"setImportantStatus:"<<replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed setImportantStatus");
    }
}

//安装和升级
bool UpdateDbus::installAndUpgrade(QString pkgName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("install_and_upgrade",pkgName);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << "installAndUpgrade:" <<replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed installAndUpgrade");
    }
}



void UpdateDbus::modifyConf(QString path, QString group, QString key, QString value)
{
    replyStr = interface->call("modify_conf",path,group,key,value);
    qDebug() << QString("Call modify_conf");

}

bool UpdateDbus::cleanUpdateList()
{
    replyBool = interface->call("clear_install_list");
    if (replyBool.isValid()) {
        qDebug() << "cleanUpdateList:"<<replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed clear_install_list");
    }
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
        qDebug() << "value method called failed!";
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
        qDebug() << QString("Call failed check_installed_or_upgrade");
    }
}

void UpdateDbus::getAppMessage(QStringList list)
{
    interface->asyncCall("get_app_message",list);
    qDebug() << "Call get_app_message";
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
    qDebug() << "update quit";
    cleanUpdateList();
    setImportantStatus(false);
}
