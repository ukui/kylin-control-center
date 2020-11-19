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
#include "screensaver.h"
#include "ui_screensaver.h"

#include <QDebug>
#include <QBoxLayout>

#define SSTHEMEPATH "/usr/share/applications/screensavers/"
#define ID_PREFIX "screensavers-ukui-"

#define SCREENSAVER_SCHEMA "org.ukui.screensaver"
#define MODE_KEY "mode"
#define THEMES_KEY "themes"
#define LOCK_KEY "lock-enabled"
#define ACTIVE_KEY "idle-activation-enabled"

#define SESSION_SCHEMA "org.ukui.session"
#define IDLE_DELAY_KEY "idle-delay"

#define BACKGROUND_SCHEMA "org.mate.background"

const QString BACK_FILENAME_KEY = "pictureFilename";

#define IDLEMIN 1
#define IDLEMAX 120
#define IDLESTEP 1

typedef enum
{
    MODE_BLANK_ONLY,
    MODE_RANDOM,
    MODE_SINGLE,
    MODE_IMAGE,
    MODE_DEFAULT_UKUI,
}SaverMode;

void PreviewWidget::paintEvent(QPaintEvent *e){
//    QPainter painter(this);
//    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//    qDebug() << Q_FUNC_INFO << this->rect();
//    painter.fillRect(this->rect(), Qt::red);
}


Screensaver::Screensaver() : mFirstLoad(true)
{
    pluginName = tr("Screensaver");
    pluginType = PERSONALIZED;
}

Screensaver::~Screensaver() {
    if (!mFirstLoad) {
        delete ui;
        delete process;
        process = nullptr;
    }
}

QString Screensaver::get_plugin_name() {
    return pluginName;
}

int Screensaver::get_plugin_type() {
    return pluginType;
}

QWidget *Screensaver::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::Screensaver;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);
        ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

        ui->previewWidget->setStyleSheet("#previewWidget{background: black;}");
        ui->previewWidget->setAutoFillBackground(true);

        process = new QProcess();

        initSearchText();
        _acquireThemeinfoList();
        initComponent();
        initEnableBtnStatus();
        initThemeStatus();
        initIdleSliderStatus();
    }
    return pluginWidget;
}

void Screensaver::plugin_delay_control() {
    // 初始化屏保预览Widget
    initPreviewWidget();
}

const QString Screensaver::name() const {

    return QStringLiteral("screensaver");
}

void Screensaver::initSearchText() {
    //~ contents_path /screensaver/Enable screensaver
    ui->enableLabel->setText(tr("Enable screensaver"));
    //~ contents_path /screensaver/Screensaver program
    ui->programLabel->setText(tr("Screensaver program"));
    //~ contents_path /screensaver/idle time
    ui->idleLabel->setText(tr("idle time"));
}

