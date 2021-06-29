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
#include "notice.h"
#include "ui_notice.h"
#include "appdetail.h"
#include "realizenotice.h"
#include "commonComponent/HoverWidget/hoverwidget.h"
#include <QFileDialog>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QSettings>

#define NOTICE_SCHEMA         "org.ukui.control-center.notice"
#define NEW_FEATURE_KEY       "show-new-feature"
#define ENABLE_NOTICE_KEY     "enable-notice"
#define SHOWON_LOCKSCREEN_KEY "show-on-lockscreen"

#define DESKTOPPATH           "/usr/share/applications/"
#define DESKTOPOTHERPAYH      "/etc/xdg/autostart/"

Notice::Notice() : mFirstLoad(true)
{
    pluginName = tr("Notice");
    pluginType = NOTICEANDTASKS;
}

Notice::~Notice()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
        delete mstringlist;
        mstringlist = nullptr;
        qDeleteAll(vecGsettins);
        vecGsettins.clear();
    }
}

QString Notice::get_plugin_name()
{
    return pluginName;
}

int Notice::get_plugin_type()
{
    return pluginType;
}

QWidget *Notice::get_plugin_ui()
{
    if (mFirstLoad) {
        ui = new Ui::Notice;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);
        mFirstLoad = false;
        //获取已经存在的动态路径
        listChar = listExistsCustomNoticePath();

        ui->newfeatureWidget->setVisible(false);
        ui->lockscreenWidget->setVisible(false);
        ui->title2Label->hide();
        ui->verticalSpacer->changeSize(0,0);
        ui->noticeLabel->hide();


        mstringlist = new QStringList();

        initSearchText();
        setupGSettings();
        setupComponent();
        initNoticeStatus();

        //设置白名单
        whitelist.append("kylin-screenshot.desktop");
        whitelist.append("peony.desktop");
        whitelist.append("kylin-nm.desktop");
        whitelist.append("ukui-flash-disk.desktop");
        whitelist.append("kylin-background-upgrade.desktop");
        whitelist.append("ukui-power-manager.desktop");
        initOriNoticeStatus();
        nSetting->set(IS_CN,mEnv);
        //加载列表
//        QTimer *mtimer = new QTimer(this);
//        connect(mtimer, &QTimer::timeout, this,[=](){
//            int i = count;
//            initOriNoticeStatus();
//            if (i == count) {
//                mstringlist->clear();
//                nSetting->set(IS_CN,mEnv);
//                mtimer->stop();
//            }
//        } );
//        mtimer->start(500);
        //监视desktop文件列表
        QFileSystemWatcher *m_fileWatcher=new QFileSystemWatcher;
        m_fileWatcher->addPaths(QStringList()<<QString(DESKTOPPATH));
        //有应用卸载或安装时
        connect(m_fileWatcher,&QFileSystemWatcher::directoryChanged,[=](){
            loadlist();
        });

    }
    return pluginWidget;
}

void Notice::plugin_delay_control()
{
}

const QString Notice::name() const
{
    return QStringLiteral("notice");
}


void Notice::initSearchText()
{
    // ~ contents_path /notice/Set notice type of operation center
    ui->noticeLabel->setText(tr("Set notice type of operation center"));
    // ~ contents_path /notice/Notice Origin
    ui->title2Label->setText(tr("Notice Origin"));
}

void Notice::setupComponent()
{
    newfeatureSwitchBtn = new SwitchButton(pluginWidget);
    enableSwitchBtn = new SwitchButton(pluginWidget);
    lockscreenSwitchBtn = new SwitchButton(pluginWidget);
    applistverticalLayout = new QVBoxLayout();
    applistverticalLayout->setSpacing(1);
    applistverticalLayout->setContentsMargins(0, 0, 0, 1);
    ui->newfeatureHorLayout->addWidget(newfeatureSwitchBtn);
    ui->enableHorLayout->addWidget(enableSwitchBtn);
    ui->lockscreenHorLayout->addWidget(lockscreenSwitchBtn);
    ui->frame->setLayout(applistverticalLayout);

    connect(newfeatureSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        nSetting->set(NEW_FEATURE_KEY, checked);
    });
    connect(enableSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        nSetting->set(ENABLE_NOTICE_KEY, checked);
        setHiddenNoticeApp(checked);
    });
    connect(lockscreenSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        nSetting->set(SHOWON_LOCKSCREEN_KEY, checked);
    });
}

