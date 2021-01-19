#include "tabwidget.h"

TabWid::TabWid(QWidget *parent):QWidget(parent)
{
    allComponents();
}

void TabWid::initDbus()
{
    updateMutual = UpdateDbus::getInstance();
    updateSource = new UpdateSource();
    ukscConnect = new UKSCConn();
//    this->resize(620,580);


    getAllDisplayInformation();
    connect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
    connect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QVariantList)),this,SLOT(slotUpdateCache(QVariantList)));
    connect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QVariantList)),this,SLOT(slotUpdateCacheProgress(QVariantList)));


    connect(updateMutual,&UpdateDbus::sendAppMessageSignal,this,&TabWid::loadingOneUpdateMsgSlot);
    connect(updateMutual,&UpdateDbus::sendFinishGetMsgSignal,this,&TabWid::loadingFinishedSlot);
    connect(checkUpdateBtn,&QPushButton::clicked,this,&TabWid::checkUpdateBtnClicked);
    connect(historyUpdateLog,&QPushButton::clicked,this,&TabWid::showHistoryWidget);
    connect(isAutoCheckSBtn,&SwitchButton::checkedChanged,this,&TabWid::isAutoCheckedChanged);
    connect(isAutoBackupSBtn,&SwitchButton::checkedChanged,this,&TabWid::isAutoBackupChanged);

    bacupInit();//初始化备份

    checkUpdateBtn->stop();
    checkUpdateBtn->setText(tr("检查更新"));
    if(firstCheckedStatus == false)
    {
        checkUpdateBtnClicked();
        firstCheckedStatus = true;
    }
}

TabWid::~TabWid()
{
    qDebug() << "~TabWid" ;
    delete updateMutual;
    backupDelete();//回收资源
//    updateMutual->cleanUpdateList();
}

void TabWid::backupMessageBox(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.setWindowTitle("提示信息");
//        msgBox.setInformativeText(tr("备份还原可以保存当前系统状态。"));
//    msgBox.setDetailedText(tr("Differences here..."));
    msgBox.setStandardButtons(QMessageBox::Save
                              | QMessageBox::Discard|QMessageBox::Abort);
    msgBox.setButtonText(QMessageBox::Save,"立即更新");
    msgBox.setButtonText(QMessageBox::Discard,"取消更新");
    msgBox.setButtonText(QMessageBox::Abort,"否，我不备份");
    msgBox.button(QMessageBox::Abort)->hide();
    int ret = msgBox.exec();
    if(ret == QMessageBox::Save)
    {
        qDebug() << "立即更新!";
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
        versionInformationLab->setText("正在更新...");
        emit updateAllSignal();
    }
    else if(ret == QMessageBox::Discard)
    {
       qDebug() << "不进行全部更新。";
       checkUpdateBtn->stop();
       checkUpdateBtn->setEnabled(true);
       checkUpdateBtn->setText("全部更新");
    }
    else if(ret == QMessageBox::Abort)
    {
       qDebug() << "不进行全部更新。";
       checkUpdateBtn->stop();
       checkUpdateBtn->setEnabled(true);
       checkUpdateBtn->setText("全部更新");
    }
}

void TabWid::backupCore()
{
    int initresult = emit needBackUp();
    switch (initresult) {
    case -1:
        backupMessageBox("未能找到备份还原分区，本次更新不会备份系统!");
        //如果是则立即更新,否的话取消全部更新
        return;
    case -2:
        versionInformationLab->setText("麒麟备份还原工具正在进行其他操作，请稍后更新");
        return;
    case -3:
        versionInformationLab->setText("源管理器配置文件异常，本次更新不会备份系统！");
        return;
    case -4:
        versionInformationLab->setText("已备份，无需再次备份");
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
        emit updateAllSignal();
        return;
    case 1://正在备份
        emit startBackUp(0);
        break;
    case needBack://需要备份
        emit startBackUp(1);
        break;
    case -9://备份还原工具不存在
        backupMessageBox("麒麟备份还原工具不存在，本次更新不会备份系统!");
        return;
    default:
        qDebug()<<"备份还原工具状态码"<<initresult;
        return;
    }
     qDebug()<<"符合备份条件";
}

void TabWid::backupProgress(int progress)
{
    if(progress==100)
    {
        //备份完成，开始安装
        qDebug()<<"备份完成，开始安装";
        versionInformationLab->setText("备份完成");
        return;
    }
    versionInformationLab->setText("备份中："+QString::number(progress)+"%");
}

void TabWid::bakeupFinish(int result)
{
    switch (result) {
    case 0:
        versionInformationLab->setText("开始备份,正在获取进度...");
        return;
    case -1:
        backupMessageBox("写入配置文件失败,本次更新不会备份系统!");
        break;
    case -2:
        backupMessageBox("备份空间不足,本次更新不会备份系统!");
        break;
    case -8:
        backupMessageBox("麒麟备份还原工具无法找到UUID,本次更新不会备份系统!");
        break;
    case 99:
        versionInformationLab->setText("备份完成！");
        break;
    default:
        backupMessageBox("麒麟备份还原工具异常："+QString::number(result)+",本次更新不会备份系统!");
        break;
    }
}

