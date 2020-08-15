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
#include "ukmedia_sound_effects_widget.h"
#include <QHBoxLayout>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>

UkuiMessageBox::UkuiMessageBox()
{
}

UkmediaSoundEffectsWidget::UkmediaSoundEffectsWidget(QWidget *parent) : QWidget(parent)
{
    m_pThemeWidget = new QFrame(this);
    m_pShutDownWidget = new QFrame(this);
    m_pLagoutWidget = new QFrame(this);
    m_pAlertSoundSwitchWidget = new QFrame(this);
    m_pWindowClosedWidget = new QFrame(this);
    m_pVolumeChangeWidget = new QFrame(this);
    m_pSettingSoundWidget = new QFrame(this);

    m_pThemeWidget->setFrameShape(QFrame::Shape::Box);
    m_pShutDownWidget->setFrameShape(QFrame::Shape::Box);
    m_pLagoutWidget->setFrameShape(QFrame::Shape::Box);
    m_pAlertSoundSwitchWidget->setFrameShape(QFrame::Shape::Box);
    m_pWindowClosedWidget->setFrameShape(QFrame::Shape::Box);
    m_pVolumeChangeWidget->setFrameShape(QFrame::Shape::Box);
    m_pSettingSoundWidget->setFrameShape(QFrame::Shape::Box);

    //~ contents_path /audio/System Sound
    m_pSoundEffectLabel = new QLabel(tr("System Sound"),this);
    m_pSoundEffectLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    //~ contents_path /audio/Sound Theme
    m_pSoundThemeLabel = new QLabel(tr("Sound Theme"),m_pThemeWidget);
    m_pSoundThemeCombobox = new QComboBox(m_pThemeWidget);
    //~ contents_path /audio/Alert Sound
    m_pShutdownlabel = new QLabel(tr("Alert Sound"),m_pShutDownWidget);
    m_pShutdownCombobox = new QComboBox(m_pShutDownWidget);
    //~ contents_path /audio/Boot Music
    m_pLagoutLabel = new QLabel(tr("Boot Music"),m_pLagoutWidget);
    m_pLagoutCombobox = new QComboBox(m_pLagoutWidget);
    m_pBootButton = new SwitchButton(m_pLagoutWidget);
    m_pWindowClosedCombobox = new QComboBox(m_pWindowClosedWidget);
    m_pVolumeChangeCombobox = new QComboBox(m_pVolumeChangeWidget);
    m_pSettingSoundCombobox = new QComboBox(m_pSettingSoundWidget);

    //~ contents_path /audio/Beep Switch
    m_pAlertSoundSwitchLabel = new QLabel(tr("Beep Switch"),m_pAlertSoundSwitchWidget);
    m_pAlertSoundSwitchButton = new SwitchButton(m_pAlertSoundSwitchWidget);

    m_pWindowClosedLabel = new QLabel(tr("Window Closed"),m_pWindowClosedWidget);
    m_pVolumeChangeLabel = new QLabel(tr("Volume Change"),m_pVolumeChangeWidget);
    m_pSettingSoundLabel = new QLabel(tr("Setting Menu"),m_pSettingSoundWidget);
    //设置大小
    m_pThemeWidget->setMinimumSize(550,50);
    m_pThemeWidget->setMaximumSize(960,50);
    m_pShutDownWidget->setMinimumSize(550,50);
    m_pShutDownWidget->setMaximumSize(960,50);
    m_pLagoutWidget->setMinimumSize(550,50);
    m_pLagoutWidget->setMaximumSize(960,50);
    m_pAlertSoundSwitchWidget->setMinimumSize(550,50);
    m_pAlertSoundSwitchWidget->setMaximumSize(960,50);
    m_pWindowClosedWidget->setMinimumSize(550,50);
    m_pWindowClosedWidget->setMaximumSize(960,50);
    m_pVolumeChangeWidget->setMinimumSize(550,50);
    m_pVolumeChangeWidget->setMaximumSize(960,50);
    m_pSettingSoundWidget->setMinimumSize(550,50);
    m_pSettingSoundWidget->setMaximumSize(960,50);

    m_pSoundEffectLabel->setFixedSize(120,24);
    m_pSoundThemeLabel->setFixedSize(115,24);
    m_pShutdownlabel->setFixedSize(115,24);
    m_pLagoutLabel->setFixedSize(115,24);
    m_pWindowClosedLabel->setFixedSize(115,24);
    m_pVolumeChangeLabel->setFixedSize(115,24);
    m_pSettingSoundLabel->setFixedSize(115,24);

    m_pSoundThemeCombobox->setMinimumSize(50,32);
    m_pSoundThemeCombobox->setMaximumSize(900,32);

    m_pShutdownCombobox->setMinimumSize(50,32);
    m_pShutdownCombobox->setMaximumSize(900,32);
    m_pLagoutCombobox->setMinimumSize(50,32);
    m_pLagoutCombobox->setMaximumSize(900,32);
    m_pWindowClosedCombobox->setMinimumSize(50,32);
    m_pWindowClosedCombobox->setMaximumSize(900,32);
    m_pVolumeChangeCombobox->setMinimumSize(50,32);
    m_pVolumeChangeCombobox->setMaximumSize(900,32);
    m_pSettingSoundCombobox->setMinimumSize(50,32);
    m_pSettingSoundCombobox->setMaximumSize(900,32);
    //添加布局
    QHBoxLayout *themeLayout = new QHBoxLayout(m_pThemeWidget);
    themeLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    themeLayout->addWidget(m_pSoundThemeLabel);
    themeLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    themeLayout->addWidget(m_pSoundThemeCombobox);
    themeLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    themeLayout->setSpacing(0);
    m_pThemeWidget->setLayout(themeLayout);
    m_pThemeWidget->layout()->setContentsMargins(0,0,0,0);

    QHBoxLayout *shutdownLayout = new QHBoxLayout(m_pShutDownWidget);
    shutdownLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    shutdownLayout->addWidget(m_pShutdownlabel);
    shutdownLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    shutdownLayout->addWidget(m_pShutdownCombobox);
    shutdownLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    shutdownLayout->setSpacing(0);
    m_pShutDownWidget->setLayout(shutdownLayout);
    m_pShutDownWidget->layout()->setContentsMargins(0,0,0,0);

    //开关机音乐设置开关
    QHBoxLayout *lagoutLayout = new QHBoxLayout(m_pLagoutWidget);
    lagoutLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    lagoutLayout->addWidget(m_pLagoutLabel);
    lagoutLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Expanding));