void Notice::setupGSettings()
{
    if (QGSettings::isSchemaInstalled(NOTICE_SCHEMA)) {
        QByteArray id(NOTICE_SCHEMA);
        nSetting = new QGSettings(id, QByteArray(), this);
    }
    if (QGSettings::isSchemaInstalled(THEME_QT_SCHEMA)) {
        QByteArray id(THEME_QT_SCHEMA);
        mThemeSetting = new QGSettings(id, QByteArray(), this);
        connect(mThemeSetting, &QGSettings::changed, [=](){
            loadlist();
        });

    }
}

void Notice::initNoticeStatus()
{
    newfeatureSwitchBtn->blockSignals(true);
    enableSwitchBtn->blockSignals(true);
    lockscreenSwitchBtn->blockSignals(true);
    newfeatureSwitchBtn->setChecked(nSetting->get(NEW_FEATURE_KEY).toBool());
    enableSwitchBtn->setChecked(nSetting->get(ENABLE_NOTICE_KEY).toBool());
    lockscreenSwitchBtn->setChecked(nSetting->get(SHOWON_LOCKSCREEN_KEY).toBool());
    newfeatureSwitchBtn->blockSignals(false);
    enableSwitchBtn->blockSignals(false);
    lockscreenSwitchBtn->blockSignals(false);
    isCN_env = nSetting->get(IS_CN).toBool();
    mlocale = QLocale::system().name();
    if (mlocale == "zh_CN") {
        mEnv = true;
    } else {
        mEnv = false;
    }
    setHiddenNoticeApp(enableSwitchBtn->isChecked());

}

void Notice::initOriNoticeStatus()
{
    QDir dir(QString(DESKTOPPATH).toUtf8());
    QDir otherdir(QDir::homePath() + "/.local/share/applications");
    QDir autodir(QString(DESKTOPOTHERPAYH).toUtf8());

    QStringList filters;
    filters<<QString("*.desktop");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); // 设置类型过滤器，只为文件格式
    otherdir.setFilter(QDir::Files | QDir::NoSymLinks);
    autodir.setFilter(QDir::Files | QDir::NoSymLinks);

    dir.setNameFilters(filters);  // 设置文件名称过滤器，只为filters格式
    otherdir.setNameFilters(filters);
    autodir.setNameFilters(filters);

    //初始化列表界面
    initListUI(dir,DESKTOPPATH,mstringlist);
    initListUI(autodir,DESKTOPOTHERPAYH,mstringlist);
    mstringlist->clear();

}

