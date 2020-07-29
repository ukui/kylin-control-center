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
#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QDebug>
#include <QPainterPath>

#include "colorsquare.h"
#include "gradientslider.h"
#include "ui_colordialog.h"
#include "colorpreview.h"

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
    ~ColorDialog();

public:
    QColor color() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::ColorDialog     *ui;

    ColorSquare         *colorSquare;
    GradientSlider      *gradientSlider;

    enum CheckedColor
    {
        H,S,V,R,G,B
    }checkedColor;

    ColorPreview*       colorPreview;

    QSpinBox*           spinAlpha;
    GradientSlider*     sliderAlpha;

    QRadioButton*       radioHue;
    QSpinBox*           spinHue;
    GradientSlider*     sliderHue;

    QRadioButton*       radioSat;
    QSpinBox*           spinSat;
    GradientSlider*     sliderSat;

    QRadioButton*       radioVal;
    QSpinBox*           spinVal;
    GradientSlider*     sliderVal;

    QRadioButton*       radioRed;
    QSpinBox*           spinRed;
    GradientSlider*     sliderRed;
    QRadioButton*       radioGreen;
    QSpinBox*           spinGreen;
    GradientSlider*     sliderGreen;
    QRadioButton*       radioBlue;
    QSpinBox*           spinBlue;
    GradientSlider*     sliderBlue;

    QHBoxLayout*        horizontalLayout_3;

    void setupInit();
    void signalsBind();
    void drawSlider();
    void SetVerticalSlider();
    void SetColor(QColor color);

private slots:
    void updateWidgetsSlot();
    void setRgbSlot();
    void SetHsvSlot();
    void okSlot();

signals:
    void colorChanged(QColor);
    void checkedChanged(char);
    void colorSelected(QColor);
};

#endif // COLORDIALOG_H
