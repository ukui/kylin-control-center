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

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QSizePolicy>
#include <QtDBus>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtConcurrent/QtConcurrent>

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

#define MOUSE_SCHEMA          "org.ukui.peripherals-mouse"
#define HAND_KEY              "left-handed"
#define DOUBLE_CLICK_KEY      "double-click"
#define LOCATE_KEY            "locate-pointer"
#define CURSOR_SIZE_KEY       "cursor-size"
#define ACCELERATION_KEY      "motion-acceleration"
#define THRESHOLD_KEY         "motion-threshold"
#define WHEEL_KEY             "wheel-speed"
#define ACCEL_KEY             "mouse-accel"

#define SESSION_SCHEMA        "org.ukui.session"
#define SESSION_MOUSE_KEY     "mouse-size-changed"

#define DESKTOP_SCHEMA        "org.mate.interface"
#define CURSOR_BLINK_KEY      "cursor-blink"
#define CURSOR_BLINK_TIME_KEY "cursor-blink-time"

#define THEME_SCHEMA          "org.ukui.style"

MyLabel::MyLabel() {

    setAttribute(Qt::WA_DeleteOnClose);

    QSizePolicy pSizePolicy = this->sizePolicy();
    pSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    pSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    this->setSizePolicy(pSizePolicy);

    setFixedSize(QSize(54, 28));
    setScaledContents(true);

    setPixmap(QPixmap(":/img/plugins/mouse/double-click-off.png"));

    const QByteArray id(MOUSE_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)){
        mSettings = new QGSettings(id, QByteArray(), this);
    }
    this->setToolTip(tr("double-click to test"));
}

MyLabel::~MyLabel() {

}

void MyLabel::mouseDoubleClickEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    int delay = mSettings->get(DOUBLE_CLICK_KEY).toInt();
    setPixmap(QPixmap(":/img/plugins/mouse/double-click-on.png"));
    QTimer::singleShot(delay, this, [=]{
        setPixmap(QPixmap(":/img/plugins/mouse/double-click-off.png"));
    });
}


MouseControl::MouseControl() : mFirstLoad(true)
{
    pluginName = tr("Mouse");
    pluginType = DEVICES;
}

MouseControl::~MouseControl() {
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString MouseControl::get_plugin_name() {
    return pluginName;
}

int MouseControl::get_plugin_type() {
    return pluginType;
}

QWidget *MouseControl::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;
        ui = new Ui::MouseControl;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        initSearchText();
        initTitleLabel();

        //初始化鼠标设置GSettings
        const QByteArray id(MOUSE_SCHEMA);
        const QByteArray sessionId(SESSION_SCHEMA);
        const QByteArray idd(DESKTOP_SCHEMA);
        const QByteArray themeId(THEME_SCHEMA);
        if (QGSettings::isSchemaInstalled(sessionId) &&
                QGSettings::isSchemaInstalled(id) &&
                QGSettings::isSchemaInstalled(idd)) {
            sesstionSetttings = new QGSettings(sessionId, QByteArray(), this);
            settings = new QGSettings(id, QByteArray(), this);
            desktopSettings = new QGSettings(idd, QByteArray(), this);
            mThemeSettings = new QGSettings(themeId, QByteArray(), this);

            mouseKeys = settings->keys();

            setupComponent();

            initHandHabitStatus();
            initPointerStatus();
            initCursorStatus();
            initWheelStatus();
        }
    }
    return pluginWidget;
}

void MouseControl::plugin_delay_control() {

}

const QString MouseControl::name() const {

    return QStringLiteral("mouse");
}

void MouseControl::initSearchText() {
    //~ contents_path /mouse/Hand habit
    ui->handLabel->setText(tr("Hand habit"));
    //~ contents_path /mouse/Doubleclick  delay
    ui->delayLabel->setText(tr("Doubleclick  delay"));
    //~ contents_path /mouse/Speed
    ui->speedLabel->setText(tr("Speed"));
    ui->label_2->setMinimumWidth(50);
    ui->label_3->setMinimumWidth(50);
    //~ contents_path /mouse/Acceleration
    ui->accelLabel->setText(tr("Acceleration"));
    //~ contents_path /mouse/Visibility
    ui->visLabel->setText(tr("Visibility"));
    //~ contents_path /mouse/Pointer size
    ui->sizeLabel->setText(tr("Pointer size"));
    //~ contents_path /mouse/Enable flashing on text area
    ui->flashLabel->setText(tr("Enable flashing on text area"));
    //~ contents_path /mouse/Cursor speed
    ui->cursorspdLabel->setText(tr("Cursor speed"));
    ui->label_17->setMinimumWidth(50);
    ui->label_18->setMinimumWidth(50);
    ui->label_6->setMinimumWidth(50);
    ui->label_7->setMinimumWidth(50);
    ui->label_21->setMinimumWidth(50);
    ui->label_22->setMinimumWidth(50);
}

