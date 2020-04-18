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

    m_pThemeWidget->setFrameShape(QFrame::Shape::Box);
    m_pShutDownWidget->setFrameShape(QFrame::Shape::Box);
    m_pLagoutWidget->setFrameShape(QFrame::Shape::Box);
    m_pSoundEffectLabel = new QLabel(tr("System sound"),this);
    m_pSoundEffectLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    m_pSoundThemeLabel = new QLabel(tr("System sound theme"),m_pThemeWidget);
    m_pSoundThemeCombobox = new QComboBox(m_pThemeWidget);
    m_pShutdownlabel = new QLabel(tr("prompt voice"),m_pShutDownWidget);
    m_pShutdownCombobox = new QComboBox(m_pShutDownWidget);
    m_pLagoutLabel = new QLabel(tr("Lagout"),m_pLagoutWidget);
    m_pLagoutCombobox = new QComboBox(m_pLagoutWidget);

    //设置大小
    m_pThemeWidget->setMinimumSize(550,50);
    m_pThemeWidget->setMaximumSize(960,50);
    m_pShutDownWidget->setMinimumSize(550,50);
    m_pShutDownWidget->setMaximumSize(960,50);
    m_pLagoutWidget->setMinimumSize(550,50);
    m_pLagoutWidget->setMaximumSize(960,50);

    m_pSoundEffectLabel->setFixedSize(83,24);
    m_pSoundThemeLabel->setFixedSize(83,24);
    m_pShutdownlabel->setFixedSize(83,24);
    m_pLagoutLabel->setFixedSize(83,24);

    m_pSoundThemeCombobox->setMinimumSize(50,32);
    m_pSoundThemeCombobox->setMaximumSize(900,32);

    m_pShutdownCombobox->setMinimumSize(50,32);
    m_pShutdownCombobox->setMaximumSize(900,32);
    m_pLagoutCombobox->setMinimumSize(50,32);
    m_pLagoutCombobox->setMaximumSize(900,32);

    //添加布局
    QHBoxLayout *themeLayout = new QHBoxLayout(m_pThemeWidget);
    themeLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    themeLayout->addWidget(m_pSoundThemeLabel);
    themeLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    themeLayout->addWidget(m_pSoundThemeCombobox);
    themeLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    themeLayout->setSpacing(0);
    m_pThemeWidget->setLayout(themeLayout);
    m_pThemeWidget->layout()->setContentsMargins(0,0,0,0);

    QHBoxLayout *shutdownLayout = new QHBoxLayout(m_pShutDownWidget);
    shutdownLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    shutdownLayout->addWidget(m_pShutdownlabel);
    shutdownLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    shutdownLayout->addWidget(m_pShutdownCombobox);
    shutdownLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    shutdownLayout->setSpacing(0);
    m_pShutDownWidget->setLayout(shutdownLayout);
    m_pShutDownWidget->layout()->setContentsMargins(0,0,0,0);

    QHBoxLayout *lagoutLayout = new QHBoxLayout(m_pLagoutWidget);
    lagoutLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    lagoutLayout->addWidget(m_pLagoutLabel);
    lagoutLayout->addItem(new QSpacerItem(48,20,QSizePolicy::Fixed));
    lagoutLayout->addWidget(m_pLagoutCombobox);
    lagoutLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    lagoutLayout->setSpacing(0);
    m_pLagoutWidget->setLayout(lagoutLayout);
    m_pLagoutWidget->layout()->setContentsMargins(0,0,0,0);
    m_pLagoutWidget->setVisible(false);
    //进行整体布局
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_pSoundEffectLabel);
    vLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    vLayout->addWidget(m_pThemeWidget);
    vLayout->addItem(new QSpacerItem(16,20,QSizePolicy::Fixed));
    vLayout->addWidget(m_pShutDownWidget);
//    vLayout->addWidget(m_pLagoutWidget);
    this->setLayout(vLayout);
//    vLayout->setSpacing(0);
    this->layout()->setContentsMargins(0,0,0,0);

