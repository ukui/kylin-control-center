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

#ifndef BIOMETRICENROLL_H
#define BIOMETRICENROLL_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QListWidget>
#include <QDebug>
#include <QPushButton>
#include <QMovie>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>
#include <QMessageBox>
#include <QCoreApplication>


#include "HoverWidget/hoverwidget.h"


namespace Ui {
class BiometricEnrollDialog;
}

class BiometricEnrollDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BiometricEnrollDialog(QDBusInterface *service,int bioType,
                                   int deviceId, int uid, QWidget *parent = nullptr);
    ~BiometricEnrollDialog();
    enum Result {SUCESS, ERROR, UNDEFINED};

    void setProcessed(bool val);
    void setTitle(int opsType);
    void setPrompt(QString text);

    int enroll(int drvId, int uid, int idx, const QString &idxName);
    int verify(int drvId, int uid, int idx);
    int search(int drvId, int uid, int idxStart, int idxEnd);
    QString getGif(int type);
    QString getImage(int type);
    Result getResult();
    void resetUI();

protected:
    void paintEvent(QPaintEvent * event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_closeBtn_clicked();
    void on_biometricFinishbtn_clicked();
    void on_biometricConBtn_clicked();
    void onStatusChanged(int, int);
    void onProcessChanged(int, QString,int,QString);
    void enrollCallBack(const QDBusMessage &);
    void verifyCallBack(const QDBusMessage &);
    void searchCallBack(const QDBusMessage &);
    void StopOpsCallBack(const QDBusMessage &);
    void showFinishPrompt();
    void errorCallBack(const QDBusError &);

private:
    Ui::BiometricEnrollDialog *ui;
    void setupInit();
    void handleErrorResult(int error);
    QString transferBioType(int type);

    Result opsResult;
    QDBusInterface *serviceInterface;
    enum OPS{IDLE, ENROLL, VERIFY, SEARCH} ops;

    int type;
    int deviceId;
    int uid;
    bool isProcessed;

    QMovie *movie;

private slots:

};

#endif // BIOMETRICENROLL_H
