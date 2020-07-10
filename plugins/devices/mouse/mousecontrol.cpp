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
#include "mousecontrol.h"
#include "ui_mousecontrol.h"

#include <QSizePolicy>
#include <QTimer>

#include <QFile>
#include <QDebug>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <X11/Xlib.h>
#include <glib.h>
#include <gio/gio.h>
}

#define MOUSE_SCHEMA "org.ukui.peripherals-mouse"
#define HAND_KEY "left-handed"
#define DOUBLE_CLICK_KEY "double-click"
#define LOCATE_KEY "locate-pointer"
#define CURSOR_SIZE_KEY "cursor-size"
#define ACCELERATION_KEY "motion-acceleration"
#define THRESHOLD_KEY "motion-threshold"

#define SESSION_SCHEMA "org.ukui.session"
#define SESSION_MOUSE_KEY "mouse-size-changed"

#define DESKTOP_SCHEMA "org.mate.interface"
#define CURSOR_BLINK_KEY "cursor-blink"
#define CURSOR_BLINK_TIME_KEY "cursor-blink-time"

#define MOUSE_MID_GET_CMD "/usr/bin/mouse-midbtn-speed-get"
#define MOUSE_MID_SET_CMD "/usr/bin/mouse-midbtn-speed-set"

MyLabel::MyLabel(){

    setAttribute(Qt::WA_DeleteOnClose);

    QSizePolicy pSizePolicy = this->sizePolicy();
    pSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    pSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    this->setSizePolicy(pSizePolicy);

    setFixedSize(QSize(48, 48));
    setScaledContents(true);

    setPixmap(QPixmap(":/img/plugins/mouse/double-click-off.png"));

    const QByteArray id(MOUSE_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)){
        mSettings = new QGSettings(id);
    }

}

MyLabel::~MyLabel(){
    if (QGSettings::isSchemaInstalled(MOUSE_SCHEMA))
        delete mSettings;
}

void MyLabel::mousePressEvent(QMouseEvent *ev){
    setPixmap(QPixmap(":/img/plugins/mouse/double-click-maybe.png"));
    int delay = mSettings->get(DOUBLE_CLICK_KEY).toInt();
    QTimer::singleShot(delay, this, [=]{
        setPixmap(QPixmap(":/img/plugins/mouse/double-click-off.png"));
    });
}

void MyLabel::mouseDoubleClickEvent(QMouseEvent *event){
    setPixmap(QPixmap(":/img/plugins/mouse/double-click-on.png"));
    QTimer::singleShot(2500, this, [=]{
        setPixmap(QPixmap(":/img/plugins/mouse/double-click-off.png"));
    });
}


MouseControl::MouseControl()
{
    ui = new Ui::MouseControl;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Mouse");
    pluginType = DEVICES;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

//    QString qss;
//    QFile QssFile("://combox.qss");
//    QssFile.open(QFile::ReadOnly);

//    if (QssFile.isOpen()){
//        qss = QLatin1String(QssFile.readAll());
//        QssFile.close();
//    }

//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->handWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->pointerSpeedWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->sensitivityWidget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->visibilityWidget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->pointerSizeWidget->setStyleSheet("QWidget{background: #F4F4F4; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

    //全局未生效，再次设置
//    ui->pointerSizeComBox->setView(new QListView());
//    ui->pointerSizeComBox->setStyleSheet(qss);
//    ui->handHabitComBox->setView(new QListView());
//    ui->handHabitComBox->setStyleSheet(qss);


//    ui->cursorWeightWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->cursorSpeedWidget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->flashingWidget->setStyleSheet("QWidget{background: #F4F4F4;  border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

    //初始化鼠标设置GSettings
    const QByteArray id(MOUSE_SCHEMA);
    const QByteArray sessionId(SESSION_SCHEMA);
    const QByteArray idd(DESKTOP_SCHEMA);
    if (QGSettings::isSchemaInstalled(sessionId) && QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(DESKTOP_SCHEMA)) {
        sesstionSetttings = new QGSettings(sessionId);
        settings = new QGSettings(id);
        desktopSettings = new QGSettings(idd);

        setupComponent();

        initHandHabitStatus();
        initPointerStatus();
        initCursorStatus();
        initWheelStatus();
    }



}

