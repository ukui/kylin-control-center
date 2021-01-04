/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "upgrademain.h"
#include "metatypes.h"
//#include "connection.h"

UpgradeMain::UpgradeMain(QString arg,QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "new";
    qDBusRegisterMetaType<CustomData>();
    // 界面获取焦点
//    setFocus();

//    CreatConnection();
    if(!CreatConnection())
    {
        qDebug() << "out";
    }

    // 用户手册功能
    mDaemonIpcDbus = new DaemonIpcDbus();

    // 初始化组件
    setWidgetUi();

    // 初始化样式
    setWidgetStyle();


//    qDebug()<<QDir::homePath();

//    QString fromDir = "/var/lib/kylin-software-properties/kylin-update-timer/";
//    QString toDir = QDir::homePath();
//    toDir += "/.config/kylin-update-manager/";
//    copyFolder(fromDir,toDir,true);


}

UpgradeMain::~UpgradeMain()
{
//    qDebug() << "quit mainwindow";
}

// 初始化组件
void UpgradeMain::setWidgetUi()
{
    // 窗口设置
    // 无边框、界面置顶
//     this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // 窗体透明
//    this->setAttribute(Qt::WA_TranslucentBackground, true);


    // 设置图标
    // this->setWindowTitle("麒麟计算器");
    // this->setWindowIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/64x64/apps/calc.png"));

    // 整体界面widget
    mainWid = new QWidget(this);

    // 整体界面布局
    mainLayout = new QVBoxLayout();

    // 设置整体界面布局
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    myTabwidget = new TabWid(this);
//    myTabwidget->resize(620,580);

    mainLayout->addWidget(myTabwidget);


    this->mainWid->setLayout(mainLayout);

    // 将mainWid作为整体界面
    this->setCentralWidget(mainWid);
    this->show();
}

// 初始化样式
void UpgradeMain::setWidgetStyle()
{
//    // 设置窗体大小
//    this->resize(WINDOWW, WINDOWH);

//    //在屏幕中央显示
//    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
//    this->move((availableGeometry.width() - this->width())/2, (availableGeometry.height() - this->height())/2);

    // 读取主题颜色配置文件 监听主题颜色改变
    if(QGSettings::isSchemaInstalled(FITTHEMEWINDOW))
    {
        gSettings = new QGSettings(FITTHEMEWINDOW);
        if(gSettings->get("style-name").toString() == "ukui-dark" || gSettings->get("style-name").toString() == "ukui-black"){
            WidgetStyle::themeColor = 1;
        }
        else
        {
            WidgetStyle::themeColor = 0;
        }

        connect(gSettings,&QGSettings::changed,this,[=]()
        {
            qDebug() << "主题颜色" << gSettings->get("style-name").toString();
            if(gSettings->get("style-name").toString() == "ukui-dark" || gSettings->get("style-name").toString() == "ukui-black"){
                WidgetStyle::themeColor = 1;
                changeDarkTheme();
            }
            else
            {
                WidgetStyle::themeColor = 0;
                changeLightTheme();
            }

        });
    }

    this->mainWid->setObjectName("mainWid");
}

// 切换深色模式
void UpgradeMain::changeDarkTheme()
{
}

// 切换浅色模式
void UpgradeMain::changeLightTheme()
{
}

// 实现键盘响应
void UpgradeMain::keyPressEvent(QKeyEvent *event)
{
    // F1快捷键打开用户手册
    if (event->key() == Qt::Key_F1) {
        if (!mDaemonIpcDbus->daemonIsNotRunning()){
            // F1快捷键打开用户手册，如kylin-update-manager
            mDaemonIpcDbus->showGuide("kylin-update-manager");
        }
    }
}

bool UpgradeMain::copyFolder(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyFolder(fileInfo.filePath(),
                targetDir.filePath("kylin-update-manager.conf"),
                coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists("kylin-update-manager.conf")){
                targetDir.remove("kylin-update-manager.conf");
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath("kylin-update-manager.conf"))){
                    return false;
            }
            qDebug()<<fileInfo.filePath();
            qDebug()<<targetDir.filePath("kylin-update-manager.conf");
        }
    }
    return true;

}
void UpgradeMain::readConf()
{
    /*
    QString toDir = QDir::homePath();
    toDir += "/.config/kylin-update-manager/kylin-update-manager.conf";
    mqsetting = new QSettings(toDir,QSettings::IniFormat);
    mqsetting->setIniCodec(QTextCodec::codecForName("UTF-8")); //在此添加设置，即可读写conf文件中的中文


    //开始组Generic
    mqsetting->beginGroup(QString::fromLocal8Bit("Generic"));
    //读取
//    qDebug()<<mqsetting->value("service").toString();
//    qDebug()<<mqsetting->value("prefix").toString();
//    qDebug()<<mqsetting->value("enable").toString();
    //写入
//    mqsetting->setValue("service","qwrewqr");
    //结束组
    mqsetting->endGroup();



    //开始组T1
    mqsetting->beginGroup(QString::fromLocal8Bit("T1"));
    //读取
//    qDebug()<<mqsetting->value("offsettime").toString();
    //写入
//    mqsetting->setValue("offsettime","qwrewqr");
    //结束组
    mqsetting->endGroup();

    */
}

