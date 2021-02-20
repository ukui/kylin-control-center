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

#include <QDebug>

//放在.h中报错，放.cpp不报错
extern "C" {
#include <X11/extensions/XInput.h>
#include <X11/Xatom.h>
//#include <X11/Xcursor/Xcursor.h>
}


#define NONE_ID 0
#define CURSORSIZE_SMALLER 18
#define CURSORSIZE_MEDIUM 32
#define CURSORSIZE_LARGER 48

XDevice* _device_is_touchpad (XDeviceInfo * deviceinfo);
bool _device_has_property (XDevice * device, const char * property_name);

struct KindsRolling : QObjectUserData{
    QString kind;
};

MouseControl::MouseControl()
{
    ui = new Ui::MouseControl;
    pluginWidget = new CustomWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("mousecontrol");
    pluginType = DEVICES;

//    const QByteArray id(TOUCHPAD_SCHEMA);
//    tpsettings = new QGSettings(id);

    const QByteArray idd(GNOME_TOUCHPAD_SCHEMA);
    gnomeSettings = new QGSettings(idd);

    InitDBusMouse();

    component_init();
    status_init();
}

MouseControl::~MouseControl()
{
    delete ui;
    ui = nullptr;
    DeInitDBusMouse();
}

QString MouseControl::get_plugin_name(){
    return pluginName;
}

int MouseControl::get_plugin_type(){
    return pluginType;
}

CustomWidget * MouseControl::get_plugin_ui(){
    return pluginWidget;
}

void MouseControl::plugin_delay_control(){

}

void MouseControl::component_init(){

    //
    ui->lefthandbuttonGroup->setId(ui->rightRadioBtn, 0);
    ui->lefthandbuttonGroup->setId(ui->leftRadioBtn, 1);

    // Cursors themes
    QStringList themes = _get_cursors_themes();
    ui->CursorthemesComboBox->addItem(tr("Default"));
    ui->CursorthemesComboBox->addItems(themes);

    // add switchbtn for active touchpad
    activeBtn = new SwitchButton(pluginWidget);
    ui->activeHLayout->addWidget(activeBtn);
    ui->activeHLayout->addStretch();

    //无接口可用暂时屏蔽鼠标滚轮设置
//    for (int i = 0; i < ui->horizontalLayout_2->count(); ++i) {
//        QLayoutItem * it = ui->horizontalLayout_2->itemAt(i);
////        it->widget()->hide(); //这种遍历无法得知类型，弹簧控件没有hide方法，导致段错误
//    }
    ui->label_3->hide(); ui->comboBox->hide();
    ui->label_4->hide(); ui->checkBox->hide();


    //不存在触摸板设备，则隐藏触摸板设置按钮
    if (!find_synaptics())
        ui->touchpadBtn->hide();

    // hide helper radiobutton
    ui->noneRadioButton->hide();
    // set buttongroup id
    ui->rollingbuttonGroup->setId(ui->noneRadioButton, NONE_ID);

    // set user data rolling radiobutton
    KindsRolling * vedge = new KindsRolling(); vedge->kind = V_EDGE_KEY;
    ui->vedgeRadioBtn->setUserData(Qt::UserRole, vedge);
    KindsRolling * hedge = new KindsRolling(); hedge->kind = H_EDGE_KEY;
    ui->hedgeRadioBtn->setUserData(Qt::UserRole, hedge);
    KindsRolling * vfinger = new KindsRolling(); vfinger->kind = V_FINGER_KEY;
    ui->vfingerRadioBtn->setUserData(Qt::UserRole, vfinger);
    KindsRolling * hfinger = new KindsRolling(); hfinger->kind = H_FINGER_KEY;
    ui->hfingerRadioBtn->setUserData(Qt::UserRole, hfinger);


    // set buttongroup id
    ui->cursorsizebuttonGroup->setId(ui->smallerRadioBtn, CURSORSIZE_SMALLER);
    ui->cursorsizebuttonGroup->setId(ui->mediumRadioBtn, CURSORSIZE_MEDIUM);
    ui->cursorsizebuttonGroup->setId(ui->largerRadioBtn, CURSORSIZE_LARGER);

}