void MouseControl::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    ui->title2Label->setFont(font);
    ui->title3Label->setFont(font);
}

void MouseControl::setupComponent() {

    ui->cursorWeightFrame->hide();

    //设置左手右手鼠标控件
    ui->handHabitComBox->addItem(tr("Lefthand"), true);
    ui->handHabitComBox->addItem(tr("Righthand"), false);

    ui->doubleClickHorLayout->addWidget(new MyLabel());

    //设置指针可见性控件
    visiblityBtn = new SwitchButton(pluginWidget);
    ui->visibilityHorLayout->addWidget(visiblityBtn);

    // 鼠标加速度控件
    mAccelBtn = new SwitchButton(pluginWidget);
    mAccelBtn->setChecked(settings->get(ACCEL_KEY).toBool());
    ui->accelLayout->addStretch();
    ui->accelLayout->addWidget(mAccelBtn);

    //设置指针大小
    ui->pointerSizeComBox->setMaxVisibleItems(5);
    ui->pointerSizeComBox->addItem(tr("Default(Recommended)"), 24); //100%
    ui->pointerSizeComBox->addItem(tr("Medium"), 32); //125%
    ui->pointerSizeComBox->addItem(tr("Large"), 48); //150%

    //设置鼠标滚轮是否显示
    if (!mouseKeys.contains("wheelSpeed")) {
        ui->midSpeedFrame->setVisible(false);
    }

    //设置启用光标闪烁
    flashingBtn = new SwitchButton(pluginWidget);
    ui->enableFlashingHorLayout->addWidget(flashingBtn);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
     connect(ui->handHabitComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
#else
     connect(ui->handHabitComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {

#endif
        Q_UNUSED(index)
        settings->set(HAND_KEY, ui->handHabitComBox->currentData().toBool());
    });

     connect(ui->doubleclickHorSlider, &QSlider::sliderReleased, [=] {
        settings->set(DOUBLE_CLICK_KEY, ui->doubleclickHorSlider->value());
        qApp->setDoubleClickInterval(ui->doubleclickHorSlider->value());
     });

    connect(ui->pointerSpeedSlider, &QSlider::valueChanged, [=](int value) {
        settings->set(ACCELERATION_KEY, static_cast<double>(value)/ui->pointerSpeedSlider->maximum()*10);
    });

    connect(visiblityBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        settings->set(LOCATE_KEY, checked);
    });

    connect(ui->pointerSizeComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MouseControl::mouseSizeChange);

    connect(flashingBtn, &SwitchButton::checkedChanged, [=](bool checked) {
        ui->cursorSpeedFrame->setVisible(checked);
        desktopSettings->set(CURSOR_BLINK_KEY, checked);
        mThemeSettings->set(CURSOR_BLINK_KEY, checked);
        if (!checked) {
            mThemeSettings->set(CURSOR_BLINK_TIME_KEY, 0);
        } else {
            int mValue = ui->cursorSpeedSlider->maximum() - ui->cursorSpeedSlider->value() + ui->cursorSpeedSlider->minimum();
            mThemeSettings->set(CURSOR_BLINK_TIME_KEY, mValue);
        }
    });

    connect(ui->midHorSlider, &QSlider::sliderReleased, [=] {
        settings->set(WHEEL_KEY, ui->midHorSlider->value());
    });

    connect(settings,&QGSettings::changed,[=] (const QString &key){
        if(key == "locatePointer") {
            visiblityBtn->blockSignals(true);
            visiblityBtn->setChecked(settings->get(LOCATE_KEY).toBool());
            visiblityBtn->blockSignals(false);
        } else if(key == "mouseAccel") {
            ui->doubleclickHorSlider->blockSignals(true);
            mAccelBtn->setChecked(settings->get(ACCEL_KEY).toBool());
            ui->doubleclickHorSlider->blockSignals(false);
        } else if(key == "doubleClick") {
            int dc = settings->get(DOUBLE_CLICK_KEY).toInt();
            ui->doubleclickHorSlider->blockSignals(true);
            ui->doubleclickHorSlider->setValue(dc);
            ui->doubleclickHorSlider->blockSignals(false);
        } else if(key == "wheelSpeed") {
            ui->midHorSlider->setValue(settings->get(WHEEL_KEY).toInt());
        } else if(key == "motionAcceleration") {
            ui->pointerSpeedSlider->blockSignals(true);
            ui->pointerSpeedSlider->setValue(static_cast<int>(settings->get(ACCELERATION_KEY).toDouble()*100));
            ui->pointerSpeedSlider->blockSignals(false);
        } else if(key == "leftHanded") {
            int handHabitIndex = ui->handHabitComBox->findData(settings->get(HAND_KEY).toBool());
            ui->handHabitComBox->blockSignals(true);
            ui->handHabitComBox->setCurrentIndex(handHabitIndex);
            ui->handHabitComBox->blockSignals(false);
        } else if(key == "cursorSize") {
            int pointerSizeIndex = ui->pointerSizeComBox->findData(settings->get(CURSOR_SIZE_KEY).toInt());
            ui->pointerSizeComBox->blockSignals(true);
            ui->pointerSizeComBox->setCurrentIndex(pointerSizeIndex);
            ui->pointerSizeComBox->blockSignals(false);
        }
    });

    connect(desktopSettings,&QGSettings::changed,[=](const QString &key) {
        if(key == "cursorBlinkTime") {
            ui->cursorSpeedSlider->blockSignals(true);
            int mValue = ui->cursorSpeedSlider->maximum() - desktopSettings->get(CURSOR_BLINK_TIME_KEY).toInt() + ui->cursorSpeedSlider->minimum();
            ui->cursorSpeedSlider->setValue(mValue);
            ui->cursorSpeedSlider->blockSignals(false);
        } else if(key == "cursorBlink") {
            flashingBtn->blockSignals(true);
            flashingBtn->setChecked(desktopSettings->get(CURSOR_BLINK_KEY).toBool());
            ui->cursorSpeedFrame->setVisible(desktopSettings->get(CURSOR_BLINK_KEY).toBool());
            flashingBtn->blockSignals(false);
        }
    });

    connect(ui->cursorSpeedSlider, &QSlider::sliderReleased, [=] {
        
        int mValue = ui->cursorSpeedSlider->maximum() - ui->cursorSpeedSlider->value() + ui->cursorSpeedSlider->minimum();
        desktopSettings->set(CURSOR_BLINK_TIME_KEY, mValue);
        mThemeSettings->set(CURSOR_BLINK_TIME_KEY, mValue);
    });

    connect(mAccelBtn, &SwitchButton::checkedChanged, this, [=] (bool checked) {
       settings->set(ACCEL_KEY, checked);
    });
}

