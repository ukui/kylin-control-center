#include "tabwidget.h"
#include <kybackup/backuptools-define.h>

TabWid::TabWid(QWidget *parent):QWidget(parent)
{
    allComponents();
}

void TabWid::initDbus()
{
    updateMutual = UpdateDbus::getInstance();

    updateSource = new UpdateSource();
    QThread *tmp_thread = new QThread;
    connect(tmp_thread,&QThread::started,updateSource,&UpdateSource::startDbus);
    connect(updateSource,&UpdateSource::startDbusFinished,this,&TabWid::dbusFinished);
//    connect(updateSource,&UpdateSource::startDbusFinished,tmp_thread,&QThread::deleteLater);
    updateSource->moveToThread(tmp_thread);
    tmp_thread->start();

    ukscConnect = new UKSCConn();
//    this->resize(620,580);

    connect(updateMutual,&UpdateDbus::sendAppMessageSignal,this,&TabWid::loadingOneUpdateMsgSlot);
    connect(updateMutual,&UpdateDbus::sendFinishGetMsgSignal,this,&TabWid::loadingFinishedSlot);
    connect(checkUpdateBtn,&QPushButton::clicked,this,&TabWid::checkUpdateBtnClicked);
    connect(historyUpdateLog,&QPushButton::clicked,this,&TabWid::showHistoryWidget);
    connect(isAutoCheckSBtn,&SwitchButton::checkedChanged,this,&TabWid::isAutoCheckedChanged);
    connect(isAutoBackupSBtn,&SwitchButton::checkedChanged,this,&TabWid::isAutoBackupChanged);
    connect(updateSource,&UpdateSource::getReplyFalseSignal,this,&TabWid::getReplyFalseSlot);
    bacupInit();//初始化备份

    checkUpdateBtn->stop();
//    checkUpdateBtn->setText(tr("检查更新"));
    checkUpdateBtn->setText(tr("Check Update"));
//    checkUpdateBtn->setText(tr("正在初始化"));
    checkUpdateBtn->setText(tr("initializing"));
    checkUpdateBtn->setEnabled(false);
}

void TabWid::dbusFinished()
{
    checkUpdateBtn->setEnabled(true);
    checkUpdateBtn->setText(tr("Check Update"));
    checkUpdateBtnClicked();
}

void TabWid::unableToConnectSource()
{
    qDebug() << "源管理器信号是否连接成功：" << isConnectSourceSignal;
    if(isConnectSourceSignal == false)
    {
        disconnectSource();
    }
}
void TabWid::disconnectSource()
{
    disconnect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
    disconnect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QVariantList)),this,SLOT(slotUpdateCache(QVariantList)));
    disconnect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QVariantList)),this,SLOT(slotUpdateCacheProgress(QVariantList)));
    checkUpdateBtn->setEnabled(true);
    checkUpdateBtn->stop();
//        checkUpdateBtn->setText(tr("检查更新"));
    checkUpdateBtn->setText(tr("Check Update"));
//        versionInformationLab->setText(tr("服务连接异常，请重新检测!") );
    versionInformationLab->setText(tr("Service connection abnormal,please retest!") );
}
TabWid::~TabWid()
{
    qDebug() << "~TabWid" ;
    delete updateMutual;
    updateMutual = nullptr;
    backupDelete();//回收资源
//    updateMutual->cleanUpdateList();
}

