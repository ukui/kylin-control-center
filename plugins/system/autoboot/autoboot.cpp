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


//struct SaveData : QObjectUserData {
//    QString bname;
//};

AutoBoot::AutoBoot(){
    ui = new Ui::AutoBoot;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Auto Boot");
    pluginType = SYSTEM;

//    ui->addFrame->installEventFilter(this);
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    //~ contents_path /autoboot/Autoboot Settings
    ui->titleLabel->setText(tr("Autoboot Settings"));


    localconfigdir = g_build_filename(g_get_user_config_dir(), "autostart", NULL);
    //初始化添加界面
    dialog = new AddAutoBoot();

    initAddBtn();
    initUI();

    connect(dialog, SIGNAL(autoboot_adding_signals(QString, QString,QString,QString)), this, SLOT(add_autoboot_realize_slot(QString ,QString,QString,QString)));
}

AutoBoot::~AutoBoot()
{
    delete ui;
//    delete dialog;
    g_free(localconfigdir);
}

QString AutoBoot::get_plugin_name(){
    return pluginName;
}

int AutoBoot::get_plugin_type(){
    return pluginType;
}

QWidget *AutoBoot::get_plugin_ui(){
    return pluginWidget;
}

void AutoBoot::plugin_delay_control(){

}

const QString AutoBoot::name() const {
    return QStringLiteral("autoboot");
}

void AutoBoot::initAddBtn() {
    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add autoboot app "));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });
    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        dialog->exec();
    });

    ui->addLyt->addWidget(addWgt);

}

