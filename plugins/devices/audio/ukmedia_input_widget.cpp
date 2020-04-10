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
#include "ukmedia_input_widget.h"
#include <QDebug>
#include <QFile>
#include <QApplication>

UkmediaInputWidget::UkmediaInputWidget(QWidget *parent) : QWidget(parent)
{
    m_pInputWidget = new QWidget(this);
    m_pInputDeviceWidget = new QWidget(m_pInputWidget);
    m_pVolumeWidget = new QWidget(m_pInputWidget);
    m_pInputLevelWidget = new QWidget(m_pInputWidget);

    itemDelege = new QStyledItemDelegate();

    //加载qss样式文件
    QFile QssFile("://combox.qss");
    QssFile.open(QFile::ReadOnly);

    if (QssFile.isOpen()){
        sliderQss = QLatin1String(QssFile.readAll());
        QssFile.close();
    } else {
        qDebug()<<"combox.qss is not found"<<endl;
    }
    itemDelege = new QStyledItemDelegate();

    //设置大小
    m_pInputWidget->setMinimumSize(550,150);
    m_pInputWidget->setMaximumSize(960,150);
    m_pInputDeviceWidget->setMinimumSize(550,50);
    m_pInputDeviceWidget->setMaximumSize(960,50);
    m_pVolumeWidget->setMinimumSize(550,50);
    m_pVolumeWidget->setMaximumSize(960,50);
    m_pInputLevelWidget->setMinimumSize(550,50);
    m_pInputLevelWidget->setMaximumSize(960,50);

    m_pInputLabel = new QLabel(tr("Input"),this);
    m_pInputDeviceLabel = new QLabel(tr("Select input device"),m_pInputWidget);
    m_pInputDeviceCombobox = new QComboBox(m_pInputDeviceWidget);

    m_pIpVolumeLabel = new QLabel(tr("volume"),m_pVolumeWidget);
    m_pInputIconBtn = new QPushButton(m_pVolumeWidget);
    m_pIpVolumeSlider = new AudioSlider(m_pVolumeWidget);
    m_pIpVolumePercentLabel = new QLabel(m_pVolumeWidget);
    m_pInputLevelLabel = new QLabel(tr("Input level"),m_pInputLevelWidget);
    m_pLowLevelLabel = new QLabel(tr("low"),m_pInputLevelWidget);
    m_pInputLevelSlider = new AudioSlider(m_pInputLevelWidget);
    m_pHighLevelLabel = new QLabel(tr("high"),m_pInputLevelWidget);

    m_pIpVolumeSlider->setOrientation(Qt::Horizontal);
    m_pInputLevelSlider->setOrientation(Qt::Horizontal);
    m_pIpVolumeSlider->setRange(0,100);
    m_pInputIconBtn->setFocusPolicy(Qt::NoFocus);
    //输出设备添加布局
    QHBoxLayout *m_pInputDeviceLayout = new QHBoxLayout(m_pInputDeviceWidget);
    m_pInputLabel->setFixedSize(83,24);
    m_pInputDeviceCombobox->setMinimumSize(50,32);
    m_pInputDeviceCombobox->setMaximumSize(900,32);
    m_pInputDeviceLabel->setFixedSize(83,24);
    m_pInputDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pInputDeviceLayout->addWidget(m_pInputDeviceLabel);
    m_pInputDeviceLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    m_pInputDeviceLayout->addWidget(m_pInputDeviceCombobox);
    m_pInputDeviceLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pInputDeviceLayout->setSpacing(0);
    m_pInputDeviceWidget->setLayout(m_pInputDeviceLayout);
    m_pInputDeviceLayout->layout()->setContentsMargins(0,0,0,0);
    //主音量添加布局
    QHBoxLayout *m_pMasterLayout = new QHBoxLayout(m_pVolumeWidget);
    m_pIpVolumeLabel->setFixedSize(83,24);
    m_pInputIconBtn->setFixedSize(24,24);
    m_pIpVolumeSlider->setFixedHeight(20);
    m_pIpVolumePercentLabel->setFixedSize(36,24);
    m_pMasterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pMasterLayout->addWidget(m_pIpVolumeLabel);
    m_pMasterLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    m_pMasterLayout->addWidget(m_pInputIconBtn);
    m_pMasterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pMasterLayout->addWidget(m_pIpVolumeSlider);
    m_pMasterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pMasterLayout->addWidget(m_pIpVolumePercentLabel);
    m_pMasterLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pMasterLayout->setSpacing(0);
    m_pVolumeWidget->setLayout(m_pMasterLayout);
    m_pVolumeWidget->layout()->setContentsMargins(0,0,0,0);
    //声道平衡添加布局
    QHBoxLayout *m_pSoundLayout = new QHBoxLayout(m_pInputLevelWidget);
    m_pInputLevelLabel->setFixedSize(83,24);
    m_pLowLevelLabel->setFixedSize(24,24);
    m_pInputLevelSlider->setFixedHeight(20);
    m_pHighLevelLabel->setFixedSize(36,24);
    m_pSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pSoundLayout->addWidget(m_pInputLevelLabel);
    m_pSoundLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    m_pSoundLayout->addWidget(m_pLowLevelLabel);
    m_pSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pSoundLayout->addWidget(m_pInputLevelSlider);
    m_pSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pSoundLayout->addWidget(m_pHighLevelLabel);
    m_pSoundLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pSoundLayout->setSpacing(0);
    m_pInputLevelWidget->setLayout(m_pSoundLayout);
    m_pInputLevelWidget->layout()->setContentsMargins(0,0,0,0);
    //进行整体布局
    QVBoxLayout *m_pVlayout = new QVBoxLayout(m_pInputWidget);
    m_pVlayout->addWidget(m_pInputDeviceWidget);
    m_pVlayout->addWidget(m_pVolumeWidget);
    m_pVlayout->addWidget(m_pInputLevelWidget);
    m_pInputWidget->setLayout(m_pVlayout);
    m_pVlayout->setSpacing(0);
    m_pInputWidget->layout()->setContentsMargins(0,0,0,0);

    QVBoxLayout *m_pVlayout1 = new QVBoxLayout(this);
    m_pVlayout1->addWidget(m_pInputLabel);
    m_pVlayout1->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    m_pVlayout1->addWidget(m_pInputWidget);
    this->setLayout(m_pVlayout1);
    this->layout()->setContentsMargins(0,0,0,0);


//    m_pInputDeviceCombobox->setStyleSheet(qss);
    m_pInputDeviceCombobox->setItemDelegate(itemDelege);

    //设置样式
    m_pInputLabel->setStyleSheet("QLabel{font-size:18px;"
                                   "font-family:Noto Sans S Chinese;"
                                   "font-weight:500;"
                                   "color:rgba(0,0,0,1);"
                                   "background-color: rgba(0,0,0,0);"
                                   "line-height:40px;}");
    m_pInputDeviceLabel->setStyleSheet("QLabel{font-size:14px;"
                                       "font-family:Noto Sans S Chinese;"
                                       "font-weight:400;"
                                       "color:rgba(67,67,69,1);"
                                       "line-height:40px;}");
    m_pIpVolumeLabel->setStyleSheet("QLabel{font-size:14px;"
                                    "font-family:Noto Sans S Chinese;"
                                    "font-weight:400;"
                                    "color:rgba(67,67,69,1);"
                                    "line-height:40px;}");
    m_pIpVolumePercentLabel->setStyleSheet("QLabel{font-size:14px;"
                                           "font-family:Noto Sans S Chinese;"
                                           "font-weight:400;"
                                           "color:rgba(67,67,69,1);"
                                           "line-height:40px;}");
    m_pInputLevelLabel->setStyleSheet("QLabel{font-size:14px;"
                                      "font-family:Noto Sans S Chinese;"
                                      "font-weight:400;"
                                      "color:rgba(67,67,69,1);"
                                      "line-height:40px;}");

    m_pLowLevelLabel->setStyleSheet("QLabel{font-size:14px;"
                                    "font-family:Noto Sans S Chinese;"
                                    "font-weight:400;"
                                    "color:rgba(67,67,69,1);"
                                    "line-height:40px;}");
    m_pHighLevelLabel->setStyleSheet("QLabel{font-size:14px;"
                                     "font-family:Noto Sans S Chinese;"
                                     "font-weight:400;"
                                     "color:rgba(67,67,69,1);"
                                     "line-height:40px;}");

    m_pIpVolumeSlider->setStyleSheet("QSlider::groove:horizontal {"
                                  "border: 0px solid #bbb; }"
                                  "QSlider::sub-page:horizontal {"
                                  "background: #3D6BE5;border-radius: 2px;"
                                  "margin-top:8px;margin-bottom:9px;}"
                                  "QSlider::add-page:horizontal {"
                                  "background:  rgba(52,70,80,90%);"
                                  "border: 0px solid #777;"
                                  "border-radius: 2px;"
                                  "margin-top:8px;"
                                  "margin-bottom:9px;}"
                                  "QSlider::handle:horizontal {"
                                  "width: 20px;"
                                  "height: 20px;"
                                  "background: rgb(61,107,229);"
                                  "border-radius:10px;}");
    m_pInputLevelSlider->setStyleSheet("QSlider::groove:horizontal {"
                                   "border: 0px solid #bbb; }"
                                   "QSlider::sub-page:horizontal {"
                                   "background: #3D6BE5;border-radius: 2px;"
                                   "margin-top:8px;margin-bottom:9px;}"
                                   "QSlider::add-page:horizontal {"
                                   "background:  rgba(52,70,80,90%);"
                                   "border: 0px solid #777;"
                                   "border-radius: 2px;"
                                   "margin-top:8px;"
                                   "margin-bottom:9px;}"
                                   "QSlider::handle:horizontal {"
                                   "width: 20px;"
                                   "height: 20px;"
                                   "background: rgb(61,107,229);"
                                   "border-radius:10px;}");

    m_pInputDeviceCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
                                       "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
                                       "QComboBox QAbstractItemView{"
                                       "font-size: 14px;height: 80px;width: 140px;}"
                                       "QComboBox QAbstractItemView::item {"
                                       "height: 30px;}"
                                       "QComboBox QAbstractItemView::item:hover {"
                                       "background: rgba(218,227,250,0.9);"
                                       "border-radius: 2px;"
                                       "font-color: rgb(225, 0, 0);}"
                                       "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
                                       "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
                                       "border-radius:4px;}"
                                       "QComboBox::down-arrow:hover{"
                                       "width:184px;height:30px;"
                                       "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                       "border-radius:2px;}"
                                       "QComboBox::down-arrow {"
                                       "image: url(/usr/share/ukui-media/img/dowm.png);"
                                       "height:6px;width:11px;}"
                                       "QComboBox::down-arrow:hover{"
                                       "width:184px;height:30px;"
                                       "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                       "border-radius:2px;}");

    m_pInputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    m_pInputWidget->setStyleSheet("QWidget{width: 552px;height: 150px;"
                        "background: rgba(244,244,244,1);"
                        "border-radius: 4px;}");
    m_pInputDeviceWidget->setObjectName("inputDeviceWidget");
    m_pVolumeWidget->setObjectName("volumeWidget");
    m_pInputDeviceWidget->setStyleSheet("QWidget#inputDeviceWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
    m_pVolumeWidget->setStyleSheet("QWidget#volumeWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
}

UkmediaInputWidget::~UkmediaInputWidget()
{

}
