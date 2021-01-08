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
#include "utils.h"
#include <QMetaType>
#include <fcntl.h>


//Q_DECLARE_METATYPE(AppAllMsg) //注册AppMsg结构用于信号槽传输

class UpdateDbus : public QObject
{
    Q_OBJECT
public:
    static UpdateDbus *getInstance();
    ~UpdateDbus();
    QDBusInterface *interface;  //dbus接口
    QDBusReply<QString> replyStr;   //string类型的返回值
    QDBusReply<bool> replyBool;     //bool类型的返回值
    QDBusReply<QStringList> replyStrList;
    QDBusReply<int> replyInt; //int类型的返回值
    QDBusInterface *interface1;


    //取消更新包
    bool cancel(QString pkgName);

    // 取消更新应用
    void cancelDownloadApp(QString appName);


    //函数：解决冲突
    bool configureDpkgByShell(bool queit);

    //拷贝软件包到安装目录
    void copyFinsh(QStringList srcPath,QString appName);

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

    void getAppMessage(QStringList list);

    //下载pkg列表
    bool Install(QStringList pkgNames);

    //安装和升级
    bool installAndUpgrade(QString pkgName);

    //更新软件源
    bool Update(bool quiet);

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
    QSqlDatabase db1,db2;  //数据库相关操作
    QString selectCNFromDatebase(QString appName);
    QString selectIconFromDatebase(QString appName);

    QStringList sourcesList;


    void onRequestSendDesktopNotify(QString message);

    QStringList inameList;  //重要更新列表
    QStringList cnameList; //紧急更新列表
    QStringList RUNLIST;   //运行列表

    QStringList ordinaryList;
    QStringList importantList;
    QStringList crucialList;


    bool fileLock();
    void fileUnLock();
    //调用接口插入数据库
    void insertInstallStates(QString item, QString info);
signals:
    void copyFinish(QString appName);
    void transferAptProgress(QString status,QString appName,float aptPercent);
    void sendImportant();
    void updatelist(QStringList arg);
    void sendAppMessageSignal(AppAllMsg msg);
//    void emitInameList(QStringList list);
    void sendFinishGetMsgSignal(int size);

public slots:
    void getAptSignal(QString arg, QMap<QString, QVariant> map);
    void slotCopyFinished(QString appName);
    void getInameAndCnameList(QString arg);    //获取重要更新列表和紧急更新列表
//    void initD_bus(bool status);
    void getAppMessageSignal(QMap<QString, QVariant> map, QStringList urlList, QStringList nameList,QStringList fullnameList,QStringList sizeList, QString allSize, bool dependState);
    void initTrayD_bus(QStringList arg); //接收线程发过来的信号
    void slotFinishGetMessage(QString num);

private:
    explicit UpdateDbus(QObject *parent = nullptr);
    static UpdateDbus *updateMutual;  //UpdateDbus类静态对象
    QString lockPath = "/home/liujialin/YYF-auto-network-lock123";
};


#endif // UPDATEDBUS_H
