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
    qDBusRegisterMetaType<CustomData>();
    // 界面获取焦点
//    setFocus();

//    CreatConnection();
    if(!CreatConnection())
    {
        qDebug() << "datebase can not open.";
    }

    // 用户手册功能
    mDaemonIpcDbus = new DaemonIpcDbus();

    // 初始化组件
    setWidgetUi();

    // 初始化样式
    setWidgetStyle();


    //初始化DBus
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    connect(timer,&QTimer::timeout,myTabwidget,&TabWid::initDbus);
    timer->start(1);
}

UpgradeMain::~UpgradeMain()
{
//    qDebug() << "quit mainwindow";
}

// 初始化组件
void UpgradeMain::setWidgetUi()
{
    // 整体界面widget
    mainWid = new QWidget(this);

    // 整体界面布局
    mainLayout = new QVBoxLayout();

    // 设置整体界面布局
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    myTabwidget = new TabWid(this);
    mainLayout->addWidget(myTabwidget);

    this->mainWid->setLayout(mainLayout);
    // 将mainWid作为整体界面
    this->setCentralWidget(mainWid);
}

// 初始化样式
void UpgradeMain::setWidgetStyle()
{
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

}


