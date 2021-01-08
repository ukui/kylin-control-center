#include "appupdate.h"

AppUpdateWid::AppUpdateWid(AppAllMsg msg,QWidget *parent):QWidget(parent)
{
    m_updateMutual = UpdateDbus::getInstance();
    qRegisterMetaType<AppMsg>("AppMsg"); //注册信号槽类型
    qRegisterMetaType<AppAllMsg>("AppAllMsg"); //注册信号槽类型
    appAllMsg = msg;
    timer = new QTimer();
    downloadPath = QString("%1%2/").arg(DOWN_CACHE_PATH).arg(appAllMsg.name);
    downloadProcess = new QProcess();
    updateAppUi(appAllMsg.name);
    this->setObjectName("AppUpdateWid");
    initConnect();
}

void AppUpdateWid::initConnect()
{
    connect(detaileInfo,&QPushButton::clicked,this,&AppUpdateWid::showDetails);
    connect(updatelogBtn,&QPushButton::clicked,this,&AppUpdateWid::showUpdateLog);
    connect(updateAPPBtn,&QPushButton::clicked,this,&AppUpdateWid::cancelOrUpdate);
    connect(m_updateMutual,&UpdateDbus::transferAptProgress,this,&AppUpdateWid::showInstallStatues);
    //绑定wget进程结束的信号getAppMessage
    connect(downloadProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                         [=] (int exitCode, QProcess::ExitStatus exitStatus) {
        if(!isCancel) //当包存在依赖时，需要显示包的所以依赖以及自身包的总大小
        {
            if(exitCode == 0) //正常下载结束
            {
                connectTimes = 0;
                slotDownloadPackages();
            }
            else //网络或者服务器异常
            {
                if(connectTimes < 20) //判断重连次数，重连20次后退出下载
                {
                    connectTimes += 1;
                    curlDownload(urlmsg,path);
                    qDebug()  << "exitcode:" << exitCode << exitStatus << "connectTimes:" << connectTimes;
                }
                else
                {
                    isCancel = true;
    //                downloadProcess->terminate();
                    timer->stop();
                    updateAPPBtn->setText("更新");
                    emit downloadFailedSignal();
                    emit changeUpdateAllSignal();
                }
            }
        }
//            qDebug()  << "exitcode:" << exitCode << exitStatus;
        });
    connect(m_updateMutual, &UpdateDbus::copyFinish, this, &AppUpdateWid::startInstall);
    connect(timer, &QTimer::timeout, this, &AppUpdateWid::calculateSpeedProgress);
}