MouseControl::~MouseControl()
{
    delete ui;
    if (QGSettings::isSchemaInstalled(MOUSE_SCHEMA) && QGSettings::isSchemaInstalled(SESSION_SCHEMA)) {
        delete settings;
        delete sesstionSetttings;
        delete desktopSettings;
    }
}

QString MouseControl::get_plugin_name(){
    return pluginName;
}

int MouseControl::get_plugin_type(){
    return pluginType;
}

QWidget *MouseControl::get_plugin_ui(){
    return pluginWidget;
}

void MouseControl::plugin_delay_control(){

}

void MouseControl::setupComponent(){

//    ui->title3Label->hide();
//    ui->cursorSpeedFrame->hide();
    ui->cursorWeightFrame->hide();

    //设置左手右手鼠标控件
    ui->handHabitComBox->addItem(tr("Lefthand"), true);
    ui->handHabitComBox->addItem(tr("Righthand"), false);

    MyLabel * testLabel = new MyLabel();
    ui->doubleClickHorLayout->addWidget(testLabel);

    //设置指针可见性控件
    visiblityBtn = new SwitchButton(pluginWidget);
    ui->visibilityHorLayout->addWidget(visiblityBtn);

    //设置指针大小
    ui->pointerSizeComBox->setMaxVisibleItems(5);
    ui->pointerSizeComBox->addItem(tr("Default(Recommended)"), 24); //100%
    ui->pointerSizeComBox->addItem(tr("Medium"), 32); //125%
    ui->pointerSizeComBox->addItem(tr("Large"), 48); //150%

    //设置鼠标滚轮是否显示
    if (!g_file_test(MOUSE_MID_GET_CMD, G_FILE_TEST_EXISTS) || !g_file_test(MOUSE_MID_SET_CMD, G_FILE_TEST_EXISTS)){
        ui->midSpeedFrame->hide();
    }

    //设置启用光标闪烁
    flashingBtn = new SwitchButton(pluginWidget);
    ui->enableFlashingHorLayout->addWidget(flashingBtn);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
     connect(ui->handHabitComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
#else
     connect(ui->handHabitComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){

#endif
        Q_UNUSED(index)
        settings->set(HAND_KEY, ui->handHabitComBox->currentData().toBool());
    });

     connect(ui->doubleclickHorSlider, &QSlider::sliderReleased, [=]{
        settings->set(DOUBLE_CLICK_KEY, ui->doubleclickHorSlider->value());
     });

    connect(ui->pointerSpeedSlider, &QSlider::valueChanged, [=](int value){
        settings->set(ACCELERATION_KEY, static_cast<double>(value)/ui->pointerSpeedSlider->maximum()*10);
    });

    connect(ui->pointerSensitivitySlider, &QSlider::valueChanged, [=](int value){
        settings->set(THRESHOLD_KEY, 10*value/ui->pointerSensitivitySlider->maximum());
    });

    connect(visiblityBtn, &SwitchButton::checkedChanged, [=](bool checked){
        settings->set(LOCATE_KEY, checked);
    });

    connect(ui->pointerSizeComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        Q_UNUSED(index)
        settings->set(CURSOR_SIZE_KEY, ui->pointerSizeComBox->currentData().toInt());

        QStringList keys = sesstionSetttings->keys();
        if (keys.contains("mouseSizeChanged")) {
            sesstionSetttings->set(SESSION_MOUSE_KEY, true);
        }
    });

    connect(flashingBtn, &SwitchButton::checkedChanged, [=](bool checked){
        desktopSettings->set(CURSOR_BLINK_KEY, checked);
    });

    connect(ui->midHorSlider, &QSlider::sliderReleased, [=]{
        _set_mouse_mid_speed(ui->midHorSlider->value());
    });

    connect(ui->cursorSpeedSlider, &QSlider::sliderReleased, [=]{
        desktopSettings->set(CURSOR_BLINK_TIME_KEY, ui->cursorSpeedSlider->value());
    });
}

