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
#include "shortcut.h"
#include "ui_shortcut.h"

#include <QListWidgetItem>
#include <QDebug>

#include "customlineedit.h"

#include "realizeshortcutwheel.h"
#include "defineshortcutitem.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gi18n.h>
}

#define ITEMHEIGH 36
#define TITLEWIDGETHEIGH 40
#define SYSTEMTITLEWIDGETHEIGH 50

// 快捷键屏蔽键
QStringList forbiddenKeys = {
    // Navigation keys
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Page_Up",
    "Page_Down",
    "End",
    "Tab",

    // Return
    "Return",
    "Enter",

    "Space",
};

QList<KeyEntry *> generalEntries;
QList<KeyEntry *> customEntries;

Shortcut::Shortcut() : mFirstLoad(true)
{
    pluginName = tr("Shortcut");
    pluginType = DEVICES;
}

Shortcut::~Shortcut()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
        delete pKeyMap;
        pKeyMap = nullptr;
    }
}

QString Shortcut::get_plugin_name()
{
    return pluginName;
}

int Shortcut::get_plugin_type()
{
    return pluginType;
}

QWidget *Shortcut::get_plugin_ui()
{
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::Shortcut;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        pKeyMap = new KeyMap;

        isCloudService = false;

        initTitleLabel();
        setupComponent();
        setupConnect();
        initFunctionStatus();
        connectToServer();
    }
    return pluginWidget;
}

void Shortcut::plugin_delay_control()
{
}

const QString Shortcut::name() const
{
    return QStringLiteral("shortcut");
}

void Shortcut::initTitleLabel()
{
    QFont font;
    font.setPixelSize(18);
    ui->systemLabel->setFont(font);
    ui->customLabel->setFont(font);
}

void Shortcut::connectToServer()
{
    cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                        "/org/kylinssoclient/path",
                                        "org.freedesktop.kylinssoclient.interface",
                                        QDBusConnection::sessionBus());
    if (!cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"),
                                          QString(
                                              "org.freedesktop.kylinssoclient.interface"), "shortcutChanged", this,
                                          SLOT(shortcutChangedSlot()));
    // 将以后所有DBus调用的超时设置为 milliseconds
    cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void Shortcut::setupComponent()
{
    ui->systemLabel->setText(tr("System Shortcut"));
    ui->customLabel->setText(tr("Customize Shortcut"));

    QWidget *systemTitleWidget = new QWidget;
    QVBoxLayout *systemVerLayout = new QVBoxLayout(systemTitleWidget);

    systemTitleWidget->setFixedHeight(SYSTEMTITLEWIDGETHEIGH);
    systemTitleWidget->setStyleSheet("QWidget{background: palette(window);"
                                     "border: none; border-radius: 4px}");
    systemVerLayout->setSpacing(0);
    systemVerLayout->setContentsMargins(16, 15, 19, 0);

    QLabel *titleLabel = new QLabel(systemTitleWidget);
    titleLabel->setText(tr("System Shortcut"));

    systemVerLayout->addWidget(titleLabel);
    systemVerLayout->addStretch();
    systemTitleWidget->setLayout(systemVerLayout);

    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}"
                          "HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel *iconLabel = new QLabel();
    QLabel *textLabel = new QLabel(tr("Add custom shortcut"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);

    iconLabel->setPixmap(pixgray);
    iconLabel->setProperty("useIconHighlightEffect", true);
    iconLabel->setProperty("iconHighlightEffectMode", 1);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    ui->addLyt->addWidget(addWgt);
}

void Shortcut::setupConnect()
{
    connect(addWgt, &HoverWidget::widgetClicked, this, [=](){
        addShortcutDialog *addDialog = new addShortcutDialog(generalEntries, customEntries);
        addDialog->setTitleText(QObject::tr("Customize Shortcut"));

        connect(addDialog, &addShortcutDialog::shortcutInfoSignal,
                [=](QString path, QString name, QString exec, QString key){
            createNewShortcut(path, name, exec, key);
        });

        addDialog->exec();
    });
}