//    lagoutLayout->addWidget(m_pLagoutCombobox);
    lagoutLayout->addWidget(m_pBootButton);
    lagoutLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    lagoutLayout->setSpacing(0);
    m_pLagoutWidget->setLayout(lagoutLayout);
    m_pLagoutWidget->layout()->setContentsMargins(0,0,0,0);
//    m_pLagoutWidget->setVisible(false);
    m_pLagoutCombobox->setVisible(false);

    //提示音开关布局
    QHBoxLayout *alertSoundLayout = new QHBoxLayout(m_pAlertSoundSwitchWidget);
    alertSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    alertSoundLayout->addWidget(m_pAlertSoundSwitchLabel);
    alertSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Expanding));
//    lagoutLayout->addWidget(m_pLagoutCombobox);
    alertSoundLayout->addWidget(m_pAlertSoundSwitchButton);
    alertSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    alertSoundLayout->setSpacing(0);
    m_pAlertSoundSwitchWidget->setLayout(alertSoundLayout);
    m_pAlertSoundSwitchWidget->layout()->setContentsMargins(0,0,0,0);

    //窗口关闭提示音
    QHBoxLayout *windowClosedLayout = new QHBoxLayout(m_pWindowClosedWidget);
    windowClosedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    windowClosedLayout->addWidget(m_pWindowClosedLabel);
    windowClosedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    windowClosedLayout->addWidget(m_pWindowClosedCombobox);
    windowClosedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    windowClosedLayout->setSpacing(0);
    m_pWindowClosedWidget->setLayout(windowClosedLayout);
    m_pWindowClosedWidget->layout()->setContentsMargins(0,0,0,0);

    //音量改变提示音
    QHBoxLayout *volumeChangedLayout = new QHBoxLayout(m_pVolumeChangeWidget);
    volumeChangedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    volumeChangedLayout->addWidget(m_pVolumeChangeLabel);
    volumeChangedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    volumeChangedLayout->addWidget(m_pVolumeChangeCombobox);
    volumeChangedLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    volumeChangedLayout->setSpacing(0);
    m_pVolumeChangeWidget->setLayout(volumeChangedLayout);
    m_pVolumeChangeWidget->layout()->setContentsMargins(0,0,0,0);
    //设置菜单提示音
    QHBoxLayout *settingMenuLayout = new QHBoxLayout(m_pSettingSoundWidget);
    settingMenuLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    settingMenuLayout->addWidget(m_pSettingSoundLabel);
    settingMenuLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    settingMenuLayout->addWidget(m_pSettingSoundCombobox);
    settingMenuLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    settingMenuLayout->setSpacing(0);
    m_pSettingSoundWidget->setLayout(settingMenuLayout);
    m_pSettingSoundWidget->layout()->setContentsMargins(0,0,0,0);

    //进行整体布局
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_pSoundEffectLabel);
    vLayout->addItem(new QSpacerItem(16,14,QSizePolicy::Fixed));
    vLayout->addWidget(m_pLagoutWidget);
    vLayout->addItem(new QSpacerItem(16,6,QSizePolicy::Fixed));
    vLayout->addWidget(m_pAlertSoundSwitchWidget);

    vLayout->addItem(new QSpacerItem(16,7,QSizePolicy::Fixed));
    vLayout->addWidget(m_pThemeWidget);
    vLayout->addWidget(m_pShutDownWidget);
//    vLayout->addWidget(m_pWindowClosedWidget);
    vLayout->addWidget(m_pVolumeChangeWidget);
//    vLayout->addWidget(m_pSettingSoundWidget);
    this->setLayout(vLayout);
    vLayout->setSpacing(1);
    this->layout()->setContentsMargins(0,0,0,0);
    m_pSettingSoundWidget->hide();
    m_pWindowClosedWidget->hide();
}

UkmediaSoundEffectsWidget::~UkmediaSoundEffectsWidget()
{

}