void MouseControl::initHandHabitStatus(){

    int handHabitIndex = ui->handHabitComBox->findData(settings->get(HAND_KEY).toBool());
    ui->handHabitComBox->blockSignals(true);
    ui->handHabitComBox->setCurrentIndex(handHabitIndex);
    ui->handHabitComBox->blockSignals(false);

    int dc = settings->get(DOUBLE_CLICK_KEY).toInt();
    ui->doubleclickHorSlider->blockSignals(true);
    ui->doubleclickHorSlider->setValue(dc);
    ui->doubleclickHorSlider->blockSignals(false);
}

void MouseControl::initPointerStatus(){

    //当前系统指针加速值，-1为系统默认
    double mouse_acceleration = settings->get(ACCELERATION_KEY).toDouble();

    //当前系统指针灵敏度，-1为系统默认
    int mouse_threshold =  settings->get(THRESHOLD_KEY).toInt();

    //当从接口获取的是-1,则代表系统默认值，真实值需要从底层获取
    if (mouse_threshold == -1 || static_cast<int>(mouse_acceleration) == -1){
        // speed sensitivity
        int accel_numerator, accel_denominator, threshold;  //当加速值和灵敏度为系统默认的-1时，从底层获取到默认的具体值

        XGetPointerControl(QX11Info::display(), &accel_numerator, &accel_denominator, &threshold);

        settings->set(ACCELERATION_KEY, static_cast<double>(accel_numerator/accel_denominator));

        settings->set(THRESHOLD_KEY, threshold);
    }

    //初始化指针速度控件
    ui->pointerSpeedSlider->blockSignals(true);
    ui->pointerSpeedSlider->setValue(static_cast<int>(settings->get(ACCELERATION_KEY).toDouble()*100));
    ui->pointerSpeedSlider->blockSignals(false);

    //初始化指针敏感度控件
    ui->pointerSensitivitySlider->blockSignals(true);
    ui->pointerSensitivitySlider->setValue(settings->get(THRESHOLD_KEY).toInt()*100);
    ui->pointerSensitivitySlider->blockSignals(false);

    //初始化可见性控件
    visiblityBtn->blockSignals(true);
    visiblityBtn->setChecked(settings->get(LOCATE_KEY).toBool());
    visiblityBtn->blockSignals(false);

    //初始化指针大小
    int sizeIndex = ui->pointerSizeComBox->findData(settings->get(CURSOR_SIZE_KEY).toInt());
    ui->pointerSizeComBox->blockSignals(true);
    ui->pointerSizeComBox->setCurrentIndex(sizeIndex);
    ui->pointerSizeComBox->blockSignals(false);
}

void MouseControl::initCursorStatus(){
    flashingBtn->blockSignals(true);
    flashingBtn->setChecked(desktopSettings->get(CURSOR_BLINK_KEY).toBool());
    flashingBtn->blockSignals(false);

    ui->cursorSpeedSlider->blockSignals(true);
    ui->cursorSpeedSlider->setValue(desktopSettings->get(CURSOR_BLINK_TIME_KEY).toInt());
    ui->cursorSpeedSlider->blockSignals(false);
}

void MouseControl::initWheelStatus(){
    int value = _get_mouse_mid_speed();
    ui->midHorSlider->blockSignals(true);
    ui->midHorSlider->setValue(value);
    ui->midHorSlider->blockSignals(false);
}

int MouseControl::_get_mouse_mid_speed(){

    int value = 0;

    if (g_file_test(MOUSE_MID_GET_CMD, G_FILE_TEST_EXISTS)){
        QProcess * getProcess = new QProcess();
        getProcess->start(MOUSE_MID_GET_CMD);
        getProcess->waitForFinished();

        QByteArray ba = getProcess->readAllStandardOutput();
        QString speedStr = QString(ba.data()).simplified();
        value = speedStr.toInt();
    }

    return value;
}

void MouseControl::_set_mouse_mid_speed(int value){
    QString cmd;

    cmd = MOUSE_MID_SET_CMD + QString(" ") + QString::number(value);

    QProcess * setProcess = new QProcess();
    setProcess->start(cmd);
    setProcess->waitForFinished();
}