//    m_pLagoutCombobox->setStyleSheet(qss);
//    m_pLagoutCombobox->setItemDelegate(itemDelegate);
//    m_pShutDownWidget->setObjectName("shutDownWidget");
//    m_pSoundThemeCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
//                                      "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
//                                      "QComboBox QAbstractItemView{"
//                                      "font-size: 14px;height: 80px;width: 140px;}"
//                                      "QComboBox QAbstractItemView::item {"
//                                      "height: 30px;}"
//                                      "QComboBox QAbstractItemView::item:hover {"
//                                      "background: rgba(218,227,250,0.9);"
//                                      "border-radius: 2px;"
//                                      "font-color: rgb(225, 0, 0);}"
//                                      "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
//                                      "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
//                                      "border-radius:4px;}"
//                                      "QComboBox::down-arrow:hover{"
//                                      "width:184px;height:30px;"
//                                      "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                      "border-radius:2px;}"
//                                      "QComboBox::down-arrow {"
//                                      "image: url(/usr/share/ukui-media/img/dowm.png);"
//                                      "height:6px;width:11px;}"
//                                      "QComboBox::down-arrow:hover{"
//                                      "width:184px;height:30px;"
//                                      "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                      "border-radius:2px;}");
//    m_pShutdownCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
//                                    "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
//                                    "QComboBox QAbstractItemView{"
//                                    "font-size: 14px;height: 80px;width: 140px;}"
//                                    "QComboBox QAbstractItemView::item {"
//                                    "height: 30px;}"
//                                    "QComboBox QAbstractItemView::item:hover {"
//                                    "background: rgba(218,227,250,0.9);"
//                                    "border-radius: 2px;"
//                                    "font-color: rgb(225, 0, 0);}"
//                                    "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
//                                    "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
//                                    "border-radius:4px;}"
//                                    "QComboBox::down-arrow:hover{"
//                                    "width:184px;height:30px;"
//                                    "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                    "border-radius:2px;}"
//                                    "QComboBox::down-arrow {"
//                                    "image: url(/usr/share/ukui-media/img/dowm.png);"
//                                    "height:6px;width:11px;}"
//                                    "QComboBox::down-arrow:hover{"
//                                    "width:184px;height:30px;"
//                                    "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                    "border-radius:2px;}");

//    m_pLagoutCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
//                                     "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
//                                     "QComboBox QAbstractItemView{"
//                                     "font-size: 14px;height: 80px;width: 140px;}"
//                                     "QComboBox QAbstractItemView::item {"
//                                     "height: 30px;}"
//                                     "QComboBox QAbstractItemView::item:hover {"
//                                     "background: rgba(218,227,250,0.9);"
//                                     "border-radius: 2px;"
//                                     "font-color: rgb(225, 0, 0);}"
//                                     "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
//                                     "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
//                                     "border-radius:4px;}"
//                                     "QComboBox::down-arrow:hover{"
//                                     "width:184px;height:30px;"
//                                     "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                     "border-radius:2px;}"
//                                     "QComboBox::down-arrow {"
//                                     "image: url(/usr/share/ukui-media/img/dowm.png);"
//                                     "height:6px;width:11px;}"
//                                     "QComboBox::down-arrow:hover{"
//                                     "width:184px;height:30px;"
//                                     "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                     "border-radius:2px;}");

//    m_pSoundEffectLabel->setStyleSheet("QLabel{font-size:18px;"
//                                     "font-family:Noto Sans S Chinese;"
//                                     "font-weight:500;"
//                                     "color:rgba(0,0,0,1);"
//                                     "background-color: rgba(0,0,0,0);"
//                                     "line-height:40px;}");
//   m_pSoundThemeLabel->setStyleSheet("QLabel{font-size:14px;"
//                                       "font-family:Noto Sans S Chinese;"
//                                       "font-weight:400;"
//                                       "color:rgba(67,67,69,1);"
//                                       "line-height:40px;}");
//    m_pShutdownlabel->setStyleSheet("QLabel{font-size:14px;"
//                                    "font-family:Noto Sans S Chinese;"
//                                    "font-weight:400;"
//                                    "color:rgba(67,67,69,1);"
//                                    "line-height:40px;}");
//    m_pLagoutLabel->setStyleSheet("QLabel{font-size:14px;"
//                                  "font-family:Noto Sans S Chinese;"
//                                  "font-weight:400;"
//                                  "color:rgba(67,67,69,1);"
//                                  "line-height:40px;}");

//    this->setStyleSheet("QWidget{width: 552px;height: 150px;"
//                        "background: rgba(244,244,244,1);"
//                        "border-radius: 4px;}");
//    m_pShutDownWidget->setStyleSheet("QWidget#shutDownWidget{border-bottom:1px solid white;"
//                                      "border-radius:0px;}");


}

UkmediaSoundEffectsWidget::~UkmediaSoundEffectsWidget()
{

}