void MouseControl::initHandHabitStatus() {

    int handHabitIndex = ui->handHabitComBox->findData(settings->get(HAND_KEY).toBool());
    ui->handHabitComBox->blockSignals(true);
    ui->handHabitComBox->setCurrentIndex(handHabitIndex);
    ui->handHabitComBox->blockSignals(false);

    int dc = settings->get(DOUBLE_CLICK_KEY).toInt();
    ui->doubleclickHorSlider->blockSignals(true);
    ui->doubleclickHorSlider->setValue(dc);
    ui->doubleclickHorSlider->blockSignals(false);
}

void MouseControl::initPointerStatus() {

    //当前系统指针加速值，-1为系统默认
    double mouse_acceleration = settings->get(ACCELERATION_KEY).toDouble();

    //当前系统指针灵敏度，-1为系统默认
    int mouse_threshold =  settings->get(THRESHOLD_KEY).toInt();

    //当从接口获取的是-1,则代表系统默认值，真实值需要从底层获取
    if (mouse_threshold == -1 || static_cast<int>(mouse_acceleration) == -1) {
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

void MouseControl::initCursorStatus() {
    flashingBtn->blockSignals(true);
    flashingBtn->setChecked(desktopSettings->get(CURSOR_BLINK_KEY).toBool());
    ui->cursorSpeedFrame->setVisible(desktopSettings->get(CURSOR_BLINK_KEY).toBool());
    flashingBtn->blockSignals(false);

    ui->cursorSpeedSlider->blockSignals(true);
    int mValue = ui->cursorSpeedSlider->maximum() - desktopSettings->get(CURSOR_BLINK_TIME_KEY).toInt() + ui->cursorSpeedSlider->minimum();
    ui->cursorSpeedSlider->setValue(mValue);
    ui->cursorSpeedSlider->blockSignals(false);
}

void MouseControl::initWheelStatus() {

    ui->midHorSlider->blockSignals(true);
    if (mouseKeys.contains("wheelSpeed")) {
        ui->midHorSlider->setValue(settings->get(WHEEL_KEY).toInt());
    }
    ui->midHorSlider->blockSignals(false);
}

void MouseControl::mouseSizeChange() {

    settings->set(CURSOR_SIZE_KEY, ui->pointerSizeComBox->currentData().toInt());

    QStringList keys = sesstionSetttings->keys();
    if (keys.contains("mouseSizeChanged")) {
        sesstionSetttings->set(SESSION_MOUSE_KEY, true);
    }

    QString filename = QDir::homePath() + "/.config/kcminputrc";
    QSettings *mouseSettings = new QSettings(filename, QSettings::IniFormat);

    mouseSettings->beginGroup("Mouse");
    mouseSettings->setValue("cursorSize", ui->pointerSizeComBox->currentData().toInt());
    mouseSettings->endGroup();

    delete mouseSettings;
    mouseSettings = nullptr;

    QDBusMessage message = QDBusMessage::createSignal("/KGlobalSettings", "org.kde.KGlobalSettings", "notifyChange");
    QList<QVariant> args;
    args.append(5);
    args.append(0);
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}
