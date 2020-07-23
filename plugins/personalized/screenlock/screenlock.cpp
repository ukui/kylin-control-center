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
#include <QDir>

#define BGPATH "/usr/share/backgrounds/"
#define SCREENLOCK_BG_SCHEMA "org.ukui.screensaver"
#define SCREENLOCK_BG_KEY "background"
#define SCREENLOCK_DELAY_KEY "lock-delay"
#define SCREENLOCK_LOCK_KEY "lock-enabled"
#define SCREENLOCK_ACTIVE_KEY "idle-activation-enabled"

#include "bgfileparse.h"
#include "pictureunit.h"
#include "MaskWidget/maskwidget.h"

Screenlock::Screenlock()
{
    ui = new Ui::Screenlock;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Screenlock");
    pluginType = PERSONALIZED;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title1Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->loginWidget->setStyleSheet("QWidget{background: #F4F4F4; border: none; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->enableWidget->setStyleSheet("QWidget{background: #F4F4F4; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

//    QString btnQss = QString("background: #E9E9E9; border: none; border-radius: 4px;");
//    ui->browserLocalwpBtn->setStyleSheet(btnQss);
//    ui->browserOnlinewpBtn->setStyleSheet(btnQss);

//    ui->delaySlider->setStyleSheet("QSlider{height: 20px;}"
//                                   "QSlider::groove:horizontal{border: none;}"
//                                   "QSlider::add-page:horizontal{background: #808080; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
//                                   "QSlider::sub-page:horizontal{background: #3D6BE5; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
//                                   "QSlider::handle:horizontal{width: 20px; height: 20px; border-image: url(:/img/plugins/fonts/bigRoller.png);}"
//                                   "");

    const QByteArray id(SCREENLOCK_BG_SCHEMA);
    lSetting = new QGSettings(id);


    setupComponent();
    setupConnect();
    initScreenlockStatus();

    lockbgSize = QSize(400, 240);

}

Screenlock::~Screenlock()
{
    delete ui;
    delete lSetting;
    delete lockSetting;
    delete lockLoginSettings;
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
    QString filename = QDir::homePath() + "/.config/ukui/ukui-control-center.conf";
    lockSetting = new QSettings(filename, QSettings::IniFormat);

    //锁屏延时暂时不可用，屏蔽
    ui->enableFrame->hide();

    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }

    QString lockfilename = "/var/lib/lightdm-data/" + name + "/ukui-greeter.conf";
    lockLoginSettings = new QSettings(lockfilename, QSettings::IniFormat);

    QStringList scaleList;
    scaleList<< tr("1m") << tr("5m") << tr("10m") << tr("30m") << tr("45m")
              <<tr("1h") << tr("1.5h") << tr("3h");

    uslider = new Uslider(scaleList);
    uslider->setRange(1,8);
    uslider->setTickInterval(1);
    uslider->setPageStep(1);

    ui->lockhorizontalLayout->addWidget(uslider);

    ui->browserLocalwpBtn->hide();
    ui->browserOnlinewpBtn->hide();

    loginbgSwitchBtn = new SwitchButton(pluginWidget);
    ui->loginbgHorLayout->addWidget(loginbgSwitchBtn);
    loginbgSwitchBtn->setChecked(getLockStatus());

    lockSwitchBtn = new SwitchButton(pluginWidget);
    ui->lockHorLayout->addWidget(lockSwitchBtn);

    bool lockKey = false;
    QStringList keys =  lSetting->keys();
    if (keys.contains("lockEnabled")) {
        lockKey = true;
        bool status = lSetting->get(SCREENLOCK_LOCK_KEY).toBool();
        lockSwitchBtn->setChecked(status);
    }

    connect(lockSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){
        if (lockKey) {
            lSetting->set(SCREENLOCK_LOCK_KEY,  checked);
        }
    });

    connect(lSetting, &QGSettings::changed, this, [=](QString key) {
        if ( key == "idleActivationEnabled") {
            bool judge = lSetting->get(SCREENLOCK_ACTIVE_KEY).toBool();
            if (!judge) {
                if (lockSwitchBtn->isChecked()) {
                    lockSwitchBtn->setChecked(judge);
                }
            }
        }
    });

    //设置布局
    flowLayout = new FlowLayout;
    flowLayout->setContentsMargins(0, 0, 0, 0);
    ui->backgroundsWidget->setLayout(flowLayout);
}