void Screensaver::initComponent() {
    if (QGSettings::isSchemaInstalled(SCREENSAVER_SCHEMA)) {
        const QByteArray id(SCREENSAVER_SCHEMA);
        screenlock_settings = new QGSettings(id, QByteArray(), this);

        connect(screenlock_settings, &QGSettings::changed, [=](QString key) {
            if (key == "lockEnabled") {
                bool judge = screenlock_settings->get(LOCK_KEY).toBool();
                if (judge && !enableSwitchBtn->isChecked()) {
                    enableSwitchBtn->setChecked(judge);
                }
            }
        });
    }

    if (QGSettings::isSchemaInstalled(SESSION_SCHEMA)) {
        qSessionSetting = new QGSettings(SESSION_SCHEMA, QByteArray(), this);
    }

    if (QGSettings::isSchemaInstalled(SCREENSAVER_SCHEMA)) {
        qScreenSaverSetting = new QGSettings(SCREENSAVER_SCHEMA, QByteArray(), this);
    }

    screensaver_bin = "/usr/lib/ukui-screensaver/ukui-screensaver-default";

    //添加开启屏保按钮
    enableSwitchBtn = new SwitchButton(ui->enableFrame);
    ui->enableHorLayout->addStretch();
    ui->enableHorLayout->addWidget(enableSwitchBtn);

    //添加锁定屏幕开关按钮
    lockSwitchBtn = new SwitchButton(ui->lockFrame);
    ui->lockHorLayout->addStretch();
    ui->lockHorLayout->addWidget(lockSwitchBtn);

    //初始化屏保程序下拉列表
    ui->comboBox->addItem(tr("Default_ukui"));
    ui->comboBox->addItem(tr("Blank_Only"));
//    ui->comboBox->addItem(tr("Random"));
    QMap<QString, SSThemeInfo>::iterator it = infoMap.begin();
    for (int index = 2; it != infoMap.end(); it++, index++) {
        SSThemeInfo info = (SSThemeInfo)it.value();
        ui->comboBox->addItem(info.name);
        ui->comboBox->setItemData(index, QVariant::fromValue(info));
    }

    //初始化滑动条
    QStringList scaleList;
    scaleList<< tr("1m") << tr("5m") << tr("10m") << tr("30m") << tr("45m")
              <<tr("1h") << tr("1.5h") << tr("3h");

    uslider = new Uslider(scaleList);
    uslider->setRange(1,8);
    uslider->setTickInterval(1);
    uslider->setPageStep(1);

    ui->lockhorizontalLayout->addWidget(uslider);
    ui->lockhorizontalLayout->addSpacing(15);

//    ui->idleSlider->setMinimum(IDLEMIN);
//    ui->idleSlider->setMaximum(IDLEMAX);
//    ui->idleSlider->setSingleStep(IDLESTEP);
//    ui->idleSlider->setPageStep(IDLESTEP);

    connect(enableSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked) {
        screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
        g_settings_set_boolean(screensaver_settings, ACTIVE_KEY, checked);

        //刷新LockWidget状态
//        ui->lockFrame->setVisible(checked);
        g_object_unref(screensaver_settings);
    });

    connect(qScreenSaverSetting, &QGSettings::changed, this, [=](const QString key) {
        if ("idleActivationEnabled" == key) {
            auto status = qScreenSaverSetting->get(ACTIVE_KEY).toBool();
            enableSwitchBtn->setChecked(status);
        }
    });


//    connect(lockSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked){
//        //REVIEW*** g_object_unref faild
//        //    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
//        //    g_settings_set_boolean(screensaver_settings, LOCK_KEY, status);
//        //    if (screensaver_settings)
//        //            g_object_unref(screensaver_settings);
//        const QByteArray ba(SCREENSAVER_SCHEMA);
//        QGSettings * settings = new QGSettings(ba);
//        settings->set(LOCK_KEY, checked);
//        delete settings;
//    });

//    connect(uslider, &QSlider::valueChanged, this, [=](int value){
//        //刷新分钟显示
//        ui->idleLineEdit->blockSignals(true);
//        ui->idleLineEdit->setText(QString::number(value));
//        ui->idleLineEdit->blockSignals(false);
//    });
    connect(uslider, &QSlider::valueChanged, this, [=] {
        int value = convertToLocktime(uslider->value());
        session_settings = g_settings_new(SESSION_SCHEMA);
        g_settings_set_int(session_settings, IDLE_DELAY_KEY, value);
        g_object_unref(session_settings);
    });
    connectToServer();

    connect(qSessionSetting, &QGSettings::changed, this,[=](const QString& key) {
       if ("idleDelay" == key) {
           auto value = qSessionSetting->get(key).toInt();
           uslider->setValue(lockConvertToSlider(value));
       }
    });

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(themesComboxChanged(int)));

    connect(ui->previewWidget, &QWidget::destroyed, this, [=] {
        closeScreensaver();
    });
}

void Screensaver::initPreviewWidget() {
    startupScreensaver();
}

void Screensaver::initEnableBtnStatus() {

    const QByteArray ba(SCREENSAVER_SCHEMA);
    QGSettings * settings = new QGSettings(ba);

    //初始化EnableBtn状态
    bool active;
    active = settings->get(ACTIVE_KEY).toBool();
    enableSwitchBtn->blockSignals(true);
    enableSwitchBtn->setChecked(active);
    enableSwitchBtn->blockSignals(false);

    //初始化LockWidget状态
    ui->lockFrame->setVisible(false);

    bool locked;
    locked = settings->get(LOCK_KEY).toBool();
    initLockBtnStatus(locked);

    delete settings;
}

