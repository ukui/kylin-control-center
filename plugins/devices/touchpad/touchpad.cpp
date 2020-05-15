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
#include "touchpad.h"
#include "ui_touchpad.h"

#include <QFile>
#include <QDebug>

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <X11/extensions/XInput.h>
#include <X11/Xatom.h>
}

#define TOUCHPAD_SCHEMA "org.ukui.peripherals-touchpad"
#define ACTIVE_TOUCHPAD_KEY "touchpad-enabled"
#define DISABLE_WHILE_TYPING_KEY "disable-while-typing"
#define TOUCHPAD_CLICK_KEY "tap-to-click"
#define V_EDGE_KEY "vertical-edge-scrolling"
#define H_EDGE_KEY "horizontal-edge-scrolling"
#define V_FINGER_KEY "vertical-two-finger-scrolling"
#define H_FINGER_KEY "horizontal-two-finger-scrolling"
#define N_SCROLLING "none"

bool findSynaptics();
bool _supportsXinputDevices();
XDevice* _deviceIsTouchpad (XDeviceInfo * deviceinfo);
bool _deviceHasProperty (XDevice * device, const char * property_name);

Touchpad::Touchpad()
{
    ui = new Ui::Touchpad;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Touchpad");
    pluginType = DEVICES;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

//    QString qss;
//    QFile QssFile("://combox.qss");
//    QssFile.open(QFile::ReadOnly);

//    if (QssFile.isOpen()){
//        qss = QLatin1String(QssFile.readAll());
//        QssFile.close();
//    }

//    pluginWidget->setStyleSheet("background: #ffffff;");

    ui->scrollingTypeComBox->setView(new QListView());
//    ui->scrollingTypeComBox->setStyleSheet(qss);

//    ui->enableWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->typingWidget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->clickWidget->setStyleSheet("QWidget{background: #F4F4F4;}");
//    ui->scrollingWidget->setStyleSheet("QWidget{background: #F4F4F4; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

    const QByteArray id(TOUCHPAD_SCHEMA);

    if (QGSettings::isSchemaInstalled(TOUCHPAD_SCHEMA)){
        tpsettings = new QGSettings(id);
        setupComponent();
        if (findSynaptics()){
            qDebug() << "Touch Devices Available";
            initTouchpadStatus();
        }
    }



}

Touchpad::~Touchpad()
{
    delete ui;
    if (QGSettings::isSchemaInstalled(TOUCHPAD_SCHEMA)){
        delete tpsettings;
    }


}

QString Touchpad::get_plugin_name(){
    return pluginName;
}

int Touchpad::get_plugin_type(){
    return pluginType;
}

QWidget *Touchpad::get_plugin_ui(){
    return pluginWidget;
}

void Touchpad::plugin_delay_control(){

}

void Touchpad::setupComponent(){
    //
    enableBtn = new SwitchButton(pluginWidget);
    ui->enableHorLayout->addWidget(enableBtn);

    //
    typingBtn = new SwitchButton(pluginWidget);
    ui->typingHorLayout->addWidget(typingBtn);

    //
    clickBtn = new SwitchButton(pluginWidget);
    ui->clickHorLayout->addWidget(clickBtn);

    //
    ui->scrollingTypeComBox->addItem(tr("Disable rolling"), N_SCROLLING);
    ui->scrollingTypeComBox->addItem(tr("Vertical edge scrolling"), V_EDGE_KEY);
    ui->scrollingTypeComBox->addItem(tr("Horizontal edge scrolling"), H_EDGE_KEY);
    ui->scrollingTypeComBox->addItem(tr("Vertical two-finger scrolling"), V_FINGER_KEY);
    ui->scrollingTypeComBox->addItem(tr("Horizontal two-finger scrolling"), H_FINGER_KEY);

    connect(enableBtn, &SwitchButton::checkedChanged, [=](bool checked){
        tpsettings->set(ACTIVE_TOUCHPAD_KEY, checked);
    });

    connect(typingBtn, &SwitchButton::checkedChanged, [=](bool checked){
        tpsettings->set(DISABLE_WHILE_TYPING_KEY, checked);
    });

    connect(clickBtn, &SwitchButton::checkedChanged, [=](bool checked){
        tpsettings->set(TOUCHPAD_CLICK_KEY, checked);
    });

    connect(ui->scrollingTypeComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        Q_UNUSED(index)
        //旧滚动类型设置为false,跳过N_SCROLLING
        QString oldType = _findKeyScrollingType();
        if (QString::compare(oldType, N_SCROLLING) != 0)
        tpsettings->set(oldType, false);

        //新滚动类型设置为true,跳过N_SCROLLING
        QString data = ui->scrollingTypeComBox->currentData().toString();
        if (QString::compare(data, N_SCROLLING) != 0)
            tpsettings->set(data, true);

    });
}

