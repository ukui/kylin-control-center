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

#include "appupdate.h"
#include "switchbutton.h"
#include "logwindow.h"
#include "m_updatelog.h"
#include "updatesource.h"

#define CRUCIAL_FILE_PATH "/var/lib/kylin-software-properties/template/crucial.list"
#define IMPORTANT_FIEL_PATH "/var/lib/kylin-software-properties/template/important.list"

class TabWid : public QWidget
{
    Q_OBJECT
public:
    explicit TabWid(QWidget *parent = nullptr);
    ~TabWid();
    void createUI();  //自动更新关闭
    void tabUI2();  //自动更新开启
    void allComponents(); //更新页面里所有组件

    void getAllDisplayInformation();
//    void allBinding();  //更新页面里所有组件绑定

//    void updateSettings(); //更新设置页面

    //选项卡页面，具有更新和更新设置两个选项卡及对应界面
    QTabWidget *updateWidget;
    QWidget *updateTab;
    QWidget *updateSettingTab;
    QVBoxLayout *mainTabLayout;

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

    //更新按钮
    //自动更新关闭时 点击检查更新  检查更新之后变为现在更新或系统已经是最新的状态
    QPushButton *checkUpdateBtn;  //检测更新
//    QPushButton *nowUpdateBtn;    //现在更新
    QPushButton *updateAllBtn;   //全部更新

    //三种状态下的版本信息   显示当前版本、可更新版本、或最新版本
    QLabel *versionInformationLab;
    QLabel *lastRefreshTime;
    QPushButton *historyUpdateLog;  //历史更新日志界面
    QVBoxLayout *inforLayout;

    // 更新设置页面组件
    SwitchButton *isAutoCheckSBtn;    //是否自动检查更新
    QFrame *isAutoCheckWidget;
    QLabel *isAutoCheckedLab;

    QWidget *allUpdateWid;
    QVBoxLayout *allUpdateLayout;


    QPushButton *updateSourceSetting;
    int inumber = 0;
//    LogWindow *logwindow;
    m_updatelog *historyLog;
    bool historyLogIsCreate = false;
    UpdateDbus *updateMutual;
    void getAppUpdateMsg(QStringList arg);   //加载所有应用的具体更新信息

//    QStringList importantList;

    //源管理器Dbus对象
    UpdateSource *updateSource;


signals:
//    void send_Signal();
//    void parameterSignal(int i);
    void updateAllSignal();
public slots:
    void showHistoryWidget();
    void checkUpdateBtnClicked();  //检测更新、全部更新按钮
    void isAutoCheckedChanged();
    void slotCancelDownload();
    void slotGetImportant(QStringList args);
//    void recieveUpgradList(QStringList args);

    void hideUpdateBtnSlot();
    void changeUpdateAllSlot();

    //调用源管理器相关
    void slotUpdateTemplate(QString status);
    void slotUpdateCache(QString status);
    void slotUpdateCacheProgress(QString progress);

private:
};

#endif // TABWIDGET_H
