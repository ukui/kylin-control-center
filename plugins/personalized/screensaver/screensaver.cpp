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
#include <QTextEdit>
#include <QTextBrowser>
#include <QFileDialog>
#include "ComboBox/combobox.h"

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
const int silderNeverValue = -1;

#define IDLEMIN 1
#define IDLEMAX 120
#define IDLESTEP 1

typedef enum
{
    MODE_BLANK_ONLY,       //纯黑屏保
    MODE_RANDOM,          //暂无
    MODE_SINGLE,         //单独的屏保设置(多用于其它屏保)
    MODE_IMAGE,          //暂无
    MODE_DEFAULT_UKUI,   //UKUI
    MODE_CUSTOMIZE,     //自定义
}SaverMode;

/*
 选择框中的序号     
*/
typedef enum
{
    INDEX_MODE_DEFAULT_UKUI,
    INDEX_MODE_BLANK_ONLY,
}SaverModeIndex;
static int INDEX_MODE_CUSTOMIZE = -1;

Screensaver::Screensaver() : mFirstLoad(true)
{
    pluginName = tr("Screensaver");
    pluginType = PERSONALIZED;

}

Screensaver::~Screensaver() {
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
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
  
        process = new QProcess();

        initTitleLabel();
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

void Screensaver::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    QLabel *previewLabel = new QLabel(ui->previewWidget->topLevelWidget());
    previewLabel->setStyleSheet("background-color:palette(button);border-radius:0px;");
    QFont fontLabel;
    fontLabel.setPixelSize(12);
    QRect rect = ui->previewWidget->geometry();
    previewLabel->setFont(fontLabel);
    previewLabel->setGeometry(rect.x()+rect.width()/2-40/2,rect.y()+rect.height()+23,40,20);
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setText(tr("View"));//预览
}

void Screensaver::initSearchText() {
    //~ contents_path /screensaver/Show rest time
    ui->showTimeLabel->setText(tr("Show rest time"));
    //~ contents_path /screensaver/Screensaver program
    ui->programLabel->setText(tr("Screensaver program"));
    //~ contents_path /screensaver/Idle time
    ui->idleLabel->setText(tr("Idle time"));
}