void Shortcut::initFunctionStatus()
{
    generalEntries.clear();
    customEntries.clear();

    // 使用线程获取快捷键
    pThread = new QThread;
    pWorker = new GetShortcutWorker;
    if (isCloudService == false) {
        connect(pWorker, &GetShortcutWorker::generalShortcutGenerate, this,
                [=](QString schema, QString key, QString value){
            // qDebug() << "general shortcut" << schema << key << value;
            KeyEntry *generalKeyEntry = new KeyEntry;
            generalKeyEntry->gsSchema = schema;
            generalKeyEntry->keyStr = key;
            generalKeyEntry->valueStr = value;
            generalEntries.append(generalKeyEntry);
        });
    }
    connect(pWorker, &GetShortcutWorker::customShortcutGenerate, this,
            [=](QString path, QString name, QString binding, QString action){
        // qDebug() << "custom shortcut" << path << name << binding;
        KeyEntry *customKeyEntry = new KeyEntry;
        customKeyEntry->gsSchema = KEYBINDINGS_CUSTOM_SCHEMA;
        customKeyEntry->gsPath = path;
        customKeyEntry->nameStr = name;
        customKeyEntry->bindingStr = binding;
        customKeyEntry->actionStr = action;
        customEntries.append(customKeyEntry);
    });
    connect(pWorker, &GetShortcutWorker::workerComplete, this, [=] {
        pThread->quit(); // 退出事件循环
        pThread->wait(); // 释放资源
    });

    pWorker->moveToThread(pThread);
    connect(pThread, &QThread::started, pWorker, &GetShortcutWorker::run);
    connect(pThread, &QThread::finished, this, [=] {
        QMap<QString, QString> systemMap;
        QMap<QString, QString> desktopMap;

        // 最新快捷键数据
        for (int i = 0; i < generalEntries.count(); i++) {
            if (generalEntries[i]->gsSchema == KEYBINDINGS_DESKTOP_SCHEMA) {
                desktopMap.insert(generalEntries[i]->keyStr, generalEntries[i]->valueStr);
            } else if (generalEntries[i]->gsSchema == KEYBINDINGS_SYSTEM_SCHEMA) {
                systemMap.insert(generalEntries[i]->keyStr, generalEntries[i]->valueStr);
            }
        }

        desktopMap = MergerOfTheSamekind(desktopMap);

        QMap<QString, QMap<QString, QString> > generalMaps;
        if (desktopMap.count() != 0) {
            generalMaps.insert("Desktop", desktopMap);
        }
        // 构建系统快捷键界面
        appendGeneralItems(generalMaps);

        // 构建自定义快捷键界面
        appendCustomItems();
        isCloudService = false;
    });
    connect(pThread, &QThread::finished, pWorker, &GetShortcutWorker::deleteLater);

    pThread->start();
}

QMap<QString, QString> Shortcut:: MergerOfTheSamekind(QMap<QString, QString> desktopMap)
{
    QMap<QString, QString>::iterator it = desktopMap.begin();
    for (; it != desktopMap.end(); it++) {
        QString name = it.key().at(it.key().size() - 1);
        QString name_modification = it.key().left(it.key().length() - 1);
        if (name == '2') {
            desktopMap[name_modification] = desktopMap[name_modification]+" or "+it.value();
            desktopMap.erase(it);
            it = desktopMap.begin()+1;// 除之后要将指针指向后面一个
        }
    }
    return desktopMap;
}