void TabWid::backupMessageBox(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.setWindowTitle(tr("Prompt information"));
//    msgBox.setWindowTitle("提示信息");
    msgBox.setStandardButtons(QMessageBox::Save
                              | QMessageBox::Discard|QMessageBox::Abort);
//    msgBox.setButtonText(QMessageBox::Save,"立即更新");
//    msgBox.setButtonText(QMessageBox::Discard,"取消更新");
//    msgBox.setButtonText(QMessageBox::Abort,"否，我不备份");
    msgBox.setButtonText(QMessageBox::Save,tr("Update now"));
    msgBox.setButtonText(QMessageBox::Discard,tr("Cancel update"));
    msgBox.setButtonText(QMessageBox::Abort,tr("No,I Don't Backup"));
    msgBox.button(QMessageBox::Abort)->hide();
    int ret = msgBox.exec();
    if(ret == QMessageBox::Save)
    {
        qDebug() << "立即更新!";
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
        versionInformationLab->setText(tr("Being updated..."));
//         versionInformationLab->setText("正在更新...");
        updateMutual->isPointOutNotBackup = false;   //全部更新时不再弹出单个更新未备份提示
        emit updateAllSignal();
    }
    else if(ret == QMessageBox::Discard)
    {
       qDebug() << "不进行全部更新。";
       checkUpdateBtn->stop();
       checkUpdateBtn->setEnabled(true);
//       checkUpdateBtn->setText(tr("全部更新"));
       versionInformationLab->setText(tr("Updatable app detected on your system!"));
       checkUpdateBtn->setText(tr("UpdateAll"));
    }
    else if(ret == QMessageBox::Abort)
    {
       qDebug() << "不进行全部更新。";
       checkUpdateBtn->stop();
       checkUpdateBtn->setEnabled(true);
//       checkUpdateBtn->setText(tr("全部更新"));
       checkUpdateBtn->setText(tr("UpdateAll"));

    }
}

void TabWid::backupCore()
{
    int initresult = emit needBackUp();
    switch (initresult) {
    case -1:
        backupMessageBox(tr("The backup restore partition could not be found. The system will not be backed up in this update!"));
//        backupMessageBox(tr("未能找到备份还原分区，本次更新不会备份系统!"));
        //如果是则立即更新,否的话取消全部更新
        return;
    case -2:
        versionInformationLab->setText(tr("Kylin backup restore tool is doing other operations, please update later."));
//        versionInformationLab->setText("麒麟备份还原工具正在进行其他操作，请稍后更新");
        return;
    case -3:
        versionInformationLab->setText(tr("The source manager configuration file is abnormal, the system temporarily unable to update!"));
//        versionInformationLab->setText("源管理器配置文件异常，暂时无法更新！");
        return;
    case -4:
//        versionInformationLab->setText("已备份，无需再次备份");
        versionInformationLab->setText(tr("Backup already, no need to backup again."));
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
        updateMutual->isPointOutNotBackup = false;   //全部更新时不再弹出单个更新未备份提示
        emit updateAllSignal();
        return;
    case 1://正在备份
        emit startBackUp(0);
        versionInformationLab->setText(tr("Start backup,getting progress")+"...");
        break;
    case needBack://需要备份
        emit startBackUp(1);
        break;
    case -9://备份还原工具不存在
//        backupMessageBox("麒麟备份还原工具不存在，本次更新不会备份系统!");
        backupMessageBox(tr("Kylin backup restore tool does not exist, this update will not backup the system!"));
        return;
    default:
        qDebug()<<"备份还原工具状态码"<<initresult;
        return;
    }
     qDebug()<<"符合备份工具运行条件";
}

void TabWid::backupProgress(int progress)
{
    if(progress==100)
    {
        //备份完成，开始安装
        qDebug()<<"备份完成，开始安装";
//        versionInformationLab->setText("备份完成");
        versionInformationLab->setText(tr("Backup complete."));
        return;
    }
//    versionInformationLab->setText("备份中："+QString::number(progress)+"%");
    versionInformationLab->setText(tr("In backup:")+QString::number(progress)+"%");

}

void TabWid::bakeupFinish(int result)
{
    switch (result) {
    case -20:
//        versionInformationLab->setText(tr("备份过程被中断，停止更新！"));
        versionInformationLab->setText(tr("Backup interrupted, stop updating!"));
        break;
    case 99:
//        versionInformationLab->setText(tr("备份完成！"));
        versionInformationLab->setText(tr("Backup finished!"));
        break;
    default:
//        backupMessageBox(tr("麒麟备份还原工具异常：")+QString::number(result)+","+tr("本次更新不会备份系统！"));
        backupMessageBox(tr("Kylin backup restore tool exception:")+QString::number(result)+","+tr("There will be no backup in this update!"));
        break;
    }
}