void Screensaver::initLockBtnStatus(bool status) {
    lockSwitchBtn->blockSignals(true);
    lockSwitchBtn->setChecked(status);
    lockSwitchBtn->blockSignals(false);
}

void Screensaver::initThemeStatus() {
    int mode;
    char * name;

    ui->comboBox->blockSignals(true);

    QByteArray id(SCREENSAVER_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)) {
        screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    } else {
        qDebug()<<"org.ukui.screensaver not installed"<<endl;
        return ;
    }

    mode = g_settings_get_enum(screensaver_settings, MODE_KEY);

    if (mode == MODE_DEFAULT_UKUI) {
        ui->comboBox->setCurrentIndex(0); //UKUI
    }
    else if (mode == MODE_BLANK_ONLY) {
        ui->comboBox->setCurrentIndex(1); //Black_Only
    }
//    else if (mode == MODE_RANDOM){
//        ui->comboBox->setCurrentIndex(2); //Random
//    }
    else{
        gchar ** strv;
        strv = g_settings_get_strv(screensaver_settings, THEMES_KEY);
        if (strv != NULL) {
            name = g_strdup(strv[0]);

            QString dest =  (infoMap.find(name) != infoMap.end()) ? infoMap.value(name).name : "";

            if (dest == "") {
                ui->comboBox->setCurrentIndex(1);
            } else {
                ui->comboBox->setCurrentText(dest);
            }
        } else {
            ui->comboBox->setCurrentIndex(1); //no data, default Blank_Only
        }
        g_strfreev(strv);
    }
    g_object_unref(screensaver_settings);

    ui->comboBox->blockSignals(false);
}

void Screensaver::initIdleSliderStatus() {
    int minutes;
    session_settings = g_settings_new(SESSION_SCHEMA);
    minutes = g_settings_get_int(session_settings, IDLE_DELAY_KEY);

    uslider->blockSignals(true);
    uslider->setValue(lockConvertToSlider(minutes));
    uslider->blockSignals(false);

//    ui->idleSlider->blockSignals(true);
//    ui->idleSlider->setValue(minutes);
//    ui->idleSlider->blockSignals(false);

//    ui->idleLineEdit->blockSignals(true);
//    ui->idleLineEdit->setText(QString::number(minutes));
//    ui->idleLineEdit->blockSignals(true);

    g_object_unref(session_settings);
}

void Screensaver::component_init() {

    //设置屏保预览widget的背景为黑色
//    mPreviewWidget->setStyleSheet("#previewWidget{background: black}");

    //
//    activeswitchbtn = new SwitchButton();
//    activeswitchbtn->setAttribute(Qt::WA_DeleteOnClose);
//    ui->activeHLayout->addWidget(activeswitchbtn);
//    ui->activeHLayout->addStretch();

//    lockswitchbtn = new SwitchButton();
//    lockswitchbtn->setAttribute(Qt::WA_DeleteOnClose);
//    ui->lockHLayout->addWidget(lockswitchbtn);
//    ui->lockHLayout->addStretch();

//    ui->comboBox->addItem(tr("Blank_Only"));
//    ui->comboBox->addItem(tr("Random"));

//    QMap<QString, SSThemeInfo>::iterator it = infoMap.begin();
//    for (int itemsindex = 2; it != infoMap.end(); it++, itemsindex++){
//        SSThemeInfo info = (SSThemeInfo)it.value();
//        ui->comboBox->addItem(info.name);
//        ui->comboBox->setItemData(itemsindex, QVariant::fromValue(info));
//    }

    //init slider
//    int min = 1;
//    int max = 120;
//    int singlestep = 1;

//    ui->idleSlider->setMinimum(min);
//    ui->idleSlider->setMaximum(max);
//    ui->idleSlider->setSingleStep(singlestep);
//    ui->idleSlider->installEventFilter(this);

//    connect(this, SIGNAL(kill_signals()), this, SLOT(kill_screensaver_preview()));
}

