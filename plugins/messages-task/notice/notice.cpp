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

#define NOTICE_SCHEMA         "org.ukui.control-center.notice"
#define NEW_FEATURE_KEY       "show-new-feature"
#define ENABLE_NOTICE_KEY     "enable-notice"
#define SHOWON_LOCKSCREEN_KEY "show-on-lockscreen"

#define DESKTOPPATH           "/usr/share/applications/"

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

        ui->title2Label->setContentsMargins(0, 0, 0, 0);

        initSearchText();
        setupGSettings();
        setupComponent();
        initNoticeStatus();
        initOriNoticeStatus();
        //加载列表
        QTimer *mtimer = new QTimer(this);
        connect(mtimer, &QTimer::timeout, this,[=](){
            int i = count;
            initOriNoticeStatus();
            if (i == count) {
                mtimer->stop();
            }
        } );
        mtimer->start(500);
        //监视desktop文件列表
        QFileSystemWatcher *m_fileWatcher=new QFileSystemWatcher;
        m_fileWatcher->addPaths(QStringList()<<QString(DESKTOPPATH));
        //有应用卸载或安装时
        connect(m_fileWatcher,&QFileSystemWatcher::directoryChanged,[=](){
            //清空列表
            QLayoutItem *child;
            while ((child = applistverticalLayout->takeAt(0)) != nullptr)
            {
                child->widget()->setParent(nullptr);
                delete child;
            }
            //重新加载列表
            QTimer *timer = new QTimer(this);
            count = 0;
            connect(timer, &QTimer::timeout, this,[=](){
                int i = count;
                initOriNoticeStatus();
                if (i == count) {
                    timer->stop();
                }
            } );
            timer->start(300);
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
}

void Notice::initOriNoticeStatus()
{

    QDir dir(QString(DESKTOPPATH).toUtf8());

    QStringList filters;
    filters<<QString("*.desktop");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); // 设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);  // 设置文件名称过滤器，只为filters格式
    // 存储文件名称
    QStringList string_list;
    for (int i = 0; count < dir.count() && i < 10; count++,i++) {

        QString file_name = dir[count];  // 文件名称
        string_list.append(file_name);

        QByteArray ba = QString(DESKTOPPATH + file_name).toUtf8();
        GError **error = nullptr;
        GKeyFileFlags flags = G_KEY_FILE_NONE;
        GKeyFile *keyfile = g_key_file_new();
        g_key_file_load_from_file(keyfile, ba, flags, error);
        char *fname_1 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NoDisplay", nullptr,
                                                     nullptr);
        char *fname_2 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NotShowIn", nullptr,
                                                     nullptr);
        char *fname_3 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "OnlyShowIn",
                                                     nullptr, nullptr);
        char *appname = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Name",
                                                     nullptr, nullptr);
        char *appname_CN = g_key_file_get_string(keyfile, "Desktop Entry", "Name[zh_CN]",
                                                      nullptr);
        char *appname_US = g_key_file_get_string(keyfile, "Desktop Entry", "Name",
                                                 nullptr);
        if (fname_1 != nullptr) {
            QString str = QString::fromLocal8Bit(fname_1);
            if (str.contains("true")) {
                g_key_file_free(keyfile);
                continue;
            }
        } else if (fname_2 != nullptr) {
            QString str = QString::fromLocal8Bit(fname_2);
            if (str.contains("UKUI")) {
                g_key_file_free(keyfile);
                continue;
            }
        } else if (fname_3 != nullptr) {
            QString str = QString::fromLocal8Bit(fname_3);
            if (str.contains("LXQt") || str.contains("KDE")) {
                g_key_file_free(keyfile);
                continue;
            }
        }
        // 构建Widget
        QPushButton *iconBtn = new QPushButton(pluginWidget);
        char *icon
            = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Icon", nullptr, nullptr);
        iconBtn->setStyleSheet("QPushButton{background-color:transparent;border-radius:4px}"
                               "QPushButton:hover{background-color: transparent ;color:transparent;}");
        iconBtn->setIconSize(QSize(32, 32));
        iconBtn->setIcon(QIcon::fromTheme(QString(icon),
                                          QIcon(QString("/usr/share/pixmaps/"+QString(QLatin1String(icon))
                                                        +".png"))));


        QFrame *baseWidget = new QFrame();
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout *baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setMargin(0);

        HoverWidget *devWidget = new HoverWidget(appname);
        devWidget->setObjectName("hovorWidget");
        devWidget->setMinimumWidth(550);
        devWidget->setMaximumWidth(960);
        devWidget->setFixedHeight(50);
        QPalette pal;
        QBrush brush = pal.highlight();  //获取window的色值
        QColor highLightColor = brush.color();
        QString stringColor = QString("rgba(%1,%2,%3)") //叠加20%白色
               .arg(highLightColor.red()*0.8 + 255*0.2)
               .arg(highLightColor.green()*0.8 + 255*0.2)
               .arg(highLightColor.blue()*0.8 + 255*0.2);

        devWidget->setStyleSheet(QString("HoverWidget#hovorWidget{background: palette(button);\
                                       border-radius: 4px;}\
                                       HoverWidget:hover:!pressed#hovorWidget{background: %1;  \
                                       border-radius: 4px;}").arg(stringColor));

        QHBoxLayout *devHorLayout = new QHBoxLayout();
        devHorLayout->setSpacing(8);
        devHorLayout->setContentsMargins(16, 0, 16, 0);


        QLabel *nameLabel = new QLabel(pluginWidget);
        nameLabel->setText(appname);

        SwitchButton *appSwitch = new SwitchButton(pluginWidget);

        devHorLayout->addWidget(iconBtn);
        devHorLayout->addWidget(nameLabel);
        devHorLayout->addStretch();
        devHorLayout->addWidget(appSwitch);

        devWidget->setLayout(devHorLayout);
        baseVerLayout->addWidget(devWidget);
        baseVerLayout->addStretch();

        baseWidget->setLayout(baseVerLayout);
        applistverticalLayout->addWidget(baseWidget);


        file_name = file_name.remove(".desktop");
        //创建gsettings对象
        const QByteArray id(NOTICE_ORIGIN_SCHEMA);
        QGSettings *settings = nullptr;
        vecGsettins.append(settings);
        QString path = QString("%1%2%3").arg(NOTICE_ORIGIN_PATH).arg(file_name).arg("/");
        settings = new QGSettings(id, path.toLatin1().data(), this);

        //判断该文件是否已创建了动态路径，未创建则创建后赋初值
        char *mfile_path ;

        QByteArray ba1 = (QString("%1%2").arg(file_name).arg("/")).toLatin1();
        mfile_path = ba1.data();
        bool found = false;
        for (int j = 0; j < listChar.count(); j++) {
            if (!g_strcmp0(mfile_path, listChar.at(j))){
                found = true;
                break;
            }
        }
        if (!found){
           if(appname_CN == nullptr) {
               appname_CN = appname_US;
           }
           settings->set(NAME_KEY_CN, appname_CN);
           settings->set(NAME_KEY_US, appname_US);
           settings->set(MAXIMINE_KEY,3);
           settings->set(MESSAGES_KEY,true);
        }


        bool isCheck = settings->get(MESSAGES_KEY).toBool();
        appSwitch->setChecked(isCheck);


        connect(devWidget, &HoverWidget::enterWidget, this, [=](QString name) {
            Q_UNUSED(name)
            nameLabel->setStyleSheet("color: white;");
        });

        connect(devWidget, &HoverWidget::leaveWidget, this, [=](QString name) {
            Q_UNUSED(name)
            nameLabel->setStyleSheet("color: palette(windowText);");
        });

        connect(devWidget, &HoverWidget::widgetClicked, this, [=](QString name) {
            AppDetail *app;
            app = new AppDetail(name, file_name, settings,pluginWidget);
            app->exec();
        });

        connect(settings, &QGSettings::changed, [=](QString key) {
            if (static_cast<QString>(MESSAGES_KEY) == key) {
                bool judge = settings->get(MESSAGES_KEY).toBool();
                appSwitch->setChecked(judge);
            }
        });

        connect(enableSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {
            setHiddenNoticeApp(checked);
        });

        connect(appSwitch, &SwitchButton::checkedChanged, [=](bool checked) {
            settings->set(MESSAGES_KEY, checked);
        });
    }
    setHiddenNoticeApp(enableSwitchBtn->isChecked());
}

void Notice::setHiddenNoticeApp(bool status)
{
    ui->frame->setVisible(status);
}