void TabWid::backupHideUpdateBtn(int result)
{
    if(result == 99)
    {
        checkUpdateBtn->start();
//        versionInformationLab->setText("正在更新...");
        versionInformationLab->setText(tr("Being updated..."));
        updateMutual->isPointOutNotBackup = false;   //全部更新时不再弹出单个更新未备份提示
        emit updateAllSignal();
    }
    else if(result == -20)
    {
        checkUpdateBtn->stop();
        checkUpdateBtn->setEnabled(true);
//        checkUpdateBtn->setText(tr("全部更新"));
        checkUpdateBtn->setText(tr("UpdateAll"));

    }
}

void TabWid::bacupInit()
{
    backup = new BackUp;
    backupThread = new QThread;
    qDebug() << "======>tabwid info: " <<backupThread ;
    backup->moveToThread(backupThread);
    connect(this,&TabWid::needBackUp,backup,&BackUp::needBacdUp,Qt::BlockingQueuedConnection);//同步信号，阻塞，取返回值
    connect(this,&TabWid::startBackUp,backup,&BackUp::startBackUp);
    connect(backup, &BackUp::calCapacity, this, &TabWid::whenStateIsDuing);
    connect(backup, &BackUp::backupStartRestult, this, &TabWid::receiveBackupStartResult);
    connect(backup,&BackUp::bakeupFinish,this,&TabWid::bakeupFinish);
    connect(backup,&BackUp::backupProgress,this,&TabWid::backupProgress);
    connect(backup,&BackUp::bakeupFinish,this,&TabWid::backupHideUpdateBtn);
    backupThread->start();
}

void TabWid::backupDelete()
{
    backup->deleteLater();
    backupThread->deleteLater();
}

void TabWid::slotUpdateTemplate(QString status)
{
    qDebug() <<  "源管理器：" <<"update template status :" << status;

}
void TabWid::slotUpdateCache(QVariantList sta)
{
    QString status = sta.at(1).toString();
    QString nowsymbol = sta.at(0).toString();
    qDebug() << "源管理器：" <<"slotUpdateCache" << "nowsymbol" <<nowsymbol << status;
    if(nowsymbol == Symbol)
    {
        isConnectSourceSignal = true;
        qDebug() <<"源管理器：" << "update cache status :" << status;
        if (!status.compare("success")) {
//            versionInformationLab->setText(tr("正在获取更新列表..."));
            versionInformationLab->setText(tr("Getting update list")+"...");
            QFile file(IMPORTANT_FIEL_PATH);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() <<"file open failed!" << IMPORTANT_FIEL_PATH;
                versionInformationLab->setText(tr("Software source update failed: "));  //软件源更新失败
                checkUpdateBtn->setEnabled(true);
                checkUpdateBtn->stop();
                checkUpdateBtn->setText(tr("Check Update"));
                return ;
            }
            QString str =  file.readAll();
            QStringList list;
            str = str.simplified();
            if (!str.isEmpty()) {
                list = str.split(" ");
            }
            qDebug() << "slotUpdateCache函数：获取到的包列表：" << list;
            updateMutual->getAppMessage(list);
            retryTimes = 0;
        }
        else
        {
            int statuscode = status.toInt();
            if(statuscode == 400 && retryTimes < netErrorRetry)
            {
                updateSource->callDBusUpdateTemplate();
                qDebug() << "源管理器：" <<"statuscode = :" << statuscode;
                QString failedInfo = updateSource->getFailInfo(statuscode);
                qDebug() << "源管理器：" <<"failedInfo:" << failedInfo;
                retryTimes++;
            }
            else
            {
                QString failedInfo = updateSource->getFailInfo(statuscode);
                checkUpdateBtn->setEnabled(true);
                checkUpdateBtn->stop();
//                checkUpdateBtn->setText(tr("检查更新"));
                checkUpdateBtn->setText(tr("Check Update"));
//                versionInformationLab->setText(tr("软件源更新失败：")+failedInfo );
                versionInformationLab->setText(tr("Software source update failed: ")+failedInfo );
                disconnect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
                disconnect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QVariantList)),this,SLOT(slotUpdateCache(QVariantList)));
                disconnect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QVariantList)),this,SLOT(slotUpdateCacheProgress(QVariantList)));
                retryTimes = 0;
            }
        }
    }
}
void TabWid::slotUpdateCacheProgress(QVariantList pro)
{
    int progress = pro.at(1).toInt();
    QString nowsymbol = pro.at(0).toString();
    //    qDebug() << "update cache progress :" << progress;
    if(nowsymbol == Symbol)
    {
        if(retryTimes == 0)
        {
            versionInformationLab->setText(tr("Update software source :") + QString::number(progress)+"%");
//            versionInformationLab->setText(tr("更新软件源进度：") + QString::number(progress)+"%");
        }
        else
        {
            versionInformationLab->setText(tr("Update software source :") + QString::number(progress)+"%(" + tr("Reconnect times:")+ QString::number(retryTimes)+")");
//            versionInformationLab->setText(tr("更新软件源进度：") + QString::number(progress)+"%" + "（第" + QString::number(retryTimes) +"次重试）");
        }
    }
}

