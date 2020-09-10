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
#include "desktop.h"
#include "ui_desktop.h"

#include <QGSettings>
#include "SwitchButton/switchbutton.h"
#include "realizedesktop.h"

#include <QDebug>
#include <QPushButton>

//#define DESKTOP_SCHEMA "org.ukui.peony.desktop"
#define DESKTOP_SCHEMA "org.ukui.control-center.desktop"

#define COMPUTER_VISIBLE_KEY "computer-icon-visible"
#define HOME_VISIBLE_KEY "home-icon-visible"
#define NETWORK_VISIBLE_KEY "network-icon-visible"
#define TRASH_VISIBLE_KEY "trash-icon-visible"
#define VOLUMES_VISIBLE_KEY "volumes-visible"

#define MENU_FULL_SCREEN_KEY "menufull-screen"
#define COMPUTER_LOCK_KEY "computer-icon-locking"
#define PERSONAL_LOCK_KEY "personal-icon-locking"
#define SETTINGS_LOCK_KEY "settings-icon-locking"
#define TRASH_LOCK_KEY "trash-icon-locking"

const QString kComputerLock = "computerIconLocking";
const QString kPersonalLock = "personalIconLocking";
const QString kSettingsLock = "settingsIconLocking";
const QString kTrashLock    = "trashIconLocking";

Desktop::Desktop()
{
    ui = new Ui::Desktop;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Desktop");
    pluginType = PERSONALIZED;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title3Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->menuLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    ui->titleLabel->setVisible(false);
//    ui->title2Label->setVisible(false);

    ui->deskComputerFrame->setVisible(false);
    ui->deskTrashFrame->setVisible(false);
    ui->deskHomeFrame->setVisible(false);
    ui->deskVolumeFrame->setVisible(false);
    ui->deskNetworkFrame->setVisible(false);

    ui->titleLabel->setVisible(false);
//    ui->title2Label->setVisible(false);

    ui->deskComputerFrame->setVisible(false);
    ui->deskTrashFrame->setVisible(false);
    ui->deskHomeFrame->setVisible(false);
    ui->deskVolumeFrame->setVisible(false);
    ui->deskNetworkFrame->setVisible(false);

    ui->title2Label->hide();
    ui->fullScreenMenuFrame->setVisible(false);

    vecGsettings = new QVector<QGSettings*>();
    const QByteArray id(DESKTOP_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)) {
        dSettings = new QGSettings(id);
    }

    initSearchText();
    initTranslation();
    setupComponent();
    setupConnect();
    initVisibleStatus();
    initLockingStatus();
    initTraySettings();
}

Desktop::~Desktop()
{
    delete ui;
    clearContent();
    if (!dSettings ){
        delete dSettings;
    }

    if (!vecGsettings) {
        delete vecGsettings;
    }
}

QString Desktop::get_plugin_name(){
    return pluginName;
}

int Desktop::get_plugin_type(){
    return pluginType;
}

QWidget *Desktop::get_plugin_ui(){
    return pluginWidget;
}

void Desktop::plugin_delay_control(){

}

const QString Desktop::name() const {

    return QStringLiteral("desktop");
}

void Desktop::initSearchText() {
    //~ contents_path /desktop/Icon Show On Desktop
    ui->titleLabel->setText(tr("Icon Show On Desktop"));
    //~ contents_path /desktop/Tray icon
    ui->title3Label->setText(tr("Tray icon"));
}

void Desktop::initTranslation() {
    transMap.insert("blueman", "蓝牙");
    transMap.insert("fcitx", "输入法");
    transMap.insert("indicator-china-weather", "麒麟天气");
    transMap.insert("explorer.exe", "微信");
    transMap.insert("ukui-flash-disk", "U盘管理工具");
    transMap.insert("kylin-nm", "网络工具");
    transMap.insert("ukui-volume-control-applet-qt", "音量控制");
    transMap.insert("ukui-sidebar", "侧边栏");
    transMap.insert("ukui-power-manager-tray", "电源管理");

    iconMap.insert("ukui-volume-control-applet-qt", "audio-card");
    iconMap.insert("kylin-nm", "network-workgroup");
    iconMap.insert("indicator-china-weather", "indicator-china-weather");
    iconMap.insert("ukui-flash-disk", "drive-removable-media");
    iconMap.insert("ukui-power-manager-tray", "cs-power");
    iconMap.insert("fcitx", "fcitx");
    iconMap.insert("blueman", "preferences-system-bluetooth");
    iconMap.insert("kylin-video", "kylin-video");

    disList<<"ukui-sidebar"<<"kylin-nm"<<"ukui-volume-control-applet-qt"<<"update-notifier"<<"software-update-available"
          <<"blueman-tray"<<"ukui-power-manager"<<"ukui-settings-daemon"<<"blueman-applet"
         <<"ErrorApplication"<<"livepatch";
}

