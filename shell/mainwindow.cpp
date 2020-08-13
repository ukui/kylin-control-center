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
#include "utils/keyvalueconverter.h"
#include "utils/functionselect.h"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPluginLoader>
#include <QPainter>
#include <QPainterPath>
#include <QProcess>
#include <libmatemixer/matemixer.h>
#include <QDebug>
#include <QMessageBox>
#include <QGSettings>

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

const int dbWitdth = 50;
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化mixer
    mate_mixer_init();
    //设置初始大小
    resize(QSize(820, 600));
    //设置窗体无边框
    setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
    this->installEventFilter(this);
    ui->closeBtn->setFixedSize(32,32);
    //该设置去掉了窗体透明后的黑色背景
    setAttribute(Qt::WA_TranslucentBackground, true);
    //将最外层窗体设置为透明

//    setStyleSheet("QMainWindow#MainWindow{background-color: transparent;}");

    const QByteArray id("org.ukui.style");
    QGSettings * fontSetting = new QGSettings(id);
    connect(fontSetting, &QGSettings::changed,[=](QString key){
        if ("systemFont" == key || "systemFontSize" ==key) {
            QFont font = this->font();
            int width = font.pointSize();
            for (auto widget : qApp->allWidgets()) {
                widget->setFont(font);
            }
            ui->leftsidebarWidget->setMaximumWidth(width * 10 +20);
        }
    });

    //设置panel图标
    QIcon panelicon;
    if (QIcon::hasThemeIcon("ukui-control-center"))
        panelicon = QIcon::fromTheme("ukui-control-center");
//    else
//        panelicon = QIcon("://applications-system.svg");
    this->setWindowIcon(panelicon);
    this->setWindowTitle(tr("ukcc"));

    ui->searchLineEdit->setVisible(false);
    //中部内容区域
    ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: palette(base); border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
    //标题栏widget
    ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background: palette(base); border-top-left-radius: 6px; border-top-right-radius: 6px;}");
////    //左上角文字
////    ui->mainLabel->setStyleSheet("QLabel#mainLabel{font-size: 18px; color: #40000000;}");

    //左上角返回按钮
    ui->backBtn->setProperty("useIconHighlightEffect", true);
    ui->backBtn->setProperty("iconHighlightEffectMode", 1);
    ui->backBtn->setFlat(true);

//    ui->backBtn->setStyleSheet("QPushButton#backBtn{background: #ffffff; border: none;}");
//    //顶部搜索框
//    ui->searchLineEdit->setStyleSheet("QLineEdit#searchLineEdit{background: #FFEDEDED; border: none; border-radius: 6px;}");
    //右上角按钮stylesheet
    ui->minBtn->setProperty("useIconHighlightEffect", true);
    ui->minBtn->setProperty("iconHighlightEffectMode", 1);
    ui->minBtn->setFlat(true);
    ui->maxBtn->setProperty("useIconHighlightEffect", true);
    ui->maxBtn->setProperty("iconHighlightEffectMode", 1);
    ui->maxBtn->setFlat(true);
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);
    ui->closeBtn->installEventFilter(this);

//    ui->minBtn->setStyleSheet("QPushButton#minBtn{background: #ffffff; border: none;}"
//                              "QPushButton:hover:!pressed#minBtn{background: #FF3D6BE5; border-radius: 2px;}"
//                              "QPushButton:hover:pressed#minBtn{background: #415FC4; border-radius: 2px;}");
//    ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background: #ffffff; border: none;}"
//                              "QPushButton:hover:!pressed#maxBtn{background: #FF3D6BE5; border-radius: 2px;}"
//                              "QPushButton:hover:pressed#maxBtn{background: #415FC4; border-radius: 2px;}");
    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    //左侧一级菜单
//    ui->leftsidebarWidget->setStyleSheet("QWidget#leftsidebarWidget{background: #cccccc; border: none; border-top-left-radius: 6px; border-bottom-left-radius: 6px;}");
    ui->leftsidebarWidget->setStyleSheet("QWidget#leftsidebarWidget{background-color: palette(button);border: none; border-top-left-radius: 6px; border-bottom-left-radius: 6px;}");

    //设置左上角按钮图标
    ui->backBtn->setIcon(QIcon("://img/titlebar/back.svg"));

    //设置右上角按钮图标
    ui->minBtn->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    ui->maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    ui->closeBtn->setIcon(renderSvg(QIcon::fromTheme("window-close-symbolic"),"default"));


    //初始化功能列表数据
    FunctionSelect::initValue();

    //构建枚举键值转换对象
    kvConverter = new KeyValueConverter(); //继承QObject，No Delete

    //加载插件
    loadPlugins();

    connect(ui->minBtn, SIGNAL(clicked()), this, SLOT(showMinimized()));