void TabWid::allComponents()
{
    mainTabLayout = new QVBoxLayout();  //整个页面的主布局
    scrollArea = new QScrollArea(this);
    updateTab = new QWidget(this);  //更新页面

    AppMessage = new QVBoxLayout();
    AppMessage->setAlignment(Qt::AlignTop);
    updateTab->setLayout(AppMessage);
    systemWidget = new QFrame(updateTab);
    systemWidget->setFrameShape(QFrame::Box);
    systemPortraitLab = new QLabel(systemWidget);

    allUpdateWid = new QWidget(this);
    allUpdateLayout = new QVBoxLayout();
    allUpdateLayout->setAlignment(Qt::AlignTop);
    allUpdateWid->setLayout(allUpdateLayout);
    allUpdateLayout->setSpacing(2);
    allUpdateLayout->setMargin(0);

    labUpdate = new QLabel(this);
//    labUpdate->setText(tr("更新"));
    labUpdate->setText(tr("Update"));
    labUpdate->adjustSize();
    labUpdate->setStyleSheet("font-size:18px;font-weight: 500;line-height: 25px;");

    scrollArea->setWidget(updateTab);
    scrollArea->setFrameStyle(0);
//    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollArea->setWidgetResizable(true);

    systemWidget->resize(560,140);
    systemWidget->setFixedHeight(140);

    systemPortraitLab = new QLabel();
    tab1HLayout = new QHBoxLayout();
    systemWidget->setLayout(tab1HLayout);
    systemPortraitLab->setFixedSize(96,96);
    systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/normal.png").scaled(QSize(96,96)));

    QWidget *historyUpdateLogWid = new QWidget(this);
    QHBoxLayout *historyUpdateLogLayout = new QHBoxLayout();
    historyUpdateLog = new QPushButton(this); // 历史日志弹出窗口控制按钮
//    historyUpdateLog->setText(tr("查看更新历史"));
    historyUpdateLog->setText(tr("View history"));
    historyUpdateLogWid->setLayout(historyUpdateLogLayout);
    historyUpdateLogLayout->setAlignment(Qt::AlignLeft);
    historyUpdateLogLayout->addWidget(historyUpdateLog);
    historyUpdateLogLayout->setSpacing(0);
    historyUpdateLogLayout->setMargin(0);


    inforLayout = new QVBoxLayout();
    lastRefreshTime = new QLabel(systemWidget);
    versionInformationLab = new QLabel(systemWidget);
    versionInformationLab->setStyleSheet("font-size:16px;font-weight:500;line-height: 22px;");

    inforLayout->setAlignment(Qt::AlignTop);
    inforLayout->addWidget(versionInformationLab);
    inforLayout->addWidget(lastRefreshTime);
    inforLayout->setSpacing(8);
    inforLayout->setContentsMargins(8,20,0,0);
    QWidget *inforWidget = new QWidget(systemWidget);
    inforWidget->setLayout(inforLayout);

    checkUpdateBtn = new m_button(systemWidget);
    checkUpdateBtn->setDefault(true);
    checkUpdateBtn->start();
    checkUpdateBtn->setFixedSize(120,36);


    tab1HLayout->addWidget(systemPortraitLab,0,Qt::AlignLeft);
    tab1HLayout->addWidget(inforWidget,Qt::AlignLeft|Qt::AlignTop);
    tab1HLayout->addWidget(checkUpdateBtn);
    tab1HLayout->setContentsMargins(8,25,20,20);
    tab1HLayout->setSpacing(0);

    updateSettingLab = new QLabel();
//    updateSettingLab->setText(tr("更新设置"));
    updateSettingLab->setText(tr("Update Settings"));
    updateSettingLab->setStyleSheet("font-size:18px;font-weight: 500;line-height: 25px;");
    updateSettingWidget = new QWidget(this);
    updatesettingLayout = new QVBoxLayout();
    updateSettingWidget->setLayout(updatesettingLayout);

    isAutoCheckWidget = new QFrame();
    isAutoCheckWidget->setFrameShape(QFrame::Box);
    isAutoCheckLayout = new QHBoxLayout();
    isAutoCheckedLab = new QLabel();
//    isAutoCheckedLab->setText(tr("允许通知可更新的应用"));
    isAutoCheckedLab->setText(tr("Allowed to renewable notice"));
    isAutoCheckSBtn = new SwitchButton();
    isAutoCheckWidget->setLayout(isAutoCheckLayout);
    isAutoCheckLayout->addWidget(isAutoCheckedLab);
    isAutoCheckLayout->addWidget(isAutoCheckSBtn);

    isAutoBackupWidget = new QFrame();
    isAutoBackupWidget->setFrameShape(QFrame::Box);
    isAutoBackupLayout = new QHBoxLayout();
    isAutoBackupLab = new QLabel();
    isAutoBackupLab->setText(tr("Backup current system before updates all"));
//    isAutoBackupLab->setText(tr("全部更新前备份系统"));
    isAutoBackupSBtn = new SwitchButton();
    isAutoBackupLayout->addWidget(isAutoBackupLab);
    isAutoBackupLayout->addWidget(isAutoBackupSBtn);
    isAutoBackupWidget->setLayout(isAutoBackupLayout);

    updatesettingLayout->setAlignment(Qt::AlignTop);
    updatesettingLayout->addWidget(updateSettingLab);
    updatesettingLayout->addSpacing(10);
    updatesettingLayout->addWidget(isAutoCheckWidget);
    updatesettingLayout->addWidget(isAutoBackupWidget);
    updatesettingLayout->setSpacing(2);
    updatesettingLayout->setMargin(0);


    AppMessage->addWidget(labUpdate);
    AppMessage->addWidget(systemWidget);
    AppMessage->addWidget(allUpdateWid);
    AppMessage->addWidget(historyUpdateLogWid);
    AppMessage->addSpacing(30);
    AppMessage->addWidget(updateSettingWidget);
    AppMessage->addStretch();
    AppMessage->setContentsMargins(0,0,28,10);

    mainTabLayout->setAlignment(Qt::AlignTop);
    mainTabLayout->addWidget(scrollArea);
//    mainTabLayout->setSpacing(0);
    mainTabLayout->setMargin(0);
    this->setLayout(mainTabLayout);
    getAllDisplayInformation();

}