void MouseControl::status_init(){
    if (kylin_hardware_mouse_get_lefthanded())
        ui->leftRadioBtn->setChecked(true);
    else
        ui->rightRadioBtn->setChecked(true);

    //cursor theme
    QString curtheme  = kylin_hardware_mouse_get_cursortheme();
    if (curtheme == "")
        ui->CursorthemesComboBox->setCurrentIndex(0);
    else
        ui->CursorthemesComboBox->setCurrentText(curtheme);



    double mouse_acceleration = kylin_hardware_mouse_get_motionacceleration();//当前系统指针加速值，-1为系统默认
    int mouse_threshold =  kylin_hardware_mouse_get_motionthreshold();//当前系统指针灵敏度，-1为系统默认

    //当从接口获取的是-1,则代表系统默认值，真实值需要从底层获取
    if (mouse_threshold == -1 || static_cast<int>(mouse_acceleration) == -1){
        // speed sensitivity
        int accel_numerator, accel_denominator, threshold;  //当加速值和灵敏度为系统默认的-1时，从底层获取到默认的具体值

        XGetPointerControl(QX11Info::display(), &accel_numerator, &accel_denominator, &threshold);
        qDebug() << "--->" << accel_numerator << accel_denominator << threshold;
        kylin_hardware_mouse_set_motionacceleration(static_cast<double>(accel_numerator/accel_denominator));

        kylin_hardware_mouse_set_motionthreshold(threshold);
    }

    //set speed
//    qDebug() << kylin_hardware_mouse_get_motionacceleration() << kylin_hardware_mouse_get_motionthreshold();
    ui->speedSlider->setValue(static_cast<int>(kylin_hardware_mouse_get_motionacceleration())*100);

    //set sensitivity
    ui->sensitivitySlider->setValue(kylin_hardware_mouse_get_motionthreshold()*100);

    //set visibility position
    ui->posCheckBtn->setChecked(kylin_hardware_mouse_get_locatepointer());

    connect(ui->CursorthemesComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(cursor_themes_changed_slot(QString)));
    connect(ui->lefthandbuttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(mouseprimarykey_changed_slot(int)));
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speed_value_changed_slot(int)));
    connect(ui->sensitivitySlider, SIGNAL(valueChanged(int)), this, SLOT(sensitivity_value_changed_slot(int)));
    connect(ui->posCheckBtn, SIGNAL(clicked(bool)), this, SLOT(show_pointer_position_slot(bool)));

    connect(ui->touchpadBtn, &QPushButton::clicked, this, [=]{ui->StackedWidget->setCurrentIndex(1);});
    connect(ui->cursorBtn, &QPushButton::clicked, this, [=]{ui->StackedWidget->setCurrentIndex(2);});


    //touchpad settings
//    activeBtn->setChecked(tpsettings->get(ACTIVE_TOUCHPAD_KEY).toBool());
    if (gnomeSettings->get(GNOME_ACTIVE_TOUCHPAD_KEY).toString() == "enabled")
        activeBtn->setChecked(true);
    else
        activeBtn->setChecked(false);
    _refresh_touchpad_widget_status();

    // disable touchpad when typing
//    ui->disablecheckBox->setChecked(tpsettings->get(DISABLE_WHILE_TYPING_KEY).toBool());
    ui->disablecheckBox->setChecked(gnomeSettings->get(GNOME_DISABLE_WHILE_TYPEING_KEY).toBool());

    // enable touchpad click
//    ui->tpclickcheckBox->setChecked(tpsettings->get(TOUCHPAD_CLICK_KEY).toBool());
    ui->tpclickcheckBox->setChecked(gnomeSettings->get(GNOME_TOUCHPAD_CLICK_KEY).toBool());

    // scrolling