void AutoBoot::initUI(){
    _walk_config_dirs();

    appgroupMultiMaps.clear();

    int num = statusMaps.count();

    //显示全部ITEM，设置高
//    ui->listWidget->setFixedHeight(num * ITEMHEIGHT + HEADHEIGHT);

    //构建行头基础Widget
    QFrame * headbaseFrame = new QFrame;
    headbaseFrame->setMinimumWidth(550);
    headbaseFrame->setMaximumWidth(960);
    headbaseFrame->setFrameShape(QFrame::Shape::Box);
    headbaseFrame->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * headbaseVerLayout = new QVBoxLayout(headbaseFrame);
    headbaseVerLayout->setSpacing(0);
    headbaseVerLayout->setContentsMargins(0, 0, 0, 2);

    //构建行头
    QWidget * headWidget = new QWidget;
    headWidget->setMinimumWidth(550);
    headWidget->setMaximumWidth(960);

    headWidget->setMinimumHeight(36);
    headWidget->setMaximumHeight(36);
    headWidget->setAttribute(Qt::WA_DeleteOnClose);
    headWidget->setObjectName("headWidget");
//    headWidget->setFixedHeight(36);
//    headWidget->setStyleSheet("QWidget#headWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");

    QHBoxLayout * headHorLayout = new QHBoxLayout(headWidget);
    headHorLayout->setSpacing(16);
    headHorLayout->setContentsMargins(64, 0, 32, 0);

    QLabel * nameLabel = new QLabel(headWidget);
//    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
//    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
//    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setFixedWidth(220);
    nameLabel->setText(tr("Name"));
//    nameLabel->setStyleSheet("background: #F4F4F4;");

    QLabel * statusLabel = new QLabel(headWidget);
//    QSizePolicy statusSizePolicy = statusLabel->sizePolicy();
//    statusSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
//    statusLabel->setSizePolicy(statusSizePolicy);
    statusLabel->setFixedWidth(68);
    statusLabel->setText(tr("Status"));
//    statusLabel->setStyleSheet("background: #F4F4F4;");

    headHorLayout->addWidget(nameLabel);
    headHorLayout->addStretch();
    headHorLayout->addWidget(statusLabel);
    headHorLayout->addStretch();

    headWidget->setLayout(headHorLayout);

    headbaseVerLayout->addWidget(headWidget);
    headbaseVerLayout->addStretch();

//    headbaseFrame->setLayout(headbaseVerLayout);

//    QListWidgetItem * hItem = new QListWidgetItem(ui->listWidget);
//    hItem->setSizeHint(QSize(ITEMWIDTH, HEADHEIGHT));
//    ui->listWidget->setItemWidget(hItem, headbaseFrame);
    ui->autoLayout->addWidget(headbaseFrame);

    //构建每个启动项
    QSignalMapper * checkSignalMapper = new QSignalMapper(this);
    QMap<QString, AutoApp>::iterator it = statusMaps.begin();
    for (int index = 0; it != statusMaps.end(); it++, index++){
        QString bname = it.value().bname;
        QString appName = it.value().name;

        QFrame * baseWidget = new QFrame;
        baseWidget->setMinimumWidth(550);
        baseWidget->setMaximumWidth(960);
        baseWidget->setFrameShape(QFrame::Shape::Box);
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setContentsMargins(0, 0, 0, 2);

        HoverWidget * widget = new HoverWidget(bname);
//        widget->setFixedHeight(60); //
        widget->setMinimumWidth(550);
        widget->setMaximumWidth(960);

        widget->setMinimumHeight(60);
        widget->setMaximumHeight(60);

        widget->setAttribute(Qt::WA_DeleteOnClose);
//        widget->setStyleSheet("background: #F4F4F4;");

        QHBoxLayout * mainHLayout = new QHBoxLayout(widget);
        mainHLayout->setContentsMargins(16, 0, 32, 0);
        mainHLayout->setSpacing(16);

        QLabel * iconLabel = new QLabel(widget);
        iconLabel->setFixedSize(32, 32);
        iconLabel->setPixmap(it.value().pixmap);
//        iconLabel->setStyleSheet("background: #F4F4F4");

        QLabel * textLabel = new QLabel(widget);
//        textLabel->setStyleSheet("background: #F4F4F4");
        textLabel->setFixedWidth(250);
        textLabel->setText(appName);

        SwitchButton * button = new SwitchButton();
        button->setAttribute(Qt::WA_DeleteOnClose);
//        button->setChecked(it.value().enable);
        button->setChecked(!it.value().hidden);
        connect(button, SIGNAL(checkedChanged(bool)), checkSignalMapper, SLOT(map()));
        checkSignalMapper->setMapping(button, it.key());
        appgroupMultiMaps.insert(it.key(), button);

        QPushButton * dBtn = new QPushButton(widget);
        dBtn->setFixedSize(QSize(64, 32));
        dBtn->setText(tr("Delete"));
        dBtn->setHidden(true);
        connect(dBtn, &QPushButton::clicked, this, [=]{
            del_autoboot_realize(bname);
        });
//        dBtn->setStyleSheet(""
//                            "QPushButton{background: #FA6056; border-radius: 2px;}"
//                            "QPushButton:hover:pressed{background: #E54A50; border-radius: 2px;}");

        QLabel * pLabel = new QLabel(widget);
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

        if (it.value().xdg_position == LOCALPOS){
            connect(widget, &HoverWidget::enterWidget, this, [=](QString name){
                Q_UNUSED(name)
                dBtn->setHidden(false);
                pLabel->setHidden(true);
                //            widget->setStyleSheet("background: #EEF2FD;");

            });
            connect(widget, &HoverWidget::leaveWidget, this, [=](QString name){
                Q_UNUSED(name)
                dBtn->setHidden(true);
                pLabel->setHidden(false);
                //            widget->setStyleSheet("background: #F4F4F4;");
            });
        }

        baseVerLayout->addWidget(widget);
        baseVerLayout->addStretch();

        baseWidget->setLayout(baseVerLayout);

//        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
//        item->setSizeHint(QSize(ITEMWIDTH, ITEMHEIGHT));
//        ui->listWidget->setItemWidget(item, baseWidget);
//        ui->listWidget->setSpacing(1);
        ui->autoLayout->addWidget(baseWidget);
    }
    connect(checkSignalMapper, SIGNAL(mapped(QString)), this, SLOT(checkbox_changed_cb(QString)));
}


