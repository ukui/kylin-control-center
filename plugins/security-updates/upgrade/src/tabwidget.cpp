#include "tabwidget.h"

TabWid::TabWid(QWidget *parent):QWidget(parent)
{
    updateMutual = UpdateDbus::getInstance();
    ukscConnect = new UKSCConn();
    updateSource = new UpdateSource();
//    this->resize(620,580);
    allComponents();
    //打开时是否自动检测

    getAllDisplayInformation();
    connect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
    connect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QString)),this,SLOT(slotUpdateCache(QString)));
    connect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QString)),this,SLOT(slotUpdateCacheProgress(QString)));
    bacupInit();//初始化备份

}

TabWid::~TabWid()
{
    qDebug() << "~TabWid" ;
    backupDelete();//回收资源
//    updateMutual->cleanUpdateList();
}
void TabWid::backupMessageBox(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
//        msgBox.setInformativeText(tr("备份还原可以保存当前系统状态。"));
//    msgBox.setDetailedText(tr("Differences here..."));
    msgBox.setStandardButtons(QMessageBox::Save
                              | QMessageBox::Discard);
    msgBox.setButtonText(QMessageBox::Save,"立即更新");
    msgBox.setButtonText(QMessageBox::Discard,"取消更新");
//        msgBox.setButtonText(QMessageBox::Abort,"否，我不备份");
    int ret = msgBox.exec();
    if(ret == QMessageBox::Save)
    {
        qDebug() << "立即更新!";
        checkUpdateBtn->setText("正在更新...");
        checkUpdateBtn->setEnabled(false);
        emit updateAllSignal();
    }
    else if(ret == QMessageBox::Discard)
    {
       qDebug() << "不进行全部更新。";
       checkUpdateBtn->setEnabled(true);
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
        return;
    }
     qDebug()<<"开始备份";
     versionInformationLab->setText("开始备份,正在获取进度...");
}

void TabWid::backupProgress(int progress)
{
    if(progress==100)
    {
        //备份完成，开始安装
        qDebug()<<"备份完成，开始安装";
        checkUpdateBtn->setText("正在更新...");
        checkUpdateBtn->setEnabled(false);
        emit updateAllSignal();
        return;
    }
    versionInformationLab->setText("备份中："+QString::number(progress)+"%");
}

void TabWid::bakeupFinish(int result)
{
    if(result!=0)
    {
        versionInformationLab->setText("麒麟备份还原工具异常，异常代码："+QString::number(result));
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
    updateSource->callDBusUpdateSource();
    checkUpdateBtn->setText(tr("正在更新源"));
}
void TabWid::slotUpdateCache(QString status)
{
    qDebug() << "update cache status :" << status;

    if (!status.compare("success")) {
        checkUpdateBtn->setText(tr("获取更新列表"));
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
        qDebug() << "获取到的包列表：" << list;

        updateMutual->getAppMessage(list);


    }
    else
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText("更新软件源失败!");
    }
}
void TabWid::slotUpdateCacheProgress(QString progress)
{
    qDebug() << "update cache progress :" << progress;
    checkUpdateBtn->setText(tr("更新源:") + progress);
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

    QLabel *labUpdate = new QLabel(this);
    labUpdate->setText(tr("更新"));
    ft.setPointSize(28);
    labUpdate->setFont(ft);

    scrollArea->setWidget(updateTab);
    scrollArea->setFrameStyle(0);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

    isAutoCheckBox = new QCheckBox(this);
    isAutoCheckBox->setCheckable(true);
    isAutoCheckBox->setText(tr("允许通知可更新的应用"));

    inforLayout = new QVBoxLayout();
    lastRefreshTime = new QLabel(systemWidget);
    lastRefreshTime->setText("上次更新时间：0000.00.00 00:00");
    versionInformationLab = new QLabel(systemWidget);
    versionInformationLab->setText("上次检测时间：0000.00.00 00:00");
    inforLayout->addWidget(versionInformationLab);
    inforLayout->addWidget(lastRefreshTime);
    inforLayout->addWidget(isAutoCheckBox);
    QWidget *inforWidget = new QWidget(systemWidget);
    inforWidget->setLayout(inforLayout);

    checkUpdateBtn = new QPushButton(systemWidget);
    checkUpdateBtn->setDefault(true);
    checkUpdateBtn->setText(tr("检查更新"));
    checkUpdateBtn->setFixedSize(120,36);


    tab1HLayout->addWidget(systemPortraitLab,0,Qt::AlignLeft);
    tab1HLayout->addWidget(inforWidget,Qt::AlignLeft|Qt::AlignTop);
    tab1HLayout->addWidget(checkUpdateBtn);
    tab1HLayout->setContentsMargins(8,25,20,20);
    tab1HLayout->setSpacing(0);

    AppMessage->addWidget(labUpdate);
    AppMessage->addWidget(systemWidget);
    AppMessage->addWidget(allUpdateWid);
    AppMessage->addWidget(historyUpdateLogWid);
    AppMessage->setContentsMargins(0,0,28,10);

    mainTabLayout->setAlignment(Qt::AlignTop);
    mainTabLayout->addWidget(scrollArea);
    mainTabLayout->setSpacing(0);
    mainTabLayout->setMargin(0);
    this->setLayout(mainTabLayout);

    connect(updateMutual,&UpdateDbus::updatelist,this,&TabWid::slotGetImportant);
    connect(updateMutual,&UpdateDbus::sendAppMessageSignal,this,&TabWid::loadingOneUpdateMsgSlot);
    connect(updateMutual,&UpdateDbus::sendFinishGetMsgSignal,this,&TabWid::loadingFinishedSlot);
    connect(checkUpdateBtn,&QPushButton::clicked,this,&TabWid::checkUpdateBtnClicked);
    connect(historyUpdateLog,&QPushButton::clicked,this,&TabWid::showHistoryWidget);
    connect(isAutoCheckBox,&QCheckBox::clicked,this,&TabWid::isAutoCheckedChanged);
//    connect(historyUpdateLog,&QPushButton::clicked,this,&TabWid::downloadFailedSlot);


}

