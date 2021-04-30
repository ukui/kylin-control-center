/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2020 KYLINOS Information Technology Co., Ltd.
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

#ifndef BIOMETRICMOREINFO_H
#define BIOMETRICMOREINFO_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QListWidget>
#include <QDebug>
#include <QPushButton>
#include <QFileSystemWatcher>
#include <QMovie>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>
#include <QMessageBox>
#include <QCoreApplication>


#include "HoverWidget/hoverwidget.h"
#include "biometricdeviceinfo.h"
#include "biometricproxy.h"
#include "SwitchButton/switchbutton.h"

namespace Ui {
class BiometricMoreInfoDialog;
}

class BiometricMoreInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BiometricMoreInfoDialog(DeviceInfoPtr deviceinfo, QWidget *parent = nullptr);

    ~BiometricMoreInfoDialog();


protected:
    void paintEvent(QPaintEvent * event);


private slots:
    void on_closeBtn_clicked();

private:
    Ui::BiometricMoreInfoDialog *ui;

    void setupInit();

    QString transferBioType(int type);
    QString transferVerifyType(int type);
    QString transferStorageType(int type);
    QString transferBusType(int type);
    QString transferIdentifyType(int type);

    DeviceInfoPtr deviceInfo;
    SwitchButton  *defaultDeviceBtn;
    QFileSystemWatcher  *mWatcher;

private slots:

};

#endif // BIOMETRICENROLL_H