bool AutoBoot::_copy_desktop_file_to_local(QString bname){
//    GFile * srcfile;
//    GFile * dstfile;
//    GError * error;
//    char * dstpath, * srcpath;

    QString srcPath;
    QString dstPath;

    //不存在则创建~/.config/autostart/
    if (!g_file_test(localconfigdir, G_FILE_TEST_EXISTS)){
        GFile * dstdirfile;
        dstdirfile = g_file_new_for_path(localconfigdir);
        g_file_make_directory(dstdirfile, NULL, NULL);
    }

    QMap<QString, AutoApp>::iterator it = appMaps.find(bname);
//    dstpath = g_build_filename(localconfigdir, bname.toLatin1().data(), NULL);
//    srcpath = it.value().path.toLatin1().data();

    dstPath = QString(localconfigdir) + "/" + bname;
    srcPath = it.value().path;

//    srcfile = g_file_new_for_path(srcpath);
//    dstfile = g_file_new_for_path(dstpath);

//    if (!g_file_copy(srcfile, dstfile, G_FILE_COPY_NONE, NULL, NULL, NULL, &error)){
//        qDebug() << "Could not copy desktop file for autoboot";
//        g_object_unref(srcfile);
//        g_object_unref(dstfile);
//        g_free(dstpath);
//        return false;
//    }

    if (!QFile::copy(srcPath, dstPath))
        return false;

    //更新数据
    AutoApp addapp;
    addapp = _app_new(dstPath.toLatin1().data());
    addapp.xdg_position = ALLPOS;

    localappMaps.insert(addapp.bname, addapp);

    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    updateit.value().xdg_position = ALLPOS;
    updateit.value().path = dstPath;

//    g_object_unref(srcfile);
//    g_object_unref(dstfile);
//    g_free(dstpath);
    return true;
}

void AutoBoot::clearAutoItem()
{
    if (ui->autoLayout->layout() != NULL) {
        QLayoutItem* item;
        while ((item = ui->autoLayout->layout()->takeAt(0)) != NULL )
        {
            delete item->widget();
            delete item;
        }
//        delete ui->availableLayout->layout();
    }
}

bool AutoBoot::_delete_local_autoapp(QString bname){
    char * dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    if (g_remove(dstpath) == -1){
        g_free(dstpath);
        return false;
    }

    //更新数据
    localappMaps.remove(bname);

    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "statusMaps Data Error when delete local file";
    else{

        if (updateit.value().xdg_position == LOCALPOS){
            statusMaps.remove(bname);
        } else if (updateit.value().xdg_position == ALLPOS){
            QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
            if (appit == appMaps.end())
                qDebug() << "appMaps Data Error when delete local file";
            else{
                updateit.value().hidden = appit.value().hidden;
                updateit.value().path = appit.value().path;
            }
            updateit.value().xdg_position = SYSTEMPOS;
        }
    }

    g_free(dstpath);
    return true;
}

