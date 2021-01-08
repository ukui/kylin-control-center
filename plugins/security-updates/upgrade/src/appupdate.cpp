#include "appupdate.h"

WorkClass::WorkClass(QObject *parent)
{
    m_updateMutual = UpdateDbus::getInstance();

}

//获取包的所有依赖url、总大小、依赖状态
void WorkClass::getDependPackages(QString appname)
{
//    qDebug() << "appname:" << appname;
    UrlMsg urlmsg;
    QStringList result = m_updateMutual->getDependsPkgs(appname);
//    qDebug() << result;
    foreach (QString msg, result) {
//        qDebug() << "msg:" << msg;
        if(msg.contains("http:")){
            urlmsg.url = msg.split(" ")[0];
            urlmsg.fullname = msg.split(" ")[1];
            QString name = msg.split(" ")[1];
            urlmsg.name = name.split("_")[0];
            QString size = msg.split(" ")[2];
            urlmsg.size = size.toInt();
            appMsg.allSize += size.toInt();
            appMsg.depList.append(urlmsg); //解析数据，添加单包信息：下载链接、包名、全名、大小
        }
        //存在依赖问题
        else if(msg.contains("Unable to correct problems")){
            appMsg.getDepends = false;
            break;
        }
        //包已下载可以直接安装
        else if(msg.contains("0 upgraded") && msg.contains(" 0 newly")){
            appMsg.getDepends = true;
            break;
        }
    }
    emit appMessageSignal(appMsg);
//    qDebug() << urlmsg.url << urlmsg.fullname << urlmsg.name << "allsize" << appMsg.allSize;
}

void WorkClass::writeSqlite(AppAllMsg msg,QString changelog)
{

    QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE","B");
    QString dirPath = QString("%1/.cache/kylin-update-manager/").arg(QDir::homePath());
    QString dbPath = QString("%1/kylin-update-manager.db").arg(dirPath);
    db2.setDatabaseName(dbPath);
    QDateTime nowtime = QDateTime::currentDateTime();
    QString current_date = nowtime.toString("yyyy.MM.dd hh:mm:ss");
    QString version = msg.version;
//    QSqlDatabase db2 = QSqlDatabase::database();
    QSqlQuery query(QSqlDatabase::database("B"));
    QString str = QString("insert into installed(appname, version ,time, description,statue) values('%1', '%2', '%3' ,'%4','%5')").arg(msg.name).arg(version).arg(current_date).arg(changelog).arg("Success");
//            QString str = QString("insert into installed(appname, version , time, statue) values('%1', '%2', '%3' ,'%4')").arg(appAllMsg.name).arg(version).arg(current_date).arg("Success");
    query.exec(str);
    QString str1 = QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次更新时间");
    query.exec(str1);
    if(msg.name.contains("kylin-update-desktop"))
    {
//                QString str1 = QString("update display set item = '%1' where info = '%2'").arg(current_date).arg("上次更新时间");
        QString str2 = QString("update display set item = '%1' where info = '%2'").arg(version).arg("当前系统版本");
        query.exec(str2);
    }
    qDebug() << "name:" << msg.name << "version" << version << "date:" << current_date ;
}


AppUpdateWid::AppUpdateWid(QString appName,QWidget *parent):QWidget(parent)
{
    m_updateMutual = UpdateDbus::getInstance();
    qRegisterMetaType<AppMsg>("AppMsg"); //注册信号槽类型
    qRegisterMetaType<AppAllMsg>("AppAllMsg"); //注册信号槽类型
    appAllMsg.name = appName;
    timer = new QTimer();
    getAppMessage(appName);
    downloadPath = QString("%1%2/").arg(DOWN_CACHE_PATH).arg(appName);
    qDebug() << "cache:" << DOWN_CACHE_PATH;
    downloadProcess = new QProcess();
    worker = new WorkClass();
    workThread = new QThread();
    worker->moveToThread(workThread);
    workThread->start();
    updateAppUi(appName);
    this->setObjectName("AppUpdateWid");
    initConnect();
}

