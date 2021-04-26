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

#include "biometricmoreinfo.h"
#include "ui_biometricmoreinfo.h"
#include <QPixmap>

#include "CloseButton/closebutton.h"
#include "biometricdeviceinfo.h"
#include "biometricproxy.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

enum VerifyType {
    VERIFY_HARDWARE,
    VERIFY_SOFTWARE,
    VERIFY_MIX,
    VERIFY_OTHER
};

enum StorageType {
    STORAGE_DEVICE,
    STORAGE_OS,
    STORAGE_MIX
};

enum BusType {
    BUS_SERIAL,
    BUS_USB,
    BUS_PCIE,
    BUS_ANY = 100,
    BUS_OTHER
};

enum IdentifyType {
    IDENTIFY_HARDWARE,
    IDENTIFY_SOFTWARE,
    IDENTIFY_MIX,
    IDENTIFY_OTHER
};

struct SearchResult {
    int uid;
    int index;
    QString indexName;
};

BiometricMoreInfoDialog::BiometricMoreInfoDialog( DeviceInfoPtr deviceinfo, QWidget *parent) :
    QDialog(parent),
    deviceInfo(deviceinfo),
    ui(new Ui::BiometricMoreInfoDialog)
{
    ui->setupUi(this);
    setupInit();

    QString verifyType = transferVerifyType(deviceInfo->verifyType);
    QString busType = transferBusType(deviceInfo->busType);
    QString storageType = transferStorageType(deviceInfo->storageType);
    QString identifyType = transferIdentifyType(deviceInfo->identifyType);
    QString devStatus = deviceInfo->deviceNum > 0 ? tr("Connected") : tr("Unconnected");

    qDebug()<<verifyType<<busType<<storageType<<identifyType<<devStatus;
    ui->bioBusTypeLbl->setText(busType);
    ui->bioVerifyTypeLbl->setText(verifyType);
    ui->bioStorageTypeLbl->setText(storageType);
    ui->bioIdentificationTypeLbl->setText(identifyType);
    ui->bioDeviceStatusLbll->setText(devStatus);

    defaultDeviceBtn = new SwitchButton(ui->biometricDefaultFrame);
  //  defaultDeviceBtn->setChecked(false);
    if(getDefaultDevice() == deviceinfo->shortName)
        defaultDeviceBtn->setChecked(true);
    else
        defaultDeviceBtn->setChecked(false);

    ui->biometridDefaulltDeviceLayout->addWidget(defaultDeviceBtn);
    connect(defaultDeviceBtn, &SwitchButton::checkedChanged, [=](bool checked){
        if(checked)
            setDefaultDevice(deviceinfo->shortName);
        else
            setDefaultDevice("");
    });
}

BiometricMoreInfoDialog::~BiometricMoreInfoDialog()
{
    delete ui;
}

QString BiometricMoreInfoDialog::transferBioType(int type)
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

QString BiometricMoreInfoDialog::transferVerifyType(int type)
{
    switch(type) {
    case VERIFY_HARDWARE:
        return tr("Hardware Verification");
    case VERIFY_SOFTWARE:
        return tr("Software Verification");
    case VERIFY_MIX:
        return tr("Mix Verification");
    case VERIFY_OTHER:
        return tr("Other Verification");
    }
    return QString();
}
QString BiometricMoreInfoDialog::transferStorageType(int type)
{
    switch(type) {
    case STORAGE_DEVICE:
        return tr("Device Storage");
    case STORAGE_OS:
        return tr("OS Storage");
    case STORAGE_MIX:
        return tr("Mix Storage");
    }
    return QString();
}
QString BiometricMoreInfoDialog::transferBusType(int type)
{
    switch(type) {
    case BUS_SERIAL:
        return tr("Serial");
    case BUS_USB:
        return tr("USB");
    case BUS_PCIE:
        return tr("PCIE");
    case BUS_ANY:
        return tr("Any");
    case BUS_OTHER:
        return tr("Other");
    }
    return QString();
}
QString BiometricMoreInfoDialog::transferIdentifyType(int type)
{
    switch(type) {
    case VERIFY_HARDWARE:
        return tr("Hardware Identification");
    case VERIFY_SOFTWARE:
        return tr("Software Identification");
    case VERIFY_MIX:
        return tr("Mix Identification");
    case VERIFY_OTHER:
        return tr("Other Identification");
    }
    return QString();
}

void BiometricMoreInfoDialog::on_closeBtn_clicked()
{
    close();
}

void BiometricMoreInfoDialog::setupInit()
{
    setWindowTitle(tr(""));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

}

void BiometricMoreInfoDialog::paintEvent(QPaintEvent * event){
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