//    ui->vedgeRadioBtn->setChecked(tpsettings->get(V_EDGE_KEY).toBool());
//    ui->hedgeRadioBtn->setChecked(tpsettings->get(H_EDGE_KEY).toBool());
//    ui->vfingerRadioBtn->setChecked(tpsettings->get(V_FINGER_KEY).toBool());
//    ui->hfingerRadioBtn->setChecked(tpsettings->get(H_FINGER_KEY).toBool());
//    ui->noneRadioButton->setChecked(false);

//    if (ui->rollingbuttonGroup->checkedButton() == nullptr)
//        ui->rollingCheckBtn->setChecked(true);
//    else
//        ui->rollingCheckBtn->setChecked(false);
//    _refresh_rolling_btn_status();
    ui->rollingCheckBtn->setChecked(!(gnomeSettings->get(GNOME_SCROLLING_EDGE_KEY).toBool() || gnomeSettings->get(GNOME_SCROLLING_TWO_KEY).toBool()));
    ui->rollingWidget->hide();


    connect(activeBtn, SIGNAL(checkedChanged(bool)), this, SLOT(active_touchpad_changed_slot(bool)));
    connect(ui->disablecheckBox, SIGNAL(clicked(bool)), this, SLOT(disable_while_typing_clicked_slot(bool)));
    connect(ui->tpclickcheckBox, SIGNAL(clicked(bool)), this, SLOT(touchpad_click_clicked_slot(bool)));
    connect(ui->rollingCheckBtn, SIGNAL(clicked(bool)), this, SLOT(rolling_enable_clicked_slot(bool)));
//    connect(ui->rollingbuttonGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)), this, SLOT(rolling_kind_changed_slot(QAbstractButton*, bool)));


    //cursor settings

    int cursorsize = kylin_hardware_mouse_get_cursorsize();
    if (cursorsize <= CURSORSIZE_SMALLER)
        ui->smallerRadioBtn->setChecked(true);
    else if (cursorsize <= CURSORSIZE_MEDIUM)
        ui->mediumRadioBtn->setChecked(true);
    else
        ui->largerRadioBtn->setChecked(true);
    connect(ui->cursorsizebuttonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(cursor_size_changed_slot()));

    //reset
    connect(ui->resetBtn, &QPushButton::clicked, this, [=]{ui->smallerRadioBtn->setChecked(true);});
}

bool MouseControl::find_synaptics(){
    XDeviceInfo *device_info;
    int n_devices;
    bool retval;

    if (_supports_xinput_devices() == false)
        return true;

    device_info = XListInputDevices (QX11Info::display(), &n_devices);
    if (device_info == nullptr)
        return false;

    retval = false;
    for (int i = 0; i < n_devices; i++) {
        XDevice *device;

        device = _device_is_touchpad (&device_info[i]);
        if (device != nullptr) {
            retval = true;
            break;
        }
    }
    if (device_info != nullptr)
        XFreeDeviceList (device_info);

    return retval;
}

XDevice* _device_is_touchpad (XDeviceInfo *deviceinfo)
{
    XDevice *device;
    if (deviceinfo->type != XInternAtom (QX11Info::display(), XI_TOUCHPAD, true))
        return nullptr;
    device = XOpenDevice (QX11Info::display(), deviceinfo->id);
    if(device == nullptr)
    {
        qDebug()<<"device== null";
        return nullptr;
    }

    if (_device_has_property (device, "libinput Tapping Enabled") ||
            _device_has_property (device, "Synaptics Off")) {
        return device;
    }
    XCloseDevice (QX11Info::display(), device);
    return nullptr;
}

