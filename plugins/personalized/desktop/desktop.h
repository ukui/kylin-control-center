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
#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>
#include <QtPlugin>
#include <tuple>
#include <QIcon>
#include <QVector>
#include <QPushButton>
#include <QMap>
#include <QProcess>

#include "shell/interface.h"

class SwitchButton;
class QGSettings;

namespace Ui {
class Desktop;
}

class Desktop : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Desktop();
    ~Desktop();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

    void initTitleLabel();
    void initSearchText();
    void initTranslation();
    void setupComponent();
    void setupConnect();
    void initVisibleStatus();
    void initLockingStatus();
    void initTrayStatus(QString name, QIcon icon, QGSettings *gsettings);
    void initTraySettings();
    void clearContent();

private:
    QMap<QString, QIcon> desktopConver(QString processName);
    bool isFileExist(QString fullFileName);

private:
    Ui::Desktop *ui;

    int pluginType;
    QString pluginName;
    QWidget * pluginWidget;
    QVector<QGSettings*> vecGsettings;
    QMap<QString, QString> transMap; // transaltion Map
    QMap<QString, QString> iconMap;
    QStringList disList;
    QStringList nameList;

    SwitchButton * deskComputerSwitchBtn;
    SwitchButton * deskTrashSwitchBtn;
    SwitchButton * deskHomeSwitchBtn;
    SwitchButton * deskVolumeSwitchBtn;
    SwitchButton * deskNetworkSwitchBtn;

    SwitchButton * fullMenuSwitchBtn;
    SwitchButton * menuComputerSwitchBtn;
    SwitchButton * menuTrashSwitchBtn;
    SwitchButton * menuFilesystemSwitchBtn;
    SwitchButton * menuSettingSwitchBtn;

    QGSettings * dSettings;
    QGSettings * mQtSettings;

    QSharedPointer<QProcess> cmd;

    bool mFirstLoad;

private slots:
    void removeTrayItem(QString itemName);
    void addTrayItem(QGSettings * trayGSetting);
    QString desktopToName(QString desktopfile);
    QIcon   desktopToIcon(const QString &desktopfile);
    QMap<QString, QIcon> readOuputSlot();
    void readErrorSlot();
    void slotCloudAccout(const QString &key);
};

#endif // DESKTOP_H