void Screensaver::initComponent() {
    /*
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
    }*/
    if (QGSettings::isSchemaInstalled(SESSION_SCHEMA)) {
        qSessionSetting = new QGSettings(SESSION_SCHEMA, QByteArray(), this);
    }
    if (QGSettings::isSchemaInstalled(SCREENSAVER_SCHEMA)) {
        qScreenSaverSetting = new QGSettings(SCREENSAVER_SCHEMA, QByteArray(), this);
    }
    screensaver_bin = "/usr/lib/ukui-screensaver/ukui-screensaver-default";

    //添加开启屏保按钮
    ui->enableFrame->setVisible(false); //屏保激活锁屏(原打开屏保按钮)，在锁屏界面实现，这里暂时隐藏处理，保留原打开功能
    enableSwitchBtn = new SwitchButton(ui->enableFrame);
    ui->enableHorLayout->addStretch();
    ui->enableHorLayout->addWidget(enableSwitchBtn);

  
    //添加锁定屏幕开关按钮
    showTimeBtn = new SwitchButton(ui->showTimeFrame);
    ui->showTimeLayout->addStretch();
    ui->showTimeLayout->addWidget(showTimeBtn);

    initCustomizeFrame();
    //初始化屏保程序下拉列表
    ui->comboBox->addItem(tr("UKUI"));
    ui->comboBox->addItem(tr("Blank_Only"));
    // ui->comboBox->addItem(tr("Random"));
    QMap<QString, SSThemeInfo>::iterator it = infoMap.begin();
    for (int index = 2; it != infoMap.end(); it++, index++) {
        SSThemeInfo info = (SSThemeInfo)it.value();
        ui->comboBox->addItem(info.name);
        ui->comboBox->setItemData(index, QVariant::fromValue(info));
    }
    ui->comboBox->addItem(tr("Customize"));
    INDEX_MODE_CUSTOMIZE = ui->comboBox->count() - 1;  //得到【自定义】在滑动栏中的位置
    //初始化滑动条
    QStringList scaleList;
    scaleList<< tr("5min") << tr("10min") << tr("15min") << tr("30min") << tr("1hour")
              <<tr("Never");

    uslider = new Uslider(scaleList);
    uslider->setRange(1,6);
    uslider->setTickInterval(1);
    uslider->setPageStep(1);

    ui->lockhorizontalLayout->addWidget(uslider);
    // ui->lockhorizontalLayout->addSpacing(15);

/*
        暂时废弃掉这个按钮的打开关闭屏保功能
    connect(enableSwitchBtn, &SwitchButton::checkedChanged, this, [=](bool checked) {
        screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
        g_settings_set_boolean(screensaver_settings, ACTIVE_KEY, checked);
        g_object_unref(screensaver_settings);
    });
*/
    connect(qScreenSaverSetting, &QGSettings::changed, this, [=](const QString key) {
        if ("idleActivationEnabled" == key) {
            auto status = qScreenSaverSetting->get(ACTIVE_KEY).toBool();
            //enableSwitchBtn->setChecked(status);
            if (status == false) {
                uslider->setValue(lockConvertToSlider(silderNeverValue));
            }
        }
    });

    connect(uslider, &QSlider::valueChanged, this, [=] {
        int value = convertToLocktime(uslider->value());
        if (value == silderNeverValue) {  //关闭锁屏
            screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);  
            g_settings_set_boolean(screensaver_settings, ACTIVE_KEY, false);
            g_object_unref(screensaver_settings);
        }
        else {
            if(qScreenSaverSetting->get(ACTIVE_KEY).toBool() == false) {  //需先打开屏保
                screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);  
                g_settings_set_boolean(screensaver_settings, ACTIVE_KEY, true);
                g_object_unref(screensaver_settings);
            }
            session_settings = g_settings_new(SESSION_SCHEMA);
            g_settings_set_int(session_settings, IDLE_DELAY_KEY, value);
            g_object_unref(session_settings);
        }
    });
    connectToServer();
    connect(qSessionSetting, &QGSettings::changed, this,[=](const QString& key) {
       if ("idleDelay" == key) {
            if (qScreenSaverSetting->get(ACTIVE_KEY).toBool() == true) {
                auto value = qSessionSetting->get(key).toInt();
                uslider->setValue(lockConvertToSlider(value));
            }
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
    /*bool active;
    active = settings->get(ACTIVE_KEY).toBool();
    enableSwitchBtn->blockSignals(true);
    enableSwitchBtn->setChecked(active);
    enableSwitchBtn->blockSignals(false);*/

    //初始化LockWidget状态
    // ui->showTimeFrame->setVisible(false);

    bool locked;
    locked = settings->get(LOCK_KEY).toBool();
    initShowTimeBtnStatus(locked);

    delete settings;
    settings = nullptr;
}
////////////////////////////////////这里要改为显示时间的设置
void Screensaver::initShowTimeBtnStatus(bool status) {
    showTimeBtn->blockSignals(true);
    showTimeBtn->setChecked(status);
    showTimeBtn->blockSignals(false);
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
        ui->comboBox->setCurrentIndex(INDEX_MODE_DEFAULT_UKUI); //UKUI
        hideCustomizeFrame();
    }
    else if (mode == MODE_BLANK_ONLY) {
        ui->comboBox->setCurrentIndex(INDEX_MODE_BLANK_ONLY); //Black_Only
        hideCustomizeFrame();
    }
//    else if (mode == MODE_RANDOM){
//        ui->comboBox->setCurrentIndex(2); //Random
//    }
    else if (mode == MODE_CUSTOMIZE)
    {
        ui->comboBox->setCurrentIndex(INDEX_MODE_CUSTOMIZE); //CUSTOMIZE
        showCustomizeFrame();
    }
    else{
        hideCustomizeFrame();
        gchar ** strv;
        strv = g_settings_get_strv(screensaver_settings, THEMES_KEY);
        if (strv != NULL) {
            name = g_strdup(strv[0]);

            QString dest =  (infoMap.find(name) != infoMap.end()) ? infoMap.value(name).name : "";

            if (dest == "") {
                ui->comboBox->setCurrentIndex(INDEX_MODE_BLANK_ONLY);
            } else {
                ui->comboBox->setCurrentText(dest);
            }
        } else {
            ui->comboBox->setCurrentIndex(INDEX_MODE_BLANK_ONLY); //no data, default Blank_Only
        }
        g_strfreev(strv);
    }
    g_object_unref(screensaver_settings);

    ui->comboBox->blockSignals(false);
}

void Screensaver::initIdleSliderStatus() {
    int minutes;
    //先判断是否开启屏保，未开启则[从不]
    if (qScreenSaverSetting->get(ACTIVE_KEY).toBool() == false) {
        uslider->blockSignals(true);
        uslider->setValue(lockConvertToSlider(silderNeverValue));
        uslider->blockSignals(false);
    } else {
    session_settings = g_settings_new(SESSION_SCHEMA);
    minutes = g_settings_get_int(session_settings, IDLE_DELAY_KEY);
    uslider->blockSignals(true);
    uslider->setValue(lockConvertToSlider(minutes));
    uslider->blockSignals(false);
    g_object_unref(session_settings);
}

}

void Screensaver::startupScreensaver() {
    //关闭屏保
    closeScreensaver();
    qDebug() << Q_FUNC_INFO << ui->previewWidget->winId();
    if (ui->comboBox->currentIndex() == INDEX_MODE_DEFAULT_UKUI) {//UKUI
        QStringList args;
        args << "-window-id" << QString::number(ui->previewWidget->winId());
        //启动屏保
        process->startDetached(screensaver_bin, args);
        runStringList.append(screensaver_bin);
    } else if (ui->comboBox->currentIndex() == INDEX_MODE_BLANK_ONLY) {//黑屏
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
        return 5;
        break;
    case 2:
        return 10;
        break;
    case 3:
        return 15;
        break;
    case 4:
        return 30;
        break;
    case 5:
        return 60;
        break;
    case 6:
        return silderNeverValue;
        break;
    default:
        return silderNeverValue;
        break;
    }
}

int Screensaver::lockConvertToSlider(const int value) {
    switch (value) {
    case silderNeverValue:
        return 6;
        break;
    case 5:
        return 1;
        break;
    case 10:
        return 2;
        break;
    case 15:
        return 3;
        break;
    case 30:
        return 4;
        break;
    case 60:
        return 5;
        break;
    default:
        return 6;
        break;
    }
}

void Screensaver::themesComboxChanged(int index) {

    char ** strv = NULL;

    //设置屏保
    screensaver_settings = g_settings_new(SCREENSAVER_SCHEMA);
    if (index == INDEX_MODE_DEFAULT_UKUI) {
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_DEFAULT_UKUI);
        hideCustomizeFrame();
    } else if (index == INDEX_MODE_BLANK_ONLY) {
        hideCustomizeFrame();
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_BLANK_ONLY);
    } else if (index == INDEX_MODE_CUSTOMIZE) {
        showCustomizeFrame();
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_CUSTOMIZE);
    } else {
        hideCustomizeFrame();
        g_settings_set_enum(screensaver_settings, MODE_KEY, MODE_SINGLE);
        //获取当前屏保的id
        QVariant variant = ui->comboBox->itemData(index);
        SSThemeInfo info = variant.value<SSThemeInfo>();
        QByteArray ba = info.id.toLatin1();
        strv = g_strsplit(ba.data(), "%%%", 1);
        //qDebug() << Q_FUNC_INFO << "wxy-----------" <<strv;
        g_settings_set_strv(screensaver_settings, THEMES_KEY, (const gchar * const*)strv);
    }
    g_object_unref(screensaver_settings);
    g_strfreev(strv);

    //刷新预览
    startupScreensaver();
}

