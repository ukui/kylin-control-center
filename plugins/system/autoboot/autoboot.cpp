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
#include "autoboot.h"
#include "ui_autoboot.h"
#include "SwitchButton/switchbutton.h"
#include "HoverWidget/hoverwidget.h"
#include "ImageUtil/imageutil.h"
#include "autobootworker.h"

#include <QThread>
#include <QSignalMapper>
#include <QDebug>
#include <QFont>
#include <QMouseEvent>
#include <QPushButton>
#include <QGSettings>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

#define ITEMWIDTH 522
#define ITEMHEIGHT 62
#define HEADHEIGHT 38

#define THEME_QT_SCHEMA  "org.ukui.style"
#define THEME_GTK_SCHEMA "org.mate.interface"

#define ICON_QT_KEY      "icon-theme-name"
#define ICON_GTK_KEY     "icon-theme"

AutoBoot::AutoBoot() : mFirstLoad(true)
{
    pluginName = tr("Auto Boot");
    pluginType = SYSTEM;
}

AutoBoot::~AutoBoot()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
        g_free(localconfigdir);
    }
}

QString AutoBoot::get_plugin_name()
{
    return pluginName;
}

int AutoBoot::get_plugin_type()
{
    return pluginType;
}

QWidget *AutoBoot::get_plugin_ui()
{
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::AutoBoot;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        connectToServer();
        initTitleLabel();
        initStyle();
        localconfigdir = g_build_filename(g_get_user_config_dir(), "autostart", NULL);

        // 初始化添加界面
        dialog = new AddAutoBoot();

        initConfig();
        initAddBtn();
        initUI();
        setupGSettings();
        initConnection();
    }
    return pluginWidget;
}

void AutoBoot::plugin_delay_control()
{
}

const QString AutoBoot::name() const
{
    return QStringLiteral("autoboot");
}

void AutoBoot::setupGSettings()
{
    const QByteArray id(THEME_QT_SCHEMA);
    mQtSettings = new QGSettings(id, QByteArray(), this);
}

void AutoBoot::initAddBtn()
{
    addWgt = new HoverWidget("", pluginWidget);
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet(
        "HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: rgb(64,169,251); border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel *iconLabel = new QLabel(pluginWidget);
    QLabel *textLabel = new QLabel(tr("Add autoboot app "), pluginWidget);
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    //iconLabel->setProperty("useIconHighlightEffect", true);
    //iconLabel->setProperty("iconHighlightEffectMode", 1);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");
    });
    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        Q_UNUSED(mname);
        dialog->exec();
    });

    ui->addLyt->addWidget(addWgt);
}

void AutoBoot::initTitleLabel()
{
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
}

void AutoBoot::initStyle()
{
    //~ contents_path /autoboot/Autoboot Settings
    ui->titleLabel->setText(tr("Autoboot Settings"));
}