void Screenlock::setupConnect(){
//    ui->delaySlider->setMinimum(1);
//    ui->delaySlider->setMaximum(120);


    connect(loginbgSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){
        setLockBackground(checked);
    });

    connect(uslider, &QSlider::valueChanged, [&](int value){
        QStringList keys = lSetting->keys();
        if (keys.contains("lockDelay")) {
            lSetting->set(SCREENLOCK_DELAY_KEY, convertToLocktime(value));
        }
    });

    QStringList keys = lSetting->keys();
    if (keys.contains("lockDelay")) {
        int value = lockConvertToSlider(lSetting->get(SCREENLOCK_DELAY_KEY).toInt());

        uslider->setValue(value);
    }
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
            setLockBackground(loginbgSwitchBtn->isChecked());
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
//    ui->delaySlider->blockSignals(true);
//    ui->delaySlider->setValue(lDelay);
//    ui->delaySlider->blockSignals(false);

    uslider->blockSignals(true);
    uslider->setValue(lockConvertToSlider(lDelay));
    uslider->blockSignals(false);
}

int Screenlock::convertToLocktime(const int value) {
    switch (value) {
    case 1:
        return 1;
        break;
    case 2:
        return 5;
        break;
    case 3:
        return 10;
        break;
    case 4:
        return 30;
        break;
    case 5:
        return 45;
        break;
    case 6:
        return 60;
        break;
    case 7:
        return 90;
        break;
    case 8:
        return 180;
        break;
    default:
        return 1;
        break;
    }
}

int Screenlock::lockConvertToSlider(const int value) {
    switch (value) {
    case 1:
        return 1;
        break;
    case 5:
        return 2;
        break;
    case 10:
        return 3;
        break;
    case 30:
        return 4;
        break;
    case 45:
        return 5;
        break;
    case 60:
        return 6;
        break;
    case 90:
        return 7;
        break;
    case 180:
        return 8;
        break;
    default:
        return 1;
        break;
    }
}

void Screenlock::setLockBackground(bool status)
{
    QString username;
#if QT_VERSION <= QT_VERSION_CHECK(5,12,0)
    username = qgetenv("USER");
    if (username.isEmpty()) {
        username = qgetenv("USERNAME");
    }
#else
    username = "";
#endif

    QString bgStr;
    if (lSetting && status) {
        bgStr= lSetting->get(SCREENLOCK_BG_KEY).toString();
    } else if (!status) {
        bgStr = "";
    }

    if (!bgStr.isEmpty()) {
        int index = bgStr.lastIndexOf('/');
        bgStr = bgStr.mid(index, bgStr.length() - index);
    }

    QString picname;

    if (!bgStr.isEmpty()) {
        if (!username.isEmpty()) {
            picname = "/usr/share/backgrounds/" + username + bgStr;
        } else {
            picname = "/usr/share/backgrounds"  + bgStr;
        }
    } else {
        picname = "";
    }

    lockSetting->beginGroup("ScreenLock");
    lockSetting->setValue("lockStatus", status);
    lockSetting->endGroup();


    lockLoginSettings->beginGroup("greeter");
    lockLoginSettings->setValue("backgroundPath", picname);
    lockLoginSettings->endGroup();

}

bool Screenlock::getLockStatus()
{
    lockSetting->beginGroup("ScreenLock");
    bool status = lockSetting->value("lockStatus").toBool();
    lockSetting->endGroup();
    return  status;
}
