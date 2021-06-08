#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QFont>

#include "appupdate.h"
//#include "switchbutton.h"
#include "m_updatelog.h"
#include "updatesource.h"
#include "ukscconn.h"
#include "backup.h"
#include "checkbutton.h"
#include "SwitchButton/switchbutton.h"
#define CRUCIAL_FILE_PATH "/var/lib/kylin-software-properties/template/crucial.list"
#define IMPORTANT_FIEL_PATH "/var/lib/kylin-software-properties/template/important.list"

const int needBack = 99;

const int netErrorRetry = 3;

class TabWid : public QWidget
{
    Q_OBJECT
public:
    explicit TabWid(QWidget *parent = nullptr);
    ~TabWid();
    void allComponents(); //更新页面里所有组件

    void getAllDisplayInformation();
//    void allBinding();  //更新页面里所有组件绑定
    void checkUpdateBtnClicked();  //检测更新、全部更新按钮


//    void updateSettings(); //更新设置页面

    //选项卡页面，具有更新和更新设置两个选项卡及对应界面
    QTabWidget *updateWidget;
    QLabel *labUpdate;
    QWidget *updateTab;
    QWidget *updateSettingTab;
    QVBoxLayout *mainTabLayout;
    QFont ft;
    QCheckBox *isAutoCheckBox;

    //更新页面布局
    QHBoxLayout *tab1HLayout;
    QFrame *systemWidget;
    QVBoxLayout *AppMessage;
    QScrollArea *scrollArea;
    //更新设置页面布局
//    QHBoxLayout *tab2HLayout;
    QVBoxLayout *tab2VLayout;

    //系统头像
    QLabel *systemPortraitLab;
    m_button *checkUpdateBtn;  //检测更新
    //三种状态下的版本信息   显示当前版本、可更新版本、或最新版本
    QLabel *versionInformationLab;
    QLabel *lastRefreshTime;
    QPushButton *historyUpdateLog;  //历史更新日志界面
    QVBoxLayout *inforLayout;

     //更新设置页面组件
    QWidget *updateSettingWidget;
    QLabel *updateSettingLab;
    QVBoxLayout *updatesettingLayout;
    QFrame *isAutoCheckWidget;
    QHBoxLayout *isAutoCheckLayout;
    QLabel *isAutoCheckedLab;
    SwitchButton *isAutoCheckSBtn;
    QFrame *isAutoBackupWidget;
    QHBoxLayout *isAutoBackupLayout;
    QLabel *isAutoBackupLab;
    SwitchButton *isAutoBackupSBtn;


    QWidget *allUpdateWid;
    QVBoxLayout *allUpdateLayout;

    int inumber = 0;
    int retryTimes = 0;
    m_updatelog *historyLog;
//    QDialog *historyLog;
    bool fileLockedStatus = false;  //等待静默安装提示是否弹出
    UpdateDbus *updateMutual;
    bool downloadFailedStatus = false;  //下载失败时的弹窗是否弹出
    //源管理器Dbus对象
    UpdateSource *updateSource;


    void disconnectSource();
signals:
//    void send_Signal();
//    void parameterSignal(int i);
    void updateAllSignal();
public slots:
    void showHistoryWidget();
    void isAutoCheckedChanged();
    void slotCancelDownload();
    void loadingOneUpdateMsgSlot(AppAllMsg msg); //逐个加载更新
    void loadingFinishedSlot(int size); //加载完毕信号
    void waitCrucialInstalled();  //等待静默更新安装完的消息提示

    void hideUpdateBtnSlot(bool isSucceed);
    void changeUpdateAllSlot();

    //调用源管理器相关
    void slotUpdateTemplate(QString status);
    void slotUpdateCache(QVariantList sta);
    void slotUpdateCacheProgress(QVariantList pro);

    //解决首次运行卡顿问题
    //DBus单独初始化
    void initDbus();


private:
    UKSCConn *ukscConnect;
    bool isConnectSourceSignal = false;
    void unableToConnectSource();
//备份还原相关
    void bacupInit();
    void backupDelete();
    void backupCore();
    BackUp *backup = nullptr;
    QThread *backupThread = nullptr;

    void backupMessageBox(QString str);
    void backupHideUpdateBtn(int result);
signals:
    int needBackUp();
    void startBackUp(int);

public slots:
    void bakeupFinish(int);
    void backupProgress(int);
    void isAutoBackupChanged();

    void getReplyFalseSlot();
    void dbusFinished();
private slots:
    void receiveBackupStartResult(int result);
    void whenStateIsDuing();
};

#endif // TABWIDGET_H