void AutoBoot::initUI()
{
    _walk_config_dirs();

    appgroupMultiMaps.clear();

    int num = statusMaps.count();

    // 构建行头基础Widget
    QFrame *headbaseFrame = new QFrame(pluginWidget);
    headbaseFrame->setMinimumWidth(550);
    headbaseFrame->setMaximumWidth(960);
    headbaseFrame->setFrameShape(QFrame::Shape::Box);
    headbaseFrame->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *headbaseVerLayout = new QVBoxLayout(headbaseFrame);
    headbaseVerLayout->setSpacing(0);
    headbaseVerLayout->setContentsMargins(0, 0, 0, 2);

    // 构建行头
    QWidget *headWidget = new QWidget(pluginWidget);
    headWidget->setMinimumWidth(550);
    headWidget->setMaximumWidth(960);

    headWidget->setMinimumHeight(36);
    headWidget->setMaximumHeight(36);
    headWidget->setAttribute(Qt::WA_DeleteOnClose);
    headWidget->setObjectName("headWidget");

    QHBoxLayout *headHorLayout = new QHBoxLayout(headWidget);
    headHorLayout->setSpacing(16);
    headHorLayout->setContentsMargins(64, 0, 32, 0);

    QLabel *nameLabel = new QLabel(headWidget);
    nameLabel->setFixedWidth(220);
    nameLabel->setText(tr("Name"));

    QLabel *statusLabel = new QLabel(headWidget);
    statusLabel->setFixedWidth(150);
    statusLabel->setText(tr("Status"));

    headHorLayout->addWidget(nameLabel);
    headHorLayout->addStretch();
    headHorLayout->addWidget(statusLabel);
    headHorLayout->addStretch();

    headWidget->setLayout(headHorLayout);

    headbaseVerLayout->addWidget(headWidget);
    headbaseVerLayout->addStretch();

    ui->autoLayout->addWidget(headbaseFrame);

    // 构建每个启动项
    QSignalMapper *checkSignalMapper = new QSignalMapper(this);
    QMap<QString, AutoApp>::iterator it = statusMaps.begin();
    for (int index = 0; it != statusMaps.end(); it++, index++) {
        QString bname = it.value().bname;
        QString appName = it.value().name;

        QFrame *baseWidget = new QFrame(pluginWidget);
        baseWidget->setMinimumWidth(550);
        baseWidget->setMaximumWidth(960);
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout *baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setContentsMargins(0, 0, 0, 2);

        HoverWidget *widget = new HoverWidget(bname);
        widget->setMinimumWidth(550);
        widget->setMaximumWidth(960);

        widget->setMinimumHeight(60);
        widget->setMaximumHeight(60);

        widget->setAttribute(Qt::WA_DeleteOnClose);

        QHBoxLayout *mainHLayout = new QHBoxLayout(widget);
        mainHLayout->setContentsMargins(16, 0, 32, 0);
        mainHLayout->setSpacing(16);

        QLabel *iconLabel = new QLabel(widget);
        iconLabel->setFixedSize(32, 32);
        iconLabel->setPixmap(it.value().pixmap);

        QLabel *textLabel = new QLabel(widget);
        textLabel->setFixedWidth(250);
        textLabel->setText(appName);

        SwitchButton *button = new SwitchButton(widget);
        button->setAttribute(Qt::WA_DeleteOnClose);
        button->setChecked(!it.value().hidden);
        connect(button, SIGNAL(checkedChanged(bool)), checkSignalMapper, SLOT(map()));
        checkSignalMapper->setMapping(button, it.key());
        appgroupMultiMaps.insert(it.key(), button);

        QPushButton *dBtn = new QPushButton(widget);
        dBtn->setFixedSize(QSize(64, 32));
        dBtn->setText(tr("Delete"));
        dBtn->setHidden(true);
        connect(dBtn, &QPushButton::clicked, this, [=] {
            del_autoboot_realize(bname);
        });
        QLabel *pLabel = new QLabel(widget);
        pLabel->setFixedSize(QSize(64, 32));
        pLabel->setHidden(false);

        mainHLayout->addWidget(iconLabel);
        mainHLayout->addWidget(textLabel);
        mainHLayout->addStretch(1);
        mainHLayout->addWidget(button);
        mainHLayout->addStretch(1);
        mainHLayout->addWidget(pLabel);
        mainHLayout->addWidget(dBtn);
        widget->setLayout(mainHLayout);

        if (it.value().xdg_position == LOCALPOS) {
            connect(widget, &HoverWidget::enterWidget, this, [=](QString name){
                Q_UNUSED(name)
                dBtn->setHidden(false);
                pLabel->setHidden(true);
            });
            connect(widget, &HoverWidget::leaveWidget, this, [=](QString name){
                Q_UNUSED(name)
                dBtn->setHidden(true);
                pLabel->setHidden(false);
            });
        }

        baseVerLayout->addWidget(widget);
        baseVerLayout->addStretch();

        baseWidget->setLayout(baseVerLayout);

        ui->autoLayout->addWidget(baseWidget);
    }
    connect(checkSignalMapper, SIGNAL(mapped(QString)), this, SLOT(checkbox_changed_cb(QString)));
}

void AutoBoot::initConnection()
{
    connect(mQtSettings, &QGSettings::changed, this, [=] {
        QLayoutItem *child;
        while ((child = ui->autoLayout->takeAt(0)) != nullptr)
        {
            child->widget()->setParent(nullptr);
            delete child;
        }
        initUI();
    });
    connect(dialog, SIGNAL(autoboot_adding_signals(QString,QString,QString,QString,QString)),
            this, SLOT(add_autoboot_realize_slot(QString,QString,QString,QString,QString)));
}

bool AutoBoot::_copy_desktop_file_to_local(QString bname)
{
    QString srcPath;
    QString dstPath;

    QMap<QString, AutoApp>::iterator it = appMaps.find(bname);

    dstPath = QString(localconfigdir) + "/" + bname;
    srcPath = it.value().path;

    if (!QFile::copy(srcPath, dstPath))
        return false;

    // 更新数据
    AutoApp addapp;
    addapp = _app_new(dstPath.toLatin1().data());
    addapp.xdg_position = ALLPOS;

    localappMaps.insert(addapp.bname, addapp);

    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    updateit.value().xdg_position = ALLPOS;
    updateit.value().path = dstPath;

    return true;
}