void Touchpad::initTouchpadStatus(){
    //初始化启用按钮
    enableBtn->blockSignals(true);
    enableBtn->setChecked(tpsettings->get(ACTIVE_TOUCHPAD_KEY).toBool());
    enableBtn->blockSignals(false);

    // 初始化打字禁用
    typingBtn->blockSignals(true);
    typingBtn->setChecked(tpsettings->get(DISABLE_WHILE_TYPING_KEY).toBool());
    typingBtn->blockSignals(false);

    // 初始化触摸板点击
    clickBtn->blockSignals(true);
    clickBtn->setChecked(tpsettings->get(TOUCHPAD_CLICK_KEY).toBool());
    clickBtn->blockSignals(false);

    //初始化滚动
    ui->scrollingTypeComBox->blockSignals(true);
    ui->scrollingTypeComBox->setCurrentIndex(ui->scrollingTypeComBox->findData(_findKeyScrollingType()));
    ui->scrollingTypeComBox->blockSignals(false);
}

QString Touchpad::_findKeyScrollingType(){
    if (tpsettings->get(V_EDGE_KEY).toBool())
        return V_EDGE_KEY;
    if (tpsettings->get(H_EDGE_KEY).toBool())
        return H_EDGE_KEY;
    if (tpsettings->get(V_FINGER_KEY).toBool())
        return V_FINGER_KEY;
    if (tpsettings->get(H_FINGER_KEY).toBool())
        return H_FINGER_KEY;
    return N_SCROLLING;
}

bool findSynaptics(){
    XDeviceInfo *device_info;
    int n_devices;
    bool retval;

    if (_supportsXinputDevices() == false)
        return true;

    device_info = XListInputDevices (QX11Info::display(), &n_devices);
    if (device_info == nullptr)
        return false;

    retval = false;
    for (int i = 0; i < n_devices; i++) {
        XDevice *device;

        device = _deviceIsTouchpad (&device_info[i]);
        if (device != nullptr) {
            retval = true;
            break;
        }
    }
    if (device_info != nullptr)
        XFreeDeviceList (device_info);

    return retval;
}

bool _supportsXinputDevices(){
    int op_code, event, error;

    return XQueryExtension (QX11Info::display(),
                            "XInputExtension",
                            &op_code,
                            &event,
                            &error);
}

XDevice* _deviceIsTouchpad (XDeviceInfo *deviceinfo){
    XDevice *device;
    if (deviceinfo->type != XInternAtom (QX11Info::display(), XI_TOUCHPAD, true))
        return nullptr;
    device = XOpenDevice (QX11Info::display(), deviceinfo->id);
    if(device == nullptr)
    {
        qDebug()<<"device== null";
        return nullptr;
    }

    if (_deviceHasProperty(device, "libinput Tapping Enabled") ||
            _deviceHasProperty(device, "Synaptics Off")) {
        return device;
    }
    XCloseDevice (QX11Info::display(), device);
    return nullptr;
}

bool _deviceHasProperty(XDevice *device, const char *property_name){
    Atom realtype, prop;
    int realformat;
    unsigned long nitems, bytes_after;
    unsigned char *data;

    prop = XInternAtom (QX11Info::display(), property_name, True);
    if (!prop)
        return false;
    if ((XGetDeviceProperty (QX11Info::display(), device, prop, 0, 1, False,
                             XA_INTEGER, &realtype, &realformat, &nitems,
                             &bytes_after, &data) == Success) && (realtype != None))
    {
        XFree (data);
        return true;
    }
    return false;
}
