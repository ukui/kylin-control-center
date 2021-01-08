#ifndef UPDATEDBUS_H
#define UPDATEDBUS_H

#define KYLIN_UPDATE_MANAGER_PATH "/cn/kylinos/KylinUpdateManager"

#define KYLIN_UPDATE_MANAGER_SERVICE "cn.kylinos.KylinUpdateManager"

#define KYLIN_UPDATE_MANAGER_INTERFACE "cn.kylinos.KylinUpdateManager"

//#define SERVICE_NAME_SIZE 30

#include <QObject>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QtDBus>
#include <unistd.h>
#include <sys/types.h>
#include <QFile>
#include <QSettings>
#include <QApt/Package>
#include <QApt/SourcesList>
#include <QApt/Backend>
#include <QApt/Changelog>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include "traybusthread.h"

//#include <Python.h>
//#include "appupdate.h"
#include "metatypes.h"

class UpdateDbus : public QObject
{
    Q_OBJECT
public:
    static UpdateDbus *getInstance();
    ~UpdateDbus();
    QDBusInterface *interface;  //dbus接口

    QDBusReply<QString> replyStr;   //string类型的返回值
    QDBusReply<bool> replyBool;     //bool类型的返回值
//    QDBusReply<QStringList> replyList;   //stringlist类型的返回值
    QDBusReply<QStringList> replyStrList;
    QDBusReply<int> replyInt; //int类型的返回值

//    traybusthread *m_traybusthread;
    QDBusInterface *interface1;
    void ConnectSlots();  //托盘信号


    //取消更新包
    bool cancel(QString pkgName);

    // 取消更新应用
    void cancelDownloadApp(QString appName);

    //改变source.list为默认源
    bool changeSourceListToDefault(QString serviceKey,QString pwd, QString currentUser,QString osCodename,QString availableSource);

    //改变source.list为kylin-update-server
    bool changeSourceListToKylinUpdateServer(QStringList lines);

    //函数：解决冲突
    bool configureDpkgByShell(bool queit);

    //拷贝软件包到安装目录
    void copyFileToInstall(QStringList srcPath,QString appName);

    //创建root目录
    bool makeDirs(QString path);

    // 安装单个应用
    bool instalOneApp(QString appName);

    //每日更新关闭
    bool dailyStartClose();

    //每日更新开启
    bool dailyStartOpen();

    //下载deb包
    void downDepPackage(QString appName);

    //退出dbus
    void Exit();

    //通过shell解决冲突
    bool fixConffileByShell(bool quiet);

    //获得依赖包changelog
    QStringList getChangeLog(QString appName);

    //获得依赖包
    QStringList getDependsPkgs(QString appName);

    //检查列表中的包是否可升级
    QStringList checkInstallOrUpgrade(QStringList list);


    //下载pkg列表
    bool Install(QStringList pkgNames);

    //安装和升级
    bool installAndUpgrade(QString pkgName);

    //更新软件源
    bool Update(bool quiet);

    //通过shell更新软件源
    bool updateByShell(bool quiet);

    //升级
    bool Upgrade(QStringList pkgNames);

    //单个升级
    bool upgradeOne(QString pkgName);

    //修改配置文件
    void modifyConf(QString path,QString group,QString key,QString value);

    //kill pid
    bool cleanUpdateList();

    //初始化cache
    void init_cache();
//    bool makeDirs(QString path);


//dbus接口函数定义完毕
    //
    //某个包是否已经安装
    bool checkIsInstalled(QString appName);
    //判断当前系统是否为龙芯4000，以及安装对应低版本的龙芯内核虚包
    bool checkLoongson3A4000();
    QSqlDatabase db1,db2;  //数据库相关操作
    QString selectCNFromDatebase(QString appName);
    QString selectIconFromDatebase(QString appName);
    //获取sinfo中source.list 将etc/apt/source.list设置为sinfo中[source.list]内容
    void getSourceListFromSinfo();
    QStringList sourcesList;

    //函数：获取下载的important和crucial列表

    void getDesktopOrServer();     //获取当前系统是服务器还是桌面
    QString desktopOrServer;    //当前系统是服务器还是桌面操作系统

    QStringList inameList;  //重要更新列表
    QStringList cnameList; //紧急更新列表
    QStringList RUNLIST;   //运行列表

    QStringList ordinaryList;
    QStringList importantList;
    QStringList crucialList;

    QApt::Backend *m_backend;
    QApt::Package *m_package;

signals:
    void copyFinish(QString appName);
    void transferAptProgress(QString status,QString appName,float aptPercent);

    void sendImportant();

    void updatelist(QStringList arg);

//    void emitInameList(QStringList list);

public slots:
    void getAptSignal(QString arg, QMap<QString, QVariant> map);
    void slotCopyFinished(QString appName);
    void getInameAndCnameList(QString arg);    //获取重要更新列表和紧急更新列表
//    void initD_bus(bool status);

    void initTrayD_bus(QStringList arg); //接收线程发过来的信号

    void startTray();


private:
    explicit UpdateDbus(QObject *parent = nullptr);
    static UpdateDbus *updateMutual;  //UpdateDbus类静态对象
};


#endif // UPDATEDBUS_H