void AutoBoot::clearAutoItem()
{
    if (ui->autoLayout->layout() != NULL) {
        QLayoutItem *item;
        while ((item = ui->autoLayout->layout()->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
            item = nullptr;
        }
    }
}

bool AutoBoot::_delete_local_autoapp(QString bname)
{
    char *dstpath;
    QByteArray ba = bname.toUtf8();

    dstpath = g_build_filename(localconfigdir, ba.data(), NULL);

    if (g_remove(dstpath) == -1) {
        g_free(dstpath);
        return false;
    }

    // 更新数据
    localappMaps.remove(bname);

    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "statusMaps Data Error when delete local file";
    else {
        if (updateit.value().xdg_position == LOCALPOS) {
            statusMaps.remove(bname);
        } else if (updateit.value().xdg_position == ALLPOS) {
            QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
            if (appit == appMaps.end())
                qDebug() << "appMaps Data Error when delete local file";
            else {
                updateit.value().hidden = appit.value().hidden;
                updateit.value().path = appit.value().path;
            }
            updateit.value().xdg_position = SYSTEMPOS;
        }
    }

    g_free(dstpath);
    return true;
}

bool AutoBoot::_enable_autoapp(QString bname, bool status)
{
    char *dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    // 获取并修改值
    GKeyFile *keyfile;
    GError *error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);

    if (error) {
        g_error_free(error);
        qDebug() << "Start autoboot failed  because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN,
                           !status);

    if (!_key_file_to_file(keyfile, dstpath)) {
        qDebug() << "Start autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    g_key_file_free(keyfile);

    // 更新数据
    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "Start autoboot failed because autoBoot Data Error";
    else {
        updateit.value().hidden = !status;
    }

    g_free(dstpath);
    return true;
}

bool AutoBoot::_delete_autoapp(QString bname)
{
    char *dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    // 获取并修改值
    GKeyFile *keyfile;
    GError *error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);

    if (error) {
        g_error_free(error);
        qDebug() << "Delete autoboot failed because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, true);

    if (!_key_file_to_file(keyfile, dstpath)) {
        qDebug() << "Delete autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    // 刷新界面
// ui->listWidget->clear();
    clearAutoItem();
    initUI();

    g_free(dstpath);
    g_free(keyfile);
    return true;
}

bool AutoBoot::_stop_autoapp(QString bname)
{
    char *dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    // 获取并修改值
    GKeyFile *keyfile;
    GError *error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);

    if (error) {
        g_error_free(error);
        qDebug() << "Stop autoboot failed because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, true);

    if (!_key_file_to_file(keyfile, dstpath)) {
        qDebug() << "Stop autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    g_key_file_free(keyfile);

    // 更新数据
    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "Stop autoboot failed because AutoBoot Data Error";
    else {
        updateit.value().hidden = true;
    }

    g_free(dstpath);
    return true;
}

gboolean AutoBoot::_key_file_to_file(GKeyFile *keyfile, const gchar *path)
{
    GError *werror;
    gchar *data;
    gsize length;
    gboolean res;

    werror = NULL;
    data = g_key_file_to_data(keyfile, &length, &werror);

    if (werror)
        return FALSE;

    res = g_file_set_contents(path, data, length, &werror);
    g_free(data);

    if (werror)
        return FALSE;

    return res;
}

gboolean AutoBoot::_key_file_get_shown(GKeyFile *keyfile, const char *currentdesktop)
{
    char **only_show_in, **not_show_in;
    gboolean found;

    if (!currentdesktop)
        return TRUE;

    only_show_in = g_key_file_get_string_list(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                              G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN, NULL, NULL);
    if (only_show_in) {
        found = FALSE;
        for (int i = 0; only_show_in[i] != NULL; i++) {
            if (g_strcmp0(currentdesktop, only_show_in[i]) == 0) {
                found = TRUE;
                break;
            }
        }
        g_strfreev(only_show_in);
        if (!found)
            return FALSE;
    }

    not_show_in = g_key_file_get_string_list(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                             G_KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN, NULL, NULL);
    if (not_show_in) {
        found = FALSE;
        for (int i = 0; not_show_in[i] != NULL; i++) {
            if (g_strcmp0(currentdesktop, not_show_in[i]) == 0) {
                found = TRUE;
                break;
            }
        }
        g_strfreev(not_show_in);
        if (found)
            return FALSE;
    }

    return TRUE;
}

gboolean AutoBoot::_key_file_get_boolean(GKeyFile *keyfile, const gchar *key, gboolean defaultvalue)
{
    GError *error;
    gboolean retval;

    error = NULL;
    retval = g_key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, key, &error);

    if (error != NULL) {
        retval = defaultvalue;
        g_error_free(error);
    }
    return retval;
}

AutoApp AutoBoot::_app_new(const char *path)
{
    AutoApp app;
    GKeyFile *keyfile;
    char *bname, *obpath, *name, *comment, *exec, *icon;
    bool hidden, no_display, enable, shown;

    app.bname = "";
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL)) {
        g_key_file_free(keyfile);
        return app;
    }

    bname = g_path_get_basename(path);
    obpath = g_strdup(path);
    hidden = _key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);
    no_display = _key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, FALSE);
