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
        int value = 0;
        int currentX = ev->pos().x();
        double per = currentX * 1.0 / this->width();
        if ((this->maximum() - this->minimum()) >= 50) { //减小鼠标点击像素的影响
            value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
            if (value <= (this->maximum() / 2 - this->maximum() / 10 + this->minimum() / 10)) {
                value = qRound(per*(this->maximum() - this->minimum() - 1)) + this->minimum();
            } else if (value > (this->maximum() / 2 + this->maximum() / 10 + this->minimum() / 10)) {
                value = qRound(per*(this->maximum() - this->minimum() + 1)) + this->minimum();
            } else {
                value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
            }
        } else {
            value = qRound(per*(this->maximum() - this->minimum())) + this->minimum();
        }
        this->setValue(value);
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
