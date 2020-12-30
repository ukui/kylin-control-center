#include "tabwidget.h"

TabWid::TabWid(QWidget *parent):QWidget(parent)
{
    updateMutual = UpdateDbus::getInstance();
    updateSource = new UpdateSource();
//    this->resize(620,580);
    allComponents();
    createUI();
    getAllDisplayInformation();
    connect(updateSource->serviceInterface,SIGNAL(updateTemplateStatus(QString)),this,SLOT(slotUpdateTemplate(QString)));
    connect(updateSource->serviceInterface,SIGNAL(updateCacheStatus(QString)),this,SLOT(slotUpdateCache(QString)));
    connect(updateSource->serviceInterface,SIGNAL(updateSourceProgress(QString)),this,SLOT(slotUpdateCacheProgress(QString)));
}

TabWid::~TabWid()
{
    qDebug() << "~TabWid" ;
//    updateMutual->cleanUpdateList();
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
        checkUpdateBtn->setText(tr("更新成功"));
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
        QStringList ilist =  updateMutual->checkInstallOrUpgrade(list);
        qDebug() << "处理后的包列表: " << ilist;
        getAppUpdateMsg(ilist);
    }
    else
    {
        checkUpdateBtn->setText("更新失败");
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

    updateWidget = new QTabWidget(this);   //标签页
//    updateWidget->resize(500,580);
    scrollArea = new QScrollArea(this);

    updateTab = new QWidget(this);  //更新页面
    AppMessage = new QVBoxLayout();
    updateTab->setLayout(AppMessage);
    systemWidget = new QFrame(updateTab);
    systemWidget->setFrameShape(QFrame::Box);
    systemPortraitLab = new QLabel(systemWidget);


    allUpdateWid = new QWidget(this);
    allUpdateLayout = new QVBoxLayout();
    allUpdateWid->setLayout(allUpdateLayout);
//    allUpdateLayout->setContentsMargins(0,5,0,0);
    allUpdateLayout->setSpacing(8);
    AppMessage->addWidget(systemWidget,0,Qt::AlignTop);
//    tab1VLayout->addWidget(appUpdateListWidget);
//    updateTab->setLayout(tab1VLayout);
    AppMessage->addWidget(allUpdateWid,1,Qt::AlignTop);

    scrollArea->setWidget(updateTab);
    scrollArea->setFrameStyle(0);
//    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);

//    tab1VLayout->addWidget(appWid1,0,Qt::AlignTop);
//    tab1VLayout->addWidget(appWid2,1,Qt::AlignTop);

    systemWidget->setFixedHeight(120);

    updateSettingTab = new QWidget(this);  //更新设置界面
    tab2VLayout = new QVBoxLayout();
    updateSettingTab->setLayout(tab2VLayout);
//    tab2HLayout = new QHBoxLayout(updateSettingTab);
    isAutoCheckWidget = new QFrame();
    isAutoCheckWidget->setFrameShape(QFrame::Box);
    isAutoCheckedLab = new QLabel();
    isAutoCheckWidget->setFixedHeight(50);
    isAutoCheckedLab->setText("自动检查更新");

    isAutoCheckSBtn = new SwitchButton(isAutoCheckWidget);
    QHBoxLayout *autoCheckLayout = new QHBoxLayout();
    isAutoCheckWidget->setLayout(autoCheckLayout);
    autoCheckLayout->addWidget(isAutoCheckedLab,0,Qt::AlignLeft);
    autoCheckLayout->addWidget(isAutoCheckSBtn,1,Qt::AlignRight);
    autoCheckLayout->setSpacing(0);


    tab2VLayout->addWidget(isAutoCheckWidget,0,Qt::AlignTop);

    mainTabLayout->addWidget(updateWidget);
    mainTabLayout->setContentsMargins(0,0,0,0);  //存在问题，设置右边边距为28px时会截掉右边部分窗口

    updateWidget->addTab(scrollArea,"更新");
    updateWidget->addTab(updateSettingTab,"更新设置");

    this->setLayout(mainTabLayout);
    connect(isAutoCheckSBtn,&SwitchButton::checkedChanged,this,&TabWid::isAutoCheckedChanged);
    connect(updateMutual,&UpdateDbus::updatelist,this,&TabWid::slotGetImportant);


}