void AppUpdateWid::initConnect()
{
    connect(detaileInfo,&QPushButton::clicked,this,&AppUpdateWid::showDetails);
    connect(updatelogBtn,&QPushButton::clicked,this,&AppUpdateWid::showUpdateLog);
    connect(updateAPPBtn,&QPushButton::clicked,this,&AppUpdateWid::cancelOrUpdate);
    connect(m_updateMutual,&UpdateDbus::transferAptProgress,this,&AppUpdateWid::showInstallStatues);
    connect(this, &AppUpdateWid::startWork, worker, &WorkClass::getDependPackages);
    connect(worker, &WorkClass::appMessageSignal, this, [=](AppMsg msg){
        appAllMsg.msg = msg;
        qDebug() << "allsize" << appAllMsg.msg.allSize;
        if(!appAllMsg.msg.allSize == 0)
        {
            QString allSizeofApp = modifySizeUnit(appAllMsg.msg.allSize);

            someInfoEdit->append("下载大小： "+allSizeofApp);
        }
        else
        {
            QString allSizeofApp = modifySizeUnit(appAllMsg.packageSize);
            someInfoEdit->append("下载大小： "+allSizeofApp);
        }
    });
    //绑定wget进程结束的信号
    connect(downloadProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                         [=] (int exitCode, QProcess::ExitStatus exitStatus) {
        if(!isCancel) //当包存在依赖时，需要显示包的所以依赖以及自身包的总大小
        {
            slotDownloadPackages();
            qDebug() << "one download over";
        }
//            qDebug()  << "exitcode:" << exitCode << exitStatus;
        });
    connect(m_updateMutual, &UpdateDbus::copyFinish, this, &AppUpdateWid::startInstall);
    connect(timer, &QTimer::timeout, this, &AppUpdateWid::calculateSpeedProgress);
    emit startWork(appAllMsg.name);

    connect(this,&AppUpdateWid::writeSqliteSignal,worker,&WorkClass::writeSqlite);
}

//从列表中取出下载信息开始下载
void AppUpdateWid::slotDownloadPackages()
{
    if(appAllMsg.msg.depList.length() != 0)
    {
        QDir dir = downloadPath;
        if(!dir.exists())
        {
            qDebug() << "not exists:" <<QString("%1%2").arg(DOWN_CACHE_PATH).arg(appAllMsg.name);
            dir.mkdir(QString("%1%2").arg(DOWN_CACHE_PATH).arg(appAllMsg.name));
        }
        UrlMsg urlmsg = appAllMsg.msg.depList.at(0);
        QString path = QString("%1%2").arg(downloadPath).arg(urlmsg.fullname);
        if(!firstDownload) //判断是否为第一个下载的软件包，第一次下载时不需要从容器中移除软件包，下载完成后才需要移除。
        {
            priorSize += urlmsg.size;
            downloadPackages.append(path); //下载完成后将软件包路径存放到list中，发送到workthread线程，再遍历拷贝到archives目录下
            appAllMsg.msg.depList.erase(appAllMsg.msg.depList.begin()); //下载完一个包从列表中移除
            if(appAllMsg.msg.depList.length() != 0)
            {
                UrlMsg urlmsg = appAllMsg.msg.depList.at(0);
                QString path = QString("%1%2").arg(downloadPath).arg(urlmsg.fullname);
                if(!isCancel)
                    curlDownload(urlmsg,path);
            }
            else //下载完成调用dbus接口拷贝文件到/var/cache/apt/archives目录下
            {
                downloadFinish = true;
                m_updateMutual->copyFileToInstall(downloadPackages, appAllMsg.name);
                appVersion->setText("准备安装");
                qDebug() << "download over11";
            }
        }
        else
        {
            qDebug() << "first download";
            firstDownload = false;
            if(!isCancel)
                curlDownload(urlmsg,path);
        }
    }
    else //下载完成调用dbus接口拷贝文件到/var/cache/apt/archives目录下
    {
        if(firstDownload)
        {
            startInstall(appAllMsg.name);
        }
        else
        {
            downloadFinish = true;
            m_updateMutual->copyFileToInstall(downloadPackages, appAllMsg.name);
            appVersion->setText("准备安装");
            qDebug() << "download over22";
        }
    }
}