// enable = _key_file_get_boolean(keyfile, APP_KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLE, TRUE);
    shown = _key_file_get_shown(keyfile, g_getenv("XDG_CURRENT_DESKTOP"));
    name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
    comment = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                           G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL, NULL);
    exec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC,
                                 NULL);
    icon = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_ICON, NULL, NULL);

    app.bname = QString::fromUtf8(bname);
    app.path = QString::fromUtf8(obpath);

    app.hidden = hidden;
    app.no_display = no_display;
    app.shown = shown;
// app.enable = enable;

    app.name = QString::fromUtf8(name);
    app.comment = QString::fromUtf8(comment);
    app.exec = QString::fromUtf8(exec);

    QFileInfo iconfile(static_cast<QString>(icon));

    if (!QString(icon).isEmpty() /*&& QIcon::hasThemeIcon(QString(icon))*/) {
        QIcon currenticon
            = QIcon::fromTheme(QString(icon),
                               QIcon(QString("/usr/share/pixmaps/"+QString(QLatin1String(icon))
                                             +".png")));
        app.pixmap = currenticon.pixmap(QSize(32, 32));
    } else if (iconfile.exists()) {
        app.pixmap = QPixmap(iconfile.filePath()).scaled(32, 32);
    } else {
        app.pixmap = QPixmap(QString(":/img/plugins/autoboot/desktop.png"));
    }

    g_free(bname);
    g_free(obpath);
    g_key_file_free(keyfile);

    return app;
}

void AutoBoot::_walk_config_dirs()
{
    const char * const *systemconfigdirs;
    GDir *dir;
    const char *name;

    appMaps.clear();

    systemconfigdirs = g_get_system_config_dirs(); // 获取系统配置目录
    for (int i = 0; systemconfigdirs[i]; i++) {
        char *path;
        path = g_build_filename(systemconfigdirs[i], "autostart", NULL);
        dir = g_dir_open(path, 0, NULL);
        if (!dir)
            continue;
        while ((name = g_dir_read_name(dir))) {
            AutoApp app;
            char *desktopfilepath;
            if (!g_str_has_suffix(name, ".desktop"))
                continue;
            desktopfilepath = g_build_filename(path, name, NULL);
            app = _app_new(desktopfilepath);
// if (app.bname == "" || app.hidden || app.no_display || !app.shown ||
// app.exec == "/usr/bin/ukui-settings-daemon") //gtk控制面板屏蔽ukui-settings-daemon,猜测禁止用户关闭
// continue;
            app.xdg_position = SYSTEMPOS;
            appMaps.insert(app.bname, app);
            g_free(desktopfilepath);
        }
        g_dir_close(dir);
    }

    localappMaps.clear();
    dir = g_dir_open(localconfigdir, 0, NULL);
    if (dir) {
        while ((name = g_dir_read_name(dir))) {
            AutoApp localapp;
            char *localdesktopfilepath;
            if (!g_str_has_suffix(name, ".desktop"))
                continue;
            localdesktopfilepath = g_build_filename(localconfigdir, name, NULL);
            localapp = _app_new(localdesktopfilepath);
            localapp.xdg_position = LOCALPOS;
            localappMaps.insert(localapp.bname, localapp);
            g_free(localdesktopfilepath);
        }
        g_dir_close(dir);
    }
    update_app_status();
}

void AutoBoot::update_app_status()
{
    statusMaps.clear();

    QMap<QString, AutoApp>::iterator it = appMaps.begin();
    for (; it != appMaps.end(); it++) {
        if (/*it.value().hidden || */ it.value().no_display || !it.value().shown
            || it.value().bname == "browser360-cn_preheat.desktop"
            || it.value().bname == "vmware-user.desktop"
            || it.value().exec == "/usr/bin/ukui-settings-daemon")  // gtk控制面板屏蔽ukui-settings-daemon,猜测禁止用户关闭
            continue;
        statusMaps.insert(it.key(), it.value());
    }

    QMap<QString, AutoApp>::iterator localit = localappMaps.begin();
    for (; localit != localappMaps.end(); localit++) {
        if (/*localit.value().hidden || */ localit.value().no_display || !localit.value().shown) {
            statusMaps.remove(localit.key());
            continue;
        }

        if (statusMaps.contains(localit.key())) {
            // 整合状态
            QMap<QString, AutoApp>::iterator updateit = statusMaps.find(localit.key());

            if (localit.value().hidden != updateit.value().hidden) {
                updateit.value().hidden = localit.value().hidden;
                updateit.value().path = localit.value().path;
                if (appMaps.contains(localit.key())) {
                    updateit.value().xdg_position = ALLPOS;
                }
            }
        } else {
            statusMaps.insert(localit.key(), localit.value());
        }
    }
}

