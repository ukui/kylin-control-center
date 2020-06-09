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
#include "keyboardcontrol.h"
#include "ui_keyboardcontrol.h"

#include <QGSettings/QGSettings>

#include <QDebug>
#include <QMouseEvent>

#define KEYBOARD_SCHEMA "org.ukui.peripherals-keyboard"
#define REPEAT_KEY "repeat"
#define DELAY_KEY "delay"
#define RATE_KEY "rate"

#define KBD_LAYOUTS_SCHEMA "org.mate.peripherals-keyboard-xkb.kbd"
#define KBD_LAYOUTS_KEY "layouts"

KeyboardControl::KeyboardControl()
{
    ui = new Ui::KeyboardControl;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Keyboard");
    pluginType = DEVICES;

    ui->addFrame->installEventFilter(this);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    settingsCreate = false;

    setupStylesheet();
    setupComponent();

    //初始化键盘通用设置GSettings
    const QByteArray id(KEYBOARD_SCHEMA);

    //初始化键盘布局GSettings
    const QByteArray idd(KBD_LAYOUTS_SCHEMA);

    if (QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(idd)){
        settingsCreate = true;

        kbdsettings = new QGSettings(idd);
        settings = new QGSettings(id);

        //构建布局管理器对象
        layoutmanagerObj = new KbdLayoutManager(kbdsettings->get(KBD_LAYOUTS_KEY).toStringList());

        setupConnect();
        initGeneralStatus();

        rebuildLayoutsComBox();
    }

}

KeyboardControl::~KeyboardControl()
{
    delete ui;
    if (settingsCreate){
        delete kbdsettings;
        delete settings;
    }

}

QString KeyboardControl::get_plugin_name(){
    return pluginName;
}

int KeyboardControl::get_plugin_type(){
    return pluginType;
}

QWidget *KeyboardControl::get_plugin_ui(){
    return pluginWidget;
}

void KeyboardControl::plugin_delay_control(){

}

void KeyboardControl::setupStylesheet(){
//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->repeatWidget_0->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->repeatWidget_1->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->repeatWidget_2->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

//    ui->repeatWidget_3->setStyleSheet("QWidget#repeatWidget_3{background: #F4F4F4; border-radius: 6px;}");
//    ui->repeatLabel_3->setStyleSheet("QLabel{background: #F4F4F4; font-size: 14px; color: #D9000000;}");

//    ui->repeatWidget_4->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->repeatWidget_5->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");


//    ui->layoutWidget_0->setStyleSheet("QWidget#layoutWidget_0{background: #F4F4F4; border-radius: 6px;}");
//    ui->layoutLabel_0->setStyleSheet("QLabel{background: #F4F4F4; font-size: 14px; color: #D9000000;}");

//    ui->layoutWidget_1->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");
}

void KeyboardControl::setupComponent(){
    //构建Combox代理，否则样式不全部生效
    itemDelege = new QStyledItemDelegate();

    //隐藏未开发功能
    ui->repeatFrame_4->hide();
    ui->repeatFrame_5->hide();

    //重复输入开关按钮
    keySwitchBtn = new SwitchButton(pluginWidget);
    ui->enableHorLayout->addWidget(keySwitchBtn);

    //大写锁定开关按钮
    capsLockSwitchBtn = new SwitchButton(pluginWidget);
    ui->capsLockHorLayout->addWidget(capsLockSwitchBtn);

    //小键盘开关按钮
    numLockSwitchBtn = new SwitchButton(pluginWidget);
    ui->numLockHorLayout->addWidget(numLockSwitchBtn);

    //布局代理
    ui->layoutsComBox->setItemDelegate(itemDelege);
    ui->layoutsComBox->setMaxVisibleItems(5);

    ui->addBtn->setIcon(QIcon("://img/plugins/keyboardcontrol/add.png"));
    ui->addBtn->setIconSize(QSize(48, 48));
    ui->addBtn->setStyleSheet("QPushButton{background-color:transparent;}");
}

void KeyboardControl::setupConnect(){
    connect(keySwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){
        settings->set(REPEAT_KEY, checked);
    });

    connect(ui->delayHorSlider, &QSlider::valueChanged, this, [=](int value){
        settings->set(DELAY_KEY, value);
    });

    connect(ui->speedHorSlider, &QSlider::valueChanged, this, [=](int value){
        settings->set(RATE_KEY, value);
    });


    connect(ui->addBtn, &QPushButton::clicked, this, [=]{
        layoutmanagerObj->exec();
    });

    connect(layoutmanagerObj, &KbdLayoutManager::del_variant_signals, [=](QString layout){
        rebuildLayoutsComBox();
        qDebug() << layout;
    });
    connect(layoutmanagerObj, &KbdLayoutManager::add_new_variant_signals, [=](QString layout){
        rebuildLayoutsComBox();
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->layoutsComBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
#else
    connect(ui->layoutsComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
#endif

        QStringList layoutsList;
        layoutsList.append(ui->layoutsComBox->currentData(Qt::UserRole).toString());
        for (int i = 0; i < ui->layoutsComBox->count(); i++){
            QString id = ui->layoutsComBox->itemData(i, Qt::UserRole).toString();
            if (i != index) //跳过当前item
                layoutsList.append(id);
        }
        kbdsettings->set(KBD_LAYOUTS_KEY, layoutsList);
    });
}

void KeyboardControl::initGeneralStatus(){
    //设置按键重复状态
    keySwitchBtn->setChecked(settings->get(REPEAT_KEY).toBool());

    //设置按键重复的延时
    ui->delayHorSlider->setValue(settings->get(DELAY_KEY).toInt());

    //设置按键重复的速度
    ui->speedHorSlider->setValue(settings->get(RATE_KEY).toInt());

}

void KeyboardControl::rebuildLayoutsComBox(){
    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    ui->layoutsComBox->blockSignals(true);
    //清空键盘布局下拉列表
    ui->layoutsComBox->clear();

    //重建键盘布局下拉列表
    for (QString layout : layouts){
        ui->layoutsComBox->addItem(layoutmanagerObj->kbd_get_description_by_id(const_cast<const char *>(layout.toLatin1().data())), layout);
    }
    ui->layoutsComBox->blockSignals(false);
    if (0 == ui->layoutsComBox->count()) {
        ui->layoutsComBox->setVisible(false);
    } else {
        ui->layoutsComBox->setVisible(true);
    }
}

bool KeyboardControl::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->addFrame){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton){
                layoutmanagerObj->exec();
                return true;
            } else
                return false;
        }
    }
    return QObject::eventFilter(watched, event);

}
