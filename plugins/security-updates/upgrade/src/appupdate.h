#ifndef APPUPDATE_H
#define APPUPDATE_H

#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTextEdit>
#include <QToolTip>
#include "utils.h"
#include "updatelog.h"
#include <QMetaType>
#include <QTimer>
#include "updatedbus.h"
#include <QCheckBox>
#include "mylabel.h"
class UpdateLog;
//class UpdateDbus;
class AppUpdateWid : public QWidget
{
    Q_OBJECT
public:
    explicit AppUpdateWid(AppAllMsg msg, QWidget *parent = nullptr);
    ~AppUpdateWid();

    QFrame *AppFrame;
    UpdateLog *updatelog1;
    //缩略界面
    QLabel *appIcon;
    QLabel *appIconName;
    MyLabel *appNameLab;
    QLabel *appVersion;
    QLabel *appVersionIcon;

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
    QTextEdit *someInfoEdit;  //详情内容
    QPushButton *updatelogBtn;
//    AppMsg *thisAppMessage;
    QString chlog;  //更新日志完整内容
    bool eventFilter(QObject *watched, QEvent *event); //过滤tooltip事件
    QString setDefaultDescription(QString str); //将虚包包名汉化
    void updateOneApp();  //控制更新单个app
    QString dispalyName;
private:
    bool isCancel = true;
    bool firstDownload = true;
    long downSize = 0;
    long preDownSize = 0;
    long priorSize = 0;
    int connectTimes = 0;
    bool downloadFinish = false;
    bool stopTimer = false;
    UrlMsg urlmsg;
    QString path;
    QString currentPackage;
    QStringList downloadList;
    QStringList downloadPackages;
    AppAllMsg appAllMsg;
    QProcess *downloadProcess;
    QProcess *workProcess;
    QTimer *timer;
    QString downloadPath;
    UpdateDbus *m_updateMutual;

    bool execFun;

public slots:
    void showDetails();
    void showUpdateLog();
    void cancelOrUpdate();

    void showInstallStatues(QString status, QString appAptName, float progress, QString errormsg);
    void showDownloadStatues(QString downloadSpeed, int progress);

    void slotDownloadPackages();
    void calculateSpeedProgress(); //计算下载速度和进度
    void startInstall(QString appName);
    void updateAllApp();

    void showUpdateBtn();
    void hideOrShowUpdateBtnSlot(int result);  //显示或隐藏更新按钮  备份过程中
private:
    void wgetDownload(UrlMsg msg, QString path); //断点续传下载
    bool getDownloadSpeed(QString appName, QString fullName, int fileSize); //获取下载速度
    void initConnect(); //初始化信号槽
    void changeDownloadState(int state);
    type checkSourcesType();
    QString modifySizeUnit(long size);
    QString modifySpeedUnit(long size, float time);
    QStringList analysis_config_file(char *p_file_path);
    void remove_last_enter(char *p_src_in_out);

    enum Environment{
        en,
        zh_cn
    }environment;

signals:
    void startWork(QString appName);
    void startMove(QStringList list, QString appName);
    void hideUpdateBtnSignal(bool isSucceed);
    void changeUpdateAllSignal();
    void downloadFailedSignal(int exitCode);  //网络异常或者其他情况下下载失败时
    void filelockedSignal();
    void cancel();


//    void aptFinish();
private:
    void updateAppUi(QString name);
    QString translationVirtualPackage(QString str);
    QString pkgIconPath = "";
};

#endif // APPUPDATE_H