void AutoBoot::add_autoboot_realize_slot(QString path, QString name, QString exec, QString comment,
                                         QString icon)
{
    if (exec.contains("kylin-screenshot")) {
        QStringList screenshotExec = exec.split(" ");
        exec = screenshotExec.at(0);
    }
    if (path.isEmpty())
        return;

    char *filename, *filepath;
    QByteArray ba = path.section("/", -1, -1).toUtf8();

    // filename = path.section("/", -1, -1).toUtf8().data();
    // 需要定位desktop解析失败原因，bug#37606
    // Fix: toUtf8 -> data 数据类型不可连转
    filename = ba.data();
    qDebug() << "desktop: "<< path.section("/", -1, -1).toUtf8().data();

    filepath = g_build_filename(localconfigdir, filename, NULL);
    if(!QFile::copy(path,filepath))
        return;
    clearAutoItem();
    initUI();
}

void AutoBoot::del_autoboot_realize(QString bname)
{
    QMap<QString, AutoApp>::iterator it = statusMaps.find(bname);
    if (it == statusMaps.end()) {
        qDebug() << "AutoBoot Data Error";
        return;
    }

    _delete_local_autoapp(bname);
    clearAutoItem();
    initUI();
}

void AutoBoot::checkbox_changed_cb(QString bname)
{
    foreach (QString key, appgroupMultiMaps.keys()) {
        if (key == bname) {
            QMap<QString, AutoApp>::iterator it = statusMaps.find(bname);
            if (it == statusMaps.end()) {
                qDebug() << "AutoBoot Data Error";
                return;
            }

            if (((SwitchButton *)appgroupMultiMaps.value(key))->isChecked()) { // 开启开机启动
                if (it.value().xdg_position == SYSTEMPOS) { //
                } else if (it.value().xdg_position == ALLPOS) { // 删除
                    QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
                    if (!appit.value().hidden) { // 直接删除
                        _delete_local_autoapp(bname);
                        // 更新状态
                        QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
                        if (updateit != statusMaps.end()) {
                            updateit.value().hidden = false;
                            updateit.value().xdg_position = SYSTEMPOS;
                            updateit.value().path = appit.value().path;
                        } else
                            qDebug() << "Update status failed when start autoboot";
                    }
                } else if (it.value().xdg_position == LOCALPOS) {// 改值
                    _enable_autoapp(bname, true);
                }
            } else { // 关闭
                if (it.value().xdg_position == SYSTEMPOS) { // 复制后改值
                    if (_copy_desktop_file_to_local(bname)) {
                        _stop_autoapp(bname);
                    }
                } else if (it.value().xdg_position == ALLPOS) {// 正常逻辑不应存在该情况,预防处理
                    QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
                    if (appit.value().hidden)
                        _delete_local_autoapp(bname);
                } else if (it.value().xdg_position == LOCALPOS) {// 改值
// _enable_autoapp(bname, false);
                    _stop_autoapp(bname);
                }
            }
        }
    }
}

void AutoBoot::connectToServer()
{
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"),
                                          QString(
                                              "org.freedesktop.kylinssoclient.interface"), "keyChanged", this,
                                          SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void AutoBoot::initConfig()
{
    // 不存在则创建~/.config/autostart/
    if (!g_file_test(localconfigdir, G_FILE_TEST_EXISTS)) {
        GFile *dstdirfile;
        dstdirfile = g_file_new_for_path(localconfigdir);
        gboolean status = g_file_make_directory(dstdirfile, NULL, NULL);
        if (!status) {
            qWarning() << "create autostart dir failed";
        }
    }
}

void AutoBoot::keyChangedSlot(const QString &key)
{
    if (key == "boot") {
        QLayoutItem *child;
        while ((child = ui->autoLayout->takeAt(0)) != 0) {
            // setParent为NULL，防止删除之后界面不消失
            if (child->widget()) {
                child->widget()->setParent(NULL);
            }
            delete child;
            child = nullptr;
        }
        initUI();
    }
}