bool AutoBoot::_enable_autoapp(QString bname, bool status){
    char * dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    //获取并修改值
    GKeyFile * keyfile;
    GError * error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);


    if (error){
        g_error_free(error);
        qDebug() << "Start autoboot failed  because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, !status);

    if (!_key_file_to_file(keyfile, dstpath)){
        qDebug() << "Start autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    g_key_file_free(keyfile);

    //更新数据
    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "Start autoboot failed because autoBoot Data Error";
    else{
        updateit.value().hidden = !status;
    }

    g_free(dstpath);
    return true;

}

bool AutoBoot::_delete_autoapp(QString bname){
    char * dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    //获取并修改值
    GKeyFile * keyfile;
    GError * error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);


    if (error){
        g_error_free(error);
        qDebug() << "Delete autoboot failed because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, true);

    if (!_key_file_to_file(keyfile, dstpath)){
        qDebug() << "Delete autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    //刷新界面
//    ui->listWidget->clear();
    clearAutoItem();
    initUI();

    g_free(dstpath);
    return true;
}

bool AutoBoot::_stop_autoapp(QString bname){

    char * dstpath;

    dstpath = g_build_filename(localconfigdir, bname.toUtf8().data(), NULL);

    //获取并修改值
    GKeyFile * keyfile;
    GError * error;
    keyfile = g_key_file_new();
    error = NULL;
    g_key_file_load_from_file(keyfile, dstpath, G_KEY_FILE_KEEP_COMMENTS, &error);


    if (error){
        g_error_free(error);
        qDebug() << "Stop autoboot failed because keyfile load from file error";
        g_free(dstpath);
        return false;
    }

    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, true);

    if (!_key_file_to_file(keyfile, dstpath)){
        qDebug() << "Stop autoboot failed because could not save desktop file";
        g_free(dstpath);
        return false;
    }

    g_key_file_free(keyfile);

    //更新数据
    QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
    if (updateit == statusMaps.end())
        qDebug() << "Stop autoboot failed because AutoBoot Data Error";
    else{
        updateit.value().hidden = true;
    }

    g_free(dstpath);
    return true;
}

gboolean AutoBoot::_key_file_to_file(GKeyFile *keyfile, const gchar *path){
    GError * werror;
    gchar * data;
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

gboolean AutoBoot::_key_file_get_shown(GKeyFile *keyfile, const char *currentdesktop){
    char ** only_show_in, ** not_show_in;
    gboolean found;

    if (!currentdesktop)
        return TRUE;

    only_show_in = g_key_file_get_string_list(keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                              G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN, NULL, NULL);
    if (only_show_in){
        found = FALSE;
        for (int i = 0; only_show_in[i] != NULL; i++){
            if (g_strcmp0(currentdesktop, only_show_in[i]) == 0){
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
    if (not_show_in){
        found = FALSE;
        for (int i = 0; not_show_in[i] != NULL; i++){
            if (g_strcmp0(currentdesktop, not_show_in[i]) == 0){
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

gboolean AutoBoot::_key_file_get_boolean(GKeyFile *keyfile, const gchar *key, gboolean defaultvalue){
    GError * error;
    gboolean retval;

    error = NULL;
    retval = g_key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, key, &error);

    if (error != NULL){
        retval = defaultvalue;
        g_error_free(error);
    }
    return retval;
}

AutoApp AutoBoot::_app_new(const char *path){
    AutoApp app;
    GKeyFile * keyfile;
    char * bname, * obpath, *name, * comment, * exec, * icon;
    bool  hidden, no_display, enable, shown;

    app.bname = "";
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL)){
        g_key_file_free (keyfile);
        return app;
    }

    bname = g_path_get_basename(path);
    obpath = g_strdup(path);
    hidden = _key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);
    no_display = _key_file_get_boolean(keyfile, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, FALSE);
//    enable = _key_file_get_boolean(keyfile, APP_KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLE, TRUE);
    shown = _key_file_get_shown(keyfile, g_getenv("XDG_CURRENT_DESKTOP"));
    name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
    comment = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL, NULL);
    exec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);    
    icon = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, NULL, NULL);

    app.bname = QString::fromUtf8(bname);
    app.path = QString::fromUtf8(obpath);

    app.hidden = hidden;
    app.no_display = no_display;
    app.shown = shown;
//    app.enable = enable;

    app.name = QString::fromUtf8(name);
    app.comment = QString::fromUtf8(comment);
    app.exec = QString::fromUtf8(exec);
    if (!QString(icon).isEmpty() && QIcon::hasThemeIcon(QString(icon))){
        QIcon currenticon = QIcon::fromTheme(QString(icon));
        app.pixmap = currenticon.pixmap(QSize(32, 32));
    }
    else{
        app.pixmap = QPixmap(QString(":/img/plugins/autoboot/desktop.png"));
    }

    g_free(bname);
    g_free(obpath);
    g_key_file_free(keyfile);

    return app;
}

void AutoBoot::_walk_config_dirs(){
    const char * const * systemconfigdirs;
    GDir * dir;
    const char * name;

    appMaps.clear();

    systemconfigdirs = g_get_system_config_dirs(); //获取系统配置目录
    for (int i = 0; systemconfigdirs[i]; i++){
        char * path;
        path = g_build_filename(systemconfigdirs[i], "autostart", NULL);
        dir = g_dir_open(path, 0, NULL);
        if (!dir)
            continue;
        while ((name = g_dir_read_name(dir))) {
            AutoApp app;
            char * desktopfilepath;
            if (!g_str_has_suffix(name, ".desktop"))
                continue;
            desktopfilepath = g_build_filename(path, name, NULL);
            app = _app_new(desktopfilepath);
//            if (app.bname == "" || app.hidden || app.no_display || !app.shown ||
//                    app.exec == "/usr/bin/ukui-settings-daemon") //gtk控制面板屏蔽ukui-settings-daemon,猜测禁止用户关闭
//                continue;
            app.xdg_position = SYSTEMPOS;
            appMaps.insert(app.bname, app);
            g_free (desktopfilepath);
        }
        g_dir_close(dir);
    }

    localappMaps.clear();
    dir = g_dir_open(localconfigdir, 0, NULL);
    if (dir){
        while ((name = g_dir_read_name(dir))) {
            AutoApp localapp;
            char * localdesktopfilepath;
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

void AutoBoot::update_app_status(){
    statusMaps.clear();

    QMap<QString, AutoApp>::iterator it = appMaps.begin();
    for (; it != appMaps.end(); it++){
        if (/*it.value().hidden || */it.value().no_display || !it.value().shown ||
                it.value().exec == "/usr/bin/ukui-settings-daemon") //gtk控制面板屏蔽ukui-settings-daemon,猜测禁止用户关闭
            continue;
        statusMaps.insert(it.key(), it.value());
    }

    QMap<QString, AutoApp>::iterator localit = localappMaps.begin();
    for (; localit != localappMaps.end(); localit++){
        if (/*localit.value().hidden || */localit.value().no_display || !localit.value().shown){
            statusMaps.remove(localit.key());
            continue;
        }

        if (statusMaps.contains(localit.key())){
            //整合状态
            QMap<QString, AutoApp>::iterator updateit = statusMaps.find(localit.key());

//            if (localit.value().enable != updateit.value().enable){
//                updateit.value().enable = localit.value().enable;
//                updateit.value().path = localit.value().path;
//                if (appMaps.contains(localit.key()))
//                    updateit.value().xdg_position = ALLPOS;
//            }
            if (localit.value().hidden != updateit.value().hidden){
                updateit.value().hidden = localit.value().hidden;
                updateit.value().path = localit.value().path;
                if (appMaps.contains(localit.key())){
                    updateit.value().xdg_position = ALLPOS;
                }
            }
        }
        else{
            statusMaps.insert(localit.key(), localit.value());
        }

    }
}


void AutoBoot::add_autoboot_realize_slot(QString path, QString name, QString exec, QString comment){
    if (path.isEmpty())
        return;

    char * filename, * filepath;

    filename = path.section("/", -1, -1).toUtf8().data();


    if (!g_file_test(localconfigdir, G_FILE_TEST_EXISTS)){
        GFile * dstdirfile;
        dstdirfile = g_file_new_for_path(localconfigdir);
        g_file_make_directory(dstdirfile, NULL, NULL);
    }

    filepath = g_build_filename(localconfigdir, filename, NULL);

    GKeyFile * keyfile;
    keyfile = g_key_file_new();

    const char * locale = const_cast<const char *>(QLocale::system().name().toUtf8().data());
    char * type = QString("Application").toUtf8().data();

    g_key_file_set_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_TYPE, type);
//    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, APP_KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLE, true);
    g_key_file_set_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC, exec.toUtf8().data());
    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_HIDDEN, false);
    g_key_file_set_boolean(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, false);
    g_key_file_set_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, name.toUtf8().data());
    g_key_file_set_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, locale, name.toUtf8().data());
    g_key_file_set_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT, comment.toUtf8().data());
