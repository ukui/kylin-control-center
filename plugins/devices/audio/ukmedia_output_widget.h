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
#ifndef UKMEDIAOUTPUTWIDGET_H
#define UKMEDIAOUTPUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QCoreApplication>
#include <QVBoxLayout>
#include "ukui_custom_style.h"
#include "ukui_list_widget_item.h"
#include "customstyle.h"
#include <QListWidget>

class AudioSlider : public QSlider
{
    Q_OBJECT
public:
    AudioSlider(QWidget *parent = nullptr);
    ~AudioSlider();
    friend class UkmediaInputWidget;

Q_SIGNALS:
    void silderPressSignal();
    void silderReleaseSignal();

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        mousePress = true;
        Q_EMIT silderPressSignal();
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
        QSlider::mousePressEvent(ev);
    }
    void mouseReleaseEvent(QMouseEvent *ev)
    {
        if(mousePress){
            Q_EMIT silderReleaseSignal();
        }
        mousePress = false;
        QSlider::mouseReleaseEvent(ev);
    }
private:
    bool mousePress = false;
};

class UkmediaOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaOutputWidget(QWidget *parent = nullptr);
    ~UkmediaOutputWidget();
    void outputWidgetAddPort();
    void outputWidgetRemovePort();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QWidget *m_pOutputWidget;
    QFrame *m_pOutputDeviceWidget;
    QFrame *m_pMasterVolumeWidget;
    QFrame *m_pChannelBalanceWidget;
    QFrame * m_pOutputPortWidget;
    QFrame *m_pProfileWidget;
    QFrame *m_pselectWidget;
    QListWidget *m_pOutputListWidget;

    QLabel *m_pSelectDeviceLabel;
    QLabel *m_pProfileLabel;
    QLabel *m_pOutputPortLabel;
    TitleLabel *m_pOutputLabel;
    QLabel *m_pOutputDeviceLabel;
    QLabel *m_pOpVolumeLabel;
    QLabel *m_pOpVolumePercentLabel;
    QLabel *m_pOpBalanceLabel;
    QLabel *m_pLeftBalanceLabel;
    QLabel *m_pRightBalanceLabel;

    QComboBox *m_pSelectCombobox;
    QComboBox *m_pProfileCombobox;
    QComboBox *m_pOutputPortCombobox;
    QComboBox *m_pOutputDeviceCombobox;
    UkuiButtonDrawSvg *m_pOutputIconBtn;
    AudioSlider *m_pOpVolumeSlider;
    UkmediaVolumeSlider *m_pOpBalanceSlider;
    QVBoxLayout *m_pVlayout;
    QString sliderQss;
};

#endif // UKMEDIAOUTPUTWIDGET_H