void Screensaver::kill_screensaver_preview() {

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

void Screensaver::connectToServer(){
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void Screensaver::keyChangedSlot(const QString &key) {
    if(key == "ukui-screensaver") {
        initThemeStatus();
    }
}

void Screensaver::showCustomizeFrame() {
    ui->customizeFrame->setVisible(true);
}

void Screensaver::hideCustomizeFrame() {
    ui->customizeFrame->setVisible(false);
}

void Screensaver::initCustomizeFrame() {
    ui->customizeFrame->setStyleSheet("background-color:palette(base);");
    ui->customizeFrame->setFrameShape(QFrame::NoFrame);
    ui->customizeFrame->setFixedHeight(306);
    ui->customizeLayout->setMargin(0);
    initScreensaverSourceFrame();
    initTimeSetFrame();
    initPictureSwitchFrame();
    initShowTextFrame();
    initShowTextSetFrame();
}

void Screensaver::initScreensaverSourceFrame() {
    QFrame *screensaverSourceFrame = new QFrame();
    QHBoxLayout *sourceLayout = new QHBoxLayout();
    QLabel *sourceLabel = new QLabel();
    QLabel *sourcePathLabel = new QLabel();
    QPushButton *sourceBtn = new QPushButton();
    screensaverSourceFrame->setFixedHeight(50);
    screensaverSourceFrame->setObjectName("screensaverSourceFrame");
    screensaverSourceFrame->setStyleSheet("QFrame#screensaverSourceFrame{background-color:palette(window);border-radius:6px;}");
    screensaverSourceFrame->setLayout(sourceLayout);
    sourceLayout->setContentsMargins(16,0,16,0);
    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(sourcePathLabel);
    sourceLayout->addWidget(sourceBtn);
    sourceLabel->setText(tr("Screensaver source"));
    sourceLabel->setFixedWidth(196);
    sourceLabel->setStyleSheet("background-color:palette(window);");
    sourcePathLabel->setText(("/usr/share/"));
    sourcePathLabel->setFixedHeight(36);
    sourcePathLabel->setMinimumWidth(252);
    sourcePathLabel->setStyleSheet("background-color:palette(window);");
    sourceBtn->setFixedSize(80,36);
    sourceBtn->setStyleSheet("background-color:palette(button);");
    sourceBtn->setText(tr("Select"));
    sourceBtn->raise();
    connect(sourceBtn,&QPushButton::clicked,this,[=]{
        QString path = QFileDialog::getExistingDirectory(screensaverSourceFrame,tr("Select screensaver source"),"/usr/share/backgrounds/");
        qDebug()<<"path = "<<path;
        if (path != "") {
            sourcePathLabel->setText(path);
        }
    });
    ui->customizeLayout->addWidget(screensaverSourceFrame);
}

void Screensaver::initTimeSetFrame() {
    QFrame *timeSetFrame = new QFrame();
    QHBoxLayout *timeSetLayout = new QHBoxLayout();
    QLabel *timeSetLabel = new QLabel();
    QComboBox *timeCom = new QComboBox();
    timeSetFrame->setFixedHeight(50);
    timeSetFrame->setObjectName("timeSetFrame");
    timeSetFrame->setStyleSheet("QFrame#timeSetFrame{background-color:palette(window);border-radius:6px;}");
    timeSetFrame->setLayout(timeSetLayout);
    timeSetLayout->setContentsMargins(16,0,16,0);
    timeSetLayout->addWidget(timeSetLabel);
    timeSetLayout->addWidget(timeCom);
    timeSetLabel->setStyleSheet("background-color:palette(window);");
    timeSetLabel->setText(tr("Replacement time"));
    timeSetLabel->setFixedWidth(196);
    timeCom->setFixedHeight(36);
    timeCom->setMinimumWidth(340);
    timeCom->addItem(tr("1min"));
    timeCom->addItem(tr("5min"));
    timeCom->addItem(tr("10min"));
    timeCom->addItem(tr("30min"));
    /*timeCom->setStyleSheet("QComboBox{background-color:palette(button);} QComboBox::drop-down{ \
            width: 30px; \
            background-color:red;\
            };");*/
    //timeCom->setStyleSheet(ui->comboBox->styleSheet());
    ui->customizeLayout->addWidget(timeSetFrame);
}

void Screensaver::initPictureSwitchFrame() {
    QFrame *pictureSwitchFrame = new QFrame();
    QHBoxLayout *randomLayout = new QHBoxLayout();
    QLabel *randomLabel = new QLabel();
    SwitchButton *randomBtn = new SwitchButton();
    pictureSwitchFrame->setFixedHeight(50);
    pictureSwitchFrame->setStyleSheet("background-color:palette(window);border-radius:6px;");
    pictureSwitchFrame->setLayout(randomLayout);
    randomLayout->setContentsMargins(16,0,16,0);
    randomLayout->addWidget(randomLabel);
    randomLayout->addStretch();
    randomLayout->addWidget(randomBtn);
    randomLabel->setText(tr("Random replacement"));
    randomLabel->setFixedWidth(196);
    ui->customizeLayout->addWidget(pictureSwitchFrame);
}

void Screensaver::initShowTextFrame() {
    QFrame *showTextFrame = new QFrame();
    QHBoxLayout *showTextLayout = new QHBoxLayout();
    QLabel *showLabel = new QLabel();
    QWidget *textWid = new QWidget();
    QVBoxLayout *widVLayout = new QVBoxLayout();
    QTextEdit *inputText = new QTextEdit(); //用户输入文字
    QLabel *noticeLabel = new QLabel();
    showTextFrame->setFixedHeight(98);
    showTextFrame->setStyleSheet("background-color:palette(window);border-radius:6px;");
    showTextFrame->setLayout(showTextLayout);
    showTextLayout->setContentsMargins(16,6,15,8);
    showTextLayout->addWidget(textWid);
    showTextLayout->addWidget(inputText);
    textWid->setLayout(widVLayout);
    textWid->setFixedWidth(196);
    widVLayout->setMargin(0);
    widVLayout->addWidget(showLabel);
    widVLayout->addStretch();
    widVLayout->addWidget(noticeLabel);
    noticeLabel->setVisible(false);
    noticeLabel->setStyleSheet("color:red");
    noticeLabel->setText(tr("Can only enter 30 characters"));//注：中文翻译为30个字，英语翻译为90个字符。
    showLabel->setText(tr("Display text"));
    showLabel->setFixedWidth(196);
    inputText->setFixedHeight(84);
    inputText->setFontPointSize(14);
    inputText->setAcceptRichText(false); //去掉复制文字的颜色字体等属性
    inputText->moveCursor(QTextCursor::Start); //不加这个在输入文字之前复制字体大小可能不对，会受复制内容影响，原因未知。
    inputText->setStyleSheet("background-color:palette(button);");
    inputText->setPlaceholderText(tr("Enter text, up to 30 characters"));//注：中文翻译为30个字，英语翻译为90个字符。
    ui->customizeLayout->addWidget(showTextFrame);

    connect(inputText,&QTextEdit::textChanged,this,[=]{
        qDebug()<<"words = "<<inputText->toPlainText().count();

        if(inputText->toPlainText().count() > 30) {
            noticeLabel->setVisible(true);
            QString textString = inputText->toPlainText().mid(0,30);//一个中文三个字节
            inputText->setText(textString);
            //设置光标在末尾
            QTextCursor textCursor = inputText->textCursor();
            textCursor.movePosition(QTextCursor::End);
            inputText->setTextCursor(textCursor);
        } else if (inputText->toPlainText().count() == 30) {
            noticeLabel->setVisible(true);
        } else {
            noticeLabel->setVisible(false);
        }
    });
}

void Screensaver::initShowTextSetFrame() {
    QFrame *showTextSetFrame = new QFrame();
    QHBoxLayout *textSetLayout = new QHBoxLayout();
    QLabel *textSetLabel = new QLabel();
    SwitchButton *setBtn = new SwitchButton();
    showTextSetFrame->setFixedHeight(50);
    showTextSetFrame->setStyleSheet("background-color:palette(window);border-radius:6px;");
    showTextSetFrame->setLayout(textSetLayout);
    textSetLayout->addWidget(textSetLabel);
    textSetLayout->addStretch();
    textSetLayout->setContentsMargins(16,0,16,0);
    textSetLabel->setText(tr("Text displayed in the center"));
    textSetLabel->setFixedWidth(196);
    textSetLayout->addWidget(setBtn);
    ui->customizeLayout->addWidget(showTextSetFrame);
}

PreviewWidget::PreviewWidget(QWidget *parent):QWidget(parent) {

}

PreviewWidget::~PreviewWidget() {

}
//单机对屏保进行预览
void PreviewWidget::mousePressEvent(QMouseEvent *e) {
    static int i = 0;
    if(e->button() == Qt::LeftButton) {
        qDebug()<<"   "<< ++i;
    }
}

void PreviewWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRect(rect());
}