//    g_key_file_set_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT, locale, comment.toUtf8().data());

    if (!_key_file_to_file(keyfile, filepath))
        qDebug() << "Could not save desktop file";

    g_key_file_free(keyfile);
    g_free(filepath);

    //refresh
//    ui->listWidget->clear();
    clearAutoItem();
    initUI();

}

void AutoBoot::del_autoboot_realize(QString bname){

    QMap<QString, AutoApp>::iterator it = statusMaps.find(bname);
    if (it == statusMaps.end()){
        qDebug() << "AutoBoot Data Error";
        return;
    }

//    if (it.value().xdg_position == SYSTEMPOS){ //复制改值
//        if (_copy_desktop_file_to_local(bname)){
//            _delete_autoapp(bname);
//        }
//    }
//    else if (it.value().xdg_position == ALLPOS){ //改值
//        _delete_autoapp(bname);

//    }
//    else if (it.value().xdg_position == LOCALPOS){ //删除
        _delete_local_autoapp(bname);
//        ui->listWidget->clear();
        clearAutoItem();
        initUI();
        //    }
}



//bool AutoBoot::eventFilter(QObject *watched, QEvent *event)
//{
//    if (watched == ui->addFrame){
//        if (event->type() == QEvent::MouseButtonPress){
//            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
//            if (mouseEvent->button() == Qt::LeftButton){
//                AddAutoBoot * mdialog = new AddAutoBoot();
//                mdialog->exec();
//                dialog->exec();
//                return true;
//            } else
//                return false;
//        }
//    }
//    return QObject::eventFilter(watched, event);
//}