//    connect(ui->minBtn, &QPushButton::clicked, [=]{
//        KWindowSystem::minimizeWindow(this->winId());
//    });
    connect(ui->maxBtn, &QPushButton::clicked, this, [=]{
        if (isMaximized()){
            bIsFullScreen = false;
            showNormal();
            ui->maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        } else {
            bIsFullScreen = true;
            showMaximized();
            ui->maxBtn->setIcon(QIcon::fromTheme("window-restore-symbolic"));
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
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: palette(base); border-bottom-right-radius: 6px;}");
            //标题栏widget
            ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background:  palette(base); border-top-right-radius: 6px;}");
        } else { //次页部分组件样式
            //中部内容区域
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background:  palette(base); border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
            //标题栏widget
            ui->titlebarWidget->setStyleSheet("QWidget#titlebarWidget{background:  palette(base); border-top-left-radius: 6px; border-top-right-radius: 6px;}");
        }
    });

    //加载左侧边栏一级菜单
    initLeftsideBar();

    bIsFullScreen = false;

    //加载首页Widget
    homepageWidget = new HomePageWidget(this);
    ui->stackedWidget->addWidget(homepageWidget);

    //加载功能页Widget
    modulepageWidget = new ModulePageWidget(this);
    ui->stackedWidget->addWidget(modulepageWidget);

    //top left return button
    connect(ui->backBtn, &QPushButton::clicked, this, [=]{
        FunctionSelect::popRecordValue();

        //if recordFuncStack is empty, it means there is no history record. So return to homepage
        if (FunctionSelect::recordFuncStack.length() < 1) {
            ui->stackedWidget->setCurrentIndex(0);
        } else {
            QMap<QString, QObject *> pluginsObjMap = modulesList.at(FunctionSelect::recordFuncStack.last().type);
            modulepageWidget->switchPage(pluginsObjMap.value(FunctionSelect::recordFuncStack.last().namei18nString), false);
        }
    });

    //快捷参数
    if (QApplication::arguments().length() > 1){

        bootOptionsFilter(QApplication::arguments().at(1));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bootOptionsFilter(QString opt){
    if (opt == "-m") {
        //显示器
        bootOptionsSwitch(SYSTEM, DISPLAY);

    } else if (opt == "-b") {
        //背景
        bootOptionsSwitch(PERSONALIZED, BACKGROUND);

    } else if (opt == "-d") {
        //桌面
        bootOptionsSwitch(PERSONALIZED, DESKTOP);

    } else if (opt == "-u") {
        //账户
        bootOptionsSwitch(ACCOUNT, USERINFO);

    } else if (opt == "-a") {
        //关于
        bootOptionsSwitch(NOTICEANDTASKS, ABOUT);

    } else if (opt == "-p") {
        //电源
        bootOptionsSwitch(SYSTEM, POWER);
    } else if (opt == "-t") {
        // Datetime moudle
        bootOptionsSwitch(DATETIME, DAT);
    } else if (opt == "-s") {
        // Audio module
        bootOptionsSwitch(DEVICES, AUDIO);
    } else if (opt == "-n") {
        // notice module
        bootOptionsSwitch(NOTICEANDTASKS, NOTICE);
    }
}

void MainWindow::bootOptionsSwitch(int moduleNum, int funcNum){

    QList<FuncInfo> pFuncStructList = FunctionSelect::funcinfoList[moduleNum];
    QString funcStr = pFuncStructList.at(funcNum).namei18nString;

    QMap<QString, QObject *> pluginsObjMap = modulesList.at(moduleNum);

    if (pluginsObjMap.keys().contains(funcStr)){
        //开始跳转
        ui->stackedWidget->setCurrentIndex(1);
        modulepageWidget->switchPage(pluginsObjMap.value(funcStr));
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    if(!bIsFullScreen) {
        rectPath.addRoundedRect(this->rect().adjusted(1, 1, -1, -1), 6, 6);

        // 画一个黑底
        QPixmap pixmap(this->rect().size());
        pixmap.fill(Qt::transparent);
        QPainter pixmapPainter(&pixmap);
        pixmapPainter.setRenderHint(QPainter::Antialiasing);
        pixmapPainter.setPen(Qt::transparent);
        pixmapPainter.setBrush(Qt::black);
        pixmapPainter.setOpacity(0.65);
        pixmapPainter.drawPath(rectPath);
        pixmapPainter.end();

        // 模糊这个黑底
        QImage img = pixmap.toImage();
        qt_blurImage(img, 5, false, false);

        // 挖掉中心
        pixmap = QPixmap::fromImage(img);
        QPainter pixmapPainter2(&pixmap);
        pixmapPainter2.setRenderHint(QPainter::Antialiasing);
        pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
        pixmapPainter2.setPen(Qt::transparent);
        pixmapPainter2.setBrush(Qt::transparent);
        pixmapPainter2.drawPath(rectPath);

        // 绘制阴影
        p.drawPixmap(this->rect(), pixmap, pixmap.rect());
    } else {
        rectPath.addRoundedRect(this->rect(), 0, 0);
    }

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
//    p.fillPath(rectPath,QColor(0,0,0));
    p.restore();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (this == watched) {
        if (event->type() == QEvent::WindowStateChange) {
            if (this->windowState() == Qt::WindowMaximized) {
                QFont font = this->font();
                int width = font.pointSize();
                ui->leftsidebarWidget->setMaximumWidth(width * 10 +20);
                for (int i = 0; i <= 9; i++) {
                    QPushButton * btn = static_cast<QPushButton *>(ui->leftsidebarVerLayout->itemAt(i)->widget());

                    if (btn) {
                        QLayout *layout = btn->layout();
                        QLabel * tipLabel = static_cast<QLabel *>(layout->itemAt(1)->widget());
                        tipLabel->setVisible(true);
                    }
                }
            } else {
                ui->leftsidebarWidget->setMaximumWidth(60);
                for (int i = 0; i <= 9; i++) {
                    QPushButton * btn = static_cast<QPushButton *>(ui->leftsidebarVerLayout->itemAt(i)->widget());
                    if (btn) {
                        QLayout *layout = btn->layout();
                        QLabel * tipLabel = static_cast<QLabel *>(layout->itemAt(1)->widget());
                        tipLabel->setVisible(false);
                    }
                }
            }
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            bool res = dblOnEdge(dynamic_cast<QMouseEvent*>(event));
            if (res) {
                if (this->windowState() == Qt::WindowMaximized) {
                    bIsFullScreen = false;
                    this->showNormal();
                } else {
                    bIsFullScreen = true;
                    this->showMaximized();
                }
            }
        }
    }
    if(ui->closeBtn == watched) {
        if(event->type() == QEvent::Enter) {
            ui->closeBtn->setIcon(renderSvg(QIcon::fromTheme("window-close-symbolic"),"white"));
        }else if(event->type() == QEvent::Leave) {
            ui->closeBtn->setIcon(renderSvg(QIcon::fromTheme("window-close-symbolic"),"default"));
        }
    }
    return QObject::eventFilter(watched, event);
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
    for (int index = 0; index < TOTALMODULES; index++){
        QMap<QString, QObject *> pluginsMaps;
        modulesList.append(pluginsMaps);
    }

    static bool installed = (QCoreApplication::applicationDirPath() == QDir(("/usr/bin")).canonicalPath());

    if (installed)
        pluginsDir = QDir(PLUGIN_INSTALL_DIRS);
    else {
        pluginsDir = QDir(qApp->applicationDirPath() + "/plugins");
    }

    bool isExistCloud  = isExitsCloudAccount();
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        if (!fileName.endsWith(".so"))
            continue;
        if (fileName == "libexperienceplan.so")
            continue;
        if ("libnetworkaccount.so" == fileName && !isExistCloud) {
            continue;
        }

        qDebug() << "Scan Plugin: " << fileName;

        //ukui-session-manager
        const char * sessionFile = "/usr/share/glib-2.0/schemas/org.ukui.session.gschema.xml";
        //ukui-screensaver
        const char * screensaverFile = "/usr/share/glib-2.0/schemas/org.ukui.screensaver.gschema.xml";

        //屏保功能依赖ukui-session-manager
        if ((!g_file_test(screensaverFile, G_FILE_TEST_EXISTS) ||
                !g_file_test(sessionFile, G_FILE_TEST_EXISTS)) &&
                (fileName == "libscreensaver.so" || fileName == "libscreenlock.so"))
            continue;

        const char * securityCmd = "/usr/sbin/ksc-defender";

        if ((!g_file_test(securityCmd, G_FILE_TEST_EXISTS)) && (fileName == "libsecuritycenter.so"))
            continue;

        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject * plugin = loader.instance();
        if (plugin) {
            CommonInterface * pluginInstance = qobject_cast<CommonInterface *>(plugin);
            modulesList[pluginInstance->get_plugin_type()].insert(pluginInstance->get_plugin_name(), plugin);

            qDebug() << "Load Plugin :" << kvConverter->keycodeTokeyi18nstring(pluginInstance->get_plugin_type()) << "->" << pluginInstance->get_plugin_name() ;

            int moduletypeInt = pluginInstance->get_plugin_type();
            if (!moduleIndexList.contains(moduletypeInt))
                moduleIndexList.append(moduletypeInt);
        } else {
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
    QPushButton * hBtn = buildLeftsideBtn("homepage",tr("HOME"));
    hBtn->setObjectName("homepage");
    connect(hBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentIndex(0);
    });
    hBtn->setStyleSheet("QPushButton#homepage{background: palette(button); border: none;}");
//    hBtn->setStyleSheet("QPushButton#homepage{background: palette(base);}");
    ui->leftsidebarVerLayout->addStretch();
    ui->leftsidebarVerLayout->addWidget(hBtn);

    QString locale = QLocale::system().name();
    for(int type = 0; type < TOTALMODULES; type++){
        //循环构建左侧边栏一级菜单按钮
        if (moduleIndexList.contains(type)){
            QString mnameString = kvConverter->keycodeTokeystring(type);
            QString mnamei18nString  = kvConverter->keycodeTokeyi18nstring(type); //设置TEXT

            QPushButton * button;
            QString btnName = "btn" + QString::number(type + 1);
            if ("zh_CN" == locale) {
                button = buildLeftsideBtn(mnameString,mnamei18nString);
                button->setToolTip(mnamei18nString);
            } else {
                button = buildLeftsideBtn(mnameString,mnameString);
                button->setToolTip(mnameString);
            }
            button->setObjectName(btnName);
            button->setCheckable(true);
            leftBtnGroup->addButton(button, type);

            //设置样式
//            button->setStyleSheet("QPushButton::checked{background: palette(button); border: none; border-image: url('://img/primaryleftmenu/checked.png');}"
//                                  "QPushButton::!checked{background: palette(button);border: none;}");
            button->setStyleSheet("QPushButton::checked{background: palette(base); border-top-left-radius: 6px;border-bottom-left-radius: 6px;}"
                                  "QPushButton::!checked{background: palette(button);border: none;}");

            connect(button, &QPushButton::clicked, this, [=]{
                QPushButton * btn = dynamic_cast<QPushButton *>(QObject::sender());

                int selectedInt = leftBtnGroup->id(btn);

                //获取一级菜单列表的第一项
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

QPushButton * MainWindow::buildLeftsideBtn(QString bname,QString tipName){
    QString iname = bname.toLower();
    int itype = kvConverter->keystringTokeycode(bname);

    QPushButton * leftsidebarBtn = new QPushButton();
    leftsidebarBtn->setAttribute(Qt::WA_DeleteOnClose);
    leftsidebarBtn->setCheckable(true);
//    leftsidebarBtn->setFixedSize(QSize(60, 56)); //Widget Width 60
    leftsidebarBtn->setFixedHeight(56);

    QPushButton * iconBtn = new QPushButton(leftsidebarBtn);
    iconBtn->setCheckable(true);
    iconBtn->setFixedSize(QSize(24, 24));
    iconBtn->setFocusPolicy(Qt::NoFocus);


    QString iconHomePageBtnQss = QString("QPushButton{background: palette(button); border: none;}");
    QString iconBtnQss = QString("QPushButton:checked{background: palette(base); border: none;}"
                                 "QPushButton:!checked{background: palette(button); border: none;}");
    QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
    QPixmap pix = loadSvg(path, "default");
    //单独设置HomePage按钮样式
    if (iname == "homepage") {
        iconBtn->setFlat(true);
        iconBtn->setStyleSheet(iconHomePageBtnQss);
    } else {
        iconBtn->setStyleSheet(iconBtnQss);
    }
    iconBtn->setIcon(pix);

    leftMicBtnGroup->addButton(iconBtn, itype);

    connect(iconBtn, &QPushButton::toggled, this, [=] (bool checked){
       QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
       QPixmap pix;
       if (checked) {
           pix = loadSvg(path, "blue");
       } else {
           pix = loadSvg(path, "default");
       }
       iconBtn->setIcon(pix);
    });

    connect(iconBtn, &QPushButton::clicked, leftsidebarBtn, &QPushButton::click);

    connect(leftsidebarBtn, &QPushButton::toggled, this, [=](bool checked){
        iconBtn->setChecked(checked);
        QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
        QPixmap pix;
        if (checked) {
            pix = loadSvg(path, "blue");
        } else {
            pix = loadSvg(path, "default");
        }
        iconBtn->setIcon(pix);
    });

    QLabel * textLabel = new QLabel(leftsidebarBtn);
    textLabel->setVisible(false);
    textLabel->setText(tipName);
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

bool MainWindow::isExitsCloudAccount() {
    QProcess *wifiPro = new QProcess();
    QString shellOutput = "";
    wifiPro->start("dpkg -l  | grep kylin-sso-client");
    wifiPro->waitForFinished();
    QString output = wifiPro->readAll();
    shellOutput += output;
    QStringList slist = shellOutput.split("\n");

    for (QString res : slist) {
        if (res.contains("kylin-sso-client")) {
            return true;
        }
    }
    return false;
}

const QPixmap MainWindow::loadSvg(const QString &fileName, QString color)
{
    int size = 24;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size = 48;
    } else if (3 == ratio) {
        size = 96;
    }
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);
    return drawSymbolicColoredPixmap(pixmap, color);
}

QPixmap MainWindow::drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor)
{
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if ("white" == cgColor) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                } else if ("black" == cgColor) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
//                    color.setAlpha(0.1);
                    color.setAlphaF(0.9);
                    img.setPixelColor(x, y, color);
                } else if ("gray" == cgColor) {
                    color.setRed(152);
                    color.setGreen(163);
                    color.setBlue(164);
                    img.setPixelColor(x, y, color);
                } else if ("blue" == cgColor){
                    color.setRed(61);
                    color.setGreen(107);
                    color.setBlue(229);
                    img.setPixelColor(x, y, color);
                } else {
                    return source;
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

bool MainWindow::dblOnEdge(QMouseEvent *event)
{
    QPoint pos = event->globalPos();
    int globalMouseY = pos.y();

    int frameY = this->y();

    bool onTopEdges = (globalMouseY >= frameY &&
                  globalMouseY <= frameY + dbWitdth);
    return onTopEdges;
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

void MainWindow::sltMessageReceived(const QString &msg) {

    bootOptionsFilter(msg);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    show();
    flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    showNormal();
}

const QPixmap MainWindow::renderSvg(const QIcon &icon, QString cgColor) {
    int size = 24;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size = 48;
    } else if (3 == ratio) {
        size = 96;
    }
    QPixmap iconPixmap = icon.pixmap(size,size);
    iconPixmap.setDevicePixelRatio(ratio);
    QImage img = iconPixmap.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if ("white" == cgColor) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                } else if ("black" == cgColor) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
//                    color.setAlpha(0.1);
                    color.setAlphaF(0.9);
                    img.setPixelColor(x, y, color);
                } else if ("gray" == cgColor) {
                    color.setRed(152);
                    color.setGreen(163);
                    color.setBlue(164);
                    img.setPixelColor(x, y, color);
                } else if ("blue" == cgColor){
                    color.setRed(61);
                    color.setGreen(107);
                    color.setBlue(229);
                    img.setPixelColor(x, y, color);
                } else {
                    return iconPixmap;
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}