void Desktop::setupComponent(){

    ui->deskComputerLabel->setPixmap(QPixmap("://img/plugins/desktop/computer.png"));
    ui->deskHomeLabel->setPixmap(QPixmap("://img/plugins/desktop/homefolder.png"));
    ui->deskTrashLabel->setPixmap(QPixmap("://img/plugins/desktop/trash.png"));
    ui->deskNetworkLabel->setPixmap(QPixmap("://img/plugins/desktop/default.png"));
    ui->deskVolumeLabel->setPixmap(QPixmap("://img/plugins/desktop/default.png"));

    deskComputerSwitchBtn = new SwitchButton(pluginWidget);
    ui->deskComputerHorLayout->addWidget(deskComputerSwitchBtn);

    deskTrashSwitchBtn = new SwitchButton(pluginWidget);
    ui->deskTrashHorLayout->addWidget(deskTrashSwitchBtn);

    deskHomeSwitchBtn = new SwitchButton(pluginWidget);
    ui->deskHomeHorLayout->addWidget(deskHomeSwitchBtn);

    deskVolumeSwitchBtn = new SwitchButton(pluginWidget);
    ui->deskVolumeHorLayout->addWidget(deskVolumeSwitchBtn);

    deskNetworkSwitchBtn = new SwitchButton(pluginWidget);
    ui->deskNetworkHorLayout->addWidget(deskNetworkSwitchBtn);

    ui->menuComputerLabel->setPixmap(QPixmap::fromImage(QIcon::fromTheme("computer").pixmap(32,32).toImage()));
    ui->menuFilesystemLabel->setPixmap(QPixmap::fromImage(QIcon::fromTheme("user-home").pixmap(32,32).toImage()));
    ui->menuSettingsLabel->setPixmap(QPixmap::fromImage(QIcon::fromTheme("ukui-control-center").pixmap(32,32).toImage()));
    ui->menuTrashLabel->setPixmap(QPixmap::fromImage(QIcon::fromTheme("user-trash").pixmap(32,32).toImage()));


    fullMenuSwitchBtn = new SwitchButton(pluginWidget);
    ui->fullScreenMenuLayout->addWidget(fullMenuSwitchBtn);

    menuComputerSwitchBtn = new SwitchButton(pluginWidget);
    ui->menuComputerHorLayout->addWidget(menuComputerSwitchBtn);

    menuTrashSwitchBtn = new SwitchButton(pluginWidget);
    ui->menuTrashHorLayout->addWidget(menuTrashSwitchBtn);

    menuFilesystemSwitchBtn = new SwitchButton(pluginWidget);
    ui->menuFilesystemHorLayout->addWidget(menuFilesystemSwitchBtn);

    menuSettingSwitchBtn = new SwitchButton(pluginWidget);
    ui->menuSettingHorLayout->addWidget(menuSettingSwitchBtn);
}

void Desktop::setupConnect(){
    QStringList keys = dSettings->keys();
    connect(deskComputerSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){dSettings->set(COMPUTER_VISIBLE_KEY, checked);});
    connect(deskTrashSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){dSettings->set(TRASH_VISIBLE_KEY, checked);});
    connect(deskHomeSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){dSettings->set(HOME_VISIBLE_KEY, checked);});
    connect(deskVolumeSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){dSettings->set(VOLUMES_VISIBLE_KEY, checked);});
    connect(deskNetworkSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){dSettings->set(NETWORK_VISIBLE_KEY, checked);});

    connect(fullMenuSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        if (keys.contains("menufullScreen")) {
            dSettings->set(MENU_FULL_SCREEN_KEY, checked);
        }
    });

    connect(menuComputerSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        dSettings->set(COMPUTER_LOCK_KEY, checked);
    });

    connect(menuFilesystemSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        if (keys.contains(kPersonalLock, Qt::CaseInsensitive)) {
            dSettings->set(PERSONAL_LOCK_KEY, checked);
        }
    });

    connect(menuSettingSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        dSettings->set(SETTINGS_LOCK_KEY, checked);
    });

    connect(menuTrashSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        dSettings->set(TRASH_LOCK_KEY, checked);
    });

    connect(dSettings, &QGSettings::changed, this, [=](const QString key) {
       bool status = dSettings->get(key).toBool();
       if (kComputerLock == key) {
           menuComputerSwitchBtn->setChecked(status);
       } else if (kPersonalLock == key) {
           menuFilesystemSwitchBtn->setChecked(status);
       } else if (kSettingsLock == key) {
           menuSettingSwitchBtn->setChecked(status);
       } else if (kTrashLock == key) {
           menuTrashSwitchBtn->setChecked(status);
       }
    });
}