void AutoBoot::checkbox_changed_cb(QString bname){
    foreach (QString key, appgroupMultiMaps.keys()) {
        if (key == bname){

            QMap<QString, AutoApp>::iterator it = statusMaps.find(bname);
            if (it == statusMaps.end()){
                qDebug() << "AutoBoot Data Error";
                return;
            }

            if (((SwitchButton *)appgroupMultiMaps.value(key))->isChecked()){ //开启开机启动
                if (it.value().xdg_position == SYSTEMPOS){ //

                } else if (it.value().xdg_position == ALLPOS){ //删除
                    QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
                    if (!appit.value().hidden){ //直接删除
                        _delete_local_autoapp(bname);
                        //更新状态
                        QMap<QString, AutoApp>::iterator updateit = statusMaps.find(bname);
                        if (updateit != statusMaps.end()){
                            updateit.value().hidden = false;
                            updateit.value().xdg_position = SYSTEMPOS;
                            updateit.value().path = appit.value().path;
                        }
                        else
                            qDebug() << "Update status failed when start autoboot";
                    }
//                    QMap<QString, AutoApp>::iterator statusit = statusMaps.begin();
//                    for (; statusit != statusMaps.end(); statusit++){
//                        qDebug() << statusit.value().xdg_position << statusit.value().path;
//                    }
                }
                else if (it.value().xdg_position == LOCALPOS){//改值
                    _enable_autoapp(bname, true);
                }

            }
            else{ //关闭
                if (it.value().xdg_position == SYSTEMPOS){ //复制后改值
                    if (_copy_desktop_file_to_local(bname)){
                        _stop_autoapp(bname);
                    }
                }
                else if (it.value().xdg_position == ALLPOS){//正常逻辑不应存在该情况,预防处理
                    QMap<QString, AutoApp>::iterator appit = appMaps.find(bname);
                    if (appit.value().hidden)
                        _delete_local_autoapp(bname);
                }
                else if (it.value().xdg_position == LOCALPOS){//改值
//                    _enable_autoapp(bname, false);
                    _stop_autoapp(bname);
                }

            }
        }
    }
}