void TabWid::loadingOneUpdateMsgSlot(AppAllMsg msg)
{
//    checkUpdateBtn->setText();
    if(updateMutual->importantList.indexOf(msg.name) == -1)
    {
        updateMutual->importantList.append(msg.name);   //重要更新列表中添加appname
        AppUpdateWid *appWidget = new AppUpdateWid(msg, this);
        appWidget->updateAPPBtn->hide();
        connect(appWidget, &AppUpdateWid::cancel, this, &TabWid::slotCancelDownload);
        connect(this, &TabWid::updateAllSignal, appWidget, &AppUpdateWid::updateAllApp);
        connect(appWidget,&AppUpdateWid::hideUpdateBtnSignal,this,&TabWid::hideUpdateBtnSlot);
        connect(appWidget,&AppUpdateWid::changeUpdateAllSignal,this,&TabWid::changeUpdateAllSlot);
        connect(updateMutual,&UpdateDbus::sendFinishGetMsgSignal,appWidget,&AppUpdateWid::showUpdateBtn);
        connect(appWidget,&AppUpdateWid::filelockedSignal,this,&TabWid::waitCrucialInstalled);
        connect(backup,&BackUp::bakeupFinish,appWidget,&AppUpdateWid::hideOrShowUpdateBtnSlot);
        if(ukscConnect->isConnectUskc == true)
        {
            QStringList list = ukscConnect->getInfoByName(msg.name);
            if(list[2] != "")
            {
                appWidget->appNameLab->setText(list[2]);

            }
            if(list[1] != "" && QLocale::system().name()=="zh_CN")
            {
                appWidget->appNameLab->setText(list[1]);
            }
            if(list[0] != "")
            {
                appWidget->appIcon->setPixmap(QPixmap(list[0]).scaled(32, 32));
            }
        }
        allUpdateLayout->addWidget(appWidget);
        qDebug() << "更新管理器：" << "loadingOneUpdateMsgSlot:" << appWidget->dispalyName;

    }

}