//使用libqapt的库获取到当前软件包的所有信息
void AppUpdateWid::getAppMessage(QString appName)
{
    m_backend = new QApt::Backend();
    m_backend->init();
    m_package = m_backend->package(appName);
    if (!m_package == 0) {
        appAllMsg.name    = m_package->name();
        appAllMsg.section = m_package->section();
        appAllMsg.origin  = m_package->origin();
        appAllMsg.installedSize = m_package->availableInstalledSize();
        appAllMsg.maintainer = m_package->maintainer();
        appAllMsg.source  = m_package->sourcePackage();
        appAllMsg.version = m_package->version();
        appAllMsg.packageSize = m_package->downloadSize();
        appAllMsg.shortDescription = m_package->shortDescription();
        appAllMsg.longDescription  = m_package->longDescription();

        appAllMsg.changelogUrl = m_package->changelogUrl().toString();
        appAllMsg.screenshotUrl = m_package->screenshotUrl(QApt::Thumbnail).toString();

        appAllMsg.availableVersion = m_package->availableVersion();
    }

    // 判断是否已经安装
    if (m_package->isInstalled()) {
        appAllMsg.isInstalled = true;
    }

    // 判断是否可升级
    if (m_package->state() & QApt::Package::Upgradeable) {
        appAllMsg.upgradeable = true;
    }
}

//开启wget断点续传
void AppUpdateWid::curlDownload(UrlMsg msg, QString path)
{
    QStringList args;
    args.append("-c");
    args.append("-P");
    args.append(QString("%1").arg(DOWN_CACHE_PATH));
    args.append(QString("%1").arg(msg.url.remove("'")));
    args.append("-O");
    args.append(QString("%1").arg(path));
    args.append("-T");
    args.append("10");
    currentPackage = msg.fullname;
    qDebug() << "currentPackage" << currentPackage << "size:" << msg.size;
    downloadProcess->start("/usr/bin/wget", args);
}

//接收后台dbus信号开始软件包
void AppUpdateWid::startInstall(QString appName)
{
    if(appName == appAllMsg.name)
    {
//        m_updateMutual->init_cache();
        updateAPPBtn->hide();
        m_updateMutual->installAndUpgrade(appAllMsg.name);
        workProcess = new QProcess();
//        workProcess->start(QString("rm -r %1").arg(downloadPath)); //删除创建的目录保证缓存环境的干净
    }
}

//修改界面状态值
void AppUpdateWid::showInstallStatues(QString status,QString appAptName, float progress)
{
    if(QString::compare(appAllMsg.name,appAptName) == 0)
    {
        int pgs = progress;
        appVersion->setText("正在安装..."+QString::number(pgs)+"% ");
//        progressLab->show();
//        progressLab->setText("进度:"+QString::number(pgs)+"% ");

        updateAPPBtn->hide();

        if(status == "apt_finish")
        {

//            progressLab->hide();
            updateAPPBtn->hide();
            appVersion->setText("安装完成");
            m_updateMutual->importantList.removeOne(appAllMsg.name);
            emit hideUpdateBtnSignal();
//            aptFinishQuery();
            emit writeSqliteSignal(appAllMsg,chlog);
        }
    }
}

