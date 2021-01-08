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
//    fileUnLock();
//    qDebug() << "文件锁" << fileLock();
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
    return lockf(fd, F_TLOCK, 0);
}

void UpdateDbus::fileUnLock()
{
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    lockf(fd, F_ULOCK, 0);
}
void UpdateDbus::initTrayD_bus(QStringList arg)
{

//    m_traybusthread->start();
    emit updatelist(arg);
    //    m_traybusthread->quit();
}

void UpdateDbus::slotFinishGetMessage(QString num)
{
    qDebug() << "num:" << num;
    int inum = num.toInt();
    emit sendFinishGetMsgSignal(inum);
}

bool UpdateDbus::cancel(QString pkgName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("cancel", pkgName);

    // replyStrreplyStr.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed cancel");
    }
}



// 取消更新应用
void UpdateDbus::cancelDownloadApp(QString appName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyStr = interface->call("cancel_download_app", appName);

    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed cancelDownloadApp");
    }
}

//函数：解决冲突
bool UpdateDbus::configureDpkgByShell(bool queit)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("configure_dpkg_by_shell",queit);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed configureDpkgByShell");
    }
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
    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed copyFileToInstall");
    }
}

bool UpdateDbus::makeDirs(QString path)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("makedirs", path);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed instalOneApp");
    }
}

// 安装单个应用
bool UpdateDbus::instalOneApp(QString appName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("insone", appName);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed instalOneApp");
    }
}

//每日更新关闭
bool UpdateDbus::dailyStartClose()
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("dailystart_close");

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed dailyStartClose");
    }
}

//每日更新开启
bool UpdateDbus::dailyStartOpen()
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("dailystart_open");

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed dailyStartOpen");
    }
}

//下载deb包
void UpdateDbus::downDepPackage(QString appName)
{
    replyStr = interface->call("down_dep_pkg",appName);
    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed downDepPackage");
    }
}

//退出dbus
void UpdateDbus::Exit()
{
    replyStr = interface->call("exit");
    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed Exit");
    }
}

//通过shell解决冲突
bool UpdateDbus::fixConffileByShell(bool quiet)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("fix_conffile_by_shell",quiet);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed fixConffileByShell");
    }
}

QStringList UpdateDbus::getChangeLog(QString appName)
{

    replyStrList = interface->call("get_changlog",appName);
//    qDebug() << "程序执行" ;
    // replyStrreplyStr.value()作为返回值
    if (replyStrList.isValid()) {
//        qDebug() << replyStrList.value();
        return replyStrList.value();
    }
    else{
        qDebug() << QString("Call failed get_changlog");
    }
}


//下载pkg列表
bool UpdateDbus::Install(QStringList pkgNames)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("install",pkgNames);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed Install");
    }
}

//安装和升级
bool UpdateDbus::installAndUpgrade(QString pkgName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("install_and_upgrade",pkgName);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed installAndUpgrade");
    }
}

//更新软件源
bool UpdateDbus::Update(bool quiet)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("update",quiet);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed Update");
    }
}


//升级
bool UpdateDbus::Upgrade(QStringList pkgNames)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("upgrade",pkgNames);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed Upgrade");
    }
}

//单个升级
bool UpdateDbus::upgradeOne(QString pkgName)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("upone",pkgName);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed upgradeOne");
    }
}

void UpdateDbus::modifyConf(QString path, QString group, QString key, QString value)
{
    replyStr = interface->call("modify_conf",path,group,key,value);
    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed modify_conf");
    }
}

bool UpdateDbus::cleanUpdateList()
{
    replyBool = interface->call("clear_install_list");
    // replyStrreplyStr.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed clear_install_list");
    }
}



//初始化cache
void UpdateDbus::init_cache()
{

    replyStr = interface->call("init_cache");

    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
        qDebug() << replyStr.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed init_cache");
    }
}


QString UpdateDbus::selectCNFromDatebase(QString appName)
{
//    QSqlQuery query1(db1);
//    QString displayName = "";
////    query1 = QSqlQuery::QSqlQuery(db1);
//    query1.exec(QString("select display_name_cn from application where app_name = %1;").arg(appName));
//    while(query1.next())
//    {
//        qDebug()<< "cn name" << query1.value(4).toString();
//        displayName = query1.value(4).toString();
//    }

//    return displayName;

}

QString UpdateDbus::selectIconFromDatebase(QString appName)
{
//    QSqlQuery query1(db1);
//    query1.exec(QString("select description from application where app_name = %1").arg(appName));
//    QString description = "";
//    while(query1.next())
//    {
//        qDebug()<< "des " << query1.value(4).toString();
//        description = query1.value(4).toString();
//    }
//    return description;

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
    qDebug() << "getAppMessageSignal";
    QVariant dateQVariant;
    AppAllMsg appAllMsg;
    QVariantMap::Iterator it;
//    qDebug() << "收到信号" << map.value(1).toString();
    for (it = map.begin(); it != map.end(); ++it) {
        if (it.key() == "appname")
        {
            dateQVariant = it.value();
            appAllMsg.name = dateQVariant.toString();
        }
        if(it.key() == "current_version")
        {
            dateQVariant = it.value();
            appAllMsg.version = dateQVariant.toString().section('=',1,1);
            qDebug() << appAllMsg.version;
        }
        if(it.key() == "source_version")
        {
            dateQVariant = it.value();
            appAllMsg.availableVersion = dateQVariant.toString().section('=',1,1);
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
    qDebug() <<  "urllist:" << allSize;
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
            qDebug() << "url:" << msg.url << "name:" << msg.name << "fullname:" << msg.fullname << "size:" << msg.size;
        }
    }
    appAllMsg.msg.allSize = allSize.toLong();
    appAllMsg.msg.getDepends = dependState;
    qDebug() << "allsize:" << appAllMsg.msg.allSize << "state:" <<  appAllMsg.msg.getDepends;

//    qDebug() << "获取信息" << appAllMsg.name << appAllMsg.longDescription;
    emit sendAppMessageSignal(appAllMsg);
}

QStringList UpdateDbus::getDependsPkgs(QString appName)
{
    replyStrList = interface->call("get_depends_pkgs",appName);
    if (replyStrList.isValid()) {
        return replyStrList.value();
    }
    else{
        qDebug() << QString("Call failed getDependsPkgs");
    }
}

void UpdateDbus::insertInstallStates(QString item,QString info)
{
    interface->asyncCall("insert_install_state",item,info);
    qDebug() << QString("insert_install_state") << item << info;
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
    qDebug () << "get_app_message";
    interface->asyncCall("get_app_message",list);
    qDebug() << QString("Call get_app_message");
}


void UpdateDbus::getAptSignal(QString arg, QMap<QString, QVariant> map)
{

    QString aptStatus;
    QString aptAppName;
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
//        qDebug() << "key:"<< it.key().toLatin1().data();
//        qDebug() <<"value:" <<it.value().toString().toLatin1().data();
    }


    qDebug() << "aptAppName:" << aptAppName;
    qDebug() << "aptPercent:" << aptPercent;

    emit transferAptProgress(aptStatus,aptAppName,aptPercent);

}

void UpdateDbus::slotCopyFinished(QString appName)
{
    emit copyFinish(appName);
}


UpdateDbus::~UpdateDbus()
{
    qDebug() << "update quit";
    cleanUpdateList();
}