void TabWid::loadingFinishedSlot(int size)
{
    disconnect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
    disconnect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QVariantList)),this,SLOT(slotUpdateCache(QVariantList)));
    disconnect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QVariantList)),this,SLOT(slotUpdateCacheProgress(QVariantList)));
    qDebug()<< "更新管理器：" <<"加载完毕信号 " << "size = " <<size;
    if(updateMutual->importantList.size() == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->stop();
//        checkUpdateBtn->setText(tr("检查更新"));
        checkUpdateBtn->setText(tr("Check Update"));
//        versionInformationLab->setText(tr("您的系统已是最新！"));
        versionInformationLab->setText(tr("Your system is the latest!"));
    }
    else
    {
        updateMutual->importantSize = updateMutual->importantList.size();   //此次检测结果的更新数量
        checkUpdateBtn->stop();
        checkUpdateBtn->setEnabled(true);
//        checkUpdateBtn->setText(tr("全部更新"));
        checkUpdateBtn->setText(tr("UpdateAll"));
//        versionInformationLab->setText(tr("检测到你的系统有可更新的应用！"));
        versionInformationLab->setText(tr("Updatable app detected on your system!"));
        systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/update.png").scaled(96,96));

    }
}

void TabWid::getAllDisplayInformation()
{
    QSqlQuery query(QSqlDatabase::database("A"));
    QString updatetime;
    QString checkedtime;
    QString checkedstatues;
    QString backupStatus;
    query.exec("select * from display");
    while(query.next())
    {
        updatetime = query.value("update_time").toString();
        checkedtime = query.value("check_time").toString();
        checkedstatues = query.value("auto_check").toString();
        backupStatus = query.value("auto_backup").toString();
    }
    if(QLocale::system().name()!="zh_CN" && updatetime.contains("暂无信息"))
    {
        updatetime = "No Information!";
    }
//    lastRefreshTime->setText(tr("上次更新：")+updatetime);
    lastRefreshTime->setText(tr("Last refresh:")+updatetime);
//    versionInformationLab->setText(tr("上次检测：")+checkedtime);
    versionInformationLab->setText(tr("Last Checked:")+checkedtime);
    if(checkedstatues == "false")
    {
        isAutoCheckSBtn->setChecked(false);
    }
    else
    {
        isAutoCheckSBtn->setChecked(true);
    }
    isAutoCheckSBtn->setEnabled(true);

    if(backupStatus == "false")
    {
        isAutoBackupSBtn->setChecked(false);
    }
    else
    {
        isAutoBackupSBtn->setChecked(true);
    }
}
void TabWid::showHistoryWidget()
{

    historyLog = m_updatelog::GetInstance(this);
    //在屏幕中央显示
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    historyLog->move((availableGeometry.width()-historyLog->width())/2,(availableGeometry.height()- historyLog->height())/2);
    historyLog->show();
}

