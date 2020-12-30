#ifndef APPUPDATE_H
#define APPUPDATE_H

#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTextEdit>

#include "utils.h"
#include "updatelog.h"
#include <QMetaType>
#include <QTimer>
#include "updatedbus.h"

class UpdateLog;
//class UpdateDbus;
Q_DECLARE_METATYPE(AppMsg) //注册AppMsg结构用于信号槽传输
Q_DECLARE_METATYPE(AppAllMsg) //注册AppMsg结构用于信号槽传输
//类：功能类，调用dbus接口获取到下载链接、大小等信息
class WorkClass : public QObject
{
    Q_OBJECT
public:
    explicit WorkClass(QObject *parent = nullptr);

private:
    AppMsg appMsg;
    UpdateDbus *m_updateMutual;
    bool sqliteIsConnect = false;

public slots:
    void getDependPackages(QString appname); //获取下载依赖、包名、大小
    void writeSqlite(AppAllMsg msg ,QString changelog);

signals:
    void appMessageSignal(AppMsg msg);
};

class AppUpdateWid : public QWidget
{
    Q_OBJECT
public:
    explicit AppUpdateWid(QString appname, QWidget *parent = nullptr);

    QFrame *AppFrame;
    UpdateLog *updatelog1;
    //缩略界面
    QLabel *appIcon;
    QLabel *appIconName;
    QLabel *appNameLab;
    QLabel *appVersion;
    QLabel *progressLab;  //进度
    QLabel *otherBtnLab;
    QPushButton *detaileInfo;
    QPushButton *updateAPPBtn;
    QWidget *appTitleWid;
    QHBoxLayout *iconNameLayout;
    QHBoxLayout *smallHLayout;
    QVBoxLayout *largeVLayout;
    QHBoxLayout *otherBtnLayout;
    QWidget *largeWidget;
    QVBoxLayout *mainVLayout;
    //展开界面
    QTextEdit *someInfoEdit;
    QPushButton *updatelogBtn;

//    AppMsg *thisAppMessage;
    QString chlog;

private:
    WorkClass *worker;
    QThread *workThread;
    bool isCancel = true;
    bool firstDownload = true;
    long downSize = 0;
    long preDownSize = 0;
    long priorSize = 0;
    int connectTimes = 0;
    bool downloadFinish = false;
    QString currentPackage;
    QStringList downloadList;
    QStringList downloadPackages;
    AppAllMsg appAllMsg;
    QProcess *downloadProcess;
    QProcess *workProcess;
    QTimer *timer;
    QString downloadPath;
    UpdateDbus *m_updateMutual;

    QApt::Backend *m_backend;
    QApt::Package *m_package;
    QApt::Group *m_group;
    QApt::Transaction *m_trans;

public slots:
    void showDetails();
    void showUpdateLog();
    void cancelOrUpdate();

    void showInstallStatues(QString status,QString appAptName,float progress);
    void showDownloadStatues(QString downloadSpeed, int progress);

    void aptFinishQuery();

    void slotDownloadPackages();
    void calculateSpeedProgress(); //计算下载速度和进度
    void startInstall(QString appName);
    void updateAllApp();

private:
    void curlDownload(UrlMsg msg, QString path); //断点续传下载
    bool getDownloadSpeed(QString appName, QString fullName, int fileSize); //获取下载速度
    void initConnect(); //初始化信号槽
    void getAppMessage(QString appName); //获取app的所有依赖包的信息
    QString modifySizeUnit(int size);
    QString modifySpeedUnit(int size, float time);

signals:
    void startWork(QString appName);
    void writeSqliteSignal(AppAllMsg msg ,QString changelog);
    void startMove(QStringList list, QString appName);
    void cancel();

    void hideUpdateBtnSignal();
    void changeUpdateAllSignal();

//    void aptFinish();


private:
    void updateAppUi(QString name);
};

#endif // APPUPDATE_H
