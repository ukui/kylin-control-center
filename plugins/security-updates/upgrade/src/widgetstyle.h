/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef WIDGETSTYLE_H
#define WIDGETSTYLE_H

#define WINDOWW 620     //窗口宽度
#define WINDOWH 580     //窗口高度
#define TITLEH 38       //标题栏高度

#include <QDebug>
//窗口显示在屏幕中心
#include <QApplication>
#include <QScreen>
//控件
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
//布局
#include <QBoxLayout>
//读取本地字体
#include <QFontDatabase>
//窗体阴影
#include <QPainter>
#include <QtMath>
#include <QGSettings>

#include "updatedbus.h"

#define KYLINRECORDER "org.kylin-update-manager-data.settings"
#define FITTHEMEWINDOW "org.ukui.style"


class WidgetStyle : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetStyle(QWidget *parent = 0);
    ~WidgetStyle() {}

    //主题颜色适配--浅色
    static int themeColor;

    //当前系统更新模式  是否为最新系统、是否开启自动更新
    static int systemStatus;

    //在屏幕中央显示
    void handleIconClickedSub();

    // ！！！组件和样式分离，每一个QWidget类都要有setWidgetUi()和setWidgetStyle()函数

    // 初始化组件
    void setWidgetUi();

    // 初始化样式
    void setWidgetStyle();

    // 添加圆角阴影
    static void paintEvent(QPaintEvent *event, QWidget *widget);
};

#endif // WIDGETSTYLE_H