void TabWid::checkUpdateBtnClicked()
{
    if(checkUpdateBtn->text() == tr("Check Update"))
    {
        connect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
        connect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QVariantList)),this,SLOT(slotUpdateCache(QVariantList)));
        connect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QVariantList)),this,SLOT(slotUpdateCacheProgress(QVariantList)));
        updateMutual->failedList.clear();
        QList<AppUpdateWid*> list = this->findChildren<AppUpdateWid*>();
        for(AppUpdateWid* tmp:list)
        {
            tmp->deleteLater();
        }
        updateSource->callDBusUpdateTemplate();
//        versionInformationLab->setText(tr("正在更新软件源..."));
        versionInformationLab->setText(tr("Updating the software source")+"...");
        //判断信号连接超时
        QTimer *timer = new QTimer;
        timer->setSingleShot(true);
        connect(timer,&QTimer::timeout,this,&TabWid::unableToConnectSource);
        timer->start(60000);
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        updateMutual->insertInstallStates("check_time",current_date);
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
    }
    else if(checkUpdateBtn->text() == tr("UpdateAll"))
    {
        if(isAutoBackupSBtn->isChecked() == true)
        {
            backupCore();//备份模块主函数
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("This update will not backup the current system, do you want to continue the update?"));
//            msgBox.setText(tr("本次更新不会备份当前系统，是否继续更新？"));
            msgBox.setWindowTitle(tr("Prompt information"));
//            msgBox.setWindowTitle("提示信息");
            msgBox.setStandardButtons(QMessageBox::Yes
                                      | QMessageBox::No
                                      | QMessageBox::Cancel);
            msgBox.setButtonText(QMessageBox::Yes,tr("Yes, keep updating"));
            msgBox.setButtonText(QMessageBox::No,tr("No, backup now"));
            msgBox.setButtonText(QMessageBox::Cancel,tr("Not updated"));
//            msgBox.setButtonText(QMessageBox::Yes,"是，继续更新");
//            msgBox.setButtonText(QMessageBox::No,"否，立即备份");
//            msgBox.setButtonText(QMessageBox::Cancel,"暂不更新");
            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Yes:
                qDebug() << "是，继续更新";
//                checkUpdateBtn->setText("正在更新...");
                checkUpdateBtn->setEnabled(false);
                checkUpdateBtn->start();
                updateMutual->isPointOutNotBackup = false;   //全部更新时不再弹出单个更新未备份提示
                emit updateAllSignal();
                break;
            case QMessageBox::No:
                backupCore();
                qDebug() << "否，立即备份";
                break;
            case QMessageBox::Cancel:
                qDebug() << "Close 暂不更新!";
                break;
            }
        }
    }
}

void TabWid::isAutoCheckedChanged()     //自动检测按钮绑定的槽函数
{
    if(isAutoCheckSBtn->isChecked() == false)
    {
        updateMutual->insertInstallStates("auto_check","false");

    }
    else if(isAutoCheckSBtn->isChecked() == true)
    {
        updateMutual->insertInstallStates("auto_check","true");
    }
}

void TabWid::isAutoBackupChanged()
{
    if(isAutoBackupSBtn->isChecked() == false)
    {
        updateMutual->insertInstallStates("auto_backup","false");

    }
    else if(isAutoBackupSBtn->isChecked() == true)
    {
        updateMutual->insertInstallStates("auto_backup","true");
    }
}

