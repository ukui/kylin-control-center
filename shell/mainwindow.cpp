/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPluginLoader>

#include "utils/keyvalueconverter.h"
#include "utils/functionselect.h"

#include <QDebug>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置初始大小
    resize(QSize(820, 600));

    //设置窗体无边框
    setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
    //该设置去掉了窗体透明后的黑色背景
    setAttribute(Qt::WA_TranslucentBackground, true);
    //将最外层窗体设置为透明
    setStyleSheet("QMainWindow#MainWindow{background-color: transparent;}");

    //中部内容区域
    ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: #ffffff; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
    //标题栏widget
    ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background: #ffffff; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    //左上角文字
//    ui->mainLabel->setStyleSheet("QLabel#mainLabel{font-size: 18px; color: #40000000;}");
    //左上角返回按钮
    ui->backBtn->setStyleSheet("QPushButton#backBtn{background: #ffffff; border: none;}");
    //顶部搜索框
    ui->searchLineEdit->setStyleSheet("QLineEdit#searchLineEdit{background: #FFEDEDED; border: none; border-radius: 6px;}");
    //右上角按钮stylesheet
    ui->minBtn->setStyleSheet("QPushButton#minBtn{background: #ffffff; border: none;}"
                              "QPushButton:hover:!pressed#minBtn{background: #FF3D6BE5; border-radius: 2px;}"
                              "QPushButton:hover:pressed#minBtn{background: #415FC4; border-radius: 2px;}");
    ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background: #ffffff; border: none;}"
                              "QPushButton:hover:!pressed#maxBtn{background: #FF3D6BE5; border-radius: 2px;}"
                              "QPushButton:hover:pressed#maxBtn{background: #415FC4; border-radius: 2px;}");
    ui->closeBtn->setStyleSheet("QPushButton#closeBtn{background: #ffffff; border: none;}"
                                "QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 2px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 2px;}");

    //左侧一级菜单
    ui->leftsidebarWidget->setStyleSheet("QWidget#leftsidebarWidget{background: #cccccc; border: none; border-top-left-radius: 6px; border-bottom-left-radius: 6px;}");

    //设置左上角按钮图标
    ui->backBtn->setIcon(QIcon("://img/titlebar/back.png"));

    //设置右上角按钮图标
    ui->minBtn->setIcon(QIcon(":/img/titlebar/min.png"));
    ui->maxBtn->setIcon(QIcon("://img/titlebar/max.png"));
    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.png"));

    //

    //初始化功能列表数据
    FunctionSelect::initValue();

    //构建枚举键值转换对象
    kvConverter = new KeyValueConverter(); //继承QObject，No Delete

    //加载插件
    loadPlugins();

    connect(ui->minBtn, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui->maxBtn, &QPushButton::clicked, this, [=]{
        if (isMaximized()){
            showNormal();
            ui->maxBtn->setIcon(QIcon("://img/titlebar/max.png"));
        } else {
            showMaximized();
            ui->maxBtn->setIcon(QIcon("://img/titlebar/revert.png"));
        }
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, [=]{
        close();
//        qApp->quit();
    });
//    connect(ui->backBtn, &QPushButton::clicked, this, [=]{
//        if (ui->stackedWidget->currentIndex())
//            ui->stackedWidget->setCurrentIndex(0);
//        else
//            ui->stackedWidget->setCurrentIndex(1);
//    });


//    ui->leftsidebarWidget->setVisible(ui->stackedWidget->currentIndex());
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [=](int index){
        //左侧边栏显示/不显示
        ui->leftsidebarWidget->setVisible(index);
        //左上角显示字符/返回按钮
        ui->backBtn->setVisible(index);
        ui->titleLabel->setHidden(index);

        if (index){ //首页部分组件样式
            //中部内容区域
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: #ffffff; border-bottom-right-radius: 6px;}");
            //标题栏widget
            ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background: #ffffff; border-top-right-radius: 6px;}");
        } else { //次页部分组件样式
            //中部内容区域
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: #ffffff; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
            //标题栏widget
            ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background: #ffffff; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
        }
    });

    //加载左侧边栏一级菜单
    initLeftsideBar();

    //加载首页Widget
    homepageWidget = new HomePageWidget(this);
    ui->stackedWidget->addWidget(homepageWidget);

    //加载功能页Widget
    modulepageWidget = new ModulePageWidget(this);
    ui->stackedWidget->addWidget(modulepageWidget);

    //top left return button
    connect(ui->backBtn, &QPushButton::clicked, this, [=]{
        if(ui->stackedWidget != nullptr)
            ui->stackedWidget->setCurrentIndex(0);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBtnLayout(QPushButton * &pBtn){
    QLabel * imgLabel = new QLabel(pBtn);
    QSizePolicy imgLabelPolicy = imgLabel->sizePolicy();
    imgLabelPolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    imgLabelPolicy.setVerticalPolicy(QSizePolicy::Fixed);
    imgLabel->setSizePolicy(imgLabelPolicy);
    imgLabel->setScaledContents(true);

    QVBoxLayout * baseVerLayout = new QVBoxLayout(pBtn);

    QHBoxLayout * contentHorLayout = new QHBoxLayout();
    contentHorLayout->addStretch();
    contentHorLayout->addWidget(imgLabel);
    contentHorLayout->addStretch();

    baseVerLayout->addStretch();
    baseVerLayout->addLayout(contentHorLayout);
    baseVerLayout->addStretch();

    pBtn->setLayout(baseVerLayout);
}

void MainWindow::loadPlugins(){
    for (int index = 0; index < FUNCTOTALNUM; index++){
        QMap<QString, QObject *> pluginsMaps;
        modulesList.append(pluginsMaps);
    }

    static bool installed = (QCoreApplication::applicationDirPath() == QDir(("/usr/bin")).canonicalPath());

    if (installed)
        pluginsDir = QDir("/usr/lib/control-center/pluginlibs/");
    else {
        pluginsDir = QDir(qApp->applicationDirPath() + "/pluginlibs/");
    }


    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        qDebug() << "Scan Plugin: " << fileName;
        //ukui-indicators(org.ukui.panel.indicators.gschema.xml)
        char * indicatorFile = "/usr/share/glib-2.0/schemas/org.ukui.panel.indicators.gschema.xml";
        //gsettings-desktop-schemas
        char * proxyFile = "/usr/share/glib-2.0/schemas/org.gnome.system.proxy.gschema.xml";
        char * gnomedesktopFile = "/usr/share/glib-2.0/schemas/org.gnome.desktop.wm.preferences.gschema.xml";
        //mate-desktop-common
        char * interfaceFile = "/usr/share/glib-2.0/schemas/org.mate.interface.gschema.xml";
        char * bgFile = "/usr/share/glib-2.0/schemas/org.mate.background.gschema.xml";
        //peony-common
        char * peonyFile = "/usr/share/glib-2.0/schemas/org.ukui.peony.gschema.xml";
        //libmatekbd-common
        char * kbdFile = "/usr/share/glib-2.0/schemas/org.mate.peripherals-keyboard-xkb.gschema.xml";
        //ukui-power-manager-common
        char * powerFile = "/usr/share/glib-2.0/schemas/org.ukui.power-manager.gschema.xml";
        //ukui-session-manager
        char * sessionFile = "/usr/share/glib-2.0/schemas/org.ukui.session.gschema.xml";
        //ukui-screensaver
        char * screensaverFile = "/usr/share/glib-2.0/schemas/org.ukui.screensaver.gschema.xml";
        //ukui-settings-daemon-common
        char * usdFile = "/usr/share/glib-2.0/schemas/org.ukui.font-rendering.gschema.xml";

        //时间和日期功能依赖ukui-indicators
        if (!g_file_test(indicatorFile, G_FILE_TEST_EXISTS) && fileName == "libdatetime.so")
            continue;
        //代理功能依赖gsettings-desktop-schemas
        if (!g_file_test(proxyFile, G_FILE_TEST_EXISTS) && fileName == "libproxy.so")
            continue;
        //字体功能依赖gsettings-desktop-schemas,mate-desktop-common,peony-common,ukui-settings-daemon-common
        if ((!g_file_test(interfaceFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(gnomedesktopFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(peonyFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(usdFile, G_FILE_TEST_EXISTS)) && fileName == "libfonts.so")
            continue;
        //键盘功能的键盘布局依赖libmatekbd-common
        if (!g_file_test(kbdFile, G_FILE_TEST_EXISTS) && fileName == "libkeyboard.so")
            continue;
        //电源功能依赖ukui-power-manager-common
        if (!g_file_test(powerFile, G_FILE_TEST_EXISTS) && fileName == "libpower.so")
            continue;
        //屏保功能依赖ukui-session-manager
        if ((!g_file_test(screensaverFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(sessionFile, G_FILE_TEST_EXISTS)) &&
                fileName == "libscreensaver.so")
            continue;
        //桌面功能依赖peony-common
        if (!g_file_test(peonyFile, G_FILE_TEST_EXISTS) && fileName == "libdesktop.so")
            continue;
        //wallpaper mate-desktop-common
        if (!g_file_test(bgFile, G_FILE_TEST_EXISTS) && fileName == "libwallpaper.so")
            continue;
        //主题功能依赖gsettings-desktop-schemas,mate-desktop-common
        if ((!g_file_test(interfaceFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(gnomedesktopFile, G_FILE_TEST_EXISTS)) && fileName == "libtheme.so")
            continue;

        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject * plugin = loader.instance();
        if (plugin){
            CommonInterface * pluginInstance = qobject_cast<CommonInterface *>(plugin);
            modulesList[pluginInstance->get_plugin_type()].insert(pluginInstance->get_plugin_name(), plugin);

            qDebug() << "Load Plugin :" << kvConverter->keycodeTokeyi18nstring(pluginInstance->get_plugin_type()) << "->" << pluginInstance->get_plugin_name() ;

            int moduletypeInt = pluginInstance->get_plugin_type();
            if (!moduleIndexList.contains(moduletypeInt))
                moduleIndexList.append(moduletypeInt);
        }
        else {
            //如果加载错误且文件后缀为so，输出错误
            if (fileName.endsWith(".so"))
                qDebug() << fileName << "Load Failed: " << loader.errorString() << "\n";
        }
    }
}

void MainWindow::initLeftsideBar(){

    leftBtnGroup = new QButtonGroup();
    leftMicBtnGroup = new QButtonGroup();

    //构建左侧边栏返回首页按钮
    QPushButton * hBtn = buildLeftsideBtn("homepage");
    hBtn->setObjectName("homepage");
    connect(hBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentIndex(0);
    });
    hBtn->setStyleSheet("QPushButton#homepage{background: #cccccc; border: none;}");
    ui->leftsidebarVerLayout->addWidget(hBtn);

    for(int type = 0; type < FUNCTOTALNUM; type++){
        //循环构建左侧边栏模块按钮
        if (moduleIndexList.contains(type)){
            QString mnameString = kvConverter->keycodeTokeystring(type);
            QString mnamei18nString  = kvConverter->keycodeTokeyi18nstring(type); //设置TEXT
            QPushButton * button = buildLeftsideBtn(mnameString);
            button->setCheckable(true);
            leftBtnGroup->addButton(button, type);
            //设置样式
            button->setStyleSheet("QPushButton::checked{background: #cccccc; border: none; border-image: url('://img/primaryleftmenu/checked.png');}"
                                  "QPushButton::!checked{background: #cccccc; border: none;}");

            connect(button, &QPushButton::clicked, this, [=]{
                QPushButton * btn = dynamic_cast<QPushButton *>(QObject::sender());

                int selectedInt = leftBtnGroup->id(btn);

                //获取模块功能列表的第一项
                QList<FuncInfo> tmpList = FunctionSelect::funcinfoList[selectedInt];
                QMap<QString, QObject *> currentFuncMap = modulesList[selectedInt];

                for (FuncInfo tmpStruct : tmpList){
                    if (currentFuncMap.keys().contains(tmpStruct.namei18nString)){
                        modulepageWidget->switchPage(currentFuncMap.value(tmpStruct.namei18nString));
                        break;
                    }
                }
            });

            ui->leftsidebarVerLayout->addWidget(button);
        }
    }

    ui->leftsidebarVerLayout->addStretch();
}

QPushButton * MainWindow::buildLeftsideBtn(QString bname){
    QString iname = bname.toLower();
    int itype = kvConverter->keystringTokeycode(bname);

    QPushButton * leftsidebarBtn = new QPushButton();
    leftsidebarBtn->setAttribute(Qt::WA_DeleteOnClose);
    leftsidebarBtn->setCheckable(true);
    leftsidebarBtn->setFixedSize(QSize(60, 56)); //Widget Width 60

    QPushButton * iconBtn = new QPushButton(leftsidebarBtn);
    iconBtn->setCheckable(true);
    iconBtn->setFixedSize(QSize(24, 24));


    QString iconHomePageBtnQss = QString("QPushButton{background: #cccccc; border: none; border-image: url('://img/primaryleftmenu/%1.png');}").arg(iname);
    QString iconBtnQss = QString("QPushButton:checked{background: #ffffff; border: none; border-image: url('://img/primaryleftmenu/%1Checked.png');}"
                                 "QPushButton:!checked{background: #cccccc; border: none; border-image: url('://img/primaryleftmenu/%2.png');}").arg(iname).arg(iname);
    //单独设置HomePage按钮样式
    if (iname == "homepage")
        iconBtn->setStyleSheet(iconHomePageBtnQss);
    else
        iconBtn->setStyleSheet(iconBtnQss);

    leftMicBtnGroup->addButton(iconBtn, itype);

    connect(iconBtn, &QPushButton::clicked, leftsidebarBtn, &QPushButton::click);

    connect(leftsidebarBtn, &QPushButton::clicked, this, [=](bool checked){
        iconBtn->setChecked(checked);
    });

    QLabel * textLabel = new QLabel(leftsidebarBtn);
    QSizePolicy textLabelPolicy = textLabel->sizePolicy();
    textLabelPolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    textLabelPolicy.setVerticalPolicy(QSizePolicy::Fixed);
    textLabel->setSizePolicy(textLabelPolicy);
    textLabel->setScaledContents(true);

    QHBoxLayout * btnHorLayout = new QHBoxLayout();
    btnHorLayout->addWidget(iconBtn, Qt::AlignCenter);
    btnHorLayout->addWidget(textLabel);
    btnHorLayout->addStretch();
    btnHorLayout->setSpacing(10);

    leftsidebarBtn->setLayout(btnHorLayout);

    return leftsidebarBtn;
}

void MainWindow::setModuleBtnHightLight(int id){
    leftBtnGroup->button(id)->setChecked(true);
    leftMicBtnGroup->button(id)->setChecked(true);
}

QMap<QString, QObject *> MainWindow::exportModule(int type){
    QMap<QString, QObject *> emptyMaps;
    if (type < modulesList.length())
        return modulesList[type];
    else
        return emptyMaps;
}

void MainWindow::functionBtnClicked(QObject *plugin){
    ui->stackedWidget->setCurrentIndex(1);
    modulepageWidget->switchPage(plugin);
}