//往tabwidget中插入listwidget信息
void TabWid::getAppUpdateMsg(QStringList arg)
{
//    updateMutual->ConnectSlots();
//    int cnum = updateMutual->cnameList.size();
    qDebug() << "加载更新信息！";
    checkUpdateBtn->setText(tr("加载更新..."));
    int inum = arg.size();
    allUpdateLayout->setAlignment(Qt::AlignTop);
    if(inum != 0)
    {

        for(inumber = 0; inumber < inum; inumber++)
        {
            QString app_name = arg[inumber];
            if(app_name.isEmpty())
                continue;
            qDebug() << app_name;
            updateMutual->importantList.append(app_name);   //重要更新列表中添加appname
            AppUpdateWid *appWidget = new AppUpdateWid(app_name, this);
            connect(appWidget, &AppUpdateWid::cancel, this, &TabWid::slotCancelDownload);
            connect(this, &TabWid::updateAllSignal, appWidget, &AppUpdateWid::updateAllApp);
            connect(appWidget,&AppUpdateWid::hideUpdateBtnSignal,this,&TabWid::hideUpdateBtnSlot);
            connect(appWidget,&AppUpdateWid::changeUpdateAllSignal,this,&TabWid::changeUpdateAllSlot);
            allUpdateLayout->addWidget(appWidget);
            QCoreApplication::processEvents();   //逐个加载
//            sleep(2);

        }
//        AppMessage->addStretch();
        checkUpdateBtn->setText("全部更新");

        QDateTime nowtime = QDateTime::currentDateTime();
        QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
        QSqlQuery query(QSqlDatabase::database("A"));
        query.exec(QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次检测时间"));
    }
    else
    {
        checkUpdateBtn->hide();
        versionInformationLab->setText("您的系统已是最新！");

        QDateTime nowtime = QDateTime::currentDateTime();
        QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
        QSqlQuery query(QSqlDatabase::database("A"));
        query.exec(QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次检测时间"));
    }
}

void TabWid::getAllDisplayInformation()
{
    QSqlQuery query(QSqlDatabase::database("A"));
    QStringList displayinfo;

    query.exec("select * from display");
    while(query.next())
    {
        displayinfo.append(query.value(1).toString());
    }
    QString updatetime = displayinfo[0];
    QString checkedtime = displayinfo[1];
    QString checkedstatues = displayinfo[3];

    lastRefreshTime->setText("上次更新: "+updatetime);

    versionInformationLab->setText("上次检测: "+checkedtime);

    checkUpdateBtn->setText("开始检测");



}
void TabWid::showHistoryWidget()
{
    if(historyLogIsCreate == false)
    {
        historyLog = new m_updatelog();
        historyLogIsCreate = true;
    }
    historyLog->updatesql();
    historyLog->show();
    qDebug()<<"ok";
}

void TabWid::checkUpdateBtnClicked()
{
    if(checkUpdateBtn->text() == "开始检测")
    {
        checkUpdateBtn->setText("正在更新源...");
        updateSource->callDBusUpdateTemplate();
//        QDateTime nowtime = QDateTime::currentDateTime();
//        QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
//        QSqlQuery query(QSqlDatabase::database("A"));
//        query.exec(QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次检测时间"));


    }
    else if(checkUpdateBtn->text() == "全部更新")
    {
        QMessageBox msgBox;
        msgBox.setText(tr("是否进行备份还原？"));
        msgBox.setInformativeText(tr("备份还原可以保存当前系统状态。"));
    //    msgBox.setDetailedText(tr("Differences here..."));
        msgBox.setStandardButtons(QMessageBox::Save
                                  | QMessageBox::Discard
                                  | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setButtonText(QMessageBox::Save,"是，立即备份");
        msgBox.setButtonText(QMessageBox::Discard,"我已备份");
        msgBox.setButtonText(QMessageBox::Cancel,"否，我不备份");
        int ret = msgBox.exec();

        if(ret == QMessageBox::Save)
        {
            qDebug() << "now backup";
            QProcess os(0);
            QStringList args;
            args.append("ukui-control-center");
            args.append("--backup");
            os.start("ukui-control-center --backup");
        //    os.start("whereis ",args);
            os.waitForFinished(); //等待完成
        }
        else if(ret == QMessageBox::Discard)
        {
            qDebug() << "no,i had!";
            checkUpdateBtn->setText("正在更新...");
            checkUpdateBtn->setEnabled(false);
            emit updateAllSignal();
        }
        else if (ret == QMessageBox::Cancel)
        {
            qDebug() << "no,i dont!";
            emit updateAllSignal();
            checkUpdateBtn->setText("正在更新...");
            checkUpdateBtn->setEnabled(false);
        }
    }
}

void TabWid::isAutoCheckedChanged()     //自动检测按钮绑定的槽函数
{
    QSqlQuery query(QSqlDatabase::database("A"));
    if(isAutoCheckSBtn->isChecked() == false)
    {
        QString Str = QString("update display set item = '%1' where info = '%2'").arg("false").arg("自动检测");
        query.exec(Str);

    }
    else if(isAutoCheckSBtn->isChecked() == true)
    {
        QString Str = QString("update display set item = '%1' where info = '%2'").arg("true").arg("自动检测");
        query.exec(Str);
    }
}
void TabWid::createUI()
{
    systemPortraitLab = new QLabel();
    tab1HLayout = new QHBoxLayout();
    systemWidget->setLayout(tab1HLayout);
//    systemWidget->setBaseSize(QSize(400,120));

    historyUpdateLog = new QPushButton(this); // 历史日志弹出窗口控制按钮
    historyUpdateLog->setText("历史更新");
    historyUpdateLog->setFlat(true);

    connect(historyUpdateLog,&QPushButton::clicked,this,&TabWid::showHistoryWidget);




    systemPortraitLab->setFixedSize(68,68);
    systemPortraitLab->setPixmap(QPixmap(":/img/plugins/upgrade/refresh.png").scaled(QSize(64,64)));


    inforLayout = new QVBoxLayout();
    lastRefreshTime = new QLabel(systemWidget);
    lastRefreshTime->setText("上次更新时间：0000.00.00 00:00");


    versionInformationLab = new QLabel(systemWidget);
    versionInformationLab->setText("上次检测时间：0000.00.00 00:00");



    inforLayout->addWidget(versionInformationLab);
    inforLayout->addWidget(lastRefreshTime);
    QWidget *inforWidget = new QWidget(systemWidget);
    inforWidget->setLayout(inforLayout);

    checkUpdateBtn = new QPushButton(systemWidget);
    checkUpdateBtn->setText("检测更新");
    checkUpdateBtn->setFixedSize(120,36);
    connect(checkUpdateBtn,&QPushButton::clicked,this,&TabWid::checkUpdateBtnClicked);
//    connect(checkUpdateBtn,&QPushButton::clicked,updateMutual,&UpdateDbus::startTray);




    tab1HLayout->addWidget(systemPortraitLab,0,Qt::AlignLeft);
    tab1HLayout->addWidget(inforWidget,Qt::AlignLeft|Qt::AlignTop);
    tab1HLayout->addWidget(historyUpdateLog);
    tab1HLayout->addWidget(checkUpdateBtn);

    tab1HLayout->setMargin(20);
    tab1HLayout->setSpacing(20);




}

void TabWid::tabUI2()
{
    systemPortraitLab = new QLabel();
    tab1HLayout = new QHBoxLayout();
    systemWidget->setLayout(tab1HLayout);

    systemPortraitLab->setFixedSize(68,68);

    systemPortraitLab->setPixmap(QPixmap(":/image/refresh.png").scaled(QSize(64,64)));


    historyUpdateLog = new QPushButton(this); // 历史日志弹出窗口控制按钮
    historyUpdateLog->setText("历史更新");
    historyUpdateLog->setFlat(true);


    inforLayout = new QVBoxLayout();
    lastRefreshTime = new QLabel(systemWidget);


    versionInformationLab = new QLabel(systemWidget);

    inforLayout->addWidget(versionInformationLab);
    inforLayout->addWidget(lastRefreshTime);
    QWidget *inforWidget = new QWidget(systemWidget);
    inforWidget->setLayout(inforLayout);

    updateAllBtn = new QPushButton(systemWidget);
    updateAllBtn->setText("全部更新");
    updateAllBtn->setFixedSize(120,36);


    tab1HLayout->addWidget(systemPortraitLab,0,Qt::AlignLeft);
    tab1HLayout->addWidget(inforWidget,Qt::AlignLeft|Qt::AlignTop);
    tab1HLayout->addWidget(updateAllBtn);

    tab1HLayout->setMargin(20);
    tab1HLayout->setSpacing(20);

}

void TabWid::slotCancelDownload()
{
    checkUpdateBtn->setText("全部更新");
    checkUpdateBtn->setCheckable(true);
}

void TabWid::slotGetImportant(QStringList args)
{
    qDebug() << "成功获得更新列表";
    QStringList itList = args;
    getAppUpdateMsg(itList);

}

void TabWid::hideUpdateBtnSlot()
{
    qDebug() << "当前更新列表" << updateMutual->importantList;
    QDateTime nowtime = QDateTime::currentDateTime();
    QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
    lastRefreshTime->setText("上次更新时间："+current_date);
    if(updateMutual->importantList.size() == 0)
    {
        checkUpdateBtn->hide();
        versionInformationLab->setText("您的系统已是最新！");
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


//void TabWid::recieveUpgradList(QStringList args)
//{

//}