void Desktop::initVisibleStatus(){
    deskComputerSwitchBtn->blockSignals(true);
    deskHomeSwitchBtn->blockSignals(true);
    deskTrashSwitchBtn->blockSignals(true);
    deskVolumeSwitchBtn->blockSignals(true);
    deskNetworkSwitchBtn->blockSignals(true);

    deskComputerSwitchBtn->setChecked(dSettings->get(COMPUTER_VISIBLE_KEY).toBool());
    deskHomeSwitchBtn->setChecked(dSettings->get(HOME_VISIBLE_KEY).toBool());
    deskTrashSwitchBtn->setChecked(dSettings->get(TRASH_VISIBLE_KEY).toBool());
    deskVolumeSwitchBtn->setChecked(dSettings->get(VOLUMES_VISIBLE_KEY).toBool());
    deskNetworkSwitchBtn->setChecked(dSettings->get(NETWORK_VISIBLE_KEY).toBool());

    deskComputerSwitchBtn->blockSignals(false);
    deskHomeSwitchBtn->blockSignals(false);
    deskTrashSwitchBtn->blockSignals(false);
    deskVolumeSwitchBtn->blockSignals(false);
    deskNetworkSwitchBtn->blockSignals(false);
}

void Desktop::initLockingStatus(){
    menuComputerSwitchBtn->blockSignals(true);
    menuFilesystemSwitchBtn->blockSignals(true);
    menuSettingSwitchBtn->blockSignals(true);
    menuTrashSwitchBtn->blockSignals(true);


    QStringList keys = dSettings->keys();
    if (keys.contains("menufullScreen")) {
        fullMenuSwitchBtn->setChecked(dSettings->get(MENU_FULL_SCREEN_KEY).toBool());
    }

    if (keys.contains("personalIconLocking")) {
        menuFilesystemSwitchBtn->setChecked(dSettings->get(PERSONAL_LOCK_KEY).toBool());
    }

    menuSettingSwitchBtn->setChecked(dSettings->get(SETTINGS_LOCK_KEY).toBool());
    menuTrashSwitchBtn->setChecked(dSettings->get(TRASH_LOCK_KEY).toBool());
    menuComputerSwitchBtn->setChecked(dSettings->get(COMPUTER_LOCK_KEY).toBool());

    menuComputerSwitchBtn->blockSignals(false);
    menuFilesystemSwitchBtn->blockSignals(false);
    menuSettingSwitchBtn->blockSignals(false);
    menuTrashSwitchBtn->blockSignals(false);
}

void Desktop::initTrayStatus(QString name, QIcon icon, QGSettings *gsettings) {
    nameList.append(name);
    const QString locale = QLocale::system().name();

    QVBoxLayout * baseVerLayout = new QVBoxLayout();
    baseVerLayout->setSpacing(1);

    QFrame * devFrame = new QFrame();
    devFrame->setObjectName(name);
    devFrame->setFrameShape(QFrame::Shape::Box);
    devFrame->setMinimumWidth(550);
    devFrame->setMaximumWidth(960);
    devFrame->setMinimumHeight(50);
    devFrame->setMaximumHeight(50);

    QHBoxLayout * devHorLayout = new QHBoxLayout();
    devHorLayout->setSpacing(8);
    devHorLayout->setContentsMargins(16, 0, 16, 0);

    QPushButton * iconBtn = new QPushButton();

    iconBtn->setStyleSheet("QPushButton{background-color:transparent;border-radius:4px}"
                                       "QPushButton:hover{background-color: transparent ;color:transparent;}");

//    iconBtn->setFlat(true);
    QSizePolicy iconSizePolicy = iconBtn->sizePolicy();
    iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    iconBtn->setSizePolicy(iconSizePolicy);
    iconBtn->setIconSize(QSize(32, 32));
    iconBtn->setIcon(icon);

    QLabel * nameLabel = new QLabel();
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setScaledContents(true);
    if ("zh_CN" == locale && transMap.contains(name)) {
        nameLabel->setText(transMap.value(name));
    } else {
        nameLabel->setText(name);
    }

    SwitchButton *appSwitch = new SwitchButton();
    if (disList.contains(name)) {
        appSwitch->setEnabled(false);
    }

    devHorLayout->addWidget(iconBtn);
    devHorLayout->addWidget(nameLabel);
    devHorLayout->addStretch();

    devHorLayout->addWidget(appSwitch);
    devFrame->setLayout(devHorLayout);
    baseVerLayout->addWidget(devFrame);
    baseVerLayout->addStretch();

//    ui->trayVBoxLayout->addWidget(devFrame);

    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);

    item->setSizeHint(QSize(QSizePolicy::Fixed, 50));
    item->setFlags(Qt::ItemFlag::ItemIsSelectable);
    item->setData(Qt::UserRole, QVariant(name));
    ui->listWidget->setItemWidget(item, devFrame);

    QString status = gsettings->get(TRAY_ACTION_KEY).toString();
    if ("tray" == status) {
        appSwitch->setChecked(true);
    } else {
        appSwitch->setChecked(false);
    }

    connect(appSwitch, &SwitchButton::checkedChanged, [=](bool checked) {
        if (checked) {
            gsettings->set(TRAY_ACTION_KEY, "tray");
            gsettings->set(TRAY_RECORD_KEY, "tray");
        } else {
            gsettings->set(TRAY_ACTION_KEY, "storage");
            gsettings->set(TRAY_RECORD_KEY, "storage");
        }
    });
}

