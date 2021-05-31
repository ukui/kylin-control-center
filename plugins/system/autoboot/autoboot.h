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
#ifndef AUTOBOOT_H
#define AUTOBOOT_H

#include <QObject>
#include <QtPlugin>

#include "shell/interface.h"
#include "datadefined.h"
#include "addautoboot.h"
#include "HoverWidget/hoverwidget.h"
#include <QtDBus>
#include <QGSettings>

namespace Ui {
class AutoBoot;
}

class AutoBoot : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    AutoBoot();
    ~AutoBoot();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const Q_DECL_OVERRIDE;

private:
    void initAddBtn();
    void initStyle();
    void initUI();
    void initConnection();
    void update_app_status();
    void del_autoboot_realize(QString bname);
    void connectToServer();
    void initConfig();
    void setupGSettings();

    void _walk_config_dirs();
    AutoApp _app_new(const char *path);
    gboolean _key_file_get_boolean(GKeyFile *keyfile, const gchar *key, gboolean defaultvalue);
    gboolean _key_file_get_shown(GKeyFile *keyfile, const char *currentdesktop);
    bool _stop_autoapp(QString bname);
    bool _delete_autoapp(QString bname);
    bool _enable_autoapp(QString bname, bool status);
    bool _delete_local_autoapp(QString bname);
    gboolean _key_file_to_file(GKeyFile *keyfile, const gchar *path);
    bool _copy_desktop_file_to_local(QString bname);

    void clearAutoItem();

private:
    Ui::AutoBoot *ui;

    QString pluginName;
    int pluginType;
    QWidget *pluginWidget;

    AddAutoBoot *dialog;
    QDBusInterface *m_cloudInterface;

    QMap<QString, AutoApp> appMaps;
    QMap<QString, AutoApp> localappMaps;
    QMap<QString, AutoApp> statusMaps;
    QMultiMap<QString, QWidget *> appgroupMultiMaps;

    char *localconfigdir;

    HoverWidget *addWgt;

    bool mFirstLoad;

    QGSettings *mQtSettings;

public slots:
    void checkbox_changed_cb(QString bname);
    void keyChangedSlot(const QString &key);
    void add_autoboot_realize_slot(QString path, QString name, QString exec, QString comment,
                                   QString icon);
};

#endif // AUTOBOOT_H
