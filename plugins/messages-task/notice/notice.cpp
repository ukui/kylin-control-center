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

QString Notice::get_plugin_name() {
    return pluginName;
}

int Notice::get_plugin_type() {
    return pluginType;
}

QWidget * Notice::get_plugin_ui() {
    if (mFirstLoad) {
        ui = new Ui::Notice;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        appsName<<"ukui-power-statistics";
        appsKey<<"电源管理器";

        ui->setupUi(pluginWidget);

        mFirstLoad = false;

        ui->newfeatureWidget->setVisible(false);
        ui->lockscreenWidget->setVisible(false);

        ui->title2Label->setContentsMargins(0, 0, 0, 16);
        ui->applistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        initTitleLabel();
        initSearchText();
        setupGSettings();
        setupComponent();
        initNoticeStatus();
        initOriNoticeStatus();
    }
    return pluginWidget;
}

void Notice::plugin_delay_control() {

}

const QString Notice::name() const {

    return QStringLiteral("notice");
}

void Notice::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    ui->title2Label->setFont(font);
}

void Notice::initSearchText() {
    //~ contents_path /notice/Set notice type of operation center
    ui->noticeLabel->setText(tr("Set notice type of operation center"));
    //~ contents_path /notice/Notice Origin
    ui->title2Label->setText(tr("Notice Origin"));
}

