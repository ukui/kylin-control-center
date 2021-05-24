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

        ui->newfeatureWidget->setVisible(false);
        ui->lockscreenWidget->setVisible(false);

        ui->title2Label->setContentsMargins(0, 0, 0, 16);

        initTitleLabel();
        initSearchText();
        setupGSettings();
        setupComponent();
        initNoticeStatus();
        initOriNoticeStatus();
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

void Notice::initTitleLabel()
{
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    ui->title2Label->setFont(font);
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

    for (int i = 0; i < dir.count(); i++) {
        QString file_name = dir[i];  // 文件名称
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
        GDesktopAppInfo *audioinfo = g_desktop_app_info_new_from_filename(ba.constData());
        QString appname = g_app_info_get_name(G_APP_INFO(audioinfo));
        // 构建Widget
        QFrame *baseWidget = new QFrame();
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout *baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setMargin(0);

        HoverWidget *devWidget = new HoverWidget(appname, baseWidget);
        devWidget->setObjectName("hovorWidget");
        devWidget->setMinimumWidth(550);
        devWidget->setMaximumWidth(960);
        devWidget->setMinimumHeight(50);
        devWidget->setMaximumHeight(50);

        QHBoxLayout *devHorLayout = new QHBoxLayout();
        devHorLayout->setSpacing(8);
        devHorLayout->setContentsMargins(16, 0, 16, 0);

        QPushButton *iconBtn = new QPushButton();
        iconBtn->setStyleSheet("QPushButton{background-color:transparent;border-radius:4px}"
                               "QPushButton:hover{background-color: transparent ;color:transparent;}");

        QSizePolicy iconSizePolicy = iconBtn->sizePolicy();
        iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
        iconBtn->setIconSize(QSize(32, 32));
        iconBtn->setSizePolicy(iconSizePolicy);
        char *icon
            = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Icon", nullptr, nullptr);
        iconBtn->setIcon(QIcon::fromTheme(QString(icon),
                                          QIcon(QString("/usr/share/pixmaps/"+QString(QLatin1String(icon))
                                                        +".png"))));

        QLabel *nameLabel = new QLabel(pluginWidget);
        QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
        nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
        nameLabel->setSizePolicy(nameSizePolicy);
        nameLabel->setScaledContents(true);
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


        //获取已经存在的动态路径
        QList<char *> listChar =  listExistsCustomNoticePath();

        file_name = file_name.remove(".desktop");

        //创建gsettings对象，用来设置指定文件的message值
        const QByteArray id(NOTICE_ORIGIN_SCHEMA);
        QGSettings *settings = nullptr;
        vecGsettins.append(settings);
        QString path = QString("%1%2%3").arg(NOTICE_ORIGIN_PATH).arg(file_name).arg("/");
        settings = new QGSettings(id, path.toLatin1().data(), this);
        //判断该文件是否已创建了动态路径，未创建则为key("name")赋值
        if (!listChar.contains(file_name.toLatin1().data())){
           settings->set(NAME_KEY, file_name);
        }


        bool isCheck = settings->get(MESSAGES_KEY).toBool();
        appSwitch->setChecked(isCheck);


        connect(devWidget, &HoverWidget::enterWidget, this, [=](QString name) {
            Q_UNUSED(name)
            devWidget->setStyleSheet(
                "QWidget#hovorWidget{background-color: rgba(61,107,229,40%);border-radius:4px;}");
        });

        connect(devWidget, &HoverWidget::leaveWidget, this, [=](QString name) {
            Q_UNUSED(name)
            devWidget->setStyleSheet(
                "QWidget#hovorWidget{background-color: palette();border-radius:4px;}");
        });

        connect(devWidget, &HoverWidget::widgetClicked, this, [=](QString name) {
            AppDetail *app;
            app = new AppDetail(name, file_name, settings);
            app->exec();
        });

        connect(settings, &QGSettings::changed, [=](QString key) {
            if (static_cast<QString>(MESSAGES_KEY) == key) {
                qDebug()<<key;
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
