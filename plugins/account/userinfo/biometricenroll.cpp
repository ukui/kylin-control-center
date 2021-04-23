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

#include "biometricenroll.h"
#include "ui_biometricenroll.h"
#include <QPixmap>

#include "CloseButton/closebutton.h"
#include "biometricdeviceinfo.h"
#include "biometricproxy.h"
#include "servicemanager.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

BiometricEnrollDialog::BiometricEnrollDialog(QDBusInterface *service,int bioType,
                                             int deviceId, int uid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BiometricEnrollDialog),
    serviceInterface(service),
    type(bioType),
    deviceId(deviceId),
    uid(uid),
    movie(nullptr),
    ops(IDLE),
    isProcessed(false),
    opsResult(UNDEFINED)
{
    ui->setupUi(this);
    setupInit();

    connect(serviceInterface, SIGNAL(StatusChanged(int,int)),
            this, SLOT(onStatusChanged(int,int)));

    connect(serviceInterface, SIGNAL(ProcessChanged(int,QString,int,QString)),
            this, SLOT(onProcessChanged(int,QString,int,QString)));

    ServiceManager *sm = ServiceManager::instance();
    connect(sm, &ServiceManager::serviceStatusChanged,
            this, [&](bool activate){
        if(!activate)
        {
            close();
        }
    });

}

BiometricEnrollDialog::~BiometricEnrollDialog()
{
    delete ui;
}

void BiometricEnrollDialog::setProcessed(bool val)
{
    isProcessed = val;
    if(isProcessed){
        ui->biometricEnrollLable->setPixmap(QPixmap("/usr/share/ukui-biometric/images/huawei/00.svg"));
    }
    else{
        ui->biometricEnrollLable->setPixmap(getImage(type));
        if(!movie)
            movie = new QMovie(getGif(type));
    }
}

void BiometricEnrollDialog::setupInit()
{
    setWindowTitle(tr(""));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    if(isProcessed){
        ui->biometricEnrollLable->setPixmap(QPixmap("/usr/share/ukui-biometric/images/huawei/00.svg"));
    }
    else{
        ui->biometricEnrollLable->setPixmap(getImage(type));
        if(!movie)
            movie = new QMovie(getGif(type));
    }
    ui->biometricButtonWidget->hide();
    ui->biometricFinishLabel->hide();
}

QString BiometricEnrollDialog::transferBioType(int type)
{
    switch(type) {
    case BIOTYPE_FINGERPRINT:
        return tr("FingerPrint");
    case BIOTYPE_FINGERVEIN:
        return tr("Fingervein");
    case BIOTYPE_IRIS:
        return tr("Iris");
    case BIOTYPE_FACE:
        return tr("Face");
    case BIOTYPE_VOICEPRINT:
        return tr("VoicePrint");
    }
    return QString();
}

void BiometricEnrollDialog::setTitle(int biotype)
{
    QString title = transferBioType(type);
    switch(biotype) {
    case ENROLL:
        title += tr("Enroll");
        break;
    case VERIFY:
        title += tr("Verify");
        break;
    case SEARCH:
        title += tr("Search");
        break;
    }

    ui->biometricOpsLbl->setText(title);
}

void BiometricEnrollDialog::setPrompt(QString text)
{
    ui->biometricPromptLbl->setText(text);
}

void BiometricEnrollDialog::on_closeBtn_clicked()
{
    close();
}

BiometricEnrollDialog::Result BiometricEnrollDialog::getResult()
{
    return opsResult;
}