QWidget *Shortcut::buildGeneralWidget(QString schema, QMap<QString, QString> subShortcutsMap)
{
    GSettingsSchema *pSettings;
    QString domain;

    if (schema == "Desktop") {
        pSettings
            = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory(
                                                  "/usr/share/glib-2.0/schemas/",
                                                  g_settings_schema_source_get_default(),
                                                  FALSE, NULL),
                                              KEYBINDINGS_DESKTOP_SCHEMA,
                                              FALSE);
        domain = "ukui-settings-daemon";
    } else if (schema == "System") {
        pSettings
            = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory(
                                                  "/usr/share/glib-2.0/schemas/",
                                                  g_settings_schema_source_get_default(),
                                                  FALSE, NULL),
                                              KEYBINDINGS_SYSTEM_SCHEMA,
                                              FALSE);
        domain = "gsettings-desktop-schemas";
    } else {
        return NULL;
    }

    QWidget *pWidget = new QWidget;
    pWidget->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *pVerLayout = new QVBoxLayout(pWidget);
    pVerLayout->setSpacing(2);
    // pVerLayout->setMargin(0);
    pVerLayout->setContentsMargins(0, 0, 0, 16);

    pWidget->setLayout(pVerLayout);

    QMap<QString, QString>::iterator it = subShortcutsMap.begin();

    for (; it != subShortcutsMap.end(); it++) {
        QWidget *gWidget = new QWidget;
        gWidget->setFixedHeight(TITLEWIDGETHEIGH);
        gWidget->setStyleSheet(
            "QWidget{background: palette(window); border: none; border-radius: 4px}");

        QHBoxLayout *gHorLayout = new QHBoxLayout(gWidget);
        gHorLayout->setSpacing(0);
        gHorLayout->setContentsMargins(16, 0, 19, 0);

        QByteArray ba = domain.toLatin1();
        QByteArray ba1 = it.key().toLatin1();

        GSettingsSchemaKey *keyObj = g_settings_schema_get_key(pSettings, ba1.data());

        char *i18nKey;
        QLabel *nameLabel = new QLabel(gWidget);
        i18nKey = const_cast<char *>(g_dgettext(ba.data(), g_settings_schema_key_get_summary(
                                                    keyObj)));
        nameLabel->setText(QString(i18nKey));

        QLabel *bindingLabel = new QLabel(gWidget);
        bindingLabel->setText(it.value());

        gHorLayout->addWidget(nameLabel);
        gHorLayout->addStretch();
        gHorLayout->addWidget(bindingLabel);

        gWidget->setLayout(gHorLayout);

        pVerLayout->addWidget(gWidget);

        g_settings_schema_key_unref(keyObj);
    }

    g_settings_schema_unref(pSettings);

    return pWidget;
}

void Shortcut::appendGeneralItems(QMap<QString, QMap<QString, QString> > shortcutsMap)
{
    QMap<QString, QMap<QString, QString> >::iterator it = shortcutsMap.begin();
    for (; it != shortcutsMap.end(); it++) {
        QWidget *gWidget = buildGeneralWidget(it.key(), it.value());
        if (gWidget != NULL) {
            gWidget->setMaximumWidth(960);
            ui->verticalLayout->addWidget(gWidget);
        }
    }
}

void Shortcut::appendCustomItems()
{
    for (KeyEntry *ckeyEntry : customEntries) {
        buildCustomItem(ckeyEntry);
    }
}

void Shortcut::buildCustomItem(KeyEntry *nkeyEntry)
{
    QPushButton *customBtn = new QPushButton();
    QHBoxLayout *customHorLayout = new QHBoxLayout(customBtn);

    customBtn->setContextMenuPolicy(Qt::ActionsContextMenu);
    customBtn->setFixedHeight(40);
    customBtn->setMaximumWidth(960);
    customHorLayout->setSpacing(24);
    customHorLayout->setContentsMargins(16, 0, 19, 0);

    QLabel *nameLabel = new QLabel(customBtn);
    QLabel *bindingLabel = new QLabel(customBtn);

    QFont ft;
    QFontMetrics fm(ft);
    QString nameStr = fm.elidedText(nkeyEntry->nameStr, Qt::ElideRight, 300);
    nameLabel->setText(nameStr);

    bindingLabel->setText(nkeyEntry->bindingStr);
    bindingLabel->setFixedWidth(240);
    bindingLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    customHorLayout->addWidget(nameLabel);
    customHorLayout->addStretch();
    customHorLayout->addWidget(bindingLabel);

    customBtn->setLayout(customHorLayout);
    ui->verticalLayout_3->addWidget(customBtn);

    QAction *edit = new QAction(customBtn);
    QAction *del = new QAction(customBtn);

    edit->setText(tr("Edit"));
    del->setText(tr("Delete"));
    // customBtn->addAction(edit);
    customBtn->addAction(del);
    connect(del, &QAction::triggered, this, [=](){
        customBtn->deleteLater();
        deleteCustomShortcut(nkeyEntry->gsPath);
    });
}