//往tabwidget中插入listwidget信息
void TabWid::getAppUpdateMsg(QStringList arg)
{

}

void TabWid::loadingOneUpdateMsgSlot(AppAllMsg msg)
{
//    checkUpdateBtn->setText();
    updateMutual->importantList.append(msg.name);   //重要更新列表中添加appname
    AppUpdateWid *appWidget = new AppUpdateWid(msg, this);
    connect(appWidget, &AppUpdateWid::cancel, this, &TabWid::slotCancelDownload);
    connect(this, &TabWid::updateAllSignal, appWidget, &AppUpdateWid::updateAllApp);
    connect(appWidget,&AppUpdateWid::hideUpdateBtnSignal,this,&TabWid::hideUpdateBtnSlot);
    connect(appWidget,&AppUpdateWid::changeUpdateAllSignal,this,&TabWid::changeUpdateAllSlot);
    connect(appWidget,&AppUpdateWid::downloadFailedSignal,this,&TabWid::downloadFailedSlot);
    connect(appWidget,&AppUpdateWid::filelockedSignal,this,&TabWid::waitCrucialInstalled);
    QStringList list = ukscConnect->getInfoByName(msg.name);
    if(list[1] != "")
    {
        appWidget->appNameLab->setText(list[1]);
    }
    allUpdateLayout->addWidget(appWidget);
}

void TabWid::loadingFinishedSlot(int size)
{
    if(size == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText(tr("您的系统已是最新！"));
    }
    else
    {
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
    query.exec("select * from display");
    while(query.next())
    {
        updatetime = query.value(2).toString();
        checkedtime = query.value(1).toString();
        checkedstatues = query.value(3).toString();
    }
    lastRefreshTime->setText("上次更新: "+updatetime);

    versionInformationLab->setText("上次检测: "+checkedtime);

    checkUpdateBtn->setText(tr("检查更新"));

    if(checkedstatues == "false")
    {
        isAutoCheckBox->setChecked(false);
    }
    else
    {
        isAutoCheckBox->setChecked(true);
    }



}
void TabWid::showHistoryWidget()
{

    historyLog = new m_updatelog();
    historyLog->exec();
    qDebug()<<"ok";
}

void TabWid::checkUpdateBtnClicked()
{
    if(checkUpdateBtn->text() == "检查更新")
    {
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
        checkUpdateBtn->setText("正在更新源...");
        updateSource->callDBusUpdateTemplate();
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        updateMutual->insertInstallStates("check_time",current_date);
    }
    else if(checkUpdateBtn->text() == "全部更新")
    {
        checkUpdateBtn->setEnabled(false);
        backupCore();//备份模块主函数
    }
}

void TabWid::isAutoCheckedChanged()     //自动检测按钮绑定的槽函数
{
    if(isAutoCheckBox->isChecked() == false)
    {
        updateMutual->insertInstallStates("auto_check","false");

    }
    else if(isAutoCheckBox->isChecked() == true)
    {
        updateMutual->insertInstallStates("auto_check","true");
    }
}



void TabWid::slotCancelDownload()
{
    checkUpdateBtn->setText("全部更新");
    checkUpdateBtn->setCheckable(true);
}

void TabWid::slotGetImportant(QStringList args)
{
//    qDebug() << "成功获得更新列表";
//    QStringList itList = args;
//    getAppUpdateMsg(itList);

}

void TabWid::hideUpdateBtnSlot()
{
    qDebug() << "当前更新列表" << updateMutual->importantList;
    QDateTime nowtime = QDateTime::currentDateTime();
    QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
    lastRefreshTime->setText("上次更新时间："+current_date);
    if(updateMutual->importantList.size() == 0)
    {
        checkUpdateBtn->setEnabled(true);
        checkUpdateBtn->setText(tr("检查更新"));
        versionInformationLab->setText(tr("您的系统已是最新！"));
        systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/normal.png").scaled(96,96));
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


void TabWid::downloadFailedSlot()
{
    if(downloadFailedStatus ==false)
    {
        QMessageBox::warning(NULL, "错误提示", "服务器异常或网络断开连接，暂停下载！");
        downloadFailedStatus = true;
    }
}
void TabWid::waitCrucialInstalled()
{
    if(fileLockedStatus == false)
    {
        updateMutual->onRequestSendDesktopNotify("正在进行一项重要更新，请等待。");
        fileLockedStatus == true;
    }
}
