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
#include "ukmedia_output_widget.h"
#include "ukui_list_widget_item.h"
#include <QHBoxLayout>
#include <QDebug>
AudioSlider::AudioSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

AudioSlider::~AudioSlider()
{

}

UkmediaOutputWidget::UkmediaOutputWidget(QWidget *parent) : QWidget(parent)
{
//    QFILEDEVICE_H
    m_pOutputListWidget = new QListWidget(this);
    m_pOutputListWidget->setFixedHeight(250);

    m_pOutputListWidget->setStyleSheet(

                "QListWidget{"
                "background-color:palette(base);"
                "padding-left:8;"
                "padding-right:20;"
                "padding-top:8;"
                "padding-bottom:8;}"
                "QListWidget::item{"
                "border-radius:6px;}"
                /**列表项扫过*/
                "QListWidget::item:hover{"
                "background-color:rgba(55,144,250,0.5);}"
                /**列表项选中*/
                "QListWidget::item::selected{"
                "background-color:rgba(55,144,250,1);"
                "border-width:0;}");
    //加载qss样式文件
    QFile QssFile("://combox.qss");
    QssFile.open(QFile::ReadOnly);

    if (QssFile.isOpen()){
        sliderQss = QLatin1String(QssFile.readAll());
        QssFile.close();
    } else {
        qDebug()<<"combox.qss is not found"<<endl;
    }

    m_pOutputWidget = new QWidget(this);
    m_pOutputDeviceWidget = new QFrame(m_pOutputWidget);
    m_pMasterVolumeWidget = new QFrame(m_pOutputWidget);
    m_pChannelBalanceWidget = new QFrame(m_pOutputWidget);
    m_pOutputPortWidget = new QFrame(m_pOutputWidget);
    m_pProfileWidget = new QFrame(m_pOutputWidget);
    m_pselectWidget = new QFrame(m_pOutputWidget);

    m_pselectWidget->setFrameShape(QFrame::Shape::Box);
    m_pProfileWidget->setFrameShape(QFrame::Shape::Box);
    m_pOutputDeviceWidget->setFrameShape(QFrame::Shape::Box);
    m_pMasterVolumeWidget->setFrameShape(QFrame::Shape::Box);
    m_pChannelBalanceWidget->setFrameShape(QFrame::Shape::Box);
    m_pOutputPortWidget->setFrameShape(QFrame::Shape::Box);

    //设置大小
    m_pOutputWidget->setMinimumSize(550,422);
    m_pOutputWidget->setMaximumSize(960,422);
    m_pOutputDeviceWidget->setMinimumSize(550,319);
    m_pOutputDeviceWidget->setMaximumSize(960,319);
    m_pMasterVolumeWidget->setMinimumSize(550,50);
    m_pMasterVolumeWidget->setMaximumSize(960,50);
    m_pChannelBalanceWidget->setMinimumSize(550,50);
    m_pChannelBalanceWidget->setMaximumSize(960,50);
    m_pOutputPortWidget->setMinimumSize(550,50);
    m_pOutputPortWidget->setMaximumSize(960,50);
    m_pProfileWidget->setMinimumSize(550,50);
    m_pProfileWidget->setMaximumSize(960,50);

    m_pOutputLabel = new TitleLabel(this);
    m_pOutputLabel->setText(tr("Output"));
    m_pOutputLabel->setStyleSheet("QLabel{color: palette(windowText);}");
    //~ contents_path /audio/Output Device
    m_pOutputDeviceLabel = new QLabel(tr("Output Device:"),m_pOutputWidget);
    m_pOutputDeviceCombobox = new QComboBox(m_pOutputDeviceWidget);
    m_pOutputDeviceCombobox->setFixedWidth(300);
    m_pOutputDeviceCombobox->hide();
    //~ contents_path /audio/Master Volume
    m_pOpVolumeLabel = new QLabel(tr("Master Volume"),m_pMasterVolumeWidget);
    m_pOutputIconBtn = new UkuiButtonDrawSvg(m_pMasterVolumeWidget);
    m_pOpVolumeSlider = new AudioSlider(m_pMasterVolumeWidget);
    m_pOpVolumePercentLabel = new QLabel(m_pMasterVolumeWidget);
    //~ contents_path /audio/Balance
    m_pOpBalanceLabel = new QLabel(tr("Balance"),m_pChannelBalanceWidget);
    m_pLeftBalanceLabel = new QLabel(tr("Left"),m_pChannelBalanceWidget);
    m_pOpBalanceSlider = new UkmediaVolumeSlider(m_pChannelBalanceWidget,true);
    m_pRightBalanceLabel = new QLabel(tr("Right"),m_pChannelBalanceWidget);
    m_pOutputPortLabel = new QLabel(tr("Connector"),m_pOutputPortWidget);
    m_pOutputPortCombobox = new QComboBox(m_pOutputPortWidget);
    //~ contents_path /audio/Profile
    m_pProfileLabel = new QLabel(tr("Profile"),m_pProfileWidget);
    m_pProfileCombobox = new QComboBox(m_pProfileWidget);
    m_pProfileCombobox->setMinimumSize(50,32);
    m_pProfileCombobox->setMaximumSize(900,32);
    m_pselectWidget->setMinimumSize(550,50);
    m_pselectWidget->setMaximumSize(960,50);
    //~ contents_path /audio/Card
    m_pSelectDeviceLabel = new QLabel(tr("Card"),m_pselectWidget);
    m_pSelectCombobox = new QComboBox(m_pselectWidget);
    m_pProfileLabel->setFixedSize(115,24);
    m_pProfileLabel->setFixedSize(150,32);

    m_pOpVolumeSlider->setOrientation(Qt::Horizontal);
    m_pOpBalanceSlider->setOrientation(Qt::Horizontal);
    m_pOpVolumeSlider->setRange(0,100);
    m_pOutputIconBtn->setFocusPolicy(Qt::NoFocus);
    //输出设备添加布局
//    QHBoxLayout *noOutputDeviceLayout = new QHBoxLayout();
//    noOutputDeviceLayout->addWidget(m_pNoAvailableOutputLabel);
//    m_pOutputDeviceWidget->setLayout(noOutputDeviceLayout);

    QVBoxLayout *outputDeviceLayout = new QVBoxLayout();
//    m_pOutputDeviceLabel->setFixedSize(115,24);
    m_pOutputDeviceLabel->setFixedSize(150,32);
    m_pOutputDeviceCombobox->setMinimumSize(50,32);
    m_pOutputDeviceCombobox->setMaximumSize(900,32);
    outputDeviceLayout->addWidget(m_pOutputDeviceLabel);
    outputDeviceLayout->addWidget(m_pOutputListWidget);

//    outputDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
//    outputDeviceLayout->addWidget(m_pOutputDeviceLabel);
//    outputDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
//    outputDeviceLayout->addWidget(m_pOutputDeviceCombobox);
//    outputDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
//    outputDeviceLayout->setSpacing(0);
    m_pOutputDeviceWidget->setLayout(outputDeviceLayout);
    outputDeviceLayout->layout()->setContentsMargins(16,14,16,14);

    //主音量添加布局
    QHBoxLayout *masterLayout = new QHBoxLayout(m_pMasterVolumeWidget);
    m_pOpVolumeLabel->setFixedSize(150,32);
    m_pOutputIconBtn->setFixedSize(24,24);
    m_pOpVolumeSlider->setFixedHeight(20);
    m_pOpVolumePercentLabel->setFixedSize(55,24);
    masterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    masterLayout->addWidget(m_pOpVolumeLabel);
    masterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    masterLayout->addWidget(m_pOutputIconBtn);
    masterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    masterLayout->addWidget(m_pOpVolumeSlider);
    masterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    masterLayout->addWidget(m_pOpVolumePercentLabel);
    masterLayout->addItem(new QSpacerItem(10,20,QSizePolicy::Fixed));
    masterLayout->setSpacing(0);
    m_pMasterVolumeWidget->setLayout(masterLayout);
    m_pMasterVolumeWidget->layout()->setContentsMargins(0,0,0,0);
    //声道平衡添加布局
    QHBoxLayout *soundLayout = new QHBoxLayout(m_pChannelBalanceWidget);
    m_pOpBalanceLabel->setFixedSize(150,32);
    m_pLeftBalanceLabel->setFixedSize(36,24);
    m_pOpBalanceSlider->setFixedHeight(20);
    m_pRightBalanceLabel->setFixedSize(55,28);
    soundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    soundLayout->addWidget(m_pOpBalanceLabel);
    soundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    soundLayout->addWidget(m_pLeftBalanceLabel);
    soundLayout->addItem(new QSpacerItem(4,20,QSizePolicy::Fixed));
    soundLayout->addWidget(m_pOpBalanceSlider);
    soundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    soundLayout->addWidget(m_pRightBalanceLabel);
    soundLayout->addItem(new QSpacerItem(10,20,QSizePolicy::Fixed));
    soundLayout->setSpacing(0);
    m_pChannelBalanceWidget->setLayout(soundLayout);
    m_pChannelBalanceWidget->layout()->setContentsMargins(0,0,0,0);
    //输出连接器布局
    QHBoxLayout *outputPortLayout = new QHBoxLayout(m_pOutputPortWidget);
    m_pOutputPortCombobox->setMinimumSize(50,32);
    m_pOutputPortCombobox->setMaximumSize(900,32);
    m_pOutputPortLabel->setFixedSize(150,32);
    outputPortLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    outputPortLayout->addWidget(m_pOutputPortLabel);
    outputPortLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    outputPortLayout->addWidget(m_pOutputPortCombobox);
    outputPortLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    outputPortLayout->setSpacing(0);
    m_pOutputPortWidget->setLayout(outputPortLayout);
    outputPortLayout->layout()->setContentsMargins(0,0,0,0);

    //选择的设备布局
    QHBoxLayout *pSelectDeviceLayout = new QHBoxLayout();
    m_pSelectDeviceLabel->setFixedSize(150,32);
    m_pSelectCombobox->setMinimumSize(50,32);
    m_pSelectCombobox->setMaximumSize(900,32);
    pSelectDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    pSelectDeviceLayout->addWidget(m_pSelectDeviceLabel);
    pSelectDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    pSelectDeviceLayout->addWidget(m_pSelectCombobox);
    pSelectDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    pSelectDeviceLayout->setSpacing(0);
    m_pselectWidget->setLayout(pSelectDeviceLayout);
    pSelectDeviceLayout->layout()->setContentsMargins(0,0,0,0);

    //配置文件布局
    QHBoxLayout *profileLayout = new QHBoxLayout(m_pProfileWidget);

    profileLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    profileLayout->addWidget(m_pProfileLabel);
    profileLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    profileLayout->addWidget(m_pProfileCombobox);
    profileLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    profileLayout->setSpacing(0);
    m_pProfileWidget->setLayout(profileLayout);
    profileLayout->layout()->setContentsMargins(0,0,0,0);
    //进行整体布局
    m_pVlayout = new QVBoxLayout;

    m_pVlayout->addWidget(m_pOutputDeviceWidget);
    m_pVlayout->addWidget(m_pMasterVolumeWidget);
//    m_pVlayout->addWidget(m_pselectWidget);
//    m_pVlayout->addWidget(m_pProfileWidget);
    m_pVlayout->addWidget(m_pChannelBalanceWidget);
    m_pVlayout->setSpacing(1);

    m_pOutputWidget->setLayout(m_pVlayout);
    m_pOutputWidget->layout()->setContentsMargins(0,0,0,0);
    m_pselectWidget->hide();
    m_pProfileWidget->hide();
    m_pOutputPortWidget->hide();
    QVBoxLayout *vLayout1 = new QVBoxLayout(this);
    vLayout1->addWidget(m_pOutputLabel);
    vLayout1->addItem(new QSpacerItem(16,0,QSizePolicy::Fixed));
    vLayout1->addWidget(m_pOutputWidget);
    this->setLayout(vLayout1);
    this->layout()->setContentsMargins(0,0,0,0);

    m_pOutputDeviceWidget->setObjectName("outputDeviceWidget");
    m_pMasterVolumeWidget->setObjectName("masterVolumeWidget");
    //设置样式
    m_pOutputLabel->setObjectName("m_pOutputLabel");

}

void UkmediaOutputWidget::outputWidgetAddPort()
{
    m_pOutputWidget->setMinimumSize(550,505);
    m_pOutputWidget->setMaximumSize(960,505);
    m_pVlayout->insertWidget(3,m_pOutputPortWidget);
    m_pOutputPortWidget->show();
}

void UkmediaOutputWidget::outputWidgetRemovePort()
{
    m_pOutputWidget->setMinimumSize(550,454);
    m_pOutputWidget->setMaximumSize(960,454);
    m_pVlayout->removeWidget(m_pOutputPortWidget);
    m_pOutputPortWidget->hide();
}

UkmediaOutputWidget::~UkmediaOutputWidget()
{

}