QString Shortcut::keyToUI(QString key)
{
    if (key.contains("+")) {
        QStringList keys = key.split("+");
        QString keyToUI = keys.join("    ");
        return keyToUI;
    }
    return key;
}

QString Shortcut::keyToLib(QString key)
{
    if (key.contains("+")) {
        QStringList keys = key.split("+");
        if (keys.count() == 2) {
            QString lower = keys.at(1);
            QString keyToLib = "<" + keys.at(0) + ">" + lower.toLower();
            qDebug() << "count = 2,keyToLib = " << keyToLib;
            return keyToLib;
        } else if (keys.count() == 3) {
            QString lower = keys.at(2);
            QString keyToLib = "<" + keys.at(0) + ">" + "<" + keys.at(1) + ">" + lower.toLower();
            qDebug() << "count = 3,keyToLib = " << keyToLib;
            return keyToLib;
        }
    }
    qDebug() << "count = 1,keyToLib = " << key;
    return key;
}

void Shortcut::createNewShortcut(QString path, QString name, QString exec, QString key)
{
    qDebug() << "createNewShortcut" << path << name << exec << key;
    QString availablepath;
    if (path.isEmpty()) {
        availablepath = findFreePath(); // 创建快捷键

        // 更新数据
        KeyEntry *nKeyentry = new KeyEntry;
        nKeyentry->gsPath = availablepath;
        nKeyentry->nameStr = name;
        nKeyentry->bindingStr = keyToLib(key);
        nKeyentry->actionStr = exec;

        customEntries.append(nKeyentry);

        buildCustomItem(nKeyentry);
    } else {
        availablepath = path; // 更新快捷键

        // 更新数据
        int i = 0;
        for (; i < customEntries.count(); i++) {
            if (customEntries[i]->gsPath == availablepath) {
                customEntries[i]->nameStr = name;
                customEntries[i]->actionStr = exec;
                break;
            }
        }
    }

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toLatin1().data());
    QGSettings *settings = new QGSettings(id, idd);

    settings->set(BINDING_KEY, keyToLib(key));
    settings->set(NAME_KEY, name);
    settings->set(ACTION_KEY, exec);

    delete settings;
    settings = nullptr;
}

void Shortcut::deleteCustomShortcut(QString path)
{
    if (path.isEmpty())
        return;

    QProcess p(0);
    QStringList args;

    char *fullpath = path.toLatin1().data();
    QString cmd = "dconf";

    args.append("reset");
    args.append("-f");
    args.append(fullpath);
    p.execute(cmd, args);// command是要执行的命令,args是参数
    qDebug()<<"wait for finish";
    p.waitForFinished(-1);
    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
}

bool Shortcut::keyIsForbidden(QString key)
{
    for (int i = 0; i < forbiddenKeys.length(); i++) {
        if (key == forbiddenKeys[i])
            return true;
    }
    return false;
}

void Shortcut::shortcutChangedSlot()
{
    qDebug() << "receive cloud service signal";
    while (ui->verticalLayout_3->count()) {
        QWidget *p = ui->verticalLayout_3->takeAt(0)->widget();
        ui->verticalLayout_3->removeWidget(p);
        p->deleteLater();
    }
    isCloudService = true;
    initFunctionStatus();
}
