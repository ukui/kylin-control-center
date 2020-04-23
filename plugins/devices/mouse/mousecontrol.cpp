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
}

#define MOUSE_SCHEMA "org.ukui.peripherals-mouse"
#define HAND_KEY "left-handed"
#define LOCATE_KEY "locate-pointer"
#define CURSOR_SIZE_KEY "cursor-size"
#define ACCELERATION_KEY "motion-acceleration"
#define THRESHOLD_KEY "motion-threshold"

#define SESSION_SCHEMA "org.ukui.session"
#define SESSION_MOUSE_KEY "initialize-mouse-size"


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
    if (QGSettings::isSchemaInstalled(sessionId) && QGSettings::isSchemaInstalled(id)) {
        sesstionSetttings = new QGSettings(sessionId);
        settings = new QGSettings(id);

        setupComponent();

        initHandHabitStatus();
        initPointerStatus();
    }



}

MouseControl::~MouseControl()
{
    delete ui;
    if (QGSettings::isSchemaInstalled(MOUSE_SCHEMA) && QGSettings::isSchemaInstalled(SESSION_SCHEMA)) {
        delete settings;
        delete sesstionSetttings;
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

    ui->title3Label->hide();
    ui->cursorSpeedFrame->hide();
    ui->cursorWeightFrame->hide();

    //设置左手右手鼠标控件
    ui->handHabitComBox->addItem(tr("Lefthand"), true);
    ui->handHabitComBox->addItem(tr("Righthand"), false);

    //设置指针可见性控件
    visiblityBtn = new SwitchButton(pluginWidget);
    ui->visibilityHorLayout->addWidget(visiblityBtn);

    //设置指针大小
    ui->pointerSizeComBox->setMaxVisibleItems(5);
    ui->pointerSizeComBox->addItem(tr("Default(Recommended)"), 24); //100%
    ui->pointerSizeComBox->addItem(tr("Medium"), 32); //125%
    ui->pointerSizeComBox->addItem(tr("Large"), 48); //150%

    //设置启用光标闪烁
    flashingBtn = new SwitchButton(pluginWidget);
    ui->enableFlashingHorLayout->addWidget(flashingBtn);

    connect(ui->handHabitComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        Q_UNUSED(index)
        settings->set(HAND_KEY, ui->handHabitComBox->currentData().toBool());
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

    connect(ui->pointerSizeComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        Q_UNUSED(index)
        settings->set(CURSOR_SIZE_KEY, ui->pointerSizeComBox->currentData().toInt());

        QStringList keys = sesstionSetttings->keys();
        if (keys.contains("initializeMouseSize")) {
            sesstionSetttings->set(SESSION_MOUSE_KEY, true);
        }
    });
}

void MouseControl::initHandHabitStatus(){

    int handHabitIndex = ui->handHabitComBox->findData(settings->get(HAND_KEY).toBool());
    ui->handHabitComBox->blockSignals(true);
    ui->handHabitComBox->setCurrentIndex(handHabitIndex);
    ui->handHabitComBox->blockSignals(false);
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

}