//从列表中取出下载信息开始下载
void AppUpdateWid::slotDownloadPackages()
{
    qDebug() << "list:" << appAllMsg.msg.depList.length();
    if(appAllMsg.msg.depList.length() != 0)
    {
        QDir dir = downloadPath;
        if(!dir.exists())
        {
            qDebug() << "not exists:" <<QString("%1%2").arg(DOWN_CACHE_PATH).arg(appAllMsg.name);
            dir.mkdir(QString("%1%2").arg(DOWN_CACHE_PATH).arg(appAllMsg.name));
        }
        urlmsg = appAllMsg.msg.depList.at(0);
        path = QString("%1%2").arg(downloadPath).arg(urlmsg.fullname);
        if(!firstDownload) //判断是否为第一个下载的软件包，第一次下载时不需要从容器中移除软件包，下载完成后才需要移除。
        {
            priorSize += urlmsg.size;
            downloadPackages.append(path); //下载完成后将软件包路径存放到list中，发送到workthread线程，再遍历拷贝到archives目录下
            appAllMsg.msg.depList.erase(appAllMsg.msg.depList.begin()); //下载完一个包从列表中移除
            if(appAllMsg.msg.depList.length() != 0)
            {
                urlmsg = appAllMsg.msg.depList.at(0);
                path = QString("%1%2").arg(downloadPath).arg(urlmsg.fullname);
                if(!isCancel)
                    curlDownload(urlmsg,path);
            }
            else //下载完成调用dbus接口拷贝文件到/var/cache/apt/archives目录下
            {
                downloadFinish = true;
                m_updateMutual->copyFinsh(downloadPackages, appAllMsg.name);
                if(m_updateMutual->fileLock() != false)
                {
                    emit filelockedSignal();
                }
                appVersion->setText("准备安装");
                appVersion->setToolTip("准备安装");

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
            qDebug() << "startinstall";
            startInstall(appAllMsg.name);
        }
        else
        {
            downloadFinish = true;
            m_updateMutual->copyFinsh(downloadPackages, appAllMsg.name);
            if(m_updateMutual->fileLock() != false)
            {
                emit filelockedSignal();
            }
            appVersion->setText("准备安装");
            appVersion->setToolTip("准备安装");
            qDebug() << "download over22";
        }
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
    qDebug() << "appname:" << appName;
    if(appName == appAllMsg.name)
    {
//        m_updateMutual->init_cache();
        updateAPPBtn->hide();
        m_updateMutual->installAndUpgrade(appAllMsg.name);
        workProcess = new QProcess();
        workProcess->start(QString("rm -r %1").arg(downloadPath)); //删除创建的目录保证缓存环境的干净

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        m_updateMutual->insertInstallStates("update_time",current_date);
    }
}

//修改界面状态值
void AppUpdateWid::showInstallStatues(QString status,QString appAptName, float progress)
{
    if(QString::compare(appAllMsg.name,appAptName) == 0)
    {
        int pgs = progress;
        appVersion->setText("正在安装("+QString::number(pgs)+"%)");
        appVersion->setToolTip("正在安装("+QString::number(pgs)+"%)");
        updateAPPBtn->hide();

        if(status == "apt_finish")
        {
            updateAPPBtn->hide();
            appVersion->setText(tr("更新成功！"));
            appVersion->setToolTip(tr("更新成功！"));
            m_updateMutual->importantList.removeOne(appAllMsg.name);
            QString message = QString("%1更新成功！").arg(appNameLab->text());
            m_updateMutual->onRequestSendDesktopNotify(message);
            emit hideUpdateBtnSignal();
            emit writeSqliteSignal(appAllMsg,chlog);

        }
        else if(status == "apt_error")
        {
            appVersion->setText(tr("更新失败！"));
            appVersion->setToolTip(tr("更新失败！"));

            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::red);
            appVersion->setPalette(pe);
            QString message = QString("%1更新失败！").arg(appNameLab->text());
            m_updateMutual->onRequestSendDesktopNotify(message);
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
    detaileInfo->setFixedSize(40,30);
    detaileInfo->setFlat(true);

    updateAPPBtn = new QPushButton(this);
    updateAPPBtn->setText("更新");

    otherBtnLayout = new QHBoxLayout();  //版本号、详情、更细按钮布局
    otherBtnLayout->setSpacing(0);
    otherBtnLayout->setMargin(0);
    otherBtnLab = new QLabel(this);
//    otherBtnLab->setStyleSheet("background:blue;");
    otherBtnLab->setMargin(0);
//    otherBtnLab->setStyleSheet("border:1px solid red");
    otherBtnLab->setFixedWidth(350);
    otherBtnLab->setFixedHeight(60);
    appVersion->setFixedWidth(160);
//    appVersion->setStyleSheet("background:green;");

    detaileInfo->setFixedWidth(60);
//    detaileInfo->setStyleSheet("background:green;");
    otherBtnLayout->addWidget(appVersion,0,Qt::AlignLeft);
    otherBtnLayout->addWidget(detaileInfo,1,Qt::AlignLeft);
    otherBtnLayout->addStretch(0);
    otherBtnLayout->addWidget(updateAPPBtn,1,Qt::AlignRight);
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
    largeVLayout->setSpacing(5);
    largeVLayout->setContentsMargins(50,0,120,5);
    largeWidget->setLayout(largeVLayout);
//    largeWidget->setFixedHeight(80);
    mainVLayout->addWidget(largeWidget);
    largeWidget->hide();
//    this->setLayout(mainVLayout);
    AppFrame->setLayout(mainVLayout);
    appNameLab->setText(name);
    appNameLab->setToolTip(name);
    if(QIcon::fromTheme(name).hasThemeIcon(name))    //判断是否有主题图标并输出
    {
        QIcon icon = QIcon::fromTheme(name);
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        appIcon->setPixmap(pixmap);
    }
    else
    {
        QIcon icon = QIcon::fromTheme("application-x-desktop");
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        appIcon->setPixmap(pixmap);
    }


    appVersion->setText(tr("最新：")+appAllMsg.availableVersion);
    appVersion->setToolTip(tr("最新：")+appAllMsg.availableVersion);
    //获取并输出changelog
    chlog = appAllMsg.longDescription;
    updatelog1->logContent->append(chlog);

    QTextCursor tmpCursor = updatelog1->logContent->textCursor();
    tmpCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    updatelog1->logContent->setTextCursor(tmpCursor);

    updatelog1->logAppName->setText(name+tr("更新日志"));
    updatelog1->logAppVerson->setText("最新:"+appAllMsg.availableVersion);
    someInfoEdit->append(chlog.left(150));
    if(appAllMsg.msg.allSize == 0)
    {
        someInfoEdit->append("下载大小："+QString(modifySizeUnit(appAllMsg.packageSize)));
    }
    else
    {
        someInfoEdit->append("下载大小："+QString(modifySizeUnit(appAllMsg.msg.allSize)));
    }

    if(QIcon::fromTheme(name).hasThemeIcon(name))
    {
        QIcon icon = QIcon::fromTheme(name);
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        updatelog1->logAppIcon->setPixmap(pixmap);
    }
    else
    {
        QIcon icon = QIcon::fromTheme("application-x-desktop");
        QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
        updatelog1->logAppIcon->setPixmap(pixmap);
    }
    //获取并输出包大小、当前版本号
    QString currentVersion = appAllMsg.version;
    if(currentVersion != "")
    {
        someInfoEdit->append("当前版本:  "+currentVersion);
    }
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
    updatelog1->exec();
}

void AppUpdateWid::cancelOrUpdate()
{
    if(updateAPPBtn->text() == "更新")
    {
        isCancel = false;
        firstDownload = true;
        slotDownloadPackages();
        timer->start(500); //开启定时器用于计算下载速度
        qDebug() << "time start";
        updateAPPBtn->setText("取消");
    }
    else
    {
        isCancel = true;
        downloadProcess->terminate();
        timer->stop();
        updateAPPBtn->setText("更新");
        appVersion->setText("暂停中");
        emit changeUpdateAllSignal();
    }
}

//转换包大小的单位
QString AppUpdateWid::modifySizeUnit(int size)
{
    double tmpSize = size/1024.0;
    if(tmpSize >= 1)
    {
        if((tmpSize/1024.0) >= 1)
        {
            if((tmpSize/1024.0/1024.0) >= 1)
                return QString("%1%2").arg((int)(tmpSize/1024.0/1024.0*100 + 0.5)/100.0).arg("GB");//保留小数点后两位，并四舍五入
            else
                return QString("%1%2").arg((int)(tmpSize/1024.0*100 + 0.5)/100.0).arg("MB");//保留小数点后两位，并四舍五入
        }
        else
            return QString("%1%2").arg((int)(tmpSize*100 + 0.5)/100.0).arg("KB");//保留小数点后两位，并四舍五入
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
        appVersion->setToolTip("更新中("+downloadSpeed+")"+QString::number(progress)+"%");
//        progressLab->show();
//        progressLab->setText(QString::number(progress)+"%");

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