void Screensaver::status_init() {
    int mode;
    char * name;

    screensaver_bin = QString(QT_INSTALL_LIBS) + "/ukui-screensaver/ukui-screensaver-default";

    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    mode = g_settings_get_enum(screensaver_settings, MODE_KEY);
    if (mode == MODE_DEFAULT_UKUI) {
        ui->comboBox->setCurrentIndex(0); //UKUI
    }
    else if(mode == MODE_BLANK_ONLY) {
        ui->comboBox->setCurrentIndex(1); //Black_Only
    }
    else if (mode == MODE_RANDOM) {
        ui->comboBox->setCurrentIndex(2); //Random
    } else {
        gchar ** strv;
        strv = g_settings_get_strv(screensaver_settings, THEMES_KEY);

        if (strv != NULL) {
            name = g_strdup(strv[0]);
            SSThemeInfo info = (SSThemeInfo)infoMap.find(name).value();
            ui->comboBox->setCurrentText(info.name);
        } else {
            ui->comboBox->setCurrentIndex(0); //no data, default Blank_Only
        }
        g_strfreev(strv);
    }

    //init
//    bool activation; bool lockable;
//    activation = g_settings_get_boolean(screensaver_settings, ACTIVE_KEY);
//    activeswitchbtn->setChecked(activation);
//    if (activation){
//        lockable = g_settings_get_boolean(screensaver_settings, LOCK_KEY);
//        lockswitchbtn->setChecked(lockable);
//        ui->widget->show();
//    }
//    else{
//        lockswitchbtn->setChecked(false);
//        ui->widget->hide();
//    }

    g_object_unref(screensaver_settings);

    //获取空闲时间
    int minutes;
    minutes = g_settings_get_int(session_settings, IDLE_DELAY_KEY);
    uslider->setValue(lockConvertToSlider(minutes));
//    ui->idleSlider->setValue(minutes);
//    ui->idleLabel->setText(QString("%1%2").arg(minutes).arg(tr("minutes")));


    //获取功能列表
//    PublicData * publicdata = new PublicData();
//    QStringList tmpList = publicdata->subfuncList[SYSTEM];

    //connect
//    connect(ui->powerBtn, &QPushButton::clicked, this, [=]{pluginWidget->emitting_toggle_signal(tmpList.at(2), SYSTEM, 0);});

//    connect(uslider, SIGNAL(sliderReleased()), this, SLOT(slider_released_slot())); //改gsettings
//    connect(activeswitchbtn, SIGNAL(checkedChanged(bool)), this, SLOT(activebtn_changed_slot(bool)));
//    connect(lockswitchbtn, SIGNAL(checkedChanged(bool)), this, SLOT(lockbtn_changed_slot(bool)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox_changed_slot(int)));

    connect(mPreviewWidget, SIGNAL(destroyed(QObject*)), this, SLOT(kill_screensaver_preview()));

//    delete publicdata;
}

void Screensaver::startupScreensaver() {
    //关闭屏保
    closeScreensaver();
    qDebug() << Q_FUNC_INFO << ui->previewWidget->winId();
    if (ui->comboBox->currentIndex() == 0) {//UKUI
        QStringList args;
        args << "-window-id" << QString::number(ui->previewWidget->winId());
        //启动屏保
        process->startDetached(screensaver_bin, args);
        runStringList.append(screensaver_bin);
    } else if (ui->comboBox->currentIndex() == 1) {//黑屏
        ui->previewWidget->update();
    }
//    else if (ui->comboBox->currentIndex() == 2){//随机
//        ui->previewWidget->update();
//    }
    else {// 屏保
        SSThemeInfo info = ui->comboBox->currentData().value<SSThemeInfo>();
        QStringList args;
        args << "-window-id" << QString::number(ui->previewWidget->winId());
        // 启动屏保
        process->startDetached(info.exec, args);
        runStringList.append(info.exec);
    }
}

void Screensaver::closeScreensaver() {
    //杀死分离启动的屏保预览程序
    if (!runStringList.isEmpty()) {
        process->start(QString("killall"), runStringList);
        process->waitForStarted();
        process->waitForFinished(2000);

        runStringList.clear();
    }
}