void TabWid::slotCancelDownload()
{
    checkUpdateBtn->stop();
//    checkUpdateBtn->setText("全部更新");
    checkUpdateBtn->setText(tr("UpdateAll"));
    checkUpdateBtn->setCheckable(true);
}

void TabWid::hideUpdateBtnSlot(bool isSucceed)
{
    if(isSucceed == true)
    {
        qDebug() << "当前更新列表" << updateMutual->importantList;
        QDateTime nowtime = QDateTime::currentDateTime();
        QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
//        lastRefreshTime->setText(tr("上次更新：")+current_date);
        lastRefreshTime->setText(tr("Last refresh:")+current_date);
//        updateMutual->insertInstallStates("update_time",current_date);

    }
    if(updateMutual->importantList.size() == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->stop();
//        checkUpdateBtn->setText(tr("检查更新"));
        checkUpdateBtn->setText(tr("Check Update"));
        if(updateMutual->failedList.size() == 0)
        {
//            versionInformationLab->setText(tr("您的系统已是最新！"));
            versionInformationLab->setText(tr("Your system is the latest!"));
            systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/normal.png").scaled(96,96));
        }
        else
        {
//            versionInformationLab->setText(tr("部分更新失败！"));
            versionInformationLab->setText(tr("Part of the update failed!"));
        }
    }
}

void TabWid::changeUpdateAllSlot()
{

    if(checkUpdateBtn->isEnabled() == false)
    {
//        checkUpdateBtn->setText("全部更新");
        checkUpdateBtn->setText(tr("UpdateAll"));
        checkUpdateBtn->setEnabled(true);
    }
}


void TabWid::waitCrucialInstalled()
{
    if(fileLockedStatus == false)
    {
        QString msg = tr("An important update is in progress, please wait.");
//        QString msg = tr("正在进行一项重要更新，请等待。");
        updateMutual->onRequestSendDesktopNotify(msg);
        versionInformationLab->setText(msg);
        fileLockedStatus = true;
    }
}

void TabWid::getReplyFalseSlot()
{
    isConnectSourceSignal = true;
    disconnectSource();
}


void TabWid::receiveBackupStartResult(int result)
{
    switch (result) {
    case int(backuptools::backup_result::BACKUP_START_SUCCESS):
//        versionInformationLab->setText(tr("开始备份，正在获取进度")+"...");
        versionInformationLab->setText(tr("Start backup,getting progress")+"...");
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
        return;
    case int(backuptools::backup_result::WRITE_STORAGEINFO_ADD_ITEM_FAIL):
    case int(backuptools::backup_result::WRITE_STORAGEINFO_UPDATE_ITEM_FAIL):
//        backupMessageBox(tr("写入配置文件失败，本次更新不会备份系统！"));
        backupMessageBox(tr("Failed to write configuration file, this update will not back up the system!"));
        break;
    case int(backuptools::backup_result::BACKUP_CAPACITY_IS_NOT_ENOUGH):
//        backupMessageBox(tr("备份空间不足，本次更新不会备份系统！"));
        backupMessageBox(tr("Insufficient backup space, this update will not backup your system!"));
        break;
    case int(backuptools::backup_result::INC_NOT_FOUND_UUID):
//        backupMessageBox(tr("麒麟备份还原工具无法找到UUID，本次更新不会备份系统!"));
        backupMessageBox(tr("Kylin backup restore tool could not find the UUID, this update will not backup the system!"));
        break;
    default:
        backupMessageBox(tr("The backup restore partition is abnormal. You may not have a backup restore partition.For more details,see /var/log/backup.log"));
        break;
    }
}

void TabWid::whenStateIsDuing()
{
    versionInformationLab->setText(tr("Calculating Capacity..."));
    checkUpdateBtn->start();
    checkUpdateBtn->setEnabled(false);
}