void Notice::initListUI(QDir dir,QString mpath,QStringList *stringlist)
{
    for (int i = 0; i < dir.count(); i++) {
        QString file_name = dir[i];  // 文件名称
        if (!whitelist.contains(file_name)) {
            continue;
        }
        QSettings* desktopFile = new QSettings(mpath+file_name, QSettings::IniFormat);
        QString fname_1,fname_2,fname_3,appname,appname_CN,appname_US,icon;
        if (desktopFile) {
           desktopFile->setIniCodec("utf-8");

           fname_1 = desktopFile->value(QString("Desktop Entry/NoDisplay")).toString();
           fname_2 = desktopFile->value(QString("Desktop Entry/NotShowIn")).toString();
           fname_3 = desktopFile->value(QString("Desktop Entry/OnlyShowIn")).toString();
           icon = desktopFile->value(QString("Desktop Entry/Icon")).toString();
           appname = desktopFile->value(QString("Desktop Entry/Name")).toString();
           appname_CN = desktopFile->value(QString("Desktop Entry/Name[zh_CN]")).toString();
           appname_US = desktopFile->value(QString("Desktop Entry/Name")).toString();
           delete desktopFile;
           desktopFile = nullptr;
        }
//        if (fname_1 != nullptr) {
//            if (fname_1.contains("true")) {
//                continue;
//            }
//        }
        if (fname_2 != nullptr) {
            if (fname_2.contains("UKUI")) {
                continue;
            }
        }
        if (fname_3 != nullptr) {
            if (fname_3.contains("LXQt") || fname_3.contains("KDE")) {
                continue;
            }
        }
        if (stringlist->contains(appname)) {
            qDebug()<<appname;
            continue;
        }
        stringlist->append(appname);
        // 构建Widget



        QFrame *baseWidget = new QFrame();
        baseWidget->setMinimumWidth(550);
        baseWidget->setMaximumWidth(960);
        baseWidget->setFixedHeight(50);
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QPushButton *iconBtn = new QPushButton(baseWidget);

        iconBtn->setStyleSheet("QPushButton{background-color:transparent;border-radius:4px}"
                               "QPushButton:hover{background-color: transparent ;color:transparent;}");
        iconBtn->setIconSize(QSize(32, 32));
        iconBtn->setIcon(QIcon::fromTheme(icon,
                                          QIcon(QString("/usr/share/pixmaps/"+icon)
                                                        +".png")));

        QHBoxLayout *devHorLayout = new QHBoxLayout(baseWidget);
        devHorLayout->setSpacing(8);
        devHorLayout->setContentsMargins(16, 0, 16, 0);


        QLabel *nameLabel = new QLabel(baseWidget);

        SwitchButton *appSwitch = new SwitchButton(baseWidget);

        devHorLayout->addWidget(iconBtn);
        devHorLayout->addWidget(nameLabel);
        devHorLayout->addStretch();
        devHorLayout->addWidget(appSwitch);

        baseWidget->setLayout(devHorLayout);

        applistverticalLayout->addWidget(baseWidget);

        //创建gsettings对象
        if(appname_CN == nullptr) {
            appname_CN = appname_US;
        }

        const QByteArray id(NOTICE_ORIGIN_SCHEMA);
        QGSettings *settings = nullptr;
        vecGsettins.append(settings);
        QString name = (!mEnv ? appname : appname_CN);
        QString path = QString("%1%2%3").arg(NOTICE_ORIGIN_PATH).arg(name).arg("/");
        settings = new QGSettings(id, path.toUtf8().data(), this);
       // qDebug()<<name;
        //判断该文件是否已创建了动态路径，未创建则创建后赋初值
        char *mfile_path,*mfile_path1;
        QByteArray ba1,ba2;

        //判断不同语言下是否创建动态路径
        if (!mEnv) {
            ba2 = (QString("%1%2").arg(appname_CN).arg("/")).toUtf8();
            mfile_path1 = ba2.data();
            ba1 = (QString("%1%2").arg(appname).arg("/")).toUtf8();
            mfile_path = ba1.data();
            nameLabel->setText(appname);
        } else {
            ba2 = (QString("%1%2").arg(appname).arg("/")).toUtf8();
            mfile_path1 = ba2.data();
            ba1 = (QString("%1%2").arg(appname_CN).arg("/")).toUtf8();
            mfile_path = ba1.data();
            nameLabel->setText(appname_CN);

        }
        //在已存在的动态路径列表中一一比较
        bool found = false;
        for (int j = 0; j < listChar.count(); j++) {
            if (!g_strcmp0(mfile_path, listChar.at(j))){
                found = true;
                break;
            }
        }
        if (!found || mEnv != isCN_env) {
            for (int j = 0; j < listChar.count(); j++) {
                if (!g_strcmp0(mfile_path1, listChar.at(j))){
                    QString path1 = QString("%1%2").arg(NOTICE_ORIGIN_PATH).arg(QString::fromUtf8(mfile_path1));
                    QGSettings *msettings = new QGSettings(id, path1.toUtf8().data(), this);

                    settings->set(NAME_KEY_CN, appname_CN);
                    settings->set(NAME_KEY_US, appname_US);
                    settings->set(MAXIMINE_KEY,msettings->get(MAXIMINE_KEY).toInt());
                    settings->set(MESSAGES_KEY,msettings->get(MESSAGES_KEY).toBool());
                    found = true;
                    delete msettings;
                    msettings = nullptr;
                    break;
                }
            }
        }



        if (!found){
           settings->set(NAME_KEY_CN, appname_CN);
           settings->set(NAME_KEY_US, appname_US);
           settings->set(MAXIMINE_KEY,3);
           settings->set(MESSAGES_KEY,true);
        }


        bool isCheck = settings->get(MESSAGES_KEY).toBool();
        appSwitch->setChecked(isCheck);

        connect(settings, &QGSettings::changed, [=](QString key) {
            if (static_cast<QString>(MESSAGES_KEY) == key) {
                bool judge = settings->get(MESSAGES_KEY).toBool();
                appSwitch->setChecked(judge);
            }
        });

        connect(appSwitch, &SwitchButton::checkedChanged, [=](bool checked) {
            settings->set(MESSAGES_KEY, checked);
        });
    }
}

void Notice::setHiddenNoticeApp(bool status)
{
    ui->frame->setVisible(status);
}

void Notice::loadlist()
{
    QLayoutItem *child;
    while ((child = applistverticalLayout->takeAt(0)) != nullptr)
    {
        child->widget()->setParent(nullptr);
        delete child;
    }
    initOriNoticeStatus();
    nSetting->set(IS_CN,mEnv);
    //重新加载列表
//    QTimer *timer = new QTimer(this);
//    count = 0;
//    connect(timer, &QTimer::timeout, this,[=](){
//        int i = count;
//        initOriNoticeStatus();
//        if (i == count) {
//            mstringlist->clear();
//            nSetting->set(IS_CN,mEnv);
//            timer->stop();
//        }
//    } );
//    timer->start(300);
}