void Screensaver::kill_and_start() {
    emit kill_signals(); //如果有屏保先杀死
    if (ui->comboBox->currentIndex() == 0) {//UKUI
        QStringList args;
        args << "-window-id" << QString::number(mPreviewWidget->winId());
        //启动屏保
        process->startDetached(screensaver_bin, args);
        runStringList.append(screensaver_bin);
    } else if (ui->comboBox->currentIndex() == 1) {//黑屏
        mPreviewWidget->update();
    } else if (ui->comboBox->currentIndex() == 2) {//随机
        mPreviewWidget->update();
    } else {//屏保
        SSThemeInfo info = ui->comboBox->currentData().value<SSThemeInfo>();
        QStringList args;
        args << "-window-id" << QString::number(mPreviewWidget->winId());
        //启动屏保
        process->startDetached(info.exec, args);
        killList.clear(); killList.append(info.exec);
    }
}

int Screensaver::convertToLocktime(const int value) {
    switch (value) {
    case 1:
        return 1;
        break;
    case 2:
        return 5;
        break;
    case 3:
        return 10;
        break;
    case 4:
        return 30;
        break;
    case 5:
        return 45;
        break;
    case 6:
        return 60;
        break;
    case 7:
        return 90;
        break;
    case 8:
        return 180;
        break;
    default:
        return 1;
        break;
    }
}

int Screensaver::lockConvertToSlider(const int value) {
    switch (value) {
    case 1:
        return 1;
        break;
    case 5:
        return 2;
        break;
    case 10:
        return 3;
        break;
    case 30:
        return 4;
        break;
    case 45:
        return 5;
        break;
    case 60:
        return 6;
        break;
    case 90:
        return 7;
        break;
    case 180:
        return 8;
        break;
    default:
        return 1;
        break;
    }
}



void Screensaver::set_idle_gsettings_value(int value) {
    g_settings_set_int(session_settings, IDLE_DELAY_KEY, value);
}

void Screensaver::slider_released_slot() {
    int minutes;
//    minutes = ui->idleSlider->value();
    minutes = convertToLocktime(uslider->value());
    set_idle_gsettings_value(minutes);
}

void Screensaver::lockbtn_changed_slot(bool status) {
    //REVIEW***  setchecked(false) -> g_object_unref faild
//    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
//    g_settings_set_boolean(screensaver_settings, LOCK_KEY, status);
//    if (screensaver_settings)
//            g_object_unref(screensaver_settings);
    const QByteArray ba(SCREENSAVER_SCHEMA);
    QGSettings * settings = new QGSettings(ba);
    settings->set(LOCK_KEY, status);
    delete settings;
}

void Screensaver::activebtn_changed_slot(bool status) {
    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    g_settings_set_boolean(screensaver_settings, ACTIVE_KEY, status);

//    if (status){
//        ui->widget->show();
//    }
//    else{
//        lockswitchbtn->setChecked(false);
//        ui->widget->hide();

//    }
    g_object_unref(screensaver_settings);

}

void Screensaver::themesComboxChanged(int index) {

    char ** strv = NULL;

    //设置屏保
    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    if (index == 0){
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_DEFAULT_UKUI);
    } else if (index == 1){
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_BLANK_ONLY);
    } else {
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_SINGLE);
        //获取当前屏保的id
        QVariant variant = ui->comboBox->itemData(index);
        SSThemeInfo info = variant.value<SSThemeInfo>();
        QByteArray ba = info.id.toLatin1();
        strv = g_strsplit(ba.data(), "%%%", 1);
        qDebug() << Q_FUNC_INFO << "wxy-----------" <<strv;
        g_settings_set_strv(screensaver_settings, THEMES_KEY, (const gchar * const*)strv);
    }
    g_object_unref(screensaver_settings);
    g_strfreev(strv);

    //刷新预览
    startupScreensaver();
}

void Screensaver::combobox_changed_slot(int index) {
    char ** strv = NULL;


    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    if (index == 0) { //ukui
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_DEFAULT_UKUI);
    } else if (index == 1) { //Blank_Only
        qDebug()<<"this is Blan_only----------------->"<<endl;
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_BLANK_ONLY);
    } else if (index == 10000) { //Random not in
//        int mode = MODE_RANDOM;
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_RANDOM);
        //REVIEW*** 二维字符数组赋值字符串段错误？
