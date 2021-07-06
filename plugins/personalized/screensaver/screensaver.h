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
#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QObject>
#include <QtPlugin>
#include <QPushButton>
#include <QMap>
#include <QProcess>
#include <QGSettings>
#include <QStyledItemDelegate>
#include <QPaintEvent>
#include <QShowEvent>
#include <QMoveEvent>
#include <QtDBus>
#include <QHideEvent>
#include <QTextEdit>
#include <QWindow>
#include <QLabel>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"
#include "Uslider/uslider.h"
#include "presslabel.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

typedef struct _SSThemeInfo{
    QString name;
    QString exec;
    QString id;
}SSThemeInfo;

//自定义数据类型，如果要使用QVariant，就必须使用Q_DECLARE_METATYPE注册。
Q_DECLARE_METATYPE(SSThemeInfo)

namespace Ui {
class Screensaver;
}

class PreviewWidget : public QWidget
{
    Q_OBJECT
public:
    PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget();
protected:
    void paintEvent(QPaintEvent *e);
private:
    void mousePressEvent(QMouseEvent *e);
};

class PreviewWindow : public QWindow
{
    Q_OBJECT
public:
    PreviewWindow();
    ~PreviewWindow();
    static void previewScreensaver();
private:
    void mousePressEvent(QMouseEvent *e);
};

class Screensaver : public QWidget, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    Screensaver();
    ~Screensaver();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initPreviewLabel();
    void initSearchText();
    void initComponent();
    void initPreviewWidget();
    void initEnableBtnStatus();
    void initThemeStatus();
    void initIdleSliderStatus();
    void initShowTimeBtnStatus();

    void startupScreensaver();
    void closeScreensaver();

    void _acquireThemeinfoList();
    SSThemeInfo _newThemeinfo(const char *path);

    void component_init();

    void screensaver_switch();

    void kill_and_start();

    void showCustomizeFrame();
    void hideCustomizeFrame();
    void initCustomizeFrame();
    void initScreensaverSourceFrame();
    void initTimeSetFrame();
    void initPictureSwitchFrame();
    void initShowTextFrame();
    void initShowTextSetFrame();
    void initShowtimeFrame();
    void setSourcePathText();
private:
    int convertToLocktime(const int value);
    int lockConvertToSlider(const int value);
    void connectToServer();
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::Screensaver *ui;

    int pluginType;
    QWidget       * pluginWidget;
    PreviewWidget * mPreviewWidget;

    SwitchButton * enableSwitchBtn;
    SwitchButton * showTimeBtn;

    QMap<QString, SSThemeInfo> infoMap;

    GSettings  * screensaver_settings;
    QGSettings * qScreensaverDefaultSetting;
    QGSettings * qScreenSaverSetting = nullptr;
    QGSettings * qBgSetting = nullptr;

    QProcess   * process;

    QString      pluginName;
    QString      screensaver_bin;

    QStringList killList;
    QStringList runStringList;
    QStringList mScreenSaverKeies;

    Uslider    * uslider;

    QDBusInterface *m_cloudInterface;

    bool mFirstLoad;
    QLabel *sourcePathLine = nullptr;
    QTextEdit *inputText;
    PreviewWindow *previewWind;
    QString sourcePathText;
    PressLabel *previewLabel = nullptr;

private slots:
    void themesComboxChanged(int index);
    void kill_screensaver_preview();
    void keyChangedSlot(const QString &key);

Q_SIGNALS:
    void kill_signals();
};

#endif // SCREENSAVER_H
