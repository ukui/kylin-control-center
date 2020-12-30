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
#include "prescene.h"
#include "utils/keyvalueconverter.h"
#include "utils/functionselect.h"
#include "utils/utils.h"
#include "../commonComponent/ImageUtil/imageutil.h"

#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPluginLoader>
#include <QPainter>
#include <QPainterPath>
#include <QProcess>
//#include <libmatemixer/matemixer.h>
#include <QDebug>
#include <QMessageBox>
#include <QGSettings>

#define QueryLineEditBackground        "#FFFFFF" //搜索框背景
#define QueryLineEditClickedBackground "#FFFFFF" //搜索框背景选中
#define QueryLineEditClickedBorder     "rgba(61, 107, 229, 1)" //搜索框背景选中边框

#ifdef WITHKYSEC
#include <kysec/libkysec.h>
#include <kysec/status.h>
#endif

const QByteArray kVinoSchemas    = "org.gnome.Vino";

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
    ui(new Ui::MainWindow),
    m_searchWidget(nullptr)
{
    // 初始化mixer
//    mate_mixer_init();
    // 设置初始大小
    this->setMinimumSize(895, 600);

    logoLabel  = new QLabel(tr("UKCC"), this);
    PreScene *prescene = new PreScene(logoLabel, this->size());
    prescene->setAttribute(Qt::WA_DeleteOnClose);
    prescene->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    prescene->raise();
    this->setCentralWidget(prescene);

    timer = new QTimer(this);
    timer->stop();
    connect(timer, &QTimer::timeout, this, [=]() {
        initUI();
        timer->stop();
        prescene->close();
    });
    timer->start(200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bootOptionsFilter(QString opt) {
    if (opt == "--display" || opt == "-m") {
        bootOptionsSwitch(SYSTEM, DISPLAY);
    }if (opt == "--touchscreen" || opt == "-m") {
        bootOptionsSwitch(SYSTEM, TOUCHSCREEN);
    } else if (opt == "--defaultapp") {
        bootOptionsSwitch(SYSTEM, DEFAULTAPP);
    } else if (opt == "--power" || opt == "-p") {
        bootOptionsSwitch(SYSTEM, POWER);
    } else if (opt == "--autoboot") {
        bootOptionsSwitch(SYSTEM, AUTOBOOT);
    } else if (opt == "--printer") {
        bootOptionsSwitch(DEVICES, PRINTER);
    } else if (opt == "--mouse") {
        bootOptionsSwitch(DEVICES, MOUSE);
    } else if (opt == "--touchpad") {
        bootOptionsSwitch(DEVICES, TOUCHPAD);
    } else if (opt == "--keyboard") {
        bootOptionsSwitch(DEVICES, KEYBOARD);
    } else if (opt == "--shortcut") {
        bootOptionsSwitch(DEVICES, SHORTCUT);
    } else if (opt == "--audio" || opt == "-s") {
        bootOptionsSwitch(DEVICES, AUDIO);
    } else if (opt == "--bluetooth") {
        bootOptionsSwitch(DEVICES, BLUETOOTH);
    } else if (opt == "--background" || opt == "-b") {
        bootOptionsSwitch(PERSONALIZED, BACKGROUND);
    } else if (opt == "--theme") {
        bootOptionsSwitch(PERSONALIZED, THEME);
    } else if (opt == "--screenlock") {
        bootOptionsSwitch(PERSONALIZED, SCREENLOCK);
    } else if (opt == "--screensaver") {
        bootOptionsSwitch(PERSONALIZED, SCREENSAVER);
    } else if (opt == "--fonts") {
        bootOptionsSwitch(PERSONALIZED, FONTS);
    } else if (opt == "--desktop" || opt == "-d") {
        bootOptionsSwitch(PERSONALIZED, DESKTOP);
    } else if (opt == "--netconnect") {
        bootOptionsSwitch(NETWORK, NETCONNECT);
    } else if (opt == "--vpn" || opt == "-g") {
        bootOptionsSwitch(NETWORK, VPN);
    } else if (opt == "--proxy") {
        bootOptionsSwitch(NETWORK, PROXY);
    } else if (opt == "--userinfo" || opt == "-u") {
        bootOptionsSwitch(ACCOUNT, USERINFO);
    } else if (opt == "--cloudaccount") {
        bootOptionsSwitch(ACCOUNT, NETWORKACCOUNT);
    }  else if (opt == "--datetime" || opt == "-t") {
        bootOptionsSwitch(DATETIME, DAT);
    }  else if (opt == "--area") {
        bootOptionsSwitch(DATETIME, AREA);
    } else if (opt == "--update") {
        bootOptionsSwitch(UPDATE, UPDATES);
    } else if (opt == "--upgrade") {
        bootOptionsSwitch(UPDATE, UPGRADE);
    }else if (opt == "--backup") {
        bootOptionsSwitch(UPDATE, BACKUP);
    } else if (opt == "--notice" || opt == "-n") {
        bootOptionsSwitch(NOTICEANDTASKS, NOTICE);
    } else if (opt == "--about" || opt == "-a") {
        bootOptionsSwitch(NOTICEANDTASKS, ABOUT);
    }
}

void MainWindow::bootOptionsSwitch(int moduleNum, int funcNum){

    QList<FuncInfo> pFuncStructList = FunctionSelect::funcinfoList[moduleNum];
    QString funcStr = pFuncStructList.at(funcNum).namei18nString;
    qDebug() << "moduleNum is" << moduleNum << " " << funcNum << " " << funcStr << endl;

    QMap<QString, QObject *> pluginsObjMap = modulesList.at(moduleNum);

    if (pluginsObjMap.keys().contains(funcStr)){
        //开始跳转
        ui->stackedWidget->setCurrentIndex(1);
        modulepageWidget->switchPage(pluginsObjMap.value(funcStr));
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (this == watched) {
        if (event->type() == QEvent::WindowStateChange) {
            int count = ui->leftsidebarVerLayout->count();
            if (this->windowState() == Qt::WindowMaximized) {
                QFont font = this->font();
                int width = font.pointSize();
                maxBtn->setIcon(QIcon::fromTheme("window-restore-symbolic"));
                ui->leftsidebarWidget->setMaximumWidth(width * 10 +25);
                for (int i = 0; i < count; i++) {
                    QPushButton * btn = static_cast<QPushButton *>(ui->leftsidebarVerLayout->itemAt(i)->widget());
                    if (btn) {
                        QLayout *layout = btn->layout();
                        QLabel * tipLabel = static_cast<QLabel *>(layout->itemAt(1)->widget());
                        tipLabel->setVisible(true);
                    }
                }
            } else {
                maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
                ui->leftsidebarWidget->setMaximumWidth(60);
                for (int i = 0; i < count; i++) {
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
                    this->showNormal();
                } else {
                    this->showMaximized();
                }
            }
        }
    }
    if (watched==m_searchWidget) {
        if (event->type()==QEvent::FocusIn) {
            if (m_searchWidget->text().isEmpty()) {
                m_animation->stop();
                m_animation->setStartValue(QRect((m_searchWidget->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                 m_queryIcon->width()+m_queryText->width()+30,(m_searchWidget->height()+36)/2));
                m_animation->setEndValue(QRect(0,0,
                                               m_queryIcon->width()+5,(m_searchWidget->height()+36)/2));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
                m_searchWidget->setTextMargins(30,1,0,1);
            }
            m_isSearching=true;
        } else if(event->type()==QEvent::FocusOut) {
            m_searchKeyWords.clear();
            if (m_searchWidget->text().isEmpty()) {
                if (m_isSearching) {
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0,0,
                                                     m_queryIcon->width()+5,(m_searchWidget->height()+36)/2));
                    m_animation->setEndValue(QRect((m_searchWidget->width() - (m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                   m_queryIcon->width()+m_queryText->width()+30,(m_searchWidget->height()+36)/2));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching=false;
        }
    }
    return QObject::eventFilter(watched, event);
}

void MainWindow::initUI() {
    ui->setupUi(this);
    ui->centralWidget->setStyleSheet("QWidget#centralWidget{background: palette(base); border-radius: 6px;}");

    m_ModuleMap = Utils::getModuleHideStatus();

    this->installEventFilter(this);

    const QByteArray id("org.ukui.style");
    QGSettings * fontSetting = new QGSettings(id, QByteArray(), this);
    connect(fontSetting, &QGSettings::changed,[=](QString key) {
        if ("systemFont" == key || "systemFontSize" ==key) {
            QFont font = this->font();
            int width = font.pointSize();
            for (auto widget : qApp->allWidgets()) {
                widget->setFont(font);
            }
            ui->leftsidebarWidget->setMaximumWidth(width * 10 +20);
        }
    });

    initTileBar();
    m_queryWid->setGeometry(QRect((m_searchWidget->width() - (m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                        m_queryIcon->width()+m_queryText->width()+10,(m_searchWidget->height()+36)/2));
    m_queryWid->show();
    initStyleSheet();

    //初始化功能列表数据
    FunctionSelect::initValue();

    //构建枚举键值转换对象
    kvConverter = new KeyValueConverter(); //继承QObject，No Delete

    //加载插件
    loadPlugins();

    connect(minBtn, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(maxBtn, &QPushButton::clicked, this, [=] {
        if (isMaximized()) {
            showNormal();
            maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        } else {
            showMaximized();
            maxBtn->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        }
    });
    connect(closeBtn, &QPushButton::clicked, this, [=] {
        close();
    });

    //    ui->leftsidebarWidget->setVisible(ui->stackedWidget->currentIndex());
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [=](int index){
        //左侧边栏显示/不显示
        ui->leftsidebarWidget->setVisible(index);
        //左上角显示字符/返回按钮
        backBtn->setVisible(index);
        titleLabel->setHidden(index);
        mTitleIcon->setHidden(index);

        if (index){ //首页部分组件样式
            //中部内容区域
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: palette(base); border-bottom-right-radius: 6px;}");
        } else { //次页部分组件样式
            //中部内容区域
            ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background:  palette(base); border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
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
    connect(backBtn, &QPushButton::clicked, this, [=]{
        FunctionSelect::popRecordValue();

        //if recordFuncStack is empty, it means there is no history record. So return to homepage
        if (FunctionSelect::recordFuncStack.length() < 1) {
            ui->stackedWidget->setCurrentIndex(0);
        } else {
            QMap<QString, QObject *> pluginsObjMap = modulesList.at(FunctionSelect::recordFuncStack.last().type);
            modulepageWidget->switchPage(pluginsObjMap.value(FunctionSelect::recordFuncStack.last().namei18nString), false);
        }
    });

    // 快捷参数
    if (QApplication::arguments().length() > 1) {
        bootOptionsFilter(QApplication::arguments().at(1));
    }
}

void MainWindow::initTileBar() {

    ui->titleLayout->setContentsMargins(4, 4, 4, 0);
    ui->titleLayout->setSpacing(0);
    m_searchWidget = new SearchWidget(this);
    m_searchWidget->setFocusPolicy(Qt::ClickFocus);
    m_searchWidget->installEventFilter(this);

    m_queryWid=new QWidget;
    m_queryWid->setParent(m_searchWidget);
    m_queryWid->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout* queryWidLayout = new QHBoxLayout;
    queryWidLayout->setContentsMargins(0, 0, 0, 0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(0);
    m_queryWid->setLayout(queryWidLayout);

    QPixmap pixmap = ImageUtil::loadSvg(QString("://img/dropArrow/search.svg"),"gray");
    m_queryIcon = new QLabel(this);
    m_queryIcon->setStyleSheet("background:transparent");
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);

    m_queryText = new QLabel(this);
    m_queryText->setText(tr("Search"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");

    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);

    m_searchWidget->setContextMenuPolicy(Qt::NoContextMenu);
    m_animation= new QPropertyAnimation(m_queryWid, "geometry", this);
    m_animation->setDuration(100);
    ui->titleLayout->addWidget(m_searchWidget,Qt::AlignCenter);
    connect(m_animation,&QPropertyAnimation::finished,this,&MainWindow::animationFinishedSlot);

    connect(m_searchWidget, &SearchWidget::notifyModuleSearch, this, &MainWindow::switchPage);

    backBtn     = new QPushButton(this);
    minBtn      = new QPushButton(this);
    maxBtn      = new QPushButton(this);
    closeBtn    = new QPushButton(this);
    mTitleIcon  = new QLabel(this);
    titleLabel  = new QLabel(tr("UKCC"), this);

    backBtn->setFixedSize(30, 30);
    minBtn->setFixedSize(30, 30);
    maxBtn->setFixedSize(30, 30);
    closeBtn->setFixedSize(30, 30);
    mTitleIcon->setFixedSize(30, 30);

    QIcon titleIcon = QIcon::fromTheme("ukui-control-center");
    mTitleIcon->setPixmap(titleIcon.pixmap(titleIcon.actualSize(QSize(24, 24))));

    titleLabel->setFixedSize(30, 30);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_searchWidget->setMinimumWidth(350);
    m_searchWidget->setMinimumHeight(35);
    m_searchWidget->setMaximumWidth(350);
    m_searchWidget->setMaximumHeight(35);

    ui->titleLayout->addWidget(mTitleIcon);
    ui->titleLayout->addSpacing(8);
    ui->titleLayout->addWidget(titleLabel);
    ui->titleLayout->addWidget(backBtn);
    ui->titleLayout->addStretch();
    ui->titleLayout->addWidget(m_searchWidget);
    ui->titleLayout->addStretch();
    ui->titleLayout->addWidget(minBtn);
    ui->titleLayout->addWidget(maxBtn);
    ui->titleLayout->addWidget(closeBtn);
}
void MainWindow::animationFinishedSlot()
{
    if(m_isSearching) {
        m_queryWid->layout()->removeWidget(m_queryText);
        m_queryText->setParent(nullptr);
        m_searchWidget->setTextMargins(30,1,0,1);
        if(!m_searchKeyWords.isEmpty()) {
            m_searchWidget->setText(m_searchKeyWords);
            m_searchKeyWords.clear();
        }
    } else {
        m_queryWid->layout()->addWidget(m_queryText);
    }
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

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        //三权分立开启
#ifdef WITHKYSEC
        if (!kysec_is_disabled() && kysec_get_3adm_status() && (getuid() || geteuid())){
            //时间和日期 | 用户账户 | 电源管理 |网络连接 |网络代理
            if (fileName.contains("datetime") || fileName.contains("userinfo") || fileName.contains("power") || \
                    fileName.contains("netconnect") || fileName.contains("proxy") || fileName.contains("update"))
                continue;
        }
#endif

        if (!fileName.endsWith(".so")) {
            continue;
        } else if (fileName == "libexperienceplan.so") {
            continue;
        } else if ("libnetworkaccount.so" == fileName && !isExitsCloudAccount()) {
            continue;
        } else if (!QGSettings::isSchemaInstalled(kVinoSchemas) && "libvino.so" == fileName) {
            continue;
        } else if ("libbluetooth.so" == fileName && !isExitBluetooth()) {
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

            m_searchWidget->addModulesName(pluginInstance->name(), pluginInstance->get_plugin_name(), pluginInstance->translationPath());

            int moduletypeInt = pluginInstance->get_plugin_type();
            if (!moduleIndexList.contains(moduletypeInt))
                moduleIndexList.append(moduletypeInt);
        } else {
            //如果加载错误且文件后缀为so，输出错误
            if (fileName.endsWith(".so"))
                qDebug() << fileName << "Load Failed: " << loader.errorString() << "\n";
        }
    }
    m_searchWidget->setLanguage(QLocale::system().name());
}

void MainWindow::initLeftsideBar(){

    leftBtnGroup = new QButtonGroup();
    leftMicBtnGroup = new QButtonGroup();

    //构建左侧边栏返回首页按钮
    QPushButton * hBtn = buildLeftsideBtn("homepage",tr("Home"));
    hBtn->setObjectName("homepage");
    connect(hBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentIndex(0);
    });
    hBtn->setStyleSheet("QPushButton#homepage{background: palette(window); border: none;}");
    //    hBtn->setStyleSheet("QPushButton#homepage{background: palette(base);}");
    ui->leftsidebarVerLayout->addStretch();
    ui->leftsidebarVerLayout->addWidget(hBtn);

    for(int type = 0; type < TOTALMODULES; type++) {
        //循环构建左侧边栏一级菜单按钮
        if (moduleIndexList.contains(type)){
            QString mnameString = kvConverter->keycodeTokeystring(type);
            QString mnamei18nString  = kvConverter->keycodeTokeyi18nstring(type); //设置TEXT

            if (m_ModuleMap.keys().contains(mnameString.toLower())) {
                if (!m_ModuleMap[mnameString.toLower()].toBool()) {
                    continue;
                }
            }

            QPushButton * button;
            QString btnName = "btn" + QString::number(type + 1);
            button = buildLeftsideBtn(mnameString,mnamei18nString);
            button->setToolTip(mnamei18nString);

            button->setObjectName(btnName);
            button->setCheckable(true);
            leftBtnGroup->addButton(button, type);

            button->setStyleSheet("QPushButton::checked{background: palette(base); border-top-left-radius: 6px;border-bottom-left-radius: 6px;}"
                                  "QPushButton::!checked{background: palette(window);border: none;}");

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

QPushButton * MainWindow::buildLeftsideBtn(QString bname,QString tipName) {
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


    QString iconHomePageBtnQss = QString("QPushButton{background: palette(window); border: none;}");
    QString iconBtnQss = QString("QPushButton:checked{background: palette(base); border: none;}"
                                 "QPushButton:!checked{background: palette(window); border: none;}");
    QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
    QPixmap pix = ImageUtil::loadSvg(path, "default");
    //单独设置HomePage按钮样式
    if (iname == "homepage") {
        iconBtn->setFlat(true);
        iconBtn->setStyleSheet(iconHomePageBtnQss);
    } else {
        iconBtn->setStyleSheet(iconBtnQss);
    }
    iconBtn->setIcon(pix);

    leftMicBtnGroup->addButton(iconBtn, itype);

    connect(iconBtn, &QPushButton::toggled, this, [=] (bool checked) {
        QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
        QPixmap pix;
        if (checked) {
            pix = ImageUtil::loadSvg(path, "blue");
        } else {
            pix = ImageUtil::loadSvg(path, "default");
        }
        iconBtn->setIcon(pix);
    });

    connect(iconBtn, &QPushButton::clicked, leftsidebarBtn, &QPushButton::click);

    connect(leftsidebarBtn, &QPushButton::toggled, this, [=](bool checked) {
        iconBtn->setChecked(checked);
        QString path = QString("://img/primaryleftmenu/%1.svg").arg(iname);
        QPixmap pix;
        if (checked) {
            pix = ImageUtil::loadSvg(path, "blue");
        } else {
            pix = ImageUtil::loadSvg(path, "default");
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

bool MainWindow::dblOnEdge(QMouseEvent *event) {
    QPoint pos = event->globalPos();
    int globalMouseY = pos.y();

    int frameY = this->y();

    bool onTopEdges = (globalMouseY >= frameY &&
                       globalMouseY <= frameY + dbWitdth);
    return onTopEdges;
}

void MainWindow::initStyleSheet() {
    // 设置panel图标
    QIcon panelicon;
    if (QIcon::hasThemeIcon("ukui-control-center"))
        panelicon = QIcon::fromTheme("ukui-control-center");

    this->setWindowIcon(panelicon);
    this->setWindowTitle(tr("ukcc"));

    // 中部内容区域
    ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{background: palette(base); border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

    // 左上角返回按钮
    backBtn->setProperty("useIconHighlightEffect", true);
    backBtn->setProperty("iconHighlightEffectMode", 1);
    backBtn->setFlat(true);

    minBtn->setProperty("useIconHighlightEffect", 0x2);
    minBtn->setProperty("isWindowButton", 0x01);
    minBtn->setFlat(true);
    maxBtn->setProperty("useIconHighlightEffect", 0x2);
    maxBtn->setProperty("isWindowButton", 0x1);
    maxBtn->setFlat(true);

    closeBtn->setProperty("isWindowButton", 0x02);
    closeBtn->setProperty("useIconHighlightEffect", 0x08);
    closeBtn->setFlat(true);
    ui->leftsidebarWidget->setStyleSheet("QWidget#leftsidebarWidget{background-color: palette(window);border: none; border-top-left-radius: 6px; border-bottom-left-radius: 6px;}");

    // 设置左上角按钮图标
    backBtn->setIcon(QIcon("://img/titlebar/back.svg"));

    // 设置右上角按钮图标
    minBtn->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
}

bool MainWindow::isExitBluetooth() {
    QProcess process;
    process.start("rfkill list");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    QString str_output = output;
    return str_output.contains(QString("bluetooth"), Qt::CaseInsensitive);
}

void MainWindow::setModuleBtnHightLight(int id) {
    leftBtnGroup->button(id)->setChecked(true);
    leftMicBtnGroup->button(id)->setChecked(true);
}

QMap<QString, QObject *> MainWindow::exportModule(int type) {
    QMap<QString, QObject *> emptyMaps;
    if (type < modulesList.length())
        return modulesList[type];
    else
        return emptyMaps;
}

void MainWindow::functionBtnClicked(QObject *plugin) {
    ui->stackedWidget->setCurrentIndex(1);
    modulepageWidget->switchPage(plugin);
}

void MainWindow::sltMessageReceived(const QString &msg) {

    bootOptionsFilter(msg);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    showNormal();
}

void MainWindow::switchPage(QString moduleName) {

    for (int i = 0; i < modulesList.length(); i++) {
        auto modules = modulesList.at(i);
        //开始跳转
        if (modules.keys().contains(moduleName)) {
            ui->stackedWidget->setCurrentIndex(1);
            modulepageWidget->switchPage(modules.value(moduleName));
        }
    }
}