void Notice::setupComponent() {
    newfeatureSwitchBtn = new SwitchButton(pluginWidget);
    enableSwitchBtn = new SwitchButton(pluginWidget);
    lockscreenSwitchBtn =  new SwitchButton(pluginWidget);

    ui->newfeatureHorLayout->addWidget(newfeatureSwitchBtn);
    ui->enableHorLayout->addWidget(enableSwitchBtn);
    ui->lockscreenHorLayout->addWidget(lockscreenSwitchBtn);

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

void Notice::setupGSettings() {
    if(QGSettings::isSchemaInstalled(NOTICE_SCHEMA)) {
        QByteArray id(NOTICE_SCHEMA);
        nSetting = new QGSettings(id, QByteArray(), this);
    }
}

void Notice::initNoticeStatus() {
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

void Notice::initOriNoticeStatus() {
    initGSettings();

    for (int i = 0; i < appsName.length(); i++) {
        QByteArray ba = QString(DESKTOPPATH + appsName.at(i) + ".desktop").toUtf8();
        GKeyFileFlags flags = G_KEY_FILE_NONE;
        GKeyFile *keyfile = g_key_file_new();
        g_key_file_load_from_file(keyfile, ba, flags, NULL);
        char *appname = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Name", nullptr,
                                                     nullptr);

        // 构建Widget
        QFrame * baseWidget = new QFrame();
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setContentsMargins(0, 0, 0, 2);

        HoverWidget * devWidget = new HoverWidget(appname,baseWidget);
        devWidget->setObjectName("hovorWidget");
        devWidget->setMinimumWidth(550);
        devWidget->setMaximumWidth(960);
        devWidget->setMinimumHeight(50);
        devWidget->setMaximumHeight(50);

        QHBoxLayout * devHorLayout = new QHBoxLayout();
        devHorLayout->setSpacing(8);
        devHorLayout->setContentsMargins(16, 0, 16, 0);

        QPushButton * iconBtn = new QPushButton();
        iconBtn->setStyleSheet("QPushButton{background-color:transparent;border-radius:4px}"
                        "QPushButton:hover{background-color: transparent ;color:transparent;}");

        QSizePolicy iconSizePolicy = iconBtn->sizePolicy();
        iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
        iconBtn->setIconSize(QSize(32,32));
        iconBtn->setSizePolicy(iconSizePolicy);
        QString iconame = appsName.at(i);
        if ("ukui-power-statistics" == appsName.at(i)) {
            iconame = "cs-power";
        }
        iconBtn->setIcon(QIcon::fromTheme(iconame));

        QLabel * nameLabel = new QLabel(pluginWidget);
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

        QListWidgetItem * item = new QListWidgetItem(ui->applistWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(QSizePolicy::Expanding, 52));

        ui->applistWidget->setItemWidget(item, baseWidget);

        QList<char *> listChar =  listExistsCustomNoticePath();

        const QByteArray id(NOTICE_ORIGIN_SCHEMA);
        QGSettings * settings = nullptr;
        QString path;

        for (int j = 0; j < listChar.length(); j++) {
            path = QString("%1%2").arg(NOTICE_ORIGIN_PATH).arg(QString(listChar.at(j)));
            settings = new QGSettings(id, path.toLatin1().data(), this);
            vecGsettins.append(settings);
            QStringList keys = settings->keys();

            if (keys.contains(static_cast<QString>(NAME_KEY))) {
                QString appName = settings->get(NAME_KEY).toString();
                if ( appsKey.at(i) == appName) {
                    bool isCheck = settings->get(MESSAGES_KEY).toBool();
                    appSwitch->setChecked(isCheck);
                    break;
                }
            }
            settings = nullptr;
        }

        connect(devWidget, &HoverWidget::enterWidget, this, [=](QString name) {
            Q_UNUSED(name)
            devWidget->setStyleSheet("QWidget#hovorWidget{background-color: rgba(61,107,229,40%);border-radius:4px;}");
        });

        connect(devWidget, &HoverWidget::leaveWidget, this, [=](QString name) {
            Q_UNUSED(name)
            devWidget->setStyleSheet("QWidget#hovorWidget{background: palette(button);border-radius:4px;}");
        });

        connect(devWidget, &HoverWidget::widgetClicked, this, [=](QString name) {
            AppDetail *app;
            app= new AppDetail(name,appsName.at(i), settings);
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
            changeAppstatus(checked, appname, appSwitch);
        });

        connect(appSwitch, &SwitchButton::checkedChanged, [=](bool checked) {
            settings->set(MESSAGES_KEY, checked);
        });
    }
    setHiddenNoticeApp(enableSwitchBtn->isChecked());
}


void Notice::initGSettings() {
    for (int i = 0; i < appsName.length(); i++) {
        QList<char *> listChar =  listExistsCustomNoticePath();

        const QByteArray id(NOTICE_ORIGIN_SCHEMA);
        QGSettings * settings = nullptr;
        QGSettings * newSettings = nullptr;
        QString path;
        bool isExist = false;

        for (int j = 0; j < listChar.length(); j++) {
            path = QString("%1%2").arg(NOTICE_ORIGIN_PATH).arg(QString(listChar.at(j)));
            settings = new QGSettings(id, path.toLatin1().data());
            QStringList keys = settings->keys();

            if (keys.contains(static_cast<QString>(NAME_KEY))) {
                QString appName = settings->get(NAME_KEY).toString();
                if (appsKey.at(i) == appName) {
                    isExist = true;
                }
            }
            delete settings;
            settings = nullptr;
        }
        if (!isExist) {
            path = findFreePath();
            newSettings = new QGSettings(id, path.toLatin1().data());
            QStringList keys = newSettings->keys();
            if (keys.contains(static_cast<QString>(NAME_KEY)) &&
                    keys.contains(static_cast<QString>(MESSAGES_KEY))) {
                newSettings->set(NAME_KEY, appsKey.at(i));
                newSettings->set(MESSAGES_KEY, true);
            }
            delete newSettings;
            newSettings = nullptr;
        }
    }
}

void Notice::changeAppstatus(bool checked, QString name, SwitchButton *appBtn) {

    // 记录应用之前状态
    bool judge;
    if (!checked) {
        judge = appBtn->isChecked();
        appMap.insert(name,judge);
        appBtn->setChecked(checked);
    } else {
        judge =appMap.value(name);
        appBtn->setChecked(judge);
    }
}

void Notice::setHiddenNoticeApp(bool status) {

    // To prevent jitter, need to be optimized
    for (int i = 0; i < ui->applistWidget->count(); i++) {
        QListWidgetItem * item = ui->applistWidget->item(i);
        item->setHidden(!status);
    }
}
