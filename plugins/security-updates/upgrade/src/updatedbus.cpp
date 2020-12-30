#include "updatedbus.h"
#include "connection.h"
#include <QMutexLocker>

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

    m_backend = new QApt::Backend(this);
    m_backend->init();

    interface = new QDBusInterface(KYLIN_UPDATE_MANAGER_SERVICE,
                                   KYLIN_UPDATE_MANAGER_PATH,
                                   KYLIN_UPDATE_MANAGER_INTERFACE,
                                   QDBusConnection::systemBus());

    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("kum_apt_signal"), this, SLOT(getAptSignal(QString, QMap<QString, QVariant>)));


    QDBusConnection::systemBus().connect(QString("cn.kylinos.KylinUpdateManager"), QString("/cn/kylinos/KylinUpdateManager"),
                                         QString("cn.kylinos.KylinUpdateManager"),
                                         QString("copy_finish"), this, SLOT(slotCopyFinished(QString)));
//    connect(interface, SIGNAL(kum_apt_signal(QString, QVariantMap)), this, SLOT(getAptSignal(QString, QVariantMap)));

    init_cache();
    cleanUpdateList();

//    m_traybusthread = new traybusthread();
//    QObject::connect(m_traybusthread,SIGNAL(result(QStringList)),this,SLOT(initTrayD_bus(QStringList)));
}

void UpdateDbus::initTrayD_bus(QStringList arg)
{

//    m_traybusthread->start();
    emit updatelist(arg);
    //    m_traybusthread->quit();
}

void UpdateDbus::startTray()
{
//    m_traybusthread->start();

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

bool UpdateDbus::changeSourceListToDefault(QString serviceKey, QString pwd, QString currentUser, QString osCodename, QString availableSource)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("change_source_list_to_default", serviceKey,pwd,currentUser,osCodename,availableSource);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed changeSourceListToDefault");
    }
}

bool UpdateDbus::changeSourceListToKylinUpdateServer(QStringList lines)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("change_source_list_to_kylin_update_server",lines);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed changeSourceListToKylinUpdateServer");
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
void UpdateDbus::copyFileToInstall(QStringList srcPath, QString appName)
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

void UpdateDbus::ConnectSlots()
{

    replyStr = interface1->call("connectSuccessslots");
//    qDebug() << "程序执行" ;
    // replyStrreplyStr.value()作为返回值
    if (replyStr.isValid()) {
//        qDebug() << replyStrList.value();
        return ;
    }
    else{
        qDebug() << QString("Call failed connectSuccessslots");
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

//通过shell更新软件源
bool UpdateDbus::updateByShell(bool quiet)
{
    // 有参数的情况下  传参调用dbus接口并保存返回值
    replyBool = interface->call("update_by_shell",quiet);

    // 将reply.value()作为返回值
    if (replyBool.isValid()) {
        qDebug() << replyBool.value();
        return replyBool.value();
    }
    else{
        qDebug() << QString("Call failed updateByShell");
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

bool UpdateDbus::checkIsInstalled(QString appName)
{

}

bool UpdateDbus::checkLoongson3A4000()
{
    QProcess os(0);
    QStringList args;
    args.append("grep");
    args.append("'Loongson-3A4000'");

    os.start("lscpu", args);

    os.waitForFinished(); //等待完成
//
    QString result = QString::fromLocal8Bit(os.readAllStandardOutput());
    qDebug()<<result;
    if(result.indexOf("Loongson-3A4000") == -1)
    {
//        qDebug()<<QString::fromLocal8Bit(os.readAllStandardError());

        return false;
    }

    return true;
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


// 获取sinfo中source.list 将etc/apt/source.list设置为sinfo中[source.list]内容
void UpdateDbus::getSourceListFromSinfo()
{
//    QFile file("L:/qtpro/_qtApp/text/t.txt");
//    file.open(QIODevice::ReadOnly | QIODevice::Text);
//    QByteArray t = file.readAll();
//    ui->text_r->setText(QString(t));
//    file.close();
    QFile file("/home/liujialin/kylin/sinfo.206.juniper");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray t ;
    bool start = false;
    while (!file.atEnd())
    {
        t = file.readLine();
        QString lineStr = QString(t);

        if (start == true)
        {
            // 没有源
            if (lineStr.indexOf('[') != -1 || lineStr.simplified() == "" ||
                lineStr == NULL)
            {
                break;
            }
            else
            {
                QString source = "deb http://archive.kylinos.cn" + lineStr.simplified();
                sourcesList.append(source);
            }
        }
        if (lineStr.indexOf("[sources.list]") != -1)
        {

            start = true;
        }
    }

    file.close();
//    qDebug() << "sourcesList" << sourcesList;

    return;


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

void UpdateDbus::getDesktopOrServer()
{
    QSettings settings("/etc/.kyinfo", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("dist");
    QString data = settings.value("dist_id").toString().simplified();
    settings.endGroup();

    if (data.indexOf("server") != -1)
        desktopOrServer = "server";
    else
        desktopOrServer = "desktop";
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