bool _device_has_property (XDevice * device, const char * property_name){
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

bool MouseControl::_supports_xinput_devices(){
    int op_code, event, error;

    return XQueryExtension (QX11Info::display(),
                            "XInputExtension",
                            &op_code,
                            &event,
                            &error);
}

QStringList MouseControl::_get_cursors_themes(){
    QStringList themes;
    QDir themesDir(CURSORS_THEMES_PATH);

    if (themesDir.exists()){
        foreach (QString dirname, themesDir.entryList(QDir::Dirs)){
            if (dirname == "." || dirname == "..")
                continue;
            QString fullpath(CURSORS_THEMES_PATH + dirname);
            QDir themeDir(CURSORS_THEMES_PATH + dirname + "/cursors/");
            if (themeDir.exists())
                themes.append(dirname);
        }
    }
    return themes;
}

void MouseControl::_refresh_touchpad_widget_status(){
    if (activeBtn->isChecked())
        ui->touchpadWidget->show();
    else
        ui->touchpadWidget->hide();
}

void MouseControl::_refresh_rolling_btn_status(){
    if (ui->rollingCheckBtn->isChecked())
        ui->rollingWidget->hide();
    else
        ui->rollingWidget->show();
}

void MouseControl::mouseprimarykey_changed_slot(int id){
    kylin_hardware_mouse_set_lefthanded(id);
}

void MouseControl::cursor_themes_changed_slot(QString text){
    QString value = text;
    if (text == tr("Default"))
        value = "";

    QByteArray ba = value.toLatin1();
    kylin_hardware_mouse_set_cursortheme(ba.data());
}

void MouseControl::speed_value_changed_slot(int value){
    kylin_hardware_mouse_set_motionacceleration(static_cast<double>(value/ui->speedSlider->maximum()*10));
}

void MouseControl::sensitivity_value_changed_slot(int value){
    kylin_hardware_mouse_set_motionthreshold(10*value/ui->sensitivitySlider->maximum());
}

void MouseControl::show_pointer_position_slot(bool status){
    kylin_hardware_mouse_set_locatepointer(status);
}

void MouseControl::active_touchpad_changed_slot(bool status){
//    tpsettings->set(ACTIVE_TOUCHPAD_KEY, status);
    if (status)
        gnomeSettings->set(GNOME_ACTIVE_TOUCHPAD_KEY, "enabled");
    else
        gnomeSettings->set(GNOME_ACTIVE_TOUCHPAD_KEY, "disabled");

    _refresh_touchpad_widget_status();
}

void MouseControl::disable_while_typing_clicked_slot(bool status){
//    tpsettings->set(DISABLE_WHILE_TYPING_KEY, status);
    gnomeSettings->set(GNOME_DISABLE_WHILE_TYPEING_KEY, status);
}

void MouseControl::touchpad_click_clicked_slot(bool status){
//    tpsettings->set(TOUCHPAD_CLICK_KEY, status);
    gnomeSettings->set(GNOME_TOUCHPAD_CLICK_KEY, status);
}

void MouseControl::rolling_enable_clicked_slot(bool status){

//    if (status)
//        ui->noneRadioButton->setChecked(true);
//    _refresh_rolling_btn_status();
    if (status){
        gnomeSettings->set(GNOME_SCROLLING_EDGE_KEY, !status);
        gnomeSettings->set(GNOME_SCROLLING_TWO_KEY, !status);
    }
    else{
        gnomeSettings->set(GNOME_SCROLLING_EDGE_KEY, !status);
    }
}

void MouseControl::rolling_kind_changed_slot(QAbstractButton *basebtn, bool status){
    if (ui->rollingbuttonGroup->checkedId() != NONE_ID)
        ui->rollingCheckBtn->setChecked(false);

    if (basebtn->text() == "None")
        return;

    QRadioButton * button = dynamic_cast<QRadioButton *>(basebtn);
    QString kind = static_cast<KindsRolling *>(button->userData(Qt::UserRole))->kind;
//    tpsettings->set(kind, status);
}

void MouseControl::cursor_size_changed_slot(){
    kylin_hardware_mouse_set_cursorsize(ui->cursorsizebuttonGroup->checkedId());
}
