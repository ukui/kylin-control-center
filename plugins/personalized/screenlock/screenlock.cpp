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
#include "screenlock.h"
#include "ui_screenlock.h"

#include <QDebug>

#define BGPATH "/usr/share/backgrounds/"
#define SCREENLOCK_BG_SCHEMA "org.ukui.screensaver"
#define SCREENLOCK_BG_KEY "background"
#define SCREENLOCK_DELAY_KEY "lock-delay"

#include "bgfileparse.h"
#include "pictureunit.h"
#include "MaskWidget/maskwidget.h"

Screenlock::Screenlock()
{
    ui = new Ui::Screenlock;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("screenlock");
    pluginType = PERSONALIZED;

    pluginWidget->setStyleSheet("background: #ffffff;");

    ui->loginWidget->setStyleSheet("QWidget{background: #F4F4F4; border: none; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
    ui->enableWidget->setStyleSheet("QWidget{background: #F4F4F4; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

    QString btnQss = QString("background: #E9E9E9; border: none; border-radius: 4px;");
    ui->browserLocalwpBtn->setStyleSheet(btnQss);
    ui->browserOnlinewpBtn->setStyleSheet(btnQss);

    ui->delaySlider->setStyleSheet("QSlider{height: 20px;}"
                                   "QSlider::groove:horizontal{border: none;}"
                                   "QSlider::add-page:horizontal{background: #808080; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
                                   "QSlider::sub-page:horizontal{background: #3D6BE5; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
                                   "QSlider::handle:horizontal{width: 20px; height: 20px; border-image: url(:/img/plugins/fonts/bigRoller.png);}"
                                   "");

    const QByteArray id(SCREENLOCK_BG_SCHEMA);
    lSetting = new QGSettings(id);


    setupComponent();
    initScreenlockStatus();

    lockbgSize = QSize(400, 240);

}

Screenlock::~Screenlock()
{
    delete ui;

    delete lSetting;
}

QString Screenlock::get_plugin_name(){
    return pluginName;
}

int Screenlock::get_plugin_type(){
    return pluginType;
}

QWidget *Screenlock::get_plugin_ui(){
    return pluginWidget;
}

void Screenlock::plugin_delay_control(){

}

void Screenlock::setupComponent(){
    ui->browserLocalwpBtn->hide();
    ui->browserOnlinewpBtn->hide();

    loginbgSwitchBtn = new SwitchButton(pluginWidget);
    ui->loginbgHorLayout->addWidget(loginbgSwitchBtn);

    //设置布局
    flowLayout = new FlowLayout;
    flowLayout->setContentsMargins(0, 0, 0, 0);
    ui->backgroundsWidget->setLayout(flowLayout);
}

void Screenlock::setupConnect(){
    connect(ui->delaySlider, &QSlider::valueChanged, [=](int value){
        lSetting->set(SCREENLOCK_DELAY_KEY, value);
    });
}

void Screenlock::initScreenlockStatus(){
    //获取当前锁屏壁纸
    QString bgStr = lSetting->get(SCREENLOCK_BG_KEY).toString();
//    if (bgStr.isEmpty())
        ui->previewLabel->setPixmap(QPixmap("://img/plugins/screenlock/none.png"));
//    else
//        ui->previewLabel->setPixmap(QPixmap(bgStr).scaled(ui->previewLabel->size()));
    //遮罩
    MaskWidget * maskWidget = new MaskWidget(ui->previewLabel);
    maskWidget->setGeometry(0, 0, ui->previewLabel->width(), ui->previewLabel->height());

    //使用线程解析本地壁纸文件；获取壁纸单元
    pThread = new QThread;
    pWorker = new BuildPicUnitsWorker;
    connect(pWorker, &BuildPicUnitsWorker::pixmapGeneral, this, [=](QPixmap pixmap, BgInfo bgInfo){
        //设置当前锁屏壁纸的预览
        if (bgInfo.filename == bgStr){
            ui->previewLabel->setPixmap(pixmap);
        }

        //线程中构建控件传递会报告event无法install 的警告
        PictureUnit * picUnit = new PictureUnit;
        picUnit->setPixmap(pixmap);
        picUnit->setFilenameText(bgInfo.filename);
        connect(picUnit, &PictureUnit::clicked, [=](QString filename){
            ui->previewLabel->setPixmap(pixmap);
            lSetting->set(SCREENLOCK_BG_KEY, filename);
        });

        flowLayout->addWidget(picUnit);
    });
    connect(pWorker, &BuildPicUnitsWorker::workerComplete, [=]{
        pThread->quit(); //退出事件循环
        pThread->wait(); //释放资源
    });

    pWorker->moveToThread(pThread);
    connect(pThread, &QThread::started, pWorker, &BuildPicUnitsWorker::run);
    connect(pThread, &QThread::finished, this, [=]{

    });
    connect(pThread, &QThread::finished, pWorker, &BuildPicUnitsWorker::deleteLater);

    pThread->start();

    //设置登录界面背景开关


    //设置锁屏时间，屏保激活后多久锁定屏幕
    int lDelay = lSetting->get(SCREENLOCK_DELAY_KEY).toInt();
    ui->delaySlider->blockSignals(true);
    ui->delaySlider->setValue(lDelay);
    ui->delaySlider->blockSignals(false);

}