void AppUpdateWid::updateAppUi(QString name)
{
    iconNameLayout = new QHBoxLayout();
    smallHLayout = new QHBoxLayout();
    largeVLayout = new QVBoxLayout();
    appTitleWid = new QWidget();
    largeWidget = new QWidget();
    mainVLayout = new QVBoxLayout();
    QHBoxLayout *frameLayout = new QHBoxLayout();
    AppFrame = new QFrame(this);  //最外层窗口适配主题样式
    AppFrame->setFrameShape(QFrame::Box);

    frameLayout->addWidget(AppFrame);
    frameLayout->setContentsMargins(0,0,0,0);   //更新信息之间的间距
    frameLayout->setSpacing(0);
    this->setLayout(frameLayout);

    appIconName = new QLabel(this);
    appIconName->setMinimumWidth(140);
    appIconName->setMaximumWidth(800);
    appIcon = new QLabel(appIconName);
    appNameLab = new QLabel(appIconName);
    appNameLab->setMinimumWidth(126);
    appIconName->setLayout(iconNameLayout);
    iconNameLayout->addWidget(appIcon);
//    iconNameLayout->addStretch();
    iconNameLayout->setSpacing(0);
    iconNameLayout->addSpacing(8);
    iconNameLayout->addWidget(appNameLab);
    iconNameLayout->addStretch();


    appVersion = new QLabel(this);
    progressLab = new QLabel(this);
    progressLab->setFixedWidth(40);

    appIcon->setFixedSize(32,32);
    appVersion->setText("最新：1.0.3");
    progressLab->setText("进度：");
    progressLab->hide();

    detaileInfo = new QPushButton(this);
    detaileInfo->setText("详情");
    detaileInfo->setFixedSize(40,25);
    detaileInfo->setFlat(true);

    updateAPPBtn = new QPushButton(this);
    updateAPPBtn->setText("更新");

    otherBtnLayout = new QHBoxLayout();  //版本号、详情、更细按钮布局
    otherBtnLayout->setSpacing(0);
    otherBtnLayout->setMargin(0);
    otherBtnLab = new QLabel(this);
    otherBtnLab->setMargin(0);
//    otherBtnLab->setStyleSheet("border:1px solid red");
    otherBtnLab->setMaximumWidth(400);
    otherBtnLab->setFixedHeight(60);
    appVersion->setMaximumWidth(200);
    otherBtnLayout->addWidget(appVersion,2);
    otherBtnLayout->addWidget(detaileInfo,1);
    otherBtnLayout->addWidget(updateAPPBtn,1);

    otherBtnLab->setLayout(otherBtnLayout);
//    appIconName->setStyleSheet("border:1px solid yellow");
    smallHLayout->addWidget(appIconName,4);
    smallHLayout->addStretch(0);
    smallHLayout->addWidget(otherBtnLab,6);
    smallHLayout->setSpacing(0);
    smallHLayout->setContentsMargins(0,0,10,0);
    appTitleWid->setLayout(smallHLayout);

    mainVLayout->addWidget(appTitleWid);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(0);
    someInfoEdit = new QTextEdit(this);   //部分changelog  缩略显示
    someInfoEdit->setReadOnly(true);
    QPalette pl = someInfoEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    someInfoEdit->setPalette(pl);
    updatelogBtn = new QPushButton(this);
    updatelogBtn->setText("更新日志");
    updatelogBtn->setFixedSize(70,25);
    updatelogBtn->setFlat(true);
    updatelog1 = new UpdateLog();
    largeVLayout->addWidget(someInfoEdit);
    largeVLayout->addWidget(updatelogBtn,0,Qt::AlignLeft);
    largeVLayout->setSpacing(0);
    largeVLayout->setContentsMargins(50,0,120,0);
    largeWidget->setLayout(largeVLayout);
//    largeWidget->setFixedHeight(80);
    mainVLayout->addWidget(largeWidget);
    largeWidget->hide();
//    this->setLayout(mainVLayout);
    AppFrame->setLayout(mainVLayout);
    appNameLab->setText(name);
    appIcon->setPixmap(QPixmap(":/img/plugins/upgrade/refresh.png").scaled(QSize(32,32)));  //更新信息显示默认图标
    if(QIcon::fromTheme(name).hasThemeIcon(name))    //判断是否有主题图标并输出
    {
        QIcon icon = QIcon::fromTheme(name);
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        appIcon->setPixmap(pixmap);
    }


    appVersion->setText("最新："+appAllMsg.availableVersion);
    //获取并输出changelog
    QStringList changelog = m_updateMutual->getChangeLog(name);
    chlog = changelog.join("");
    updatelog1->logContent->append(chlog);
    QTextCursor tmpCursor = updatelog1->logContent->textCursor();
    tmpCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    updatelog1->logContent->setTextCursor(tmpCursor);
    updatelog1->logAppName->setText(name+"更新日志");
    updatelog1->logAppVerson->setText("最新:"+appAllMsg.availableVersion);
    someInfoEdit->append(chlog.left(150));
//    someInfoEdit->append();
    updatelog1->logAppIcon->setPixmap(QPixmap(":/img/plugins/upgrade/refresh.png").scaled(QSize(32,32)));  //changelog页面显示默认 图标
    if(QIcon::fromTheme(name).hasThemeIcon(name))
    {
        QIcon icon = QIcon::fromTheme(name);
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        updatelog1->logAppIcon->setPixmap(pixmap);
    }
    //获取并输出包大小、当前版本号
    QString currentVersion = appAllMsg.version;
    someInfoEdit->append("版本:  "+currentVersion);
}