//        QMap<QString, SSThemeInfo>::iterator it = infoMap.begin();
//        for (guint num = 0; it != infoMap.end(); it++){
//            QString id = QString(it.key());
//            QByteArray ba = id.toLatin1();
//            char * info_id = ba.data();
//            strv[num++] = g_strdup)(info_id);
//        }
        //改用qt的gsetting
        QStringList valueStringList;
        const QByteArray ba(SCREENSAVER_SCHEMA);
        QGSettings * settings = new QGSettings(ba);
        QMap<QString, SSThemeInfo>::iterator it = infoMap.begin();
        for (; it != infoMap.end(); it++) {
            QString id = QString(it.key());
            valueStringList.append(id);
        }
        settings->set(THEMES_KEY, QVariant(valueStringList));
        delete settings;
    } else {
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_SINGLE);
        //获取当前屏保的id
        QVariant variant = ui->comboBox->itemData(index);
        SSThemeInfo info = variant.value<SSThemeInfo>();
        QByteArray ba = info.id.toLatin1();
        strv = g_strsplit(ba.data(), "%%%", 1);
        qDebug() << Q_FUNC_INFO << "wxy-----------" <<strv;
        g_settings_set_strv(screensaver_settings, THEMES_KEY, (const gchar * const*)strv);
    }
    //set mode
//    g_settings_set_enum(screensaver_settings, MODE_KEY, mode);
    //set themes
//    g_settings_set_strv(screensaver_settings, THEMES_KEY, (const gchar * const*)strv);

    g_object_unref(screensaver_settings);
    g_strfreev(strv);

    //启动屏保
    kill_and_start();
}

void Screensaver::kill_screensaver_preview() {
    //需要手动杀死分离启动的屏保预览程序
//    if (!killList.isEmpty()){
//        process->start(QString("killall"), killList);
//        process->waitForStarted();
//        process->waitForFinished(2000);

//        killList.clear();
//    }
}

SSThemeInfo Screensaver::_info_new(const char *path) {
    SSThemeInfo info;
    GKeyFile * keyfile;
    char * name, * exec;

    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL)) {
        g_key_file_free (keyfile);
        return info;
    }

    name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
    exec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_TRY_EXEC, NULL);

    info.name = QString::fromUtf8(name);
    info.exec = QString::fromUtf8(exec);
    info.id = ID_PREFIX + info.name.toLower();

    g_key_file_free(keyfile);

    return info;
}

SSThemeInfo Screensaver::_newThemeinfo(const char * path) {
    SSThemeInfo info;
    GKeyFile * keyfile;
    char * name, * exec;

    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL)) {
        g_key_file_free (keyfile);
        return info;
    }

    name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
    exec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_TRY_EXEC, NULL);


    info.name = QString::fromUtf8(name);
    info.exec = QString::fromUtf8(exec);
    info.id = ID_PREFIX + info.name.toLower();

    g_key_file_free(keyfile);

    return info;
}

void Screensaver::_acquireThemeinfoList() {
    GDir * dir;
    const char * name;

    infoMap.clear();

    dir = g_dir_open(SSTHEMEPATH, 0, NULL);
    if (!dir)
        return;
    while ((name = g_dir_read_name(dir))) {
        SSThemeInfo info;
        char * desktopfilepath;
        if (!g_str_has_suffix(name, ".desktop"))
            continue;
        desktopfilepath = g_build_filename(SSTHEMEPATH, name, NULL);
        info = _newThemeinfo(desktopfilepath);
        infoMap.insert(info.id, info);
        g_free (desktopfilepath);
    }
    g_dir_close(dir);
}

void Screensaver::init_theme_info_map() {
    GDir * dir;
    const char * name;

    infoMap.clear();

    dir = g_dir_open(SSTHEMEPATH, 0, NULL);
    if (!dir)
        return;
    while ((name = g_dir_read_name(dir))) {
        SSThemeInfo info;
        char * desktopfilepath;
        if (!g_str_has_suffix(name, ".desktop"))
            continue;
        desktopfilepath = g_build_filename(SSTHEMEPATH, name, NULL);
        info = _info_new(desktopfilepath);
        infoMap.insert(info.id, info);
        g_free (desktopfilepath);
    }
    g_dir_close(dir);
}

void Screensaver::connectToServer(){
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid())
    {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
//    QDBusConnection::sessionBus().connect(cloudInterface, SIGNAL(shortcutChanged()), this, SLOT(shortcutChangedSlot()));
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void Screensaver::keyChangedSlot(const QString &key) {
    if(key == "ukui-screensaver") {
        initThemeStatus();
    }
}