void Desktop::initTraySettings() {
    QString action;
    QString name;
    int winID;

    QList<char *> trayList = listExistsCustomDesktopPath();

    int itemCount  = 0;
    ui->listWidget->setSpacing(1);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for (int i = 0; i < trayList.length(); i++) {
        const QByteArray id(TRAY_SCHEMA);
        QGSettings * traySettings = nullptr;
        QString path = QString("%1%2").arg(TRAY_SCHEMA_PATH).arg(QString(trayList.at(i)));;

        if (QGSettings::isSchemaInstalled(id)) {
            traySettings = new QGSettings(id, path.toLatin1().data());

            connect(traySettings, &QGSettings::changed, this, [=] (const QString &key) {

                QString status = traySettings->get(key).toString();
                if ("action" == key) {
                    QString status = traySettings->get(key).toString();
                    QString itemName = traySettings->get(TRAY_NAME_KEY).toString();
                    if ("freeze" == status) {
                        removeTrayItem(itemName);
                    } else if(!nameList.contains(itemName)) {
                        addTrayItem(traySettings);
                    }
                }
            });
            vecGsettings->append(traySettings);
            QStringList keys = traySettings->keys();

            if (keys.contains(static_cast<QString>(TRAY_NAME_KEY)) &&
                    keys.contains(static_cast<QString>(TRAY_ACTION_KEY))) {
                name = traySettings->get(TRAY_NAME_KEY).toString();
                action = traySettings->get(TRAY_ACTION_KEY).toString();
                winID = traySettings->get(TRAY_BINDING_KEY).toInt();
//                show(winID);
            }

            if (!("" == name || "freeze" == action || disList.contains(name))){
                QIcon icon;
                if (!iconMap[name].isEmpty()) {
                    icon = QIcon::fromTheme(iconMap[name]);
                } else {
                    icon = QIcon::fromTheme("application-x-desktop");
                }
                itemCount++;
                initTrayStatus(name, icon, traySettings);
            }
        }
    }
    ui->listWidget->setFixedHeight(itemCount * 50);
}

// TODO: double free?
void Desktop::clearContent() {

}

void Desktop::removeTrayItem(QString itemName) {

    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem * item = ui->listWidget->item(i);
        QString value = item->data(Qt::UserRole).toString();
        qDebug () << " item data: " << ui->listWidget->count() << value;
        if (value == itemName) {
            ui->listWidget->takeItem(i);
            break;
        }
    }

    for (int i = 0; i < nameList.length(); i++) {
        if (nameList.at(i) == itemName) {
            nameList.removeAt(i);
            break;
        }
    }
}

void Desktop::addTrayItem(QGSettings * trayGSetting) {

    QString name   = trayGSetting->get(TRAY_NAME_KEY).toString();
    QString action = trayGSetting->get(TRAY_ACTION_KEY).toString();

    if (!("" == name || "freeze" == action || disList.contains(name))){
        QIcon icon;
        if (!iconMap[name].isEmpty()) {
            icon = QIcon::fromTheme(iconMap[name]);
        } else {
            icon = QIcon::fromTheme("application-x-desktop");
        }
        initTrayStatus(name, icon, trayGSetting);
    }
}