void AppUpdateWid::showDetails()
{
    if(largeWidget->isHidden())
    {
        largeWidget->show();
//        this->setFixedHeight(largeWidget->height()+10);
//        this->resize(600,160);
        detaileInfo->setText("收起");
    }
    else
    {
        largeWidget->hide();
//        this->resize(600,80);
        detaileInfo->setText("详情");
    }
}

void AppUpdateWid::showUpdateLog()
{
    updatelog1->show();
}

void AppUpdateWid::cancelOrUpdate()
{
    if(updateAPPBtn->text() == "更新")
    {
        isCancel = false;
        firstDownload = true;
        slotDownloadPackages();
        timer->start(500); //开启定时器用于计算下载速度
        updateAPPBtn->setText("取消");
    }
    else
    {
        isCancel = true;
        downloadProcess->terminate();
        timer->stop();
        updateAPPBtn->setText("更新");
        emit changeUpdateAllSignal();
    }
}

//转换包大小的单位
QString AppUpdateWid::modifySizeUnit(int size)
{
    if((size/1024) >= 1)
    {
        if((size/1024/1024) >=1 )
        {
            if((size/1024/1024/1024) >=1)
                return QString("%1%2").arg(size/1024/1024/1024).arg("GB");
            else
                return QString("%1%2").arg(size/1024/1024).arg("MB");
        }
        else
            return QString("%1%2").arg(size/1024).arg("KB");
    }
    else
        return QString("%1%2").arg(size).arg("B");
}

//当前下载速度单位换算
QString AppUpdateWid::modifySpeedUnit(int size, float time)
{
    size = (int)(size/time);
    if((size/1024) >= 1)
    {
        if((size/1024/1024) >= 1)
        {
            if((size/1024/1024/1024) >= 1)
                return QString("%1%2").arg(size/1024/1024/1024).arg("GB/S");
            else
                return QString("%1%2").arg(size/1024/1024).arg("MB/S");
        }
        else
            return QString("%1%2").arg(size/1024).arg("KB/S");
    }
    else
        return QString("%1%2").arg(size).arg("B/S");
}

void AppUpdateWid::showDownloadStatues(QString downloadSpeed, int progress)
{

        appVersion->setText("更新中("+downloadSpeed+")"+QString::number(progress)+"%");
//        progressLab->show();
//        progressLab->setText(QString::number(progress)+"%");

}

void AppUpdateWid::aptFinishQuery()
{


}

//定时器触发计算当前下载速度和下载进度
void AppUpdateWid::calculateSpeedProgress()
{
    QFile tmpFile(QString("%1%2").arg(downloadPath).arg(currentPackage));
    if(tmpFile.exists())
    {
        if(!downloadFinish)
            downSize = priorSize + tmpFile.size();
        else
            downSize = priorSize;
        QString speed = modifySpeedUnit(downSize-preDownSize, 0.5);
        int progress = (int)((downSize*100/appAllMsg.msg.allSize));
        if(priorSize == appAllMsg.msg.allSize) //确保完全下载完成后再停止定时器
            timer->stop();
        qDebug() << "priorsize:" << priorSize << "predownsize" << preDownSize << "tmpsize" << tmpFile.size() << "downsize" << downSize << "allsize" << appAllMsg.msg.allSize << "progress:" << progress << "speed" << speed << "name" << currentPackage;
        preDownSize = downSize;
        showDownloadStatues(speed,progress);
    }
}

void AppUpdateWid::updateAllApp()
{

    qDebug() << "updateall";
    if(isCancel)
    {
        cancelOrUpdate();
    }
//    m_updateMutual->startTray();
}