void TabWid::backupHideUpdateBtn(int result)
{
    if(result == 0)
    {
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
    }
    else if(result == 99)
    {
        checkUpdateBtn->start();
        versionInformationLab->setText("正在更新...");
        emit updateAllSignal();
    }
}
void TabWid::bacupInit()
{
    backup = new BackUp;
    backupThread = new QThread;
    backup->moveToThread(backupThread);
    connect(this,&TabWid::needBackUp,backup,&BackUp::needBacdUp,Qt::BlockingQueuedConnection);//同步信号，阻塞，取返回值
    connect(this,&TabWid::startBackUp,backup,&BackUp::startBackUp);
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
    qDebug() << "update template status :" << status;
    /*更新源缓存*/
//    if(updateMutual->importantList.size() == 0)
//    {

//        updateSource->callDBusUpdateSource(Symbol);
//        versionInformationLab->setText(tr("正在更新源..."));
//    }
}
void TabWid::slotUpdateCache(QVariantList sta)
{
    QString status = sta.at(1).toString();
    QString nowsymbol = sta.at(0).toString();
    qDebug() << "slotUpdateCache" << "nowsymbol" <<nowsymbol << status;
    if(nowsymbol == Symbol)
    {
        qDebug() << "update cache status :" << status;
        if (!status.compare("success")) {
            versionInformationLab->setText(tr("正在获取更新列表..."));
            QFile file(IMPORTANT_FIEL_PATH);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() <<"file open failed!" << IMPORTANT_FIEL_PATH;
                return ;
            }
            QString str =  file.readAll();
            QStringList list;
            if (!str.isEmpty() && str.contains(" ")) {
                list = str.split(" ");
            }
//            qDebug() << "获取到的包列表：" << list;
            updateMutual->getAppMessage(list);
            retryTimes = 0;
        }
        else
        {
            int statuscode = status.toInt();
            if(statuscode == 400 && retryTimes < netErrorRetry)
            {
                updateSource->callDBusUpdateTemplate();
                qDebug() << "statuscode = :" << statuscode;
                QString failedInfo = updateSource->getFailInfo(statuscode);
                qDebug() << "failedInfo:" << failedInfo;
                retryTimes++;
            }
            else
            {
                QString failedInfo = updateSource->getFailInfo(statuscode);
                checkUpdateBtn->setEnabled(true);
                checkUpdateBtn->stop();
                checkUpdateBtn->setText(tr("检查更新"));
                versionInformationLab->setText("软件源更新失败："+failedInfo );
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
            versionInformationLab->setText(tr("更新软件源进度：") + QString::number(progress)+"%");
        }
        else
        {
            versionInformationLab->setText(tr("更新软件源进度：") + QString::number(progress)+"%" + "（第" + QString::number(retryTimes) +"次重试）");
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
    labUpdate->setText(tr("更新"));
    ft.setPointSize(28);
    labUpdate->setFont(ft);

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
    historyUpdateLog->setText(tr("查看更新历史"));
    historyUpdateLogWid->setLayout(historyUpdateLogLayout);
    historyUpdateLogLayout->setAlignment(Qt::AlignLeft);
    historyUpdateLogLayout->addWidget(historyUpdateLog);
    historyUpdateLogLayout->setSpacing(0);
    historyUpdateLogLayout->setMargin(0);


    inforLayout = new QVBoxLayout();
    lastRefreshTime = new QLabel(systemWidget);
    lastRefreshTime->setText("上次更新时间：");
    versionInformationLab = new QLabel(systemWidget);
    versionInformationLab->setText("上次检测时间：");
    inforLayout->addWidget(versionInformationLab);
    inforLayout->addWidget(lastRefreshTime);
    QWidget *inforWidget = new QWidget(systemWidget);
    inforWidget->setLayout(inforLayout);

    checkUpdateBtn = new m_button(systemWidget);
    checkUpdateBtn->setDefault(true);
    checkUpdateBtn->start();
//    checkUpdateBtn->setText(tr("检查更新"));
    checkUpdateBtn->setFixedSize(120,36);


    tab1HLayout->addWidget(systemPortraitLab,0,Qt::AlignLeft);
    tab1HLayout->addWidget(inforWidget,Qt::AlignLeft|Qt::AlignTop);
    tab1HLayout->addWidget(checkUpdateBtn);
    tab1HLayout->setContentsMargins(8,25,20,20);
    tab1HLayout->setSpacing(0);

    updateSettingLab = new QLabel();
    updateSettingLab->setText(tr("更新设置"));
    ft.setPointSize(28);
    updateSettingLab->setFont(ft);
    updateSettingWidget = new QWidget(this);
    updatesettingLayout = new QVBoxLayout();
    updateSettingWidget->setLayout(updatesettingLayout);

    isAutoCheckWidget = new QFrame();
    isAutoCheckWidget->setFrameShape(QFrame::Box);
    isAutoCheckLayout = new QHBoxLayout();
    isAutoCheckedLab = new QLabel();
    isAutoCheckedLab->setText(tr("允许通知可更新的应用"));
    isAutoCheckSBtn = new SwitchButton();
    isAutoCheckWidget->setLayout(isAutoCheckLayout);
    isAutoCheckLayout->addWidget(isAutoCheckedLab);
    isAutoCheckLayout->addWidget(isAutoCheckSBtn);

    isAutoBackupWidget = new QFrame();
    isAutoBackupWidget->setFrameShape(QFrame::Box);
    isAutoBackupLayout = new QHBoxLayout();
    isAutoBackupLab = new QLabel();
    isAutoBackupLab->setText(tr("全部更新前备份当前系统为可回退的版本"));
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
    AppMessage->setContentsMargins(0,0,28,10);

    mainTabLayout->setAlignment(Qt::AlignTop);
    mainTabLayout->addWidget(scrollArea);
//    mainTabLayout->setSpacing(0);
    mainTabLayout->setMargin(0);
    this->setLayout(mainTabLayout);
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
        if(ukscConnect->isConnectUskc = true)
        {
            QStringList list = ukscConnect->getInfoByName(msg.name);
            if(list[2] != "")
            {
                appWidget->appNameLab->setText(list[2]);

            }
            if(list[1] != "")
            {
                appWidget->appNameLab->setText(list[1]);
            }
            if(list[0] != "")
            {
                appWidget->appIcon->setPixmap(QPixmap(list[0]).scaled(32, 32));
            }
        }
        allUpdateLayout->addWidget(appWidget);
        qDebug() << "loadingOneUpdateMsgSlot:" << appWidget->appNameLab->text();

    }

}

void TabWid::loadingFinishedSlot(int size)
{
    qDebug()<< "loadingFinishedSlot:" << "size = " <<size;
    if(size == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->stop();
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText(tr("您的系统已是最新！"));
    }
    else
    {
        checkUpdateBtn->stop();
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->setText(tr("全部更新"));
        versionInformationLab->setText(tr("检测到你的系统有可更新的应用！"));
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
        updatetime = query.value(2).toString();
        checkedtime = query.value(1).toString();
        checkedstatues = query.value("auto_check").toString();
        backupStatus = query.value("auto_backup").toString();
    }
    lastRefreshTime->setText("上次更新: "+updatetime);
    versionInformationLab->setText("上次检测: "+checkedtime);
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
    historyLog->show();
}

void TabWid::checkUpdateBtnClicked()
{
    if(checkUpdateBtn->text() == "检查更新")
    {
        updateMutual->failedList.clear();
        QLayoutItem *child;
        while ((child = allUpdateLayout->takeAt(0)) != 0)
        {
            //setParent为NULL，防止删除之后界面不消失
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }
            delete child;
        }
//        checkUpdateBtn->setText("正在更新源...");
        updateSource->callDBusUpdateTemplate();
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        updateMutual->insertInstallStates("check_time",current_date);
        checkUpdateBtn->start();
        checkUpdateBtn->setEnabled(false);
    }
    else if(checkUpdateBtn->text() == "全部更新")
    {
        if(isAutoBackupSBtn->isChecked() == true)
        {
            backupCore();//备份模块主函数
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("本次更新不会备份当前系统，是否继续更新？"));
            msgBox.setWindowTitle("提示信息");
            msgBox.setStandardButtons(QMessageBox::Yes
                                      | QMessageBox::No
                                      | QMessageBox::Cancel);
            msgBox.setButtonText(QMessageBox::Yes,"是，继续更新");
            msgBox.setButtonText(QMessageBox::No,"否，立即备份");
            msgBox.setButtonText(QMessageBox::Cancel,"暂不更新");
            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Yes:
                qDebug() << "是，继续更新";
//                checkUpdateBtn->setText("正在更新...");
                checkUpdateBtn->setEnabled(false);
                checkUpdateBtn->start();
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
    checkUpdateBtn->setText("全部更新");
    checkUpdateBtn->setCheckable(true);
}



void TabWid::hideUpdateBtnSlot()
{
    qDebug() << "当前更新列表" << updateMutual->importantList;
    QDateTime nowtime = QDateTime::currentDateTime();
    QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
    lastRefreshTime->setText("上次更新时间："+current_date);
    if(updateMutual->importantList.size() == 0 && updateMutual->failedList.size() == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->stop();
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText(tr("您的系统已是最新！"));
        systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/normal.png").scaled(96,96));
    }
    else if(updateMutual->importantList.size() == 0 && updateMutual->failedList.size() != 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->stop();
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText(tr("部分更新失败！"));
    }
}

void TabWid::changeUpdateAllSlot()
{

    if(checkUpdateBtn->isEnabled() == false)
    {
        checkUpdateBtn->setText("全部更新");
        checkUpdateBtn->setEnabled(true);
    }
}


void TabWid::waitCrucialInstalled()
{
    if(fileLockedStatus == false)
    {
        updateMutual->onRequestSendDesktopNotify("正在进行一项重要更新，请等待。");
        versionInformationLab->setText("正在进行一项重要更新，请等待。");
        fileLockedStatus == true;
    }
}