int BiometricEnrollDialog::enroll(int drvId, int uid, int idx, const QString &idxName)
{
    QList<QVariant> args;
    args << drvId << uid << idx << idxName;

    this->setTitle(ENROLL);
    this->setPrompt(tr("Permission is required.\n"
                       "Please authenticate yourself to continue"));
    ui->closeBtn->setEnabled(false);

    serviceInterface->callWithCallback("Enroll", args, this,
                        SLOT(enrollCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
    ops = ENROLL;

    return exec();
}

void BiometricEnrollDialog::enrollCallBack(const QDBusMessage &reply)
{
    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Enroll result: " << result;

    ui->closeBtn->setEnabled(true);

    switch(result) {
    case DBUS_RESULT_SUCCESS: { /* 录入成功 */
        opsResult = SUCESS;
        setPrompt(tr("Enroll successfully"));
        showFinishPrompt();
        break;
    }
    default:
        opsResult = ERROR;
        handleErrorResult(result);
        break;
    }
    ops = IDLE;
}

int BiometricEnrollDialog::verify(int drvId, int uid, int idx)
{
    QList<QVariant> args;
    args << drvId << uid << idx;

    this->setTitle(VERIFY);

    serviceInterface->callWithCallback("Verify", args, this,
                        SLOT(verifyCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
    ops = VERIFY;

    return exec();
}

QString BiometricEnrollDialog::getGif(int type)
{
    switch(type) {
    case BIOTYPE_FINGERPRINT:
        return "/usr/share/ukui-biometric/images/FingerPrint.gif";
    case BIOTYPE_FINGERVEIN:
        return "/usr/share/ukui-biometric/images/fingervein.gif";
    case BIOTYPE_IRIS:
        return "/usr/share/ukui-biometric/images/iris.gif";
    case BIOTYPE_VOICEPRINT:
        return "/usr/share/ukui-biometric/images/voiceprint.gif";
    }
    return QString();
}

QString BiometricEnrollDialog::getImage(int type)
{
    switch(type) {
    case BIOTYPE_FINGERPRINT:
        return "/usr/share/ukui-biometric/images/FingerPrint.png";
    case BIOTYPE_FINGERVEIN:
        return "/usr/share/ukui-biometric/images/FingerVein.png";
    case BIOTYPE_IRIS:
        return "/usr/share/ukui-biometric/images/Iris.png";
    case BIOTYPE_VOICEPRINT:
        return "/usr/share/ukui-biometric/images/VoicePrint.png";
    }
    return QString();
}

void BiometricEnrollDialog::verifyCallBack(const QDBusMessage &reply)
{
    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Verify result: " << result;

    if(result >= 0) {
        opsResult = SUCESS;
        setPrompt(tr("Verify successfully"));
        showFinishPrompt();
    } else if(result == DBUS_RESULT_NOTMATCH) {
        setPrompt(tr("Not Match"));
        ui->biometricEnrollLable->setPixmap(QIcon::fromTheme("dialog-error").pixmap(QSize(64,64)));
	//showFinishPrompt();
    } else {
        handleErrorResult(result);
    }

    ops = IDLE;
}

void BiometricEnrollDialog::resetUI()
{
    if(isProcessed){
        ui->biometricEnrollLable->setPixmap(QPixmap("/usr/share/ukui-biometric/images/huawei/00.svg"));
    }
    else{
        ui->biometricEnrollLable->setPixmap(getImage(type));
        if(!movie)
            movie = new QMovie(getGif(type));
    }
    ui->biometricButtonWidget->hide();
    ui->biometricFinishLabel->hide();
    ui->biometricPromptLbl->show();
    ui->biometricPromptLbl->clear();

    ui->biometricOpsLbl->show();

}

void BiometricEnrollDialog::on_biometricFinishbtn_clicked()
{
    close();
}

void BiometricEnrollDialog::on_biometricConBtn_clicked()
{
    resetUI();
    enroll(deviceId,uid,-1,"指纹2");
}

int BiometricEnrollDialog::search(int drvId, int uid, int idxStart, int idxEnd)
{
    QList<QVariant> args;
    args << drvId << uid << idxStart << idxEnd;

    this->setTitle(SEARCH);

    serviceInterface->callWithCallback("Search", args, this,
                        SLOT(searchCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));

    ops = SEARCH;

    return exec();
}

void BiometricEnrollDialog::searchCallBack(const QDBusMessage &reply)
{
    return ;

}


void BiometricEnrollDialog::showFinishPrompt()
{
    ui->biometricEnrollLable->setPixmap(QIcon::fromTheme("ukui-dialog-success").pixmap(QSize(64,64)));
    if(ops == ENROLL)
        ui->biometricFinishLabel->setText(tr("Enroll successfully"));
    else if(ops == VERIFY)
        ui->biometricFinishLabel->setText(tr("Verify successfully"));

    ui->biometricFinishLabel->show();

    ui->biometricPromptLbl->hide();
    ui->biometricOpsLbl->hide();
    ui->biometricButtonWidget->show();

    ui->biometricConBtn->hide();
}

void BiometricEnrollDialog::StopOpsCallBack(const QDBusMessage &reply)
{
    int ret = reply.arguments().at(0).toInt();
    accept();
}

void BiometricEnrollDialog::errorCallBack(const QDBusError &error)
{
    qDebug() << "DBus Error: " << error.message();
    accept();
}

void BiometricEnrollDialog::closeEvent(QCloseEvent *event)
{
    serviceInterface->call("StopOps", deviceId,5);
}

void BiometricEnrollDialog::onProcessChanged(int drvId,QString  aa, int statusType,QString bb)
{
    int count = statusType * 15 / 100;
    QString filename = QString("/usr/share/ukui-biometric/images/huawei/") + (count < 10 ? "0" : "") +
            QString::number(count) + ".svg";

    ui->biometricEnrollLable->setPixmap(QPixmap(filename));
}

void BiometricEnrollDialog::onStatusChanged(int drvId, int statusType)
{
    if (!(drvId == deviceId && statusType == STATUS_NOTIFY))
        return;

    ui->closeBtn->setEnabled(true);

    //过滤掉当录入时使用生物识别授权接收到的认证的提示信息
    if(ops == ENROLL) {
        QDBusMessage reply = serviceInterface->call("UpdateStatus", drvId);
        if(reply.type() == QDBusMessage::ErrorMessage) {
            qDebug() << "DBUS: " << reply.errorMessage();
            return;
        }
        int devStatus = reply.arguments().at(3).toInt();
        qDebug() << devStatus;

        if(!(devStatus >= 201 && devStatus < 203)) {
            return;
        }
    }
    else if(ops == IDLE)
    {
        return;
    }

    if(!isProcessed && movie->state() != QMovie::Running)
    {
        ui->biometricEnrollLable->setMovie(movie);
        movie->start();
    }

    QDBusMessage notifyReply = serviceInterface->call("GetNotifyMesg", drvId);
    if(notifyReply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "DBUS: " << notifyReply.errorMessage();
        return;
    }
    QString prompt = notifyReply.arguments().at(0).toString();
    qDebug() << prompt;

    setPrompt(prompt);
}

void BiometricEnrollDialog::handleErrorResult(int error)
{
    switch(error) {
    case DBUS_RESULT_ERROR: {
        //操作失败，需要进一步获取失败原因
        QDBusMessage msg = serviceInterface->call("GetOpsMesg", deviceId);
        if(msg.type() == QDBusMessage::ErrorMessage)
        {
            qDebug() << "UpdateStatus error: " << msg.errorMessage();
            setPrompt(tr("D-Bus calling error"));
            return;
        }
        setPrompt(msg.arguments().at(0).toString());
        qDebug() << "GetOpsMesg: deviceId--" << deviceId;
        break;
    }
    case DBUS_RESULT_DEVICEBUSY:
        //设备忙
        setPrompt(tr("Device is busy"));
        break;
    case DBUS_RESULT_NOSUCHDEVICE:
        //设备不存在
        setPrompt(tr("No such device"));
        break;
    case DBUS_RESULT_PERMISSIONDENIED:
        //没有权限
        setPrompt(tr("Permission denied"));
        break;
    }

}


void BiometricEnrollDialog::paintEvent(QPaintEvent * event){
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}
