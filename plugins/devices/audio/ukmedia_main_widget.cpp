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
#include "ukmedia_main_widget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QStringList>
#include <QSpacerItem>
#include <QListView>
#include <QScrollBar>
#include <QGSettings>
#include <QPixmap>
#include <qmath.h>
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_DIALOG_DBUS_NAME "org.mate.VolumeControl"
#define KEY_SOUNDS_SCHEMA   "org.ukui.sound"
#define GVC_SOUND_SOUND    (xmlChar *) "sound"
#define GVC_SOUND_NAME     (xmlChar *) "name"
#define GVC_SOUND_FILENAME (xmlChar *) "filename"
#define SOUND_SET_DIR "/usr/share/ukui-media/sounds"

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.media.sound"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/sound/keybindings/"

#define MAX_CUSTOM_SHORTCUTS 1000

#define FILENAME_KEY "filename"
#define NAME_KEY "name"

guint appnum = 0;
extern bool isCheckBluetoothInput;
typedef enum {
    BALANCE_TYPE_RL,
    BALANCE_TYPE_FR,
    BALANCE_TYPE_LFE,
} GvcBalanceType;

enum {
    SOUND_TYPE_UNSET,
    SOUND_TYPE_OFF,
    SOUND_TYPE_DEFAULT_FROM_THEME,
    SOUND_TYPE_BUILTIN,
    SOUND_TYPE_CUSTOM
};

struct profile_prio_compare {
    bool operator() (pa_card_profile_info2 const * const lhs, pa_card_profile_info2 const * const rhs) const {

        if (lhs->priority == rhs->priority)
            return strcmp(lhs->name, rhs->name) > 0;

        return lhs->priority > rhs->priority;
    }
};

static void callback(ca_context *c, uint32_t id, int error, void *userdata) {
        fprintf(stderr, "callback called for id %u, error '%s', userdata=%p\n", id, ca_strerror(error), userdata);
}
UkmediaMainWidget::UkmediaMainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pOutputWidget = new UkmediaOutputWidget();
    m_pInputWidget = new UkmediaInputWidget();
    m_pSoundWidget = new UkmediaSoundEffectsWidget();
    firstEntry = true;
    mThemeName = UKUI_THEME_WHITE;
    QVBoxLayout *m_pvLayout = new QVBoxLayout();
    m_pvLayout->addWidget(m_pOutputWidget);
    m_pvLayout->addWidget(m_pInputWidget);
    m_pvLayout->addWidget(m_pSoundWidget);
    m_pvLayout->addSpacing(32);
    m_pvLayout->addSpacerItem(new QSpacerItem(20,0,QSizePolicy::Fixed,QSizePolicy::Expanding));
    m_pvLayout->setSpacing(40);
    this->setLayout(m_pvLayout);
    this->setMinimumWidth(582);
    this->setMaximumWidth(910);
    this->layout()->setContentsMargins(0,0,31,0);
//    this->setStyleSheet("QWidget{background: white;}");

    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }

    m_pSoundList = new QStringList;
    m_pThemeNameList = new QStringList;
    m_pThemeDisplayNameList = new QStringList;
    m_pDeviceNameList = new QStringList;
    m_pDeviceLabelList = new QStringList;
    m_pOutputStreamList = new QStringList;
    m_pPrivOutputStreamList = new QStringList;
    m_pInputStreamList = new QStringList;
    m_pAppVolumeList = new QStringList;
    m_pStreamControlList = new QStringList;
    m_pAppNameList = new QStringList;
    m_pInputPortList = new QStringList;
    m_pOutputPortList = new QStringList;
    m_pSoundNameList = new QStringList;
    m_pProfileNameList = new QStringList;
    m_pSoundThemeList = new QStringList;
    m_pSoundThemeDirList = new QStringList;
    m_pSoundThemeXmlNameList = new QStringList;

    m_pOutputPortLabelList = new QStringList;
    m_pInputPortLabelList = new QStringList;
    m_pListWidgetLabelList = new QStringList;
    m_pCurrentOutputPortLabelList = new QStringList;
    m_pCurrentInputPortLabelList = new QStringList;
    m_pCurrentOutputCardList = new QStringList;
    m_pOutputCardList = new QStringList;
    m_pInputCardList = new QStringList;
    m_pCurrentInputCardList = new QStringList;
    m_pInputDeviceLabelList = new QStringList;
    eventList = new QStringList;
    eventIdNameList = new QStringList;
    m_pCardNameList = new QStringList;
    m_pInputCardNameList = new QStringList;

    eventList->append("window-close");
    eventList->append("system-setting");
    eventList->append("volume-changed");
    eventList->append("alert-sound");
    eventIdNameList->append("dialog-warning");
    eventIdNameList->append("bell");
    eventIdNameList->append("flop");
    eventIdNameList->append("gudou");

    for (int i=0;i<eventList->count();i++) {
//        getValue();
        addValue(eventList->at(i),eventIdNameList->at(i));
    }

    connectContext(this);
//    //创建context
//    m_pContext = mate_mixer_context_new();

//    mate_mixer_context_set_app_name (m_pContext,_("Volume Control"));//设置app名
//    mate_mixer_context_set_app_icon(m_pContext,"multimedia-volume-control");

//    //打开context
//    if G_UNLIKELY (mate_mixer_context_open(m_pContext) == FALSE) {
//        g_warning ("Failed to connect to a sound system**********************");
//    }

//    g_param_spec_object ("context",
//                        "Context",
//                        "MateMixer context",
//                        MATE_MIXER_TYPE_CONTEXT,
//                        (GParamFlags)(G_PARAM_READWRITE |
//                        G_PARAM_CONSTRUCT_ONLY |
//                        G_PARAM_STATIC_STRINGS));

//    MateMixerState state = mate_mixer_context_get_state(m_pContext);
//    if (mate_mixer_context_get_state (m_pContext) != MATE_MIXER_STATE_CONNECTING) {
//        g_timeout_add_seconds(3,connectContext,this);
//    }

//    //当出现获取输入输出异常时，使用默认的输入输出stream
//    contextSetProperty(this);
//    m_pInputStream = mate_mixer_context_get_default_input_stream(m_pContext);
//    m_pOutputStream = mate_mixer_context_get_default_output_stream(m_pContext);

//    connect(m_pInputWidget->m_pInputIconBtn,SIGNAL(clicked()),this,SLOT(inputMuteButtonSlot()));
//    connect(m_pOutputWidget->m_pOutputIconBtn,SIGNAL(clicked()),this,SLOT(outputMuteButtonSlot()));
//    g_signal_connect (G_OBJECT (m_pContext),
//                     "notify::state",
//                     G_CALLBACK (onContextStateNotify),
//                     this);

    //设置滑动条的最大值为100
    m_pInputWidget->m_pIpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMinimum(-100);
    m_pOutputWidget->m_pOpBalanceSlider->setSingleStep(100);
    m_pInputWidget->m_pInputLevelProgressBar->setMaximum(100);
    //设置声音主题
//    //获取声音gsettings值
//    m_pSoundSettings = g_settings_new (KEY_SOUNDS_SCHEMA);

//    g_signal_connect (G_OBJECT (m_pSoundSettings),
//                             "changed",
//                             G_CALLBACK (onKeyChanged),
//                             this);
//    //连接到pulseaudio
//    pa_glib_mainloop *m = pa_glib_mainloop_new(g_main_context_default());
//    api = pa_glib_mainloop_get_api(m);

//    role = "sink-input-by-media-role:event";

//    setupThemeSelector(this);
//    updateTheme(this);
//    //报警声音,从指定路径获取报警声音文件
//    populateModelFromDir(this,SOUND_SET_DIR);
//    //初始化combobox的值
//    comboboxCurrentTextInit();
    time = new QTimer();
    //检测系统主题
    if (QGSettings::isSchemaInstalled(UKUI_THEME_SETTING)){
        m_pThemeSetting = new QGSettings(UKUI_THEME_SETTING);
        if (m_pThemeSetting->keys().contains("styleName")) {
            mThemeName = m_pThemeSetting->get(UKUI_THEME_NAME).toString();
        }
        connect(m_pThemeSetting, SIGNAL(changed(const QString &)),this,SLOT(ukuiThemeChangedSlot(const QString &)));
    }

    //检测设计开关机音乐
    if (QGSettings::isSchemaInstalled(UKUI_SWITCH_SETTING)) {
        m_pBootSetting = new QGSettings(UKUI_SWITCH_SETTING);
        if (m_pBootSetting->keys().contains("startupMusic")) {
            bool startup = m_pBootSetting->get(UKUI_STARTUP_MUSIC_KEY).toBool();
            m_pSoundWidget->m_pStartupButton->setChecked(startup);
        }
        if (m_pBootSetting->keys().contains("poweroffMusic")) {
            bool poweroff = m_pBootSetting->get(UKUI_POWEROFF_MUSIC_KEY).toBool();
            m_pSoundWidget->m_pPoweroffButton->setChecked(poweroff);
        }
        if (m_pBootSetting->keys().contains("logoutMusic")) {
            bool logout = m_pBootSetting->get(UKUI_LOGOUT_MUSIC_KEY).toBool();
            m_pSoundWidget->m_pLogoutButton->setChecked(logout);
        }
        if (m_pBootSetting->keys().contains("weakupMusic")) {
            bool m_hasMusic = m_pBootSetting->get(UKUI_WAKEUP_MUSIC_KEY).toBool();
            m_pSoundWidget->m_pWakeupMusicButton->setChecked(m_hasMusic);
        }
        connect(m_pBootSetting,SIGNAL(changed(const QString &)),this,SLOT(bootMusicSettingsChanged()));
    }
    bool status = g_settings_get_boolean(m_pSoundSettings, EVENT_SOUNDS_KEY);
    m_pSoundWidget->m_pAlertSoundSwitchButton->setChecked(status);
    /*
    if (status) {
        m_pSoundWidget->m_pSoundLayout->insertWidget(5,m_pSoundWidget->m_pAlertSoundVolumeWidget);
    }
    else {
        m_pSoundWidget->m_pAlertSoundVolumeWidget->hide();
    }
    */
    connect(m_pSoundWidget->m_pAlertIconBtn,SIGNAL(clicked()),this,SLOT(alertSoundVolumeChangedSlot()));
    connect(m_pSoundWidget->m_pStartupButton,SIGNAL(checkedChanged(bool)),this,SLOT(startupButtonSwitchChangedSlot(bool)));
    connect(m_pSoundWidget->m_pPoweroffButton,SIGNAL(checkedChanged(bool)),this,SLOT(poweroffButtonSwitchChangedSlot(bool)));
    connect(m_pSoundWidget->m_pLogoutButton,SIGNAL(checkedChanged(bool)),this,SLOT(logoutMusicButtonSwitchChangedSlot(bool)));
    connect(m_pSoundWidget->m_pWakeupMusicButton,SIGNAL(checkedChanged(bool)),this,SLOT(wakeButtonSwitchChangedSlot(bool)));
    connect(m_pSoundWidget->m_pAlertSoundSwitchButton,SIGNAL(checkedChanged(bool)),this,SLOT(alertSoundButtonSwitchChangedSlot(bool)));
    //输出音量控制
    //输出滑动条音量控制
    timeSlider = new QTimer(this);
    connect(timeSlider,SIGNAL(timeout()),this,SLOT(timeSliderSlot()));
    connect(m_pOutputWidget->m_pOpVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(outputWidgetSliderChangedSlot(int)));
    connect(m_pOutputWidget->m_pOpVolumeSlider,&AudioSlider::silderPressSignal,this,[=](){
        mousePress = true;
        mouseReleaseState = false;
    });
    connect(m_pOutputWidget->m_pOpVolumeSlider,&AudioSlider::silderReleaseSignal,this,[=](){
        mouseReleaseState = true;
    });
    //输入滑动条音量控制
    connect(m_pInputWidget->m_pIpVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(inputWidgetSliderChangedSlot(int)));

    //点击报警音量时播放报警声音
    connect(m_pSoundWidget->m_pAlertSlider,SIGNAL(valueChanged(int)),this,SLOT(alertVolumeSliderChangedSlot(int)));
    connect(m_pSoundWidget->m_pAlertSoundCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pLagoutCombobox ,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pSoundThemeCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(themeComboxIndexChangedSlot(int)));
    connect(m_pInputWidget->m_pInputLevelProgressBar,SIGNAL(valueChanged(int)),this,SLOT(inputLevelValueChangedSlot()));
//    connect(m_pInputWidget->m_pInputPortCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(inputPortComboxChangedSlot(int)));
    connect(m_pSoundWidget->m_pVolumeChangeCombobox,SIGNAL(currentIndexChanged (int)),this,SLOT(volumeChangedComboboxChangeSlot(int)));
//    connect(m_pOutputWidget->m_pProfileCombobox,SIGNAL(currentIndexChanged (int)),this,SLOT(profileComboboxChangedSlot(int)));

    connect(m_pOutputWidget->m_pSelectCombobox,SIGNAL(currentIndexChanged (int)),this,SLOT(selectComboboxChangedSlot(int)));

    connect(m_pOutputWidget->m_pOutputListWidget,SIGNAL(currentRowChanged(int )),this,SLOT(outputListWidgetCurrentRowChangedSlot(int)));
    connect(m_pInputWidget->m_pInputListWidget,SIGNAL(currentRowChanged(int )),this,SLOT(inputListWidgetCurrentRowChangedSlot(int)));
    //输入等级
//    ukuiInputLevelSetProperty(this);
}

int UkmediaMainWidget::connectContext(gpointer userdata)
{
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);
    //创建context
    w->m_pContext = mate_mixer_context_new();

    mate_mixer_context_set_app_name (w->m_pContext,_("Volume Control"));//设置app名
    mate_mixer_context_set_app_icon(w->m_pContext,"multimedia-volume-control");

    //打开context
    if G_UNLIKELY (mate_mixer_context_open(w->m_pContext) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }

    g_param_spec_object ("context",
                        "Context",
                        "MateMixer context",
                        MATE_MIXER_TYPE_CONTEXT,
                        (GParamFlags)(G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_STATIC_STRINGS));

    MateMixerState state = mate_mixer_context_get_state(w->m_pContext);

    //当出现获取输入输出异常时，使用默认的输入输出stream
    contextSetProperty(w);
    w->m_pInputStream = mate_mixer_context_get_default_input_stream(w->m_pContext);
    w->m_pOutputStream = mate_mixer_context_get_default_output_stream(w->m_pContext);

    connect(w->m_pInputWidget->m_pInputIconBtn,SIGNAL(clicked()),w,SLOT(inputMuteButtonSlot()));
    connect(w->m_pOutputWidget->m_pOutputIconBtn,SIGNAL(clicked()),w,SLOT(outputMuteButtonSlot()));
    g_signal_connect (G_OBJECT (w->m_pContext),
                     "notify::state",
                     G_CALLBACK (onContextStateNotify),
                     w);

    //获取声音gsettings值
    w->m_pSoundSettings = g_settings_new (KEY_SOUNDS_SCHEMA);

    g_signal_connect (G_OBJECT (w->m_pSoundSettings),
                             "changed",
                             G_CALLBACK (onKeyChanged),
                             w);
    //连接到pulseaudio
    pa_glib_mainloop *m = pa_glib_mainloop_new(g_main_context_default());
    w->api = pa_glib_mainloop_get_api(m);

    w->role = "sink-input-by-media-role:event";

    w->setupThemeSelector(w);
    w->updateTheme(w);
    //报警声音,从指定路径获取报警声音文件
    w->populateModelFromDir(w,SOUND_SET_DIR);
    //初始化combobox的值
    w->comboboxCurrentTextInit();

    //输入等级
    w->ukuiInputLevelSetProperty(w);
    w->reconnectTime = 5;

    if (mate_mixer_context_get_state (w->m_pContext) != MATE_MIXER_STATE_CONNECTING) {
        qDebug() << "prety reconnect pulseaudio after 5s" ;
        g_timeout_add_seconds(w->reconnectTime,connectContext,userdata);
    }
    else {

        return 0;
    }

}

QPixmap UkmediaMainWidget::drawDarkColoredPixmap(const QPixmap &source)
{
//    QColor currentcolor=HighLightEffect::getCurrentSymbolicColor();
    QColor gray(255,255,255);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                }
                else {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap UkmediaMainWidget::drawLightColoredPixmap(const QPixmap &source)
{
//    QColor currentcolor=HighLightEffect::getCurrentSymbolicColor();
    QColor gray(255,255,255);
    QColor standard (0,0,0);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

void UkmediaMainWidget::alertIconButtonSetIcon(bool state,int value)
{
    QImage image;
    QColor color = QColor(0,0,0,216);
    if (mThemeName == UKUI_THEME_WHITE) {
        color = QColor(0,0,0,216);
    }
    else if (mThemeName == UKUI_THEME_BLACK) {
        color = QColor(255,255,255,216);
    }
    m_pSoundWidget->m_pAlertIconBtn->mColor = color;
    if (state) {
        image  = QImage("/usr/share/ukui-media/img/audio-volume-muted.svg");
        m_pSoundWidget->m_pAlertIconBtn->mImage = image;
    }
    else if (value <= 0) {
        image  = QImage("/usr/share/ukui-media/img/audio-volume-muted.svg");
        m_pSoundWidget->m_pAlertIconBtn->mImage = image;
    }
    else if (value > 0 && value <= 33) {
        image = QImage("/usr/share/ukui-media/img/audio-volume-low.svg");
        m_pSoundWidget->m_pAlertIconBtn->mImage = image;
    }
    else if (value >33 && value <= 66) {
        image = QImage("/usr/share/ukui-media/img/audio-volume-medium.svg");
        m_pSoundWidget->m_pAlertIconBtn->mImage = image;
    }
    else {
        image = QImage("/usr/share/ukui-media/img/audio-volume-high.svg");
        m_pSoundWidget->m_pAlertIconBtn->mImage = image;
    }

}

void UkmediaMainWidget::createAlertSound(UkmediaMainWidget *pWidget)
{
    const GList   *list;
    m_pOutputWidget->m_pOutputListWidget->clear();
    m_pInputWidget->m_pInputListWidget->clear();
    cardMap.clear();
    outputPortNameMap.clear();
    inputPortNameMap.clear();
    outputPortLabelMap.clear();
    currentOutputPortLabelMap.clear();
    currentInputPortLabelMap.clear();
    inputPortLabelMap.clear();
    profileNameMap.clear();
    inputPortProfileNameMap.clear();
    cardProfileMap.clear();
    cardProfilePriorityMap.clear();
    inputCardStreamMap.clear();
    outputCardStreamMap.clear();
    connect_to_pulse(this);

    /* Find an event role stored control */
    list = mate_mixer_context_list_stored_controls (pWidget->m_pContext);
    while (list != NULL) {
        MateMixerStreamControl *control = MATE_MIXER_STREAM_CONTROL (list->data);
        MateMixerStreamControlMediaRole media_role;
        MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
        media_role = mate_mixer_stream_control_get_media_role (control);
        if (media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT) {
            pWidget->m_pMediaRoleControl = control;
            //初始化提示音量的值
            int volume = mate_mixer_stream_control_get_volume(m_pMediaRoleControl);
            volume = int(volume*100/65536.0+0.5);
            pWidget->m_pSoundWidget->m_pAlertSlider->setValue(volume);
            pWidget->m_pSoundWidget->m_pAlertVolumeLabel->setText(QString::number(volume).append("%"));
            qDebug() << "media role : " << mate_mixer_stream_control_get_name(control) <<"提示音量值为:" <<volume;
            gboolean bIsMute = mate_mixer_stream_control_get_mute(control);
            alertIconButtonSetIcon(bIsMute,volume);
            ukuiBarSetStream(pWidget,stream);
            break;
        }

        list = list->next;
    }
}

/*
    初始化combobox的值
*/
void UkmediaMainWidget::comboboxCurrentTextInit()
{
    QList<char *> existsPath = listExistsPath();

    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();
            int index = 0;
            for (int i=0;i<m_pSoundList->count();i++) {
                QString str = m_pSoundList->at(i);
                if (str.contains(filenameStr,Qt::CaseSensitive)) {
                    index = i;
                    break;
                }
            }
            if (nameStr == "alert-sound") {
                QString displayName = m_pSoundNameList->at(index);
                m_pSoundWidget->m_pAlertSoundCombobox->setCurrentText(displayName);
                continue;
            }
            if (nameStr == "window-close") {
                QString displayName = m_pSoundNameList->at(index);
                continue;
            }
            else if (nameStr == "volume-changed") {
                QString displayName = m_pSoundNameList->at(index);
                m_pSoundWidget->m_pVolumeChangeCombobox->setCurrentText(displayName);
                continue;
            }
            else if (nameStr == "system-setting") {
                QString displayName = m_pSoundNameList->at(index);
                continue;
            }
        }
        else {
            continue;
        }
    }
}

/*
    是否播放开机音乐
*/
void UkmediaMainWidget::startupButtonSwitchChangedSlot(bool status)
{
    bool bBootStatus = true;
    if (m_pBootSetting->keys().contains("startupMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_STARTUP_MUSIC_KEY).toBool();
        if (bBootStatus != status) {
            m_pBootSetting->set(UKUI_STARTUP_MUSIC_KEY,status);
        }
    }
}

/*
    是否播放关机音乐
*/
void UkmediaMainWidget::poweroffButtonSwitchChangedSlot(bool status)
{
    bool bBootStatus = true;
    if (m_pBootSetting->keys().contains("poweroffMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_POWEROFF_MUSIC_KEY).toBool();
        if (bBootStatus != status) {
            m_pBootSetting->set(UKUI_POWEROFF_MUSIC_KEY,status);
        }
    }
}

/*
    是否播放注销音乐
*/
void UkmediaMainWidget::logoutMusicButtonSwitchChangedSlot(bool status)
{
    bool bBootStatus = true;
    if (m_pBootSetting->keys().contains("logoutMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_LOGOUT_MUSIC_KEY).toBool();
        if (bBootStatus != status) {
            m_pBootSetting->set(UKUI_LOGOUT_MUSIC_KEY,status);
        }
    }
}

/*
    是否播放唤醒音乐
*/
void UkmediaMainWidget::wakeButtonSwitchChangedSlot(bool status)
{
    bool bBootStatus = true;
    if (m_pBootSetting->keys().contains("weakupMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_WAKEUP_MUSIC_KEY).toBool();
        if (bBootStatus != status) {
            m_pBootSetting->set(UKUI_WAKEUP_MUSIC_KEY,status);
        }
    }
}

/*
    提示音的开关
*/
void UkmediaMainWidget::alertSoundButtonSwitchChangedSlot(bool status)
{
    g_settings_set_boolean (m_pSoundSettings, EVENT_SOUNDS_KEY, status);
    /*
    if (status == true) {
        m_pSoundWidget->m_pAlertSoundVolumeWidget->show();
        m_pSoundWidget->m_pSoundLayout->insertWidget(5,m_pSoundWidget->m_pAlertSoundVolumeWidget);
    }
    else {
        m_pSoundWidget->m_pAlertSoundVolumeWidget->hide();
        m_pSoundWidget->m_pSoundLayout->removeWidget(m_pSoundWidget->m_pAlertSoundVolumeWidget);
    }
    */
}

void UkmediaMainWidget::bootMusicSettingsChanged()
{
    bool bBootStatus = true;
    bool status;
    if (m_pBootSetting->keys().contains("startupMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_STARTUP_MUSIC_KEY).toBool();
        if (status != bBootStatus ) {
            m_pSoundWidget->m_pStartupButton->setChecked(bBootStatus);
        }
    }
    if (m_pBootSetting->keys().contains("poweroffMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_POWEROFF_MUSIC_KEY).toBool();
        if (status != bBootStatus ) {
            m_pSoundWidget->m_pPoweroffButton->setChecked(bBootStatus);
        }
    }
    if (m_pBootSetting->keys().contains("logoutMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_LOGOUT_MUSIC_KEY).toBool();
        if (status != bBootStatus ) {
            m_pSoundWidget->m_pLogoutButton->setChecked(bBootStatus);
        }
    }
    if (m_pBootSetting->keys().contains("weakupMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_WAKEUP_MUSIC_KEY).toBool();
        if (status != bBootStatus ) {
            m_pSoundWidget->m_pWakeupMusicButton->setChecked(bBootStatus);
        }
    }
}


/*
    系统主题更改
*/
void UkmediaMainWidget::ukuiThemeChangedSlot(const QString &themeStr)
{
    if (m_pThemeSetting->keys().contains("styleName")) {
        mThemeName = m_pThemeSetting->get(UKUI_THEME_NAME).toString();
    }
    int nInputValue = getInputVolume();
    int nOutputValue = getOutputVolume();
    bool inputStatus = getInputMuteStatus();
    bool outputStatus = getOutputMuteStatus();
    inputVolumeDarkThemeImage(nInputValue,inputStatus);
    outputVolumeDarkThemeImage(nOutputValue,outputStatus);
    m_pOutputWidget->m_pOutputIconBtn->repaint();
    m_pSoundWidget->m_pAlertIconBtn->repaint();
    m_pInputWidget->m_pInputIconBtn->repaint();
}

/*
 * context状态通知
*/
void UkmediaMainWidget::onContextStateNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on context state notify");
    MateMixerState state = mate_mixer_context_get_state (m_pContext);

    listDevice(m_pWidget,m_pContext);
    if (state == MATE_MIXER_STATE_READY) {
        updateIconOutput(m_pWidget);
        updateIconInput(m_pWidget);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {
        UkuiMessageBox::critical(m_pWidget,tr("sound error"),tr("load sound failed"),UkuiMessageBox::Yes | UkuiMessageBox::No,UkuiMessageBox::Yes);
        g_debug(" mate mixer state failed");
    }
    m_pWidget->createAlertSound(m_pWidget);


    //    点击输出设备
    connect(m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox,SIGNAL(currentIndexChanged(QString)),m_pWidget,SLOT(outputDeviceComboxIndexChangedSlot(QString)));
    //    点击输入设备
    connect(m_pWidget->m_pInputWidget->m_pInputDeviceCombobox,SIGNAL(currentIndexChanged(QString)),m_pWidget,SLOT(inputDeviceComboxIndexChangedSlot(QString)));
}

/*
    context 存储control增加
*/
void UkmediaMainWidget::onContextStoredControlAdded(MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on context stored control add");
    MateMixerStreamControl *m_pControl;
    MateMixerStreamControlMediaRole mediaRole;
    m_pControl = MATE_MIXER_STREAM_CONTROL (mate_mixer_context_get_stored_control (m_pContext, m_pName));
    if (G_UNLIKELY (m_pControl == nullptr))
        return;
    qDebug() << "on context stored control add" << mate_mixer_stream_control_get_name(m_pControl);
    m_pWidget->m_pMediaRoleControl = m_pControl;
    mediaRole = mate_mixer_stream_control_get_media_role (m_pControl);
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT)
        ukuiBarSetStreamControl (m_pWidget,MATE_MIXER_DIRECTION_UNKNOWN, m_pControl);
}

/*
    当其他设备插入时添加这个stream
*/
void UkmediaMainWidget::onContextStreamAdded (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    qDebug() <<"on context stream added" << m_pName;
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_stream (m_pContext, m_pName);
    if (G_UNLIKELY (m_pStream == nullptr))
        return;
    addStream (m_pWidget, m_pStream,m_pContext);
}

/*
列出设备
*/
void UkmediaMainWidget::listDevice(UkmediaMainWidget *m_pWidget,MateMixerContext *m_pContext)
{
    g_debug("list device");
    const GList *m_pList;
    m_pList = mate_mixer_context_list_streams (m_pContext);
    while (m_pList != nullptr) {
        addStream (m_pWidget, MATE_MIXER_STREAM (m_pList->data),m_pContext);
        m_pList = m_pList->next;
    }

    //初始化输入输出设备
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(m_pContext);
    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(m_pContext);
    QString inputDeviceName = mate_mixer_stream_get_name(inputStream);
    QString outputDeviceName = mate_mixer_stream_get_name(outputStream);

    MateMixerDevice *device = mate_mixer_stream_get_device(outputStream);
    QString deviceName = mate_mixer_device_get_name(device);
    int index = m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->findText(outputDeviceName);
    if (index >= 0 ) {
        m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
        m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->setCurrentIndex(index);
        m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
        qDebug() << "初始化输出设备：" << m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->currentText();
        QTimer *time = new QTimer;
        time->start(100);
        connect(time,&QTimer::timeout,[=](){
            int devIndex = m_pWidget->m_pCardNameList->indexOf(deviceName);

            qDebug() << "output device combobox index changed *******************"  << m_pWidget->m_pOutputWidget->m_pOutputListWidget->count() << outputDeviceName <<  deviceName;
            m_pWidget->findOutputListWidgetItem(m_pWidget->m_pCardNameList->at(devIndex),outputStream);

            delete time;
        });
//        if (index < m_pWidget->m_pOutputPortLabelList->count()) {
//            m_pWidget->m_pOutputPortLabelList->at(index);
//            int i =0;
//            if (index >= 0) {
//                for (i=0;i <m_pWidget->m_pOutputWidget->m_pOutputListWidget->count();i++) {

//                    QListWidgetItem *item = m_pWidget->m_pOutputWidget->m_pOutputListWidget->item(i);
//                    UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pWidget->m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
//                    //            wid->portLabel
//                    if(m_pWidget->m_pOutputPortLabelList->at(index) == wid->portLabel->text()) {
//                        m_pWidget->m_pOutputWidget->m_pOutputListWidget->blockSignals(true);
//                        m_pWidget->m_pOutputWidget->m_pOutputListWidget->setCurrentItem(item);
//                        m_pWidget->m_pOutputWidget->m_pOutputListWidget->blockSignals(false);
//                    }

//                }
//            }
//        }
    }
    device = mate_mixer_stream_get_device(inputStream);
    deviceName = mate_mixer_device_get_name(device);
    int devIndex = m_pWidget->m_pInputCardNameList->indexOf(deviceName);
    index = m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->findText(inputDeviceName);
    qDebug() << "input combobox index:" << index << inputDeviceName;
    if (index >= 0 && devIndex >= 0) {
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->setCurrentIndex(index);
        QTimer *time = new QTimer;
        time->start(100);
        connect(time,&QTimer::timeout,[=](){
            qDebug() << "input device combobox index changed *******************"  << m_pWidget->m_pInputWidget->m_pInputListWidget->count() << inputDeviceName << "index:" << devIndex;// m_pWidget->m_pInputCardNameList->at(devIndex);
            m_pWidget->findInputListWidgetItem(m_pWidget->m_pInputCardNameList->at(devIndex),inputStream);

            delete time;
        });
    }
    else {
        qDebug() << "input device index or input card index <= 0";
    }

    const GList *pDeviceList;
    const GList *switches;
    const gchar *profileName;
    const gchar *profileLabel;
    pDeviceList = mate_mixer_context_list_devices(m_pContext);
    while (pDeviceList) {
        addDevice(m_pWidget, MATE_MIXER_DEVICE (pDeviceList->data));
        switches = mate_mixer_device_list_switches (MATE_MIXER_DEVICE(pDeviceList->data));
        while (switches != nullptr) {
            MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);
            const GList *options;
            options = mate_mixer_switch_list_options ( MATE_MIXER_SWITCH(swtch));
            while (options != NULL) {   
                MateMixerSwitchOption *option = MATE_MIXER_SWITCH_OPTION (options->data);
                profileLabel = mate_mixer_switch_option_get_label (option);
                profileName = mate_mixer_switch_option_get_name(option);
                /* Select the currently active option of the switch */
                options = options->next;
            }
            switches = switches->next;
        }
        pDeviceList = pDeviceList->next;
    }
}

void UkmediaMainWidget::addStream (UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream,MateMixerContext *m_pContext)
{
    g_debug("add stream");
    const GList *m_pControls;
    MateMixerDirection direction;
    direction = mate_mixer_stream_get_direction (m_pStream);
    const gchar *m_pName;
    const gchar *m_pLabel;
    MateMixerStreamControl *m_pControl;
    qDebug() << "add stream ,stream name is :" << mate_mixer_stream_get_name(m_pStream);
    const GList *switchList;
    MateMixerSwitch *swt;
    switchList = mate_mixer_stream_list_switches(m_pStream);
    while (switchList != nullptr) {
        swt = MATE_MIXER_SWITCH(switchList->data);
        MateMixerSwitchOption *opt = mate_mixer_switch_get_active_option(swt);
        const char *name = mate_mixer_switch_option_get_name(opt);
        m_pWidget->m_pDeviceStr = name;
        switchList = switchList->next;
    }
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        MateMixerStream *m_pInput;
        m_pInput = mate_mixer_context_get_default_input_stream (m_pContext);
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        if (m_pStream == m_pInput) {
            ukuiBarSetStream(m_pWidget,m_pStream);
            m_pControl = mate_mixer_stream_get_default_control(m_pStream);
            updateInputSettings (m_pWidget,m_pControl);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        QString deviceName = m_pName;
        MateMixerDevice *device  = mate_mixer_stream_get_device(m_pStream);
        QString devName;
            if (!deviceName.contains("monitor",Qt::CaseInsensitive) /*&& !m_pWidget->m_pInputStreamList->contains(m_pName)*/) {
                m_pWidget->m_pInputStreamList->append(m_pName);
                m_pWidget->m_pInputDeviceLabelList->append(m_pLabel);
                m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->addItem(m_pName);
                if (MATE_MIXER_IS_DEVICE(device)) {
                    devName = mate_mixer_device_get_name(device);
                    m_pWidget->inputStreamMapCardName(m_pName,devName);
                }
                else {
                    devName = m_pWidget->findInputStreamCardName(m_pName);
                }
                qDebug() << "input card name append :" << devName << "stream name :" << mate_mixer_stream_get_name(m_pStream);
                m_pWidget->m_pInputCardNameList->append(devName);
            }
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *m_pOutput;
        MateMixerStreamControl *m_pControl;
        m_pOutput = mate_mixer_context_get_default_output_stream (m_pContext);
        m_pControl = mate_mixer_stream_get_default_control (m_pStream);
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        MateMixerDevice *device  = mate_mixer_stream_get_device(m_pStream);
        QString devName;

        if (m_pStream == m_pOutput) {
            updateOutputSettings(m_pWidget,m_pControl);
            ukuiBarSetStream (m_pWidget, m_pStream);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        if (!strstr(m_pName,".echo-cancel") && !strstr(m_pName,"auto_null")/* && !(m_pWidget->m_pOutputStreamList->contains(m_pName))*/) {
            m_pWidget->m_pOutputStreamList->append(m_pName);
            m_pWidget->m_pDeviceLabelList->append(m_pLabel);

            if (MATE_MIXER_IS_DEVICE(device)) {
                devName = mate_mixer_device_get_name(device);
                m_pWidget->outputStreamMapCardName(m_pName,devName);
            }
            else {
                devName = m_pWidget->findOutputStreamCardName(m_pName);
            }

            m_pWidget->m_pCardNameList->append(devName);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->addItem(m_pName);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
            qDebug() << "card name list apppend : "<<  devName << m_pName << m_pLabel;
        }

    }
    m_pControls = mate_mixer_stream_list_controls (m_pStream);
    while (m_pControls != nullptr) {
        MateMixerStreamControl    *m_pControl = MATE_MIXER_STREAM_CONTROL (m_pControls->data);
        MateMixerStreamControlRole role;
        role = mate_mixer_stream_control_get_role (m_pControl);
        const gchar *m_pStreamControlName = mate_mixer_stream_control_get_name(m_pControl);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(m_pControl);
            const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
            if (strcmp(m_pAppName,"ukui-session") != 0) {
                m_pWidget->m_pStreamControlList->append(m_pStreamControlName);
                if G_UNLIKELY (m_pControl == nullptr)
                    return;
                m_pWidget->m_pStreamControlList->append(m_pName);
                if G_UNLIKELY (m_pControl == nullptr)
                    return;
                addApplicationControl (m_pWidget, m_pControl);
            }
        }
        m_pControls = m_pControls->next;
    }

    // XXX find a way to disconnect when removed
    g_signal_connect (G_OBJECT (m_pStream),
                      "control-added",
                      G_CALLBACK (onStreamControlAdded),
                      m_pWidget);
    g_signal_connect (G_OBJECT (m_pStream),
                      "control-removed",
                      G_CALLBACK (onStreamControlRemoved),
                      m_pWidget);
}



/*
    添加应用音量控制
*/
void UkmediaMainWidget::addApplicationControl (UkmediaMainWidget *m_pWidget, MateMixerStreamControl *m_pControl)
{
    g_debug("add application control");
    MateMixerStream *m_pStream;
    MateMixerStreamControlMediaRole mediaRole;
    MateMixerAppInfo *m_pInfo;
    MateMixerDirection direction = MATE_MIXER_DIRECTION_UNKNOWN;
    const gchar *m_pAppId;
    const gchar *m_pAppName;
    const gchar *m_pAppIcon;
    appnum++;
    mediaRole = mate_mixer_stream_control_get_media_role (m_pControl);
    /* Add stream to the applications page, but make sure the stream qualifies
     * for the inclusion */
    m_pInfo = mate_mixer_stream_control_get_app_info (m_pControl);
    if (m_pInfo == nullptr)
        return;

    /* Skip streams with roles we don't care about */
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_TEST ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_ABSTRACT ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_FILTER)
            return;

    m_pAppId = mate_mixer_app_info_get_id (m_pInfo);

    /* These applications may have associated streams because they do peak
     * level monitoring, skip these too */
    if (!g_strcmp0 (m_pAppId, "org.mate.VolumeControl") ||
        !g_strcmp0 (m_pAppId, "org.gnome.VolumeControl") ||
        !g_strcmp0 (m_pAppId, "org.PulseAudio.pavucontrol"))
        return;

    QString app_icon_name = mate_mixer_app_info_get_icon(m_pInfo);

    m_pAppName = mate_mixer_app_info_get_name (m_pInfo);

    if (m_pAppName == nullptr)
        m_pAppName = mate_mixer_stream_control_get_label (m_pControl);
    if (m_pAppName == nullptr)
        m_pAppName = mate_mixer_stream_control_get_name (m_pControl);
    if (G_UNLIKELY (m_pAppName == nullptr))
        return;

    /* By default channel bars use speaker icons, use microphone icons
     * instead for recording applications */
    m_pStream = mate_mixer_stream_control_get_stream (m_pControl);
    if (m_pStream != nullptr)
        direction = mate_mixer_stream_get_direction (m_pStream);

    if (direction == MATE_MIXER_DIRECTION_INPUT) {

    }
    m_pAppIcon = mate_mixer_app_info_get_icon (m_pInfo);
    if (m_pAppIcon == nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT)
            m_pAppIcon = "audio-input-microphone";
        else
            m_pAppIcon = "applications-multimedia";
    }
    ukuiBarSetStreamControl (m_pWidget,direction, m_pControl);
}

void UkmediaMainWidget::onStreamControlAdded (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on stream control added");
    MateMixerStreamControl    *m_pControl;
    MateMixerStreamControlRole role;

    m_pWidget->updatePort = true;
    m_pControl = mate_mixer_stream_get_control (m_pStream, m_pName);
    if G_UNLIKELY (m_pControl == nullptr)
        return;

    MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(m_pControl);
    if (m_pAppInfo != nullptr) {
        const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
        if (strcmp(m_pAppName,"ukui-session") != 0) {
            m_pWidget->m_pStreamControlList->append(m_pName);
            if G_UNLIKELY (m_pControl == nullptr)
                    return;

            role = mate_mixer_stream_control_get_role (m_pControl);
            if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
                addApplicationControl(m_pWidget, m_pControl);
            }
        }
    }
}

/*
    移除control
*/
void UkmediaMainWidget::onStreamControlRemoved (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    g_debug("on stream control removed");
    m_pWidget->updatePort = true;
    if (m_pWidget->m_pStreamControlList->count() > 0 && m_pWidget->m_pAppNameList->count() > 0) {

        int i = m_pWidget->m_pStreamControlList->indexOf(m_pName);
        if (i < 0)
            return;
        m_pWidget->m_pStreamControlList->removeAt(i);
        m_pWidget->m_pAppNameList->removeAt(i);

    }
    else {
        m_pWidget->m_pStreamControlList->clear();
        m_pWidget->m_pAppNameList->clear();
    }
}

/*
    连接context，处理不同信号
*/
void UkmediaMainWidget::setContext(UkmediaMainWidget *m_pWidget,MateMixerContext *m_pContext)
{
    g_debug("set context");
    g_signal_connect (G_OBJECT (m_pContext),
                      "stream-added",
                      G_CALLBACK (onContextStreamAdded),
                      m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "stream-removed",
                    G_CALLBACK (onContextStreamRemoved),
                    m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "device-added",
                    G_CALLBACK (onContextDeviceAdded),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "device-removed",
                    G_CALLBACK (onContextDeviceRemoved),
                    m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "notify::default-input-stream",
                    G_CALLBACK (onContextDefaultInputStreamNotify),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "notify::default-output-stream",
                    G_CALLBACK (onContextDefaultOutputStreamNotify),
                    m_pWidget);


    g_signal_connect (G_OBJECT (m_pContext),
                    "stored-control-added",
                    G_CALLBACK (onContextStoredControlAdded),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "stored-control-removed",
                    G_CALLBACK (onContextStoredControlRemoved),
                    m_pWidget);

}

/*
    remove stream
*/
void UkmediaMainWidget::onContextStreamRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    Q_UNUSED(m_pName);
    g_debug("on context stream removed");

    removeStream (m_pWidget, m_pName);
}

/*
    移除stream
*/
void UkmediaMainWidget::removeStream (UkmediaMainWidget *m_pWidget, const gchar *m_pName)
{
    qDebug() << "remove stream" <<m_pName;
    int index;
    index = m_pWidget->m_pInputStreamList->indexOf(m_pName);
    if (index >= 0) {
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->blockSignals(true);
        m_pWidget->m_pInputStreamList->removeAt(index);
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->removeItem(index);
        m_pWidget->m_pInputCardNameList->removeAt(index);
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->blockSignals(false);
    }
    else {
        index = m_pWidget->m_pOutputStreamList->indexOf(m_pName);
        if (index >= 0) {
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
            m_pWidget->m_pOutputStreamList->removeAt(index);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->removeItem(index);
            m_pWidget->m_pCardNameList->removeAt(index);
            m_pWidget->m_pDeviceLabelList->removeAt(index);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
        }
    }
    if (m_pWidget->m_pAppVolumeList != nullptr) {
        ukuiBarSetStream(m_pWidget,nullptr);
    }
    m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->setValue(0);
}

/*
    context 添加设备并设置到单选框
*/
void UkmediaMainWidget::onContextDeviceAdded(MateMixerContext *m_pContext, const gchar *m_pName, UkmediaMainWidget *m_pWidget)
{
    g_debug("on context device added");
    MateMixerDevice *m_pDevice;
    m_pDevice = mate_mixer_context_get_device (m_pContext, m_pName);

    if (G_UNLIKELY (m_pDevice == nullptr))
        return;
    addDevice (m_pWidget, m_pDevice);

    int index = m_pWidget->m_pDeviceNameList->indexOf(m_pName);
    if (index >= 0 && index < m_pWidget->m_pOutputWidget->m_pSelectCombobox->count()) {
        m_pWidget->m_pOutputWidget->m_pSelectCombobox->setCurrentIndex(index);
    }
}

/*
    添加设备
*/
void UkmediaMainWidget::addDevice(UkmediaMainWidget *m_pWidget, MateMixerDevice *pDevice)
{
    g_debug("add device");
    const gchar *pName;

    const gchar *profileLabel = NULL;
    /*
         * const gchar *m_pLabel;
         * m_pLabel = mate_mixer_device_get_label (m_pDevice);
        */
    m_pWidget->m_pDevice = pDevice;
    pName  = mate_mixer_device_get_name (pDevice);
    const char *pLabel = mate_mixer_device_get_label(pDevice);
    QString sLabel(pLabel);
    m_pWidget->m_pDeviceNameList->append(pName);

    if (m_pWidget->m_pOutputWidget->m_pSelectCombobox->findText(pLabel)) {

        if (strstr(pName,"hdmi")) {
            sLabel+=" (HDMI)";
        }
        else if (strstr(pName,"dp")) {
            sLabel+=" (DP)";
        }
        else if (strstr(pName,"usb")) {
            sLabel+=" (USB)";
        }

    }
    m_pWidget->m_pOutputWidget->m_pSelectCombobox->addItem(pLabel);

    qDebug() << "add device name,device name" << pName << pLabel ;
    MateMixerSwitch *profileSwitch;

    profileSwitch = findDeviceProfileSwitch(m_pWidget,pDevice);
    MateMixerSwitchOption *activeProfile;
    activeProfile = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (profileSwitch));
    if (G_LIKELY (activeProfile != NULL))
        profileLabel = mate_mixer_switch_option_get_label(activeProfile);

    if (profileSwitch != NULL) {

        activeProfile = mate_mixer_switch_get_active_option(profileSwitch);
        if (G_LIKELY (activeProfile != NULL))
            profileLabel = mate_mixer_switch_option_get_label(activeProfile);
        g_signal_connect (G_OBJECT (profileSwitch),
                          "notify::active-option",
                          G_CALLBACK (onDeviceProfileActiveOptionNotify),
                          m_pWidget);
    }

    m_pWidget->updateInputDevicePort();
    m_pWidget->updateOutputDevicePort();

}

/*
    移除设备
*/
void UkmediaMainWidget::onContextDeviceRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    g_debug("on context device removed");
    int index = m_pWidget->m_pDeviceNameList->indexOf(m_pName);

    if (index >= 0) {
        qDebug() << "device remove " << m_pWidget->m_pDeviceNameList->at(index) << m_pWidget->m_pOutputWidget->m_pSelectCombobox->itemText(index);
        m_pWidget->m_pDeviceNameList->removeAt(index);
        m_pWidget->m_pOutputWidget->m_pSelectCombobox->removeItem(index);
        m_pWidget->m_pOutputWidget->m_pSelectCombobox->update();
    }
}

/*
    默认输入流通知
*/
void UkmediaMainWidget::onContextDefaultInputStreamNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug ("on context default input stream notify");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pWidget->m_pContext);

    if(!MATE_MIXER_IS_STREAM(m_pStream))
        return;
    QString deviceName = mate_mixer_stream_get_name(m_pStream);
    int index = m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->findText(deviceName);
    qDebug() << "onContextDefaultInputStreamNotify" << mate_mixer_stream_get_name(m_pStream) << index << m_pWidget->m_pInputPortLabelList->count();
    if (index < 0) {
        //if input stream is monitor stream,set input list widget select null
        m_pWidget->m_pInputWidget->m_pInputListWidget->setCurrentRow(-1);
        return;
    }
    else {
        QString inputDeviceName = mate_mixer_stream_get_name(m_pStream);
        MateMixerDevice *device = mate_mixer_stream_get_device(m_pStream);
        deviceName = mate_mixer_device_get_name(device);
        int devIndex = m_pWidget->m_pInputCardNameList->indexOf(deviceName);
        index = m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->findText(inputDeviceName);
        if (devIndex >=0)
        m_pWidget->findInputListWidgetItem(m_pWidget->m_pInputCardNameList->at(devIndex),m_pStream);
//        if (index < m_pWidget->m_pInputPortLabelList->count()) {
//            m_pWidget->m_pInputPortLabelList->at(index);
//            int i =0;

//            for (i=0;i <m_pWidget->m_pInputWidget->m_pInputListWidget->count();i++) {
//                QListWidgetItem *item = m_pWidget->m_pInputWidget->m_pInputListWidget->item(i);
//                UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pWidget->m_pInputWidget->m_pInputListWidget->itemWidget(item);
//                qDebug() << "1111111111" << m_pWidget->m_pInputPortLabelList->at(index) << wid->portLabel->text();
//                //            wid->portLabel
//                if(m_pWidget->m_pInputPortLabelList->at(index) == wid->portLabel->text()) {
//                    m_pWidget->m_pInputWidget->m_pInputListWidget->blockSignals(true);
//                    m_pWidget->m_pInputWidget->m_pInputListWidget->setCurrentItem(item);
//                    m_pWidget->m_pInputWidget->m_pInputListWidget->blockSignals(false);
//                }
//            }
//        }
    }
    m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->setCurrentIndex(index);
    updateIconInput(m_pWidget);
    m_pWidget->updateInputDevicePort();
    setInputStream(m_pWidget, m_pStream);
}

void UkmediaMainWidget::setInputStream(UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream)
{
    g_debug("set input stream");
    if (!MATE_MIXER_IS_STREAM(m_pStream)) {
        return;
    }
    qDebug() << "set input stream" << mate_mixer_stream_get_name(m_pStream);

    if (m_pWidget->m_pPrivInputControl != nullptr) {
        /* Disable monitoring of the previous control */
//        g_signal_handlers_disconnect_by_func (G_OBJECT (m_pWidget->m_pPrivInputControl),
//                                              G_CALLBACK (onStreamControlMonitorValue),
//                                              m_pWidget);
        g_signal_handlers_disconnect_by_data (G_OBJECT (m_pWidget->m_pPrivInputControl),
//                                              G_CALLBACK (onStreamControlMonitorValue),
                                              m_pWidget);
        mate_mixer_stream_control_set_monitor_enabled(m_pWidget->m_pPrivInputControl,false);
    }
    MateMixerStreamControl *m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    if (m_pControl != nullptr) {
//        mate_mixer_stream_control_set_monitor_enabled (m_pControl, false);
    }
    ukuiBarSetStream (m_pWidget, m_pStream);

    if (m_pStream != nullptr) {
        const GList *m_pControls;
        m_pControls = mate_mixer_context_list_stored_controls (m_pWidget->m_pContext);

        /* Move all stored controls to the newly selected default stream */
        while (m_pControls != nullptr) {
            MateMixerStream *parent;

            m_pControl = MATE_MIXER_STREAM_CONTROL (m_pControls->data);
            parent = mate_mixer_stream_control_get_stream (m_pControl);

            /* Prefer streamless controls to stay the way they are, forcing them to
             * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != m_pStream) {
                MateMixerDirection direction = mate_mixer_stream_get_direction (parent);
                if (direction == MATE_MIXER_DIRECTION_INPUT)
                    mate_mixer_stream_control_set_stream (m_pControl, m_pStream);
            }
            m_pControls = m_pControls->next;
        }

        /* Enable/disable the peak level monitor according to mute state */
        g_signal_connect (G_OBJECT (m_pStream),
                          "notify::mute",
                          G_CALLBACK (onStreamControlMuteNotify),
                          m_pWidget);
    }
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    if (G_LIKELY (m_pControl != nullptr)) {
        if (m_pWidget->m_pDeviceStr == UKUI_INPUT_REAR_MIC || m_pWidget->m_pDeviceStr == UKUI_INPUT_FRONT_MIC || m_pWidget->m_pDeviceStr == UKUI_OUTPUT_HEADPH) {
            mate_mixer_stream_control_set_monitor_enabled(m_pControl,true);
        }
    }

//    m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    updateInputSettings (m_pWidget,m_pWidget->m_pInputBarStreamControl);
}

/*
    control 静音通知
*/
void UkmediaMainWidget::onStreamControlMuteNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pWidget);
    Q_UNUSED(pspec);
    g_debug("on stream control mute notifty");
    /* Stop monitoring the input stream when it gets muted */
    if (mate_mixer_stream_control_get_mute (m_pControl) == TRUE) {
//        mate_mixer_stream_control_set_monitor_enabled (m_pControl, false);
    }
    else {
        if (m_pWidget->m_pDeviceStr == UKUI_INPUT_REAR_MIC || m_pWidget->m_pDeviceStr == UKUI_INPUT_FRONT_MIC || m_pWidget->m_pDeviceStr == UKUI_OUTPUT_HEADPH) {
            mate_mixer_stream_control_set_monitor_enabled(m_pControl,true);
        }
    }
}

/*
    默认输出流通知
*/
void UkmediaMainWidget::onContextDefaultOutputStreamNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on context default output stream notify");
    MateMixerStream *m_pStream;
    const gchar *portLabel;
    const gchar *portName;
    m_pStream = mate_mixer_context_get_default_output_stream (m_pContext);

    //修改输出设备时跟随输出设备改变
    MateMixerDevice *pDevice = mate_mixer_stream_get_device(m_pStream);
    const gchar *cardName = mate_mixer_device_get_name(pDevice);

    if (!(MATE_MIXER_IS_STREAM(m_pStream))) {
        return;
    }

//    /* Enable the port selector if the stream has one */
//    MateMixerSwitch *portSwitch;
//    portSwitch = findStreamPortSwitch (m_pWidget,m_pStream);
//    //拔插耳机时设置输出端口名
//    m_pWidget->m_pOutputPortList->clear();
//    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->clear();
//    MateMixerDirection direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(m_pStream));
//    if (MATE_MIXER_IS_STREAM(m_pStream)) {
//        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
//            if (portSwitch != nullptr) {
//                const GList *options;
//                options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(portSwitch));
//                MateMixerSwitchOption *activePort = mate_mixer_switch_get_active_option(portSwitch);
//                while (options != nullptr) {
//                    portName = mate_mixer_switch_option_get_name(activePort);
//                    portLabel = mate_mixer_switch_option_get_label(activePort);
//                    MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
//                    QString label = mate_mixer_switch_option_get_label(opt);
//                    QString name = mate_mixer_switch_option_get_name(opt);
//                    if (!m_pWidget->m_pOutputPortList->contains(name)) {
//                        m_pWidget->m_pOutputPortList->append(name);
//                        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->addItem(label);
//                    }
//                    options = options->next;
//                }
//            }
//        }
//    }
//    int portIndex = m_pWidget->m_pOutputPortList->indexOf(portName);
//    if (portIndex < 0) {
//        m_pWidget->m_pOutputWidget->m_pOutputPortWidget->hide();
//        m_pWidget->m_pOutputWidget->outputWidgetRemovePort();
//        return;
//    }
//    if (m_pStream == nullptr) {
//        //当输出流更改异常时，使用默认的输入流，不应该发生这种情况
//        m_pStream = m_pWidget->m_pOutputStream;
//    }
    QString deviceName = mate_mixer_stream_get_name(m_pStream);
    int index = m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->findText(deviceName);
    qDebug() << "on context default output steam notify:" << mate_mixer_stream_get_name(m_pStream) << cardName <<index;
    if (index < 0) {
        m_pWidget->m_pOutputWidget->m_pOutputListWidget->setCurrentRow(-1);
        return;
    }

    if (index >= 0) {

        if (index < m_pWidget->m_pOutputPortLabelList->count()) {
            m_pWidget->m_pOutputPortLabelList->at(index);
            int i =0;

            for (i=0;i <m_pWidget->m_pOutputWidget->m_pOutputListWidget->count();i++) {

                QListWidgetItem *item = m_pWidget->m_pOutputWidget->m_pOutputListWidget->item(i);
                UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pWidget->m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
                //            wid->portLabel
                if(m_pWidget->m_pOutputPortLabelList->at(index) == wid->portLabel->text()) {
                    m_pWidget->m_pOutputWidget->m_pOutputListWidget->blockSignals(true);
                    m_pWidget->m_pOutputWidget->m_pOutputListWidget->setCurrentItem(item);
                    m_pWidget->m_pOutputWidget->m_pOutputListWidget->blockSignals(false);
                }
            }
        }
    }
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->setCurrentIndex(index);
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
    m_pWidget->setOutputListWidgetRow();
    m_pWidget->setDefaultstream = false;

    updateIconOutput(m_pWidget);
    setOutputStream (m_pWidget, m_pStream);
    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
//    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->setCurrentIndex(portIndex);
    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
}

/*
    移除存储control
*/
void UkmediaMainWidget::onContextStoredControlRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    Q_UNUSED(m_pName);
    g_debug("on context stored control removed");
    if (m_pWidget->m_pAppVolumeList != nullptr) {
        ukuiBarSetStream (m_pWidget, nullptr);
    }
}

/*
 * context设置属性
*/
void UkmediaMainWidget::contextSetProperty(UkmediaMainWidget *m_pWidget)//,guint prop_id,const GValue *value,GParamSpec *pspec)
{
    g_debug("context set property");
    setContext(m_pWidget,m_pWidget->m_pContext);
}

/*
    获取输入音量值
*/
int UkmediaMainWidget::getInputVolume()
{
    return m_pInputWidget->m_pIpVolumeSlider->value();
}

/*
    获取输出音量值
*/
int UkmediaMainWidget::getOutputVolume()
{
    return m_pOutputWidget->m_pOpVolumeSlider->value();
}

/*
   获取输入状态
*/
bool UkmediaMainWidget::getInputMuteStatus()
{
    MateMixerStream *pStream = mate_mixer_context_get_default_input_stream(m_pContext);
    MateMixerStreamControl *pControl = mate_mixer_stream_get_default_control(pStream);
    return mate_mixer_stream_control_get_mute(pControl);
}

/*
    获取输出状态
*/
bool UkmediaMainWidget::getOutputMuteStatus()
{
    MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(m_pContext);
    MateMixerStreamControl *pControl = mate_mixer_stream_get_default_control(pStream);
    return mate_mixer_stream_control_get_mute(pControl);
}

/*
    深色主题时输出音量图标
*/
void UkmediaMainWidget::outputVolumeDarkThemeImage(int value,bool status)
{
    QImage image;
    QColor color = QColor(0,0,0,216);
    if (mThemeName == UKUI_THEME_WHITE || mThemeName == "ukui-default") {
        color = QColor(0,0,0,216);
    }
    else if (mThemeName == UKUI_THEME_BLACK || mThemeName == "ukui-dark") {
        color = QColor(255,255,255,216);
    }
    m_pOutputWidget->m_pOutputIconBtn->mColor = color;
    if (status) {
        image = QIcon::fromTheme("audio-volume-muted-symbolic").pixmap(24,24).toImage();
    }
    else if (value <= 0) {
        image = QIcon::fromTheme("audio-volume-muted-symbolic").pixmap(24,24).toImage();
    }
    else if (value > 0 && value <= 33) {
        image = QIcon::fromTheme("audio-volume-low-symbolic").pixmap(24,24).toImage();
    }
    else if (value >33 && value <= 66) {
        image = QIcon::fromTheme("audio-volume-medium-symbolic").pixmap(24,24).toImage();
    }
    else {
        image = QIcon::fromTheme("audio-volume-high-symbolic").pixmap(24,24).toImage();
    }
    m_pOutputWidget->m_pOutputIconBtn->mImage = image;

}

/*
    输入音量图标
*/
void UkmediaMainWidget::inputVolumeDarkThemeImage(int value,bool status)
{
    QImage image;
    QColor color = QColor(0,0,0,190);
    if (mThemeName == UKUI_THEME_WHITE || mThemeName == "ukui-default") {
        color = QColor(0,0,0,190);
    }
    else if (mThemeName == UKUI_THEME_BLACK || mThemeName == "ukui-dark") {
        color = QColor(255,255,255,190);
    }
    m_pInputWidget->m_pInputIconBtn->mColor = color;
    if (status) {
        image = QIcon::fromTheme("microphone-sensitivity-muted-symbolic").pixmap(24,24).toImage();
    }
    else if (value <= 0) {
        image = QIcon::fromTheme("microphone-sensitivity-muted-symbolic").pixmap(24,24).toImage();
    }
    else if (value > 0 && value <= 33) {
        image = QIcon::fromTheme("microphone-sensitivity-low-symbolic").pixmap(24,24).toImage();
    }
    else if (value >33 && value <= 66) {
        image = QIcon::fromTheme("microphone-sensitivity-medium-symbolic").pixmap(24,24).toImage();
    }
    else {
        image = QIcon::fromTheme("microphone-sensitivity-high-symbolic").pixmap(24,24).toImage();
    }
    m_pInputWidget->m_pInputIconBtn->mImage = image;
}

/*
    更新输入音量及图标
*/
void UkmediaMainWidget::updateIconInput (UkmediaMainWidget *m_pWidget)
{
    g_debug("update icon input");
    MateMixerStream *m_pStream;
    MateMixerStreamControl *m_pControl = nullptr;
    MateMixerStreamControlFlags flags;
    const gchar *m_pAppId;
    gboolean show = FALSE;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pWidget->m_pContext);

    qDebug() << "update icon input" << mate_mixer_stream_get_name(m_pStream);
    if (!MATE_MIXER_IS_STREAM(m_pStream)) {
        return;
    }
    const GList *m_pInputs =mate_mixer_stream_list_controls(m_pStream);
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    const gchar *inputControlName = mate_mixer_stream_control_get_name(m_pControl);

    if (inputControlName != nullptr && inputControlName != "auto_null.monitor") {
        if (strstr(inputControlName,"alsa_input") || strstr(inputControlName,"3a_source") || strstr(inputControlName,"bluez_source") || strstr(inputControlName,"bt_sco_source"))
            show = true;
    }

    if (strstr(inputControlName,".monitor")) {
        m_pWidget->m_pInputWidget->m_pInputListWidget->setCurrentRow(-1);
    }

    m_pWidget->m_pStream = m_pStream;
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    bool status = mate_mixer_stream_control_get_mute(m_pControl);
    int value = volume *100 /65536.0+0.5;
    m_pWidget->m_pInputWidget->m_pIpVolumeSlider->blockSignals(true);
    m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
    m_pWidget->m_pInputWidget->m_pIpVolumeSlider->blockSignals(false);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setFocusPolicy(Qt::NoFocus);

    const QSize icon_size = QSize(24,24);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setIconSize(icon_size);
    //修改图标为深色主题图标
    m_pWidget->inputVolumeDarkThemeImage(value,status);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->repaint();
    while (m_pInputs != nullptr) {
        MateMixerStreamControl *input = MATE_MIXER_STREAM_CONTROL (m_pInputs->data);
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (input);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info (input);
            m_pAppId = mate_mixer_app_info_get_id (app_info);
            if (m_pAppId == nullptr) {
                /* A recording application which has no
                 * identifier set */
                g_debug ("Found a recording application control %s",
                         mate_mixer_stream_control_get_label (input));

                if G_UNLIKELY (m_pControl == nullptr) {
                    /* In the unlikely case when there is no
                     * default input control, use the application
                     * control for the icon */
                    m_pControl = input;
                }
                show = true;
                break;
            }
            if (strcmp (m_pAppId, "org.mate.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.gnome.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.PulseAudio.pavucontrol") != 0) {
                g_debug ("Found a recording application %s", m_pAppId);

                if G_UNLIKELY (m_pControl == nullptr)
                    m_pControl = input;

                show = true;
                break;
            }
        }
        m_pInputs = m_pInputs->next;
    }
    //当前的麦克风可用开始监听输入等级
    if (show == true) {
        mate_mixer_stream_control_set_monitor_enabled(m_pControl,true);
        g_debug ("Input icon enabled");
    }
    else {
        mate_mixer_stream_control_set_monitor_enabled(m_pControl,false);
        g_debug ("There is no recording application, input icon disabled");
    }
    streamStatusIconSetControl(m_pWidget, m_pControl);

    if (m_pControl != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }

}

/*
    更新输出音量及图标
*/
void UkmediaMainWidget::updateIconOutput(UkmediaMainWidget *m_pWidget)
{
    g_debug("update icon output");
    MateMixerStream *m_Stream;
    MateMixerStreamControl *m_pControl = nullptr;

    m_Stream = mate_mixer_context_get_default_output_stream (m_pWidget->m_pContext);
    if (m_Stream != nullptr)
        m_pControl = mate_mixer_stream_get_default_control (m_Stream);
    qDebug() << "update icon output " << mate_mixer_stream_get_name(m_Stream);
    streamStatusIconSetControl(m_pWidget, m_pControl);
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    bool status = mate_mixer_stream_control_get_mute(m_pControl);
    int value = volume *100 /65536.0+0.5;

    m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->blockSignals(true);
    m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
    m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->blockSignals(false);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setFocusPolicy(Qt::NoFocus);
//    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    const QSize icon_size = QSize(24,24);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIconSize(icon_size);
    m_pWidget->outputVolumeDarkThemeImage(value,status);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->repaint();

    gdouble balance_value = mate_mixer_stream_control_get_balance(m_pControl);
    m_pWidget->m_pOutputWidget->m_pOpBalanceSlider->setValue(balance_value*100);
//    //输出音量控制
//    //输出滑动条和音量控制
//    connect(m_pWidget->m_pOutputWidget->m_pOpVolumeSlider,&QSlider::valueChanged,[=](int value){
//        QString percent;

//        percent = QString::number(value);
//        int volume = value*65536/100;
//        mate_mixer_stream_control_set_volume(m_pControl,guint(volume));
//        if (value <= 0) {
//            mate_mixer_stream_control_set_mute(m_pControl,TRUE);
//            mate_mixer_stream_control_set_volume(m_pControl,0);
//            percent = QString::number(0);
//        }
//        bool status = mate_mixer_stream_control_get_mute(m_pControl);
//        m_pWidget->outputVolumeDarkThemeImage(value,status);
//        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->repaint();
//        mate_mixer_stream_control_set_mute(m_pControl,FALSE);
//        percent.append("%");
//        m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
//    });

    if (m_pControl != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }
}

void UkmediaMainWidget::streamStatusIconSetControl(UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("stream status icon set control");
    qDebug() << "stream status icon set control" << mate_mixer_stream_control_get_label(m_pControl);
    g_signal_connect ( G_OBJECT (m_pControl),
                      "notify::volume",
                      G_CALLBACK (onStreamControlVolumeNotify),
                      m_pWidget);
    g_signal_connect (G_OBJECT (m_pControl),
                      "notify::mute",
                      G_CALLBACK (onStreamControlMuteNotify),
                      m_pWidget);

    MateMixerStreamControlFlags flags = mate_mixer_stream_control_get_flags(m_pControl);
    if (flags & MATE_MIXER_STREAM_CONTROL_MUTE_READABLE) {
        g_signal_connect (G_OBJECT (m_pControl),
                          "notify::mute",
                          G_CALLBACK (onControlMuteNotify),
                          m_pWidget);
    }
}

/*
    静音通知
*/
void UkmediaMainWidget::onControlMuteNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on control mute notify");
    gboolean mute = mate_mixer_stream_control_get_mute (m_pControl);
    int volume = int(mate_mixer_stream_control_get_volume(m_pControl));
    volume = int(volume*100/65536.0+0.5);
    MateMixerStream *stream = mate_mixer_stream_control_get_stream(m_pControl);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);

    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        m_pWidget->outputVolumeDarkThemeImage(volume,mute);
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->repaint();
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        m_pWidget->inputVolumeDarkThemeImage(volume,mute);
        m_pWidget->m_pInputWidget->m_pInputIconBtn->repaint();
    }

}

/*
    stream control 声音通知
*/
void UkmediaMainWidget::onStreamControlVolumeNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on stream control volume notify");
    qDebug() << "on stream control volume notify" << mate_mixer_stream_control_get_name(m_pControl);
    bool status = mate_mixer_stream_control_get_mute(m_pControl);
    MateMixerStreamControlFlags flags;
    guint volume = 0;
    QString decscription;

    if (m_pControl != nullptr)
        flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags&MATE_MIXER_STREAM_CONTROL_VOLUME_READABLE) {
        volume = mate_mixer_stream_control_get_volume(m_pControl);
    }
    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(m_pWidget->m_pContext);
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(m_pWidget->m_pContext);

    decscription = mate_mixer_stream_control_get_label(m_pControl);
    MateMixerDirection direction;
    MateMixerStream *m_pStream = mate_mixer_stream_control_get_stream(m_pControl);

    if (outputStream != m_pStream && inputStream != m_pStream) {
        return;
    }

    MateMixerSwitch *portSwitch;
    MateMixerStream *stream = mate_mixer_stream_control_get_stream(m_pControl);
    /* Enable the port selector if the stream has one */
     portSwitch = findStreamPortSwitch (m_pWidget,stream);
     direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(m_pStream));

    if (MATE_MIXER_IS_STREAM(m_pStream)) {
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        /*
            if (portSwitch != nullptr) {
                const GList *options;
                options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(portSwitch));
                if (options != nullptr) {
                    //拔插耳机时设置输出端口名
                    m_pWidget->m_pOutputPortList->clear();
                    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->clear();
                }
                MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(portSwitch));
                const gchar *outputPortLabel = mate_mixer_switch_option_get_label(option);
                while (options != nullptr) {
                    MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
                    QString label = mate_mixer_switch_option_get_label(opt);
                    QString name = mate_mixer_switch_option_get_name(opt);
                    if (!m_pWidget->m_pOutputPortList->contains(name)) {
                        m_pWidget->m_pOutputPortList->append(name);
                        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->addItem(label);
                    }
                    options = options->next;
                }
                if (m_pWidget->m_privOutputPortLabel != "") {
                    if(!strcmp(m_pWidget->m_privOutputPortLabel,outputPortLabel) == 0)
                }
                m_pWidget->m_privOutputPortLabel;
                m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
                m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(outputPortLabel);
                m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
                qDebug() << "set output label" << outputPortLabel << mate_mixer_stream_control_get_name(m_pControl);
            }
        */
        }
    }
    else {
        m_pStream = m_pWidget->m_pStream;
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            /*
            portSwitch = findStreamPortSwitch (m_pWidget,m_pStream);
            mate_mixer_context_set_default_output_stream(m_pWidget->m_pContext,m_pStream);
            if (portSwitch!= nullptr) {
                MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(portSwitch));
                QString label = mate_mixer_switch_option_get_label(option);
                m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(label);
                qDebug() << "get stream correct" << mate_mixer_stream_control_get_label(m_pControl) << mate_mixer_stream_get_label(m_pStream) <<label;

            }
            */
            setOutputStream(m_pWidget,m_pStream);
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            setInputStream(m_pWidget,m_pStream);
        }
    }

    direction = mate_mixer_stream_get_direction(m_pStream);
    //设置输出滑动条的值
    int value = volume*100/65536.0 + 0.5;
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->blockSignals(true);
        m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
        m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->blockSignals(false);
        m_pWidget->outputVolumeDarkThemeImage(value,status);
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->repaint();
        QString percentStr = QString::number(value) ;
        percentStr.append("%");
        m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percentStr);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        m_pWidget->m_pInputWidget->m_pIpVolumeSlider->blockSignals(true);
        m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
        m_pWidget->m_pInputWidget->m_pIpVolumeSlider->blockSignals(false);
        QString percentStr = QString::number(value) ;
        percentStr.append("%");
        m_pWidget->m_pInputWidget->m_pIpVolumePercentLabel->setText(percentStr);
    }
}

/*
    设置平衡属性
*/
void UkmediaMainWidget::ukuiBalanceBarSetProperty(UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("ukui balance bar set property");
    ukuiBalanceBarSetControl(m_pWidget,m_pControl);
}

/*
    平衡设置control
*/
void UkmediaMainWidget::ukuiBalanceBarSetControl(UkmediaMainWidget *m_pWidget, MateMixerStreamControl *m_pControl)
{
    g_debug("ukui balance bar set control");
    g_signal_connect (G_OBJECT (m_pControl),
                      "notify::balance",
                      G_CALLBACK (onBalanceValueChanged),
                      m_pWidget);
}

/*
    平衡值改变
*/
void UkmediaMainWidget::onBalanceValueChanged (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on balance value changed");
    gdouble value = mate_mixer_stream_control_get_balance(m_pControl);
    m_pWidget->m_pOutputWidget->m_pOpBalanceSlider->setValue(value*100);
}

/*
    更新输出设置
*/
void UkmediaMainWidget::updateOutputSettings (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("update output settings");
    QString outputPortLabel;
    MateMixerStreamControlFlags flags;
    if (m_pControl == nullptr) {
        return;
    }
    if(m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->count() != 0 || m_pWidget->m_pOutputPortList->count() != 0) {
        m_pWidget->m_pOutputPortList->clear();
        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->clear();
    }

    MateMixerSwitch *portSwitch;
    flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags & MATE_MIXER_STREAM_CONTROL_CAN_BALANCE) {
        ukuiBalanceBarSetProperty(m_pWidget,m_pControl);
    }
    MateMixerStream *stream = mate_mixer_stream_control_get_stream(m_pControl);
    /* Enable the port selector if the stream has one */
    portSwitch = findStreamPortSwitch (m_pWidget,stream);
    MateMixerDirection direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(stream));
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        if (portSwitch != nullptr) {
            const GList *options;
            options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(portSwitch));
            MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(portSwitch));
            outputPortLabel = mate_mixer_switch_option_get_label(option);
            while (options != nullptr) {
                MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
                QString label = mate_mixer_switch_option_get_label(opt);
                QString name = mate_mixer_switch_option_get_name(opt);
                if (!m_pWidget->m_pOutputPortList->contains(name)) {

                    m_pWidget->m_pOutputPortList->append(name);
                    m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->addItem(label);
                }
                options = options->next;
            }
        }
    }

    if (m_pWidget->m_pOutputPortList->count() > 0) {
//        m_pWidget->m_pOutputWidget->outputWidgetAddPort();
        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(outputPortLabel);
        m_pWidget->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
    }
    //新需求不需要此接口
    /*connect(m_pWidget->m_pOutputWidget->m_pOutputPortCombobox,SIGNAL(currentIndexChanged(int)),m_pWidget,SLOT(outputPortComboxChangedSlot(int)));*/
    m_pWidget->timeSliderBlance = new QTimer(m_pWidget);
    connect(m_pWidget->timeSliderBlance,&QTimer::timeout,m_pWidget,[=](){
        if(m_pWidget->mouseReleaseStateBlance){
            int volume = m_pWidget->m_pOutputWidget->m_pOpBalanceSlider->value();
            gdouble value = volume/100.0;
            MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pWidget->m_pContext);
            MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
            mate_mixer_stream_control_set_balance(control,value);
            m_pWidget->mousePressBlance = false;
            m_pWidget->mouseReleaseStateBlance = false;
            m_pWidget->timeSliderBlance->stop();
        }
        else{
            m_pWidget->timeSliderBlance->start(100);
        }
    });
    connect(m_pWidget->m_pOutputWidget->m_pOpBalanceSlider,&UkmediaVolumeSlider::silderPressedSignal,m_pWidget,[=](){

        m_pWidget->mousePressBlance = true;
        m_pWidget->mouseReleaseStateBlance = false;

    });
    connect(m_pWidget->m_pOutputWidget->m_pOpBalanceSlider,&UkmediaVolumeSlider::silderReleaseSignal,m_pWidget,[=](){
        m_pWidget->mouseReleaseStateBlance = true;
    });
    connect(m_pWidget->m_pOutputWidget->m_pOpBalanceSlider,&QSlider::valueChanged,[=](int volume){
//        gdouble value = volume/100.0;
//        MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pWidget->m_pContext);
//        MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
//        mate_mixer_stream_control_set_balance(control,value);

        if(m_pWidget->mousePressBlance){
            if(m_pWidget->mouseReleaseStateBlance){
                m_pWidget->timeSliderBlance->stop();
                gdouble value = volume/100.0;
                MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pWidget->m_pContext);
                MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
                mate_mixer_stream_control_set_balance(control,value);
                m_pWidget->mousePressBlance = false;
                m_pWidget->mouseReleaseStateBlance = false;
            }
            else{
                m_pWidget->timeSliderBlance->start(100);
            }
        }
        else
        {
            gdouble value = volume/100.0;
            MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pWidget->m_pContext);
            MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
            mate_mixer_stream_control_set_balance(control,value);
        }
    });
}

void UkmediaMainWidget::onKeyChanged (GSettings *settings,gchar *key,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(settings);
    g_debug("on key changed");
    if (!strcmp (key, EVENT_SOUNDS_KEY) ||
        !strcmp (key, SOUND_THEME_KEY) ||
        !strcmp (key, INPUT_SOUNDS_KEY)) {
        updateTheme (m_pWidget);
    }
}

/*
    更新主题
*/
void UkmediaMainWidget::updateTheme (UkmediaMainWidget *m_pWidget)
{
    g_debug("update theme");
    char *pThemeName;
    gboolean feedBackEnabled;
    gboolean eventsEnabled;
    feedBackEnabled = g_settings_get_boolean(m_pWidget->m_pSoundSettings, INPUT_SOUNDS_KEY);
    eventsEnabled = g_settings_get_boolean(m_pWidget->m_pSoundSettings,EVENT_SOUNDS_KEY);
//    eventsEnabled = FALSE;
    if (eventsEnabled) {
        pThemeName = g_settings_get_string (m_pWidget->m_pSoundSettings, SOUND_THEME_KEY);
    } else {
        pThemeName = g_strdup (NO_SOUNDS_THEME_NAME);
    }
    //设置combox的主题
    setComboxForThemeName (m_pWidget, pThemeName);
    updateAlertsFromThemeName (m_pWidget, pThemeName);
}

/*
    设置主题名到combox
*/
void UkmediaMainWidget::setupThemeSelector (UkmediaMainWidget *m_pWidget)
{
    g_debug("setup theme selector");
    GHashTable  *hash;
    const char * const *dataDirs;
    const char *m_pDataDir;
    char *dir;
    guint i;

    /* Add the theme names and their display name to a hash table,
     * makes it easy to avoid duplicate themes */
    hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

    dataDirs = g_get_system_data_dirs ();
    for (i = 0; dataDirs[i] != nullptr; i++) {
        dir = g_build_filename (dataDirs[i], "sounds", nullptr);
        soundThemeInDir (m_pWidget,hash, dir);
    }

    m_pDataDir = g_get_user_data_dir ();
    dir = g_build_filename (m_pDataDir, "sounds", nullptr);
    soundThemeInDir (m_pWidget,hash, dir);

    /* If there isn't at least one theme, make everything
     * insensitive, LAME! */
    if (g_hash_table_size (hash) == 0) {
        g_warning ("Bad setup, install the freedesktop sound theme");
        g_hash_table_destroy (hash);
        return;
    }
    /* Add the themes to a combobox */
    g_hash_table_destroy (hash);
}

/*
    主题名所在目录
*/
void UkmediaMainWidget::soundThemeInDir (UkmediaMainWidget *m_pWidget,GHashTable *hash,const char *dir)
{
    Q_UNUSED(hash);
    g_debug("sound theme in dir");
    GDir *d;
    const char *m_pName;
    d = g_dir_open (dir, 0, nullptr);
    if (d == nullptr) {
        return;
    }
    while ((m_pName = g_dir_read_name (d)) != nullptr) {
        char *m_pDirName, *m_pIndex, *m_pIndexName;
        /* Look for directories */
        m_pDirName = g_build_filename (dir, m_pName, nullptr);
        if (g_file_test (m_pDirName, G_FILE_TEST_IS_DIR) == FALSE) {
            continue;
        }

        /* Look for index files */
        m_pIndex = g_build_filename (m_pDirName, "index.theme", nullptr);

        /* Check the name of the theme in the index.theme file */
        m_pIndexName = loadIndexThemeName (m_pIndex, nullptr);
        if (m_pIndexName == nullptr) {
            continue;
        }

        gchar * themeName = g_settings_get_string (m_pWidget->m_pSoundSettings, SOUND_THEME_KEY);
        //设置主题到combox中
        qDebug() << "sound theme in dir" << "displayname:" << m_pIndexName << "theme name:" << m_pName << "theme："<< themeName;
        //屏蔽ubuntu custom 主题
        if (!strstr(m_pName,"ubuntu") && !strstr(m_pName,"freedesktop") && !strstr(m_pName,"custom")) {
            m_pWidget->m_pThemeDisplayNameList->append(m_pIndexName);
            m_pWidget->m_pThemeNameList->append(m_pName);
            m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->addItem(m_pIndexName);

        }
    }
    g_dir_close (d);
}

/*
    加载下标的主题名
*/
char *UkmediaMainWidget::loadIndexThemeName (const char *index,char **parent)
{
    g_debug("load index theme name");
    GKeyFile *file;
    char *indexname = nullptr;
    gboolean hidden;

    file = g_key_file_new ();
    if (g_key_file_load_from_file (file, index, G_KEY_FILE_KEEP_TRANSLATIONS, nullptr) == FALSE) {
        g_key_file_free (file);
        return nullptr;
    }
    /* Don't add hidden themes to the list */
    hidden = g_key_file_get_boolean (file, "Sound Theme", "Hidden", nullptr);
    if (!hidden) {
        indexname = g_key_file_get_locale_string (file,"Sound Theme","Name",nullptr,nullptr);
        /* Save the parent theme, if there's one */
        if (parent != nullptr) {
            *parent = g_key_file_get_string (file,"Sound Theme","Inherits",nullptr);
        }
    }
    g_key_file_free (file);
    return indexname;
}

/*
    设置combox的主题名
*/
void UkmediaMainWidget::setComboxForThemeName (UkmediaMainWidget *m_pWidget,const char *name)
{
    g_debug("set combox for theme name");
    gboolean      found;
    int count = 0;
    /* If the name is empty, use "freedesktop" */
    if (name == nullptr || *name == '\0') {
        name = "freedesktop";
    }
    QString value;
    int index = -1;
    while(!found) {
        value = m_pWidget->m_pThemeNameList->at(count);
        found = (value != "" && value == name);
        count++;
        if( count >= m_pWidget->m_pThemeNameList->size() || found) {
            count = 0;
            break;
        }
    }
    if (m_pWidget->m_pThemeNameList->contains(name)) {
        index = m_pWidget->m_pThemeNameList->indexOf(name);
        value = m_pWidget->m_pThemeNameList->at(index);
        m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->setCurrentIndex(index);
    }
    /* When we can't find the theme we need to set, try to set the default
     * one "freedesktop" */
    if (found) {
    } else if (strcmp (name, "freedesktop") != 0) {//设置为默认的主题
        qDebug() << "设置为默认的主题" << "freedesktop";
        g_debug ("not found, falling back to fdo");
        setComboxForThemeName (m_pWidget, "freedesktop");
    }
}

/*
    更新报警音
*/
void UkmediaMainWidget::updateAlertsFromThemeName (UkmediaMainWidget *m_pWidget,const gchar *m_pName)
{
    g_debug("update alerts from theme name");
    if (strcmp (m_pName, CUSTOM_THEME_NAME) != 0) {
            /* reset alert to default */
        updateAlert (m_pWidget, DEFAULT_ALERT_ID);
    } else {
        int   sound_type;
        char *linkname;
        linkname = nullptr;
        sound_type = getFileType ("bell-terminal", &linkname);
        g_debug ("Found link: %s", linkname);
        if (sound_type == SOUND_TYPE_CUSTOM) {
            updateAlert (m_pWidget, linkname);
        }
    }
}

/*
    更新报警声音
*/
void UkmediaMainWidget::updateAlert (UkmediaMainWidget *pWidget,const char *alertId)
{
    Q_UNUSED(alertId)
    g_debug("update alert");
    QString themeStr;
    char *theme;
    char *parent;
    gboolean      is_custom;
    gboolean      is_default;
    gboolean add_custom = false;
    gboolean remove_custom = false;
    QString nameStr;
    int index = -1;
    /* Get the current theme's name, and set the parent */
    index = pWidget->m_pSoundWidget->m_pSoundThemeCombobox->currentIndex();
    if (index != -1) {
        themeStr = pWidget->m_pThemeNameList->at(index);
        nameStr = pWidget->m_pThemeNameList->at(index);
    }
    else {
        themeStr = "freedesktop";
        nameStr = "freedesktop";
    }
    QByteArray ba = themeStr.toLatin1();
    theme = ba.data();

    QByteArray baParent = nameStr.toLatin1();
    parent = baParent.data();

    is_custom = strcmp (theme, CUSTOM_THEME_NAME) == 0;
    is_default = strcmp (alertId, DEFAULT_ALERT_ID) == 0;

    if (! is_custom && is_default) {
        /* remove custom just in case */
        remove_custom = TRUE;
    } else if (! is_custom && ! is_default) {
        createCustomTheme (parent);
        saveAlertSounds(pWidget->m_pSoundWidget->m_pSoundThemeCombobox, alertId);
        add_custom = TRUE;
    } else if (is_custom && is_default) {
        saveAlertSounds(pWidget->m_pSoundWidget->m_pSoundThemeCombobox, alertId);
        /* after removing files check if it is empty */
        if (customThemeDirIsEmpty ()) {
            remove_custom = TRUE;
        }
    } else if (is_custom && ! is_default) {
        saveAlertSounds(pWidget->m_pSoundWidget->m_pSoundThemeCombobox, alertId);
    }

    if (add_custom) {
        setComboxForThemeName (pWidget, CUSTOM_THEME_NAME);
    } else if (remove_custom) {
        setComboxForThemeName (pWidget, parent);
    }
}

/*
    获取声音文件类型
*/
int UkmediaMainWidget::getFileType (const char *sound_name,char **linked_name)
{
    g_debug("get file type");
    char *name, *filename;
    *linked_name = nullptr;
    name = g_strdup_printf ("%s.disabled", sound_name);
    filename = customThemeDirPath (name);
    if (g_file_test (filename, G_FILE_TEST_IS_REGULAR) != FALSE) {
        return SOUND_TYPE_OFF;
    }
    /* We only check for .ogg files because those are the
     * only ones we create */
    name = g_strdup_printf ("%s.ogg", sound_name);
    filename = customThemeDirPath (name);
    g_free (name);

    if (g_file_test (filename, G_FILE_TEST_IS_SYMLINK) != FALSE) {
        *linked_name = g_file_read_link (filename, nullptr);
        g_free (filename);
        return SOUND_TYPE_CUSTOM;
    }
    g_free (filename);
    return SOUND_TYPE_BUILTIN;
}

/*
    自定义主题路径
*/
char *UkmediaMainWidget::customThemeDirPath (const char *child)
{
    g_debug("custom theme dir path");
    static char *dir = nullptr;
    const char *data_dir;

    if (dir == nullptr) {
        data_dir = g_get_user_data_dir ();
        dir = g_build_filename (data_dir, "sounds", CUSTOM_THEME_NAME, nullptr);
    }
    if (child == nullptr)
        return g_strdup (dir);

    return g_build_filename (dir, child, nullptr);
}

/*
    获取报警声音文件的路径
*/
void UkmediaMainWidget::populateModelFromDir (UkmediaMainWidget *m_pWidget,const char *dirname)//从目录查找报警声音文件
{
    g_debug("populate model from dir");
    GDir *d;
    const char *name;
    char *path;
    d = g_dir_open (dirname, 0, nullptr);
    if (d == nullptr) {
        return;
    }
    while ((name = g_dir_read_name (d)) != nullptr) {

        if (! g_str_has_suffix (name, ".xml")) {
            continue;
        }
        QString themeName = name;
        QStringList temp = themeName.split("-");
        themeName = temp.at(0);
        if (!m_pWidget->m_pSoundThemeList->contains(themeName)) {
            m_pWidget->m_pSoundThemeList->append(themeName);
            m_pWidget->m_pSoundThemeDirList->append(dirname);
            m_pWidget->m_pSoundThemeXmlNameList->append(name);
        }
        path = g_build_filename (dirname, name, nullptr);
    }
    char *pThemeName = g_settings_get_string (m_pWidget->m_pSoundSettings, SOUND_THEME_KEY);
    int themeIndex;
    if(m_pWidget->m_pSoundThemeList->contains(pThemeName)) {
         themeIndex =  m_pWidget->m_pSoundThemeList->indexOf(pThemeName);
         if (themeIndex < 0 )
             return;

    }
    else {
        themeIndex = 1;
    }

    QString dirName = m_pWidget->m_pSoundThemeDirList->at(themeIndex);
    QString xmlName = m_pWidget->m_pSoundThemeXmlNameList->at(themeIndex);
    path = g_build_filename (dirName.toLatin1().data(), xmlName.toLatin1().data(), nullptr);
    m_pWidget->m_pSoundWidget->m_pAlertSoundCombobox->blockSignals(true);
    m_pWidget->m_pSoundWidget->m_pAlertSoundCombobox->clear();
    m_pWidget->m_pSoundWidget->m_pAlertSoundCombobox->blockSignals(false);

    populateModelFromFile (m_pWidget, path);
    //初始化声音主题

    g_free (path);
    g_dir_close (d);
}

/*
    获取报警声音文件
*/
void UkmediaMainWidget::populateModelFromFile (UkmediaMainWidget *m_pWidget,const char *filename)
{
    g_debug("populate model from file");
    xmlDocPtr  doc;
    xmlNodePtr root;
    xmlNodePtr child;
    gboolean   exists;

    exists = g_file_test (filename, G_FILE_TEST_EXISTS);
    if (! exists) {
        return;
    }
    doc = xmlParseFile (filename);
    if (doc == nullptr) {
        return;
    }
    root = xmlDocGetRootElement (doc);
    for (child = root->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
                continue;
        }
        if (xmlStrcmp (child->name, GVC_SOUND_SOUND) != 0) {
                continue;
        }
        populateModelFromNode (m_pWidget, child);
    }
    xmlFreeDoc (doc);
}

/*
    从节点查找声音文件并加载到组合框中
*/
void UkmediaMainWidget::populateModelFromNode (UkmediaMainWidget *m_pWidget,xmlNodePtr node)
{
    g_debug("populate model from node");
    xmlNodePtr child;
    xmlChar   *filename;
    xmlChar   *name;

    filename = nullptr;
    name = xmlGetAndTrimNames (node);
    for (child = node->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
            continue;
        }

        if (xmlStrcmp (child->name, GVC_SOUND_FILENAME) == 0) {
            filename = xmlNodeGetContent (child);
        } else if (xmlStrcmp (child->name, GVC_SOUND_NAME) == 0) {
                /* EH? should have been trimmed */
        }
    }

    gchar * themeName = g_settings_get_string (m_pWidget->m_pSoundSettings, SOUND_THEME_KEY);

    //将找到的声音文件名设置到combox中
    if (filename != nullptr && name != nullptr) {
        m_pWidget->m_pSoundList->append((const char *)filename);
        m_pWidget->m_pSoundNameList->append((const char *)name);
        m_pWidget->m_pSoundWidget->m_pAlertSoundCombobox->addItem((char *)name);
        m_pWidget->m_pSoundWidget->m_pLagoutCombobox->addItem((char *)name);
        m_pWidget->m_pSoundWidget->m_pVolumeChangeCombobox->addItem((char *)name);
    }
    xmlFree (filename);
    xmlFree (name);
}

/* Adapted from yelp-toc-pager.c */
xmlChar *UkmediaMainWidget::xmlGetAndTrimNames (xmlNodePtr node)
{
    g_debug("xml get and trim names");
    xmlNodePtr cur;
    xmlChar *keep_lang = nullptr;
    xmlChar *value;
    int j, keep_pri = INT_MAX;
    const gchar * const * langs = g_get_language_names ();

    value = nullptr;
    for (cur = node->children; cur; cur = cur->next) {
        if (! xmlStrcmp (cur->name, GVC_SOUND_NAME)) {
            xmlChar *cur_lang = nullptr;
            int cur_pri = INT_MAX;
            cur_lang = xmlNodeGetLang (cur);

            if (cur_lang) {
                for (j = 0; langs[j]; j++) {
                    if (g_str_equal (cur_lang, langs[j])) {
                        cur_pri = j;
                        break;
                    }
                }
            } else {
                cur_pri = INT_MAX - 1;
            }

            if (cur_pri <= keep_pri) {
                if (keep_lang)
                    xmlFree (keep_lang);
                if (value)
                    xmlFree (value);

                value = xmlNodeGetContent (cur);

                keep_lang = cur_lang;
                keep_pri = cur_pri;
            } else {
                if (cur_lang)
                    xmlFree (cur_lang);
            }
        }
    }

    /* Delete all GVC_SOUND_NAME nodes */
    cur = node->children;
    while (cur) {
            xmlNodePtr p_this = cur;
            cur = cur->next;
            if (! xmlStrcmp (p_this->name, GVC_SOUND_NAME)) {
                xmlUnlinkNode (p_this);
                xmlFreeNode (p_this);
            }
    }
    return value;
}

/*
 * 播放报警声音
*/
void UkmediaMainWidget::playAlretSoundFromPath (UkmediaMainWidget *w,QString path)
{
    g_debug("play alert sound from path");

   gchar * themeName = g_settings_get_string (w->m_pSoundSettings, SOUND_THEME_KEY);

   if (strcmp (path.toLatin1().data(), DEFAULT_ALERT_ID) == 0) {
       if (themeName != NULL) {
           caPlayForWidget (w, 0,
                            CA_PROP_APPLICATION_NAME, _("Sound Preferences"),
                            CA_PROP_EVENT_ID, "bell-window-system",
                            CA_PROP_CANBERRA_XDG_THEME_NAME, themeName,
                            CA_PROP_EVENT_DESCRIPTION, _("Testing event sound"),
                            CA_PROP_CANBERRA_CACHE_CONTROL, "never",
                            CA_PROP_APPLICATION_ID, "org.mate.VolumeControl",
                 #ifdef CA_PROP_CANBERRA_ENABLE
                            CA_PROP_CANBERRA_ENABLE, "1",
                 #endif
                            NULL);
       } else {
           caPlayForWidget (w, 0,
                            CA_PROP_APPLICATION_NAME, _("Sound Preferences"),
                            CA_PROP_EVENT_ID, "bell-window-system",
                            CA_PROP_EVENT_DESCRIPTION, _("Testing event sound"),
                            CA_PROP_CANBERRA_CACHE_CONTROL, "never",
                            CA_PROP_APPLICATION_ID, "org.mate.VolumeControl",
                 #ifdef CA_PROP_CANBERRA_ENABLE
                            CA_PROP_CANBERRA_ENABLE, "1",
                 #endif
                            NULL);
       }
   } else {
       caPlayForWidget (w, 0,
                        CA_PROP_APPLICATION_NAME, _("Sound Preferences"),
                        CA_PROP_MEDIA_FILENAME, path.toLatin1().data(),
                        CA_PROP_EVENT_DESCRIPTION, _("Testing event sound"),
                        CA_PROP_CANBERRA_CACHE_CONTROL, "never",
                        CA_PROP_APPLICATION_ID, "org.mate.VolumeControl",
                 #ifdef CA_PROP_CANBERRA_ENABLE
                        CA_PROP_CANBERRA_ENABLE, "1",
                 #endif
                        NULL);
   }
}

/*
    点击combox播放声音
*/
void UkmediaMainWidget::comboxIndexChangedSlot(int index)
{
    g_debug("combox index changed slot");
    QString sound_name = m_pSoundList->at(index);
    updateAlert(this,sound_name.toLatin1().data());
    playAlretSoundFromPath(this,sound_name);

    QString fileName = m_pSoundList->at(index);
    QStringList list = fileName.split("/");
    QString soundName = list.at(list.count()-1);
    QStringList eventIdList = soundName.split(".");
    QString eventId = eventIdList.at(0);
    QList<char *> existsPath = listExistsPath();

    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
//            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();
            if (nameStr == "alert-sound") {
                settings->set(FILENAME_KEY,eventId);
                return;
            }
        }
        else {
            continue;
        }
    }

}

/*
    设置窗口关闭的提示音
*/
void UkmediaMainWidget::windowClosedComboboxChangedSlot(int index)
{
    QString fileName = m_pSoundList->at(index);
    QStringList list = fileName.split("/");
    QString soundName = list.at(list.count()-1);
    QStringList eventIdList = soundName.split(".");
    QString eventId = eventIdList.at(0);
    QList<char *> existsPath = listExistsPath();

    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
//            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();
            if (nameStr == "window-close") {
                settings->set(FILENAME_KEY,eventId);
                return;
            }
        }
        else {
            continue;
        }
    }
}

/*
    设置音量改变的提示声音
*/
void UkmediaMainWidget::volumeChangedComboboxChangeSlot(int index)
{
    QString sound_name = m_pSoundList->at(index);
//    updateAlert(this,sound_name.toLatin1().data());
    playAlretSoundFromPath(this,sound_name);

    QString fileName = m_pSoundList->at(index);
    QStringList list = fileName.split("/");
    QString soundName = list.at(list.count()-1);
    QStringList eventIdList = soundName.split(".");
    QString eventId = eventIdList.at(0);
    QList<char *> existsPath = listExistsPath();
    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);
        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
//            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();
            if (nameStr == "volume-changed") {
                settings->set(FILENAME_KEY,eventId);
                return;
            }
        }
        else {
            continue;
        }
    }
}

void UkmediaMainWidget::settingMenuComboboxChangedSlot(int index)
{
    QString fileName = m_pSoundList->at(index);
    QStringList list = fileName.split("/");
    QString soundName = list.at(list.count()-1);
    QStringList eventIdList = soundName.split(".");
    QString eventId = eventIdList.at(0);
    QList<char *> existsPath = listExistsPath();
    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);
        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
            QString nameStr = settings->get(NAME_KEY).toString();
            if (nameStr == "system-setting") {
                settings->set(FILENAME_KEY,eventId);
                return;
            }
        }
        else {
            continue;
        }
    }
}

void UkmediaMainWidget::profileComboboxChangedSlot(int index)
{
    if (index >= m_pProfileNameList->count() )
        return;
    if (index < 0)
        return;
    QString profileName = m_pProfileNameList->at(index);
    QByteArray ba = profileName.toLatin1();
    const gchar *optionName =  ba.data();
    qDebug() << "profile combox changed ****************" << index << m_pProfileNameList->count() <<"option name" <<optionName << "当前设备名" << m_pOutputWidget->m_pSelectCombobox->currentText();
    int devIndex = m_pOutputWidget->m_pSelectCombobox->currentIndex();
    QString deviceStr = m_pDeviceNameList->at(devIndex);
    QByteArray bba = deviceStr.toLatin1();
    const gchar * deviceName = bba.data();
    if (m_pSwitch == nullptr)
        qDebug() << "switch is null ===============";
    MateMixerDevice *mDevice = mate_mixer_context_get_device(m_pContext,deviceName);
//     = mate_mixer_stream_get_device(mStream);
    m_pSwitch = findDeviceProfileSwitch(this,mDevice);

    MateMixerSwitchOption *opt = mate_mixer_switch_get_option(m_pSwitch,optionName);
    mate_mixer_switch_set_active_option(m_pSwitch,opt);
}

void UkmediaMainWidget::selectComboboxChangedSlot(int index)
{
    if (index > m_pProfileNameList->count() && index < 0)
        return;
    QString deviceStr = m_pDeviceNameList->at(index);
    QByteArray ba = deviceStr.toLatin1();
    const gchar *deviceName = ba.data();
    const gchar *profileLabel = nullptr;
    const gchar *profileName = nullptr;
    const gchar *setProfileLabel = nullptr;
    MateMixerSwitchOption *activeOption;
    MateMixerDevice *pDevice = mate_mixer_context_get_device(m_pContext,deviceName);
    const GList *switches;
    switches = mate_mixer_device_list_switches (MATE_MIXER_DEVICE(pDevice));
    m_pOutputWidget->m_pProfileCombobox->clear();
    m_pProfileNameList->clear();
    while (switches != nullptr) {
        const GList *options;
        MateMixerSwitch *swtch1 = findDeviceProfileSwitch(this,pDevice);
        options = mate_mixer_switch_list_options(swtch1);
        activeOption = mate_mixer_switch_get_active_option(swtch1);
        setProfileLabel  = mate_mixer_switch_option_get_label(activeOption) ;

        while (options != NULL) {
            MateMixerSwitchOption *option = MATE_MIXER_SWITCH_OPTION (options->data);
            profileLabel = mate_mixer_switch_option_get_label (option);
            profileName = mate_mixer_switch_option_get_name(option);
            m_pProfileNameList->append(profileName);
            m_pOutputWidget->m_pProfileCombobox->addItem(profileLabel);
            /* Select the currently active option of the switch */
            options = options->next;
        }
        switches = switches->next;
    }
    if (setProfileLabel != nullptr)
        m_pOutputWidget->m_pProfileCombobox->setCurrentText(setProfileLabel);
//    qDebug() << "设置声卡配置文件为:" << setProfileLabel;
}

/*
    点击输入音量按钮静音
*/
void UkmediaMainWidget::inputMuteButtonSlot()
{
    MateMixerStreamControl *pControl;
    MateMixerStream *pStream = mate_mixer_context_get_default_input_stream(m_pContext);
    if (pStream != nullptr)
        pControl = mate_mixer_stream_get_default_control(pStream);
    int volume = int(mate_mixer_stream_control_get_volume(pControl));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(pControl);
    if (status) {
        status = false;
       mate_mixer_stream_control_set_mute(pControl,status);
    }
    else {
        status =true;
        mate_mixer_stream_control_set_mute(pControl,status);
    }
}

/*
    点击输出音量按钮静音
*/
void UkmediaMainWidget::outputMuteButtonSlot()
{
    MateMixerStreamControl *pControl;
    MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(m_pContext);
    if (pStream != nullptr)
        pControl = mate_mixer_stream_get_default_control(pStream);
    int volume = int(mate_mixer_stream_control_get_volume(pControl));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(pControl);
    if (status) {
        status = false;
        mate_mixer_stream_control_set_mute(pControl,status);
    }
    else {
        status =true;
        mate_mixer_stream_control_set_mute(pControl,status);
    }
}


/*
    点击声音主题实现主题切换
*/
void UkmediaMainWidget::themeComboxIndexChangedSlot(int index)
{
    Q_UNUSED(index);
    g_debug("theme combox index changed slot");
    if (index == -1) {
        return;
    }
    //设置系统主题
    QString theme = m_pThemeNameList->at(index);
    QByteArray ba = theme.toLatin1();
    const char *m_pThemeName = ba.data();

    if (strcmp(m_pThemeName,"freedesktop") == 0) {
        int index = 0;
        for (int i=0;i<m_pSoundList->count();i++) {
            QString str = m_pSoundList->at(i);
            if (str.contains("gudou",Qt::CaseSensitive)) {
                index = i;
                break;
            }
        }

        QString displayName = m_pSoundNameList->at(index);
        m_pSoundWidget->m_pAlertSoundCombobox->setCurrentText(displayName);
    }

    QString dirName = m_pSoundThemeDirList->at(index);
    int themeIndex =  m_pSoundThemeList->indexOf(m_pThemeName);
    if (themeIndex < 0 )
        return;
    //qDebug() << "index changed:" << m_pSoundThemeXmlNameList->at(themeIndex) << m_pThemeNameList->at(index) << m_pThemeName << dirName.toLatin1().data() ;//<< path;
    QString xmlName = m_pSoundThemeXmlNameList->at(themeIndex);
    const gchar *path = g_build_filename (dirName.toLatin1().data(), xmlName.toLatin1().data(), nullptr);
    m_pSoundList->clear();
    m_pSoundNameList->clear();
    m_pSoundWidget->m_pAlertSoundCombobox->blockSignals(true);
    m_pSoundWidget->m_pLagoutCombobox->blockSignals(true);
    m_pSoundWidget->m_pVolumeChangeCombobox->blockSignals(true);
    m_pSoundWidget->m_pAlertSoundCombobox->clear();
    m_pSoundWidget->m_pLagoutCombobox->clear();
    m_pSoundWidget->m_pVolumeChangeCombobox->clear();
    m_pSoundWidget->m_pAlertSoundCombobox->blockSignals(false);
    m_pSoundWidget->m_pLagoutCombobox->blockSignals(false);
    m_pSoundWidget->m_pVolumeChangeCombobox->blockSignals(false);
    populateModelFromFile (this, path);

    /* special case for no sounds */
    if (strcmp (m_pThemeName, NO_SOUNDS_THEME_NAME) == 0) {
        //设置提示音关闭
        g_settings_set_boolean (m_pSoundSettings, EVENT_SOUNDS_KEY, FALSE);
        return;
    } else {
        g_settings_set_boolean (m_pSoundSettings, EVENT_SOUNDS_KEY, TRUE);
    }

}

/*
    点击输出设备combox切换设备
*/
void UkmediaMainWidget::outputDeviceComboxIndexChangedSlot(QString str)
{
    MateMixerBackendFlags flags;
    setOutputListWidgetRow();
    int index = m_pOutputWidget->m_pOutputDeviceCombobox->findText(str);
    qDebug() << "outputDeviceComboxIndexChangedSlot" << str;
    if (index == -1)
        return;
    const QString str1 =  m_pOutputStreamList->at(index);
    const gchar *name = str1.toLocal8Bit();
    MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);

//    if (!MATE_MIXER_IS_STREAM(stream)) {
//        return;
//    }

//    MateMixerDevice *device = mate_mixer_stream_get_device(stream);
//    QString deviceName;
//    if (MATE_MIXER_IS_DEVICE(device))
//        deviceName= mate_mixer_device_get_name(device);
//    else {
//        QString streamName = mate_mixer_stream_get_name(stream);
//        deviceName = findOutputStreamCardName(streamName);
//    }
//    QTimer *time = new QTimer;
//    time->start(100);
//    connect(time,&QTimer::timeout,[=](){
//        int devIndex = m_pCardNameList->indexOf(deviceName);
//        qDebug() << "card name :" << m_pCardNameList->count() << "card index:" << devIndex << deviceName << "stream name " << mate_mixer_stream_get_name(stream)<< "card count" << m_pCardNameList->count();
//        if (devIndex != -1 && devIndex < m_pCardNameList->count()) {

//            findOutputListWidgetItem(m_pCardNameList->at(devIndex),stream);
//        }
//        delete time;
//    });
//    index = m_pOutputWidget->m_pOutputDeviceCombobox->currentIndex();
//    if (index >= 0) {
//        if (index < m_pOutputPortLabelList->count()) {
//            int i =0;

//            for (i=0;i < m_pOutputWidget->m_pOutputListWidget->count();i++) {

//                QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->item(i);
//                UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
//                if(m_pOutputPortLabelList->at(index) == wid->portLabel->text()) {
//                    m_pOutputWidget->m_pOutputListWidget->blockSignals(true);
//                    m_pOutputWidget->m_pOutputListWidget->setCurrentItem(item);
//                    m_pOutputWidget->m_pOutputListWidget->blockSignals(false);
//                }

//            }
//        }
//    }
    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {
//
//        if (!strstr(mate_mixer_stream_get_name(stream),"histen"))
//        if (setDefaultstream == true) {
            mate_mixer_context_set_default_output_stream (m_pContext, stream);
            qDebug() << "output device combox index changed slot" << str << index << name <<"stream name :" << mate_mixer_stream_get_name(stream);
//        }
//        else {
//            setDefaultstream = true;
//        }
        m_pStream = stream;
        MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
        int(mate_mixer_stream_control_get_volume(c) *100 /65536.0+0.5);
        /*miniWidget->masterVolumeSlider->setValue(volume);*/
    }
    else {
        setOutputStream(this, stream);
    }
}

void UkmediaMainWidget::setOutputListWidgetRow()
{
    QString str = m_pOutputWidget->m_pOutputDeviceCombobox->currentText();
    int index = m_pOutputWidget->m_pOutputDeviceCombobox->findText(str);
    if (index == -1)
        return;
    const QString str1 =  m_pOutputStreamList->at(index);
    const gchar *name = str1.toLocal8Bit();
    MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);

    if (!MATE_MIXER_IS_STREAM(stream)) {
        return;
    }

    MateMixerDevice *device = mate_mixer_stream_get_device(stream);
    QString deviceName;
    if (MATE_MIXER_IS_DEVICE(device))
        deviceName= mate_mixer_device_get_name(device);
    else {
        QString streamName = mate_mixer_stream_get_name(stream);
        deviceName = findOutputStreamCardName(streamName);
    }
    QTimer *time = new QTimer;
    time->start(100);
    connect(time,&QTimer::timeout,[=](){
        int devIndex = m_pCardNameList->indexOf(deviceName);
        qDebug() << "card name :" << m_pCardNameList->count() << "card index:" << devIndex << deviceName << "stream name " << mate_mixer_stream_get_name(stream)<< "card count" << m_pCardNameList->count();
        if (devIndex != -1 && devIndex < m_pCardNameList->count()) {

            findOutputListWidgetItem(m_pCardNameList->at(devIndex),stream);
        }
        delete time;
    });
    index = m_pOutputWidget->m_pOutputDeviceCombobox->currentIndex();
    if (index >= 0) {
        if (index < m_pOutputPortLabelList->count()) {
            int i =0;

            for (i=0;i < m_pOutputWidget->m_pOutputListWidget->count();i++) {

                QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->item(i);
                UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
                if(m_pOutputPortLabelList->at(index) == wid->portLabel->text()) {
                    m_pOutputWidget->m_pOutputListWidget->blockSignals(true);
                    m_pOutputWidget->m_pOutputListWidget->setCurrentItem(item);
                    m_pOutputWidget->m_pOutputListWidget->blockSignals(false);
                }

            }
        }
    }
}

/*
    点击输出设备combox切换
*/
void UkmediaMainWidget::inputDeviceComboxIndexChangedSlot(QString str)
{
    g_debug("input device combox index changed slot");
    MateMixerBackendFlags flags;
    int index = m_pInputWidget->m_pInputDeviceCombobox->findText(str);
    if (index == -1)
        return;
    const QString str1 =  m_pInputStreamList->at(index);
    const gchar *name = str1.toLocal8Bit();
    MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
    if (!MATE_MIXER_IS_STREAM(stream))
        return;
    MateMixerDevice *device = mate_mixer_stream_get_device(stream);
    QString deviceName;
    if (MATE_MIXER_IS_DEVICE(device))
        deviceName= mate_mixer_device_get_name(device);
    else {
        QString streamName = mate_mixer_stream_get_name(stream);
        deviceName = findInputStreamCardName(streamName);
    }

    qDebug() << "input device combox index changed slot" << str << index << deviceName;
    QTimer *time = new QTimer;
    time->start(100);
    connect(time,&QTimer::timeout,[=](){
        int devIndex = m_pInputCardNameList->indexOf(deviceName);
        if (devIndex != -1 && devIndex < m_pInputCardNameList->count()) {
            findInputListWidgetItem(m_pInputCardNameList->at(devIndex),stream);
        }
        delete time;
    });
    index = m_pInputWidget->m_pInputDeviceCombobox->currentIndex();
    if (index >= 0) {
        if (index < m_pInputPortLabelList->count()) {
            int i =0;

            for (i=0;i < m_pInputWidget->m_pInputListWidget->count();i++) {

                QListWidgetItem *item = m_pInputWidget->m_pInputListWidget->item(i);
                UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pInputWidget->m_pInputListWidget->itemWidget(item);
                if(m_pInputPortLabelList->at(index) == wid->portLabel->text()) {
                    m_pInputWidget->m_pInputListWidget->blockSignals(true);
                    m_pInputWidget->m_pInputListWidget->setCurrentItem(item);
                    m_pInputWidget->m_pInputListWidget->blockSignals(false);
                    if (wid->deviceLabel->text().contains("bluez_card"))
                        isCheckBluetoothInput = true;
                }

            }
        }
    }

    if (G_UNLIKELY (stream == nullptr)) {
       g_warn_if_reached ();
       return;
    }

    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_INPUT_STREAM) {
        m_pStream = stream;
        mate_mixer_context_set_default_input_stream (m_pContext, stream);
        MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
    }
    else {
        setInputStream(this, stream);
    }
}

void UkmediaMainWidget::setOutputStream (UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream)
{
    g_debug("set output stream");

    int i = 0;
    if (m_pStream == nullptr) {
        return;
    }
    MateMixerStreamControl *m_pControl;
    ukuiBarSetStream(m_pWidget,m_pStream);
    if (m_pStream != nullptr) {
        const GList *controls;
        controls = mate_mixer_context_list_stored_controls (m_pWidget->m_pContext);
        if (controls == nullptr) {
            return;
        }
        /* Move all stored controls to the newly selected default stream */
        while (controls != nullptr) {
            MateMixerStream        *parent;
            MateMixerStreamControl *m_pControl;
            m_pControl = MATE_MIXER_STREAM_CONTROL (controls->data);
            parent  = mate_mixer_stream_control_get_stream (m_pControl);

            /* Prefer streamless controls to stay the way they are, forcing them to
            * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != m_pStream) {
                MateMixerDirection direction = mate_mixer_stream_get_direction (parent);

                if (direction == MATE_MIXER_DIRECTION_OUTPUT)
                    mate_mixer_stream_control_set_stream (m_pControl, m_pStream);
            }
            controls = controls->next;
        }
    }
    updateOutputStreamList (m_pWidget, m_pStream);
    if (m_pControl == nullptr) {
        return;
    }
    updateOutputSettings(m_pWidget,m_pWidget->m_pOutputBarStreamControl);
}

/*
    更新输出stream 列表
*/
void UkmediaMainWidget::updateOutputStreamList(UkmediaMainWidget *m_pWidget,MateMixerStream *m_pStream)
{
    Q_UNUSED(m_pWidget);
    g_debug("update output stream list");
    const gchar *m_pName = nullptr;
    if (m_pStream != nullptr) {
        m_pName = mate_mixer_stream_get_name(m_pStream);
    }
}

/*
    bar设置stream
*/
void UkmediaMainWidget::ukuiBarSetStream (UkmediaMainWidget  *w,MateMixerStream *m_pStream)
{
    g_debug("ukui bar set stream");
    MateMixerStreamControl *m_pControl = nullptr;

    if (m_pStream != nullptr)
        m_pControl = mate_mixer_stream_get_default_control (m_pStream);
    MateMixerDirection direction = mate_mixer_stream_get_direction(m_pStream);
    ukuiBarSetStreamControl (w,direction,m_pControl);
}

void UkmediaMainWidget::ukuiBarSetStreamControl (UkmediaMainWidget *m_pWidget,MateMixerDirection direction,MateMixerStreamControl *m_pControl)
{
    Q_UNUSED(m_pWidget);
    g_debug("ukui bar set stream control");
    const gchar *m_pName;
    if (m_pControl != nullptr) {
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            m_pWidget->m_pOutputBarStreamControl = m_pControl;
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            m_pWidget->m_pInputBarStreamControl = m_pControl;
        }
        m_pName = mate_mixer_stream_control_get_name (m_pControl);
    }
}


void UkmediaMainWidget::ukuiInputLevelSetProperty (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui input level set property");
    scale = GVC_LEVEL_SCALE_LINEAR;
    ukuiInputLevelSetScale (m_pWidget, m_pWidget->scale);
}

void UkmediaMainWidget::ukuiInputLevelSetScale (UkmediaMainWidget *m_pWidget, LevelScale scale)
{
    g_debug("ukui input level set scale");
    if (scale != m_pWidget->scale) {
        ukuiUpdatePeakValue (m_pWidget);
    }
}

void UkmediaMainWidget::ukuiUpdatePeakValue (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui update peak value");
    gdouble value = ukuiFractionFromAdjustment(m_pWidget);
    m_pWidget->peakFraction = value;

    if (value > m_pWidget->maxPeak) {
        if (m_pWidget->maxPeakId > 0)
            g_source_remove (m_pWidget->maxPeakId);
        m_pWidget->maxPeak = value;
    }
}

/*
    滚动输出音量滑动条
*/
void UkmediaMainWidget::outputWidgetSliderChangedSlot(int value)
{
    if(mousePress){
        if(mouseReleaseState){
            MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(m_pContext);
            MateMixerStreamControl *pControl;
            if (pStream != nullptr)
                 pControl = mate_mixer_stream_get_default_control(pStream);
            else {
                return;
            }
            QString percent;
            bool status = false;
            percent = QString::number(value);
            int volume = value*65536/100;

            mate_mixer_stream_control_set_volume(pControl,guint(volume));
            if (value <= 0) {
                status = true;
                mate_mixer_stream_control_set_mute(pControl,status);
        //        mate_mixer_stream_control_set_volume(m_pControl,0);
                percent = QString::number(0);
            }
            else {
                if (firstEnterSystem) {
                    bool status = mate_mixer_stream_control_get_mute(pControl);
                    mate_mixer_stream_control_set_mute(pControl,status);
                }
                else {
                    mate_mixer_stream_control_set_mute(pControl,status);
                }
            }
            firstEnterSystem = false;
            outputVolumeDarkThemeImage(value,status);
            percent.append("%");
            m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
            m_pOutputWidget->m_pOutputIconBtn->repaint();

            mousePress = false;
            mouseReleaseState = false;
        }
        else{
            timeSlider->start(50);
        }
    }
    else{
        MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(m_pContext);
        MateMixerStreamControl *pControl;
        if (pStream != nullptr)
             pControl = mate_mixer_stream_get_default_control(pStream);
        else {
            return;
        }
        QString percent;
        bool status = false;
        percent = QString::number(value);
        int volume = value*65536/100;

        mate_mixer_stream_control_set_volume(pControl,guint(volume));
        if (value <= 0) {
            status = true;
            mate_mixer_stream_control_set_mute(pControl,status);
    //        mate_mixer_stream_control_set_volume(m_pControl,0);
            percent = QString::number(0);
        }
        else {
            if (firstEnterSystem) {
                bool status = mate_mixer_stream_control_get_mute(pControl);
                mate_mixer_stream_control_set_mute(pControl,status);
            }
            else {
                mate_mixer_stream_control_set_mute(pControl,status);
            }
        }
        firstEnterSystem = false;
        outputVolumeDarkThemeImage(value,status);
        percent.append("%");
        m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
        m_pOutputWidget->m_pOutputIconBtn->repaint();
    }

}

void UkmediaMainWidget::timeSliderSlot()
{
    if(mouseReleaseState){
        int value = m_pOutputWidget->m_pOpVolumeSlider->value();
        MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(m_pContext);
        MateMixerStreamControl *pControl;
        if (pStream != nullptr)
             pControl = mate_mixer_stream_get_default_control(pStream);
        else {
            return;
        }
        QString percent;
        bool status = false;
        percent = QString::number(value);
        int volume = value*65536/100;

        mate_mixer_stream_control_set_volume(pControl,guint(volume));
        if (value <= 0) {
            status = true;
            mate_mixer_stream_control_set_mute(pControl,status);
    //        mate_mixer_stream_control_set_volume(m_pControl,0);
            percent = QString::number(0);
        }
        else {
            if (firstEnterSystem) {
                bool status = mate_mixer_stream_control_get_mute(pControl);
                mate_mixer_stream_control_set_mute(pControl,status);
            }
            else {
                mate_mixer_stream_control_set_mute(pControl,status);
            }
        }
        firstEnterSystem = false;
        outputVolumeDarkThemeImage(value,status);
        percent.append("%");
        m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
        m_pOutputWidget->m_pOutputIconBtn->repaint();

        mouseReleaseState = false;
        mousePress = false;
        timeSlider->stop();
    }
    else{
        timeSlider->start(50);
    }
}

/*
    滚动输入滑动条
*/
void UkmediaMainWidget::inputWidgetSliderChangedSlot(int value)
{
    MateMixerStream *pStream = mate_mixer_context_get_default_input_stream(m_pContext);
    MateMixerStreamControl *pControl = mate_mixer_stream_get_default_control(pStream);

    QString percent;
    bool status = false;
    if (value <= 0) {
        status = true;
        mate_mixer_stream_control_set_mute(pControl,status);
        mate_mixer_stream_control_set_volume(pControl,0);
        percent = QString::number(0);
    }
    //输入图标修改成深色主题

    inputVolumeDarkThemeImage(value,status);
    m_pInputWidget->m_pInputIconBtn->repaint();
    percent = QString::number(value);
    value = value * 65536 / 100;
    mate_mixer_stream_control_set_mute(pControl,status);
    mate_mixer_stream_control_set_volume(pControl,value);
    percent.append("%");
    m_pInputWidget->m_pInputIconBtn->repaint();
    m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
}

/*
    设置提示音大小的值
*/
void UkmediaMainWidget::alertVolumeSliderChangedSlot(int value)
{
    if (m_pMediaRoleControl != nullptr) {
        mate_mixer_stream_control_set_volume(m_pMediaRoleControl,value*65535/100);
        this->m_pSoundWidget->m_pAlertVolumeLabel->setText(QString::number(value).append("%"));

        alertIconButtonSetIcon(false,value);
        m_pSoundWidget->m_pAlertIconBtn->repaint();
    }
    else {
        volume.channels = 1;
        volume.values[0] = value*65536/100;
        info.volume = volume;
        updateRole(info);
    }
}

/*
    提示音静音设置
*/
void UkmediaMainWidget::alertSoundVolumeChangedSlot()
{
    bool states = mate_mixer_stream_control_get_mute(m_pMediaRoleControl);
    int volume = m_pSoundWidget->m_pAlertSlider->value();

    mate_mixer_stream_control_set_mute(m_pMediaRoleControl,!states);
    alertIconButtonSetIcon(!states,volume);
    m_pSoundWidget->m_pAlertIconBtn->repaint();
}

void UkmediaMainWidget::outputListWidgetCurrentRowChangedSlot(int row)
{
    //当所有可用的输出设备全部移除，台式机才会出现该情况
    if (row == -1)
        return;
    qDebug() << "outputListWidgetCurrentRowChangedSlot" << row;
    QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->item(row);
    if (item == nullptr) {
        qDebug() <<"output current item is null";
    }
    UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
    QListWidgetItem *inputCurrrentItem = m_pInputWidget->m_pInputListWidget->currentItem();
    UkuiListWidgetItem *inputWid = (UkuiListWidgetItem *)m_pInputWidget->m_pInputListWidget->itemWidget(inputCurrrentItem);

    bool isContainBlue = inputCardListContainBluetooth();
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pContext);
    const gchar *streamName = mate_mixer_stream_get_name(stream);
    //当输出设备从蓝牙切换到其他设备时，需将蓝牙声卡的配置文件切换为a2dp-sink
    if (isContainBlue && (strstr(streamName,"headset_head_unit") || strstr(streamName,"bt_sco_sink"))) {
        QString cardName = blueCardName();
        QString cmd = "pactl set-card-profile "+cardName+" a2dp_sink";

        system(cmd.toLocal8Bit().data());
    }

    QMap<QString,QString>::iterator it;
    QMap<QString,QString>::iterator inputProfileMap;
    QString endOutputProfile = "";
    QString endInputProfile = "";
    int count,i;
    for (it=profileNameMap.begin(),i=0;it!= profileNameMap.end();++i) {
        if (it.key() == wid->portLabel->text()) {
            count = i;
            endOutputProfile = it.value();
        }

        ++it;
    }

    if (inputCurrrentItem != nullptr) {

        for (inputProfileMap=inputPortProfileNameMap.begin(),count=0;inputProfileMap!= inputPortProfileNameMap.end();count++) {
            if (inputProfileMap.key() == inputWid->portLabel->text()) {
                 endInputProfile = inputProfileMap.value();
            }
            if (count == inputPortProfileNameMap.count()-1) {
            }
            ++inputProfileMap;
        }
    }
    //如果选择的输入输出设备为同一个声卡，则追加指定输入输出端口属于的配置文件
    if (inputCurrrentItem != nullptr && wid->deviceLabel->text() == inputWid->deviceLabel->text()) {

        QString  setProfile = "pactl set-card-profile ";
        setProfile += wid->deviceLabel->text();
        setProfile += " ";
        setProfile += endOutputProfile;
        setProfile += "+";
        setProfile +=endInputProfile;
        system(setProfile.toLocal8Bit().data());
        QString deviceLabel = wid->deviceLabel->text();
        QTimer *time = new QTimer;
        time->start(100);
        connect(time,&QTimer::timeout,[=](){
            int index = m_pCardNameList->indexOf(deviceLabel);
            if (index != -1) {
                const QString str1 =  m_pOutputStreamList->at(index);
                const gchar *name = str1.toLocal8Bit();
                MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
//                mate_mixer_context_set_default_output_stream (m_pContext, stream);
//                m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
                m_pOutputWidget->m_pOutputDeviceCombobox->setCurrentIndex(index);
//                m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
            }
            delete time;
        });
    }
    //如果选择的输入输出设备不是同一块声卡，需要设置一个优先级高的配置文件
    else {
        int index = findCardIndex(wid->deviceLabel->text());
        QMap <int,QList<QString>>::iterator it;
        QString profileName;
        for(it=cardProfileMap.begin();it!=cardProfileMap.end();) {

            if (it.key() == index) {
                QStringList list= it.value();
                profileName = findHighPriorityProfile(index,endOutputProfile);
                if (list.contains(endOutputProfile)) {

                }
            }
            ++it;
        }
        if (isContainBlue == false && (endOutputProfile == "headset_head_unit" || endOutputProfile == "a2dp_sink"))
            profileName = "a2dp_sink";
        else if (isContainBlue == true && (endOutputProfile == "headset_head_unit" || endOutputProfile == "a2dp_sink"))
            profileName = "headset_head_unit";
        QString  setProfile = "pactl set-card-profile ";
        setProfile += wid->deviceLabel->text();
        setProfile += " ";
        setProfile += profileName;
        system(setProfile.toLocal8Bit().data());
        QTimer *time = new QTimer;
        time->start(100);
        QString deviceLabel = wid->deviceLabel->text();
        connect(time,&QTimer::timeout,[=](){
            m_pOutputWidget->m_pOutputDeviceCombobox->update();
            int index = m_pCardNameList->indexOf(deviceLabel);
            if (index >= 0) {
                const QString str1 =  m_pOutputStreamList->at(index);
                const gchar *name = str1.toLocal8Bit();
                MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
//                mate_mixer_context_set_default_output_stream (m_pContext, stream);
//                m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(true);
                m_pOutputWidget->m_pOutputDeviceCombobox->setCurrentIndex(index);
//                m_pOutputWidget->m_pOutputDeviceCombobox->blockSignals(false);
            }
            delete time;
        });
    }
    qDebug() << "active output port:" << wid->portLabel->text();
}

void UkmediaMainWidget::inputListWidgetCurrentRowChangedSlot(int row)
{
    //当所有可用的输入设备全部移除，台式机才会出现该情况
    qDebug() << "inputListWidgetCurrentRowChangedSlot" << row;
    if (row == -1)
        return;
    QListWidgetItem *item = m_pInputWidget->m_pInputListWidget->item(row);
    UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pInputWidget->m_pInputListWidget->itemWidget(item);
    QListWidgetItem *outputCurrrentItem = m_pOutputWidget->m_pOutputListWidget->currentItem();
    UkuiListWidgetItem *outputWid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(outputCurrrentItem);

    bool isContainBlue = inputCardListContainBluetooth();
    MateMixerStream *stream = mate_mixer_context_get_default_input_stream(m_pContext);
    const gchar *streamName = mate_mixer_stream_get_name(stream);
    //当输出设备从蓝牙切换到其他设备时，需将蓝牙声卡的配置文件切换为a2dp-sink
    if (isContainBlue && (strstr(streamName,"headset_head_unit") || strstr(streamName,"bt_sco_source"))) {
        QString cardName = blueCardName();
        QString cmd = "pactl set-card-profile "+cardName+" a2dp_sink";
        system(cmd.toLocal8Bit().data());
    }

    if(wid->deviceLabel->text().contains("bluez_card")) {
        isCheckBluetoothInput = true;
    }
    else {
        isCheckBluetoothInput = false;
    }

    QMap<QString,QString>::iterator it;
    QString endOutputProfile = "";
    QString endInputProfile = "";
    int count,i;
    for (it=inputPortProfileNameMap.begin(),i=0;it!= inputPortProfileNameMap.end();++i) {
        if (it.key() == wid->portLabel->text()) {
            count = i;
            endInputProfile = it.value();
        }
        ++it;
    }
    if (outputCurrrentItem != nullptr) {
        for (it=profileNameMap.begin(),count=0;it!= profileNameMap.end();count++) {
            if (it.key() == outputWid->portLabel->text()) {
                 endOutputProfile = it.value();
            }
            if (count == profileNameMap.count()-1) {
            }
            ++it;
        }
    }
    //如果选择的输入输出设备为同一个声卡，则追加指定输入输出端口属于的配置文件
    if (outputCurrrentItem != nullptr && wid->deviceLabel->text() == outputWid->deviceLabel->text()) {

        QString  setProfile = "pactl set-card-profile ";
        setProfile += wid->deviceLabel->text();
        setProfile += " ";
        if (endOutputProfile == "a2dp-sink" || endInputProfile == "headset_head_unit") {
            setProfile += endInputProfile;
        }
        else {
            setProfile += endOutputProfile;
            setProfile += "+";
            setProfile +=endInputProfile;
        }

        QString deviceLabel = wid->deviceLabel->text();
        m_pInputWidget->m_pInputDeviceCombobox->blockSignals(true);
        system(setProfile.toLocal8Bit().data());
        m_pInputWidget->m_pInputDeviceCombobox->blockSignals(false);
        QTimer *time = new QTimer;
        time->start(100);
        connect(time,&QTimer::timeout,[=](){
            int index = m_pInputCardNameList->indexOf(deviceLabel);
            if (index != -1) {
                const QString str1 =  m_pInputStreamList->at(index);
                const gchar *name = str1.toLocal8Bit();
                MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
                mate_mixer_context_set_default_input_stream (m_pContext, stream);
//                m_pInputWidget->m_pInputDeviceCombobox->setCurrentIndex(index);
            }
            delete time;
        });
    }
    //如果选择的输入输出设备不是同一块声卡，需要设置一个优先级高的配置文件
    else {
        int index = findCardIndex(wid->deviceLabel->text());
        QMap <int,QList<QString>>::iterator it;
        QString profileName;
        for(it=cardProfileMap.begin();it!=cardProfileMap.end();) {

            if (it.key() == index) {
                QStringList list= it.value();
                profileName = findHighPriorityProfile(index,endInputProfile);
                if (list.contains(endOutputProfile)) {

                }
            }
            ++it;
        }
        QString  setProfile = "pactl set-card-profile ";
        QString deviceLabel = wid->deviceLabel->text();
        setProfile += wid->deviceLabel->text();
        setProfile += " ";
        setProfile += profileName;
//        m_pInputWidget->m_pInputDeviceCombobox->blockSignals(true);
        system(setProfile.toLocal8Bit().data());
        QTimer *time = new QTimer;
        time->start(100);
        connect(time,&QTimer::timeout,[=](){
//            m_pInputWidget->m_pInputDeviceCombobox->blockSignals(false);
            int index = m_pInputCardNameList->indexOf(deviceLabel);
            if (index >= 0)  {
                const QString str1 =  m_pInputStreamList->at(index);
                const gchar *name = str1.toLocal8Bit();
                MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
                mate_mixer_context_set_default_input_stream (m_pContext, stream);
//                m_pInputWidget->m_pInputDeviceCombobox->setCurrentIndex(index);
            }
            delete time;
        });
    }
    qDebug() << "active input port:" << wid->portLabel->text() << isCheckBluetoothInput;
}

void UkmediaMainWidget::inputPortComboxChangedSlot(int index)
{
    if (index < 0 || index >= m_pInputPortList->count())
        return;
    QString portStr = m_pInputPortList->at(index);
    QByteArray ba = portStr.toLatin1();
    const char *portName = ba.data();
    MateMixerStream *stream = mate_mixer_context_get_default_input_stream(m_pContext);
    MateMixerSwitch *portSwitch = findStreamPortSwitch (this,stream);
    if (portSwitch != nullptr) {

        MateMixerSwitchOption *opt = mate_mixer_switch_get_option(portSwitch,portName);
        mate_mixer_switch_set_active_option(MATE_MIXER_SWITCH(portSwitch),opt);
    }
}

void UkmediaMainWidget::outputPortComboxChangedSlot(int index)
{
    if (index < 0)
        return;
    QString portStr = m_pOutputPortList->at(index);
    QByteArray ba = portStr.toLatin1();
    const char *portName = ba.data();
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(m_pContext);
    MateMixerSwitch *portSwitch = findStreamPortSwitch (this,stream);
    if (portSwitch != nullptr) {

        MateMixerSwitchOption *opt = mate_mixer_switch_get_option(portSwitch,portName);
        m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
        mate_mixer_switch_set_active_option(MATE_MIXER_SWITCH(portSwitch),opt);
        m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
    }
}

void UkmediaMainWidget::inputLevelValueChangedSlot()
{
    g_debug("input level value changed slot");
    ukuiUpdatePeakValue(this);
}

gdouble UkmediaMainWidget::ukuiFractionFromAdjustment (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui fraction from adjustment");
    gdouble level;
    gdouble fraction = 0.0;
    gdouble min;
    gdouble max;

    level = m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->value();
    min = m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->minimum();
    max = m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->maximum();

    switch (m_pWidget->scale) {
    case GVC_LEVEL_SCALE_LINEAR:
            fraction = (level - min) / (max - min);
            break;
    case GVC_LEVEL_SCALE_LOG:
            fraction = log10 ((level - min + 1) / (max - min + 1));
            break;
    }
    return fraction;
}

/*
    更新输入设置w
*/
void UkmediaMainWidget::updateInputSettings (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug ("updating input settings");
    MateMixerStream            *stream;
    MateMixerStreamControlFlags flags;
    MateMixerSwitch            *portSwitch;
    /* Get the control currently associated with the input slider */
    if (m_pControl == nullptr)
        return;
    /* Get owning stream of the control */
    qDebug() << "control name is :" << mate_mixer_stream_control_get_label(m_pControl) << mate_mixer_stream_control_get_name(m_pControl);
    stream = mate_mixer_stream_control_get_stream (m_pControl);
    if (G_UNLIKELY (stream == nullptr))
        return;

    if(m_pWidget->m_pInputWidget->m_pInputPortCombobox->count() != 0 || m_pWidget->m_pInputPortList->count() != 0) {
        m_pWidget->m_pInputPortList->clear();
        m_pWidget->m_pInputWidget->m_pInputPortCombobox->clear();
//        m_pWidget->m_pInputWidget->inputWidgetRemovePort();
    }

    flags = mate_mixer_stream_control_get_flags (m_pControl);
    /* Enable level bar only if supported by the control */
    if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
        m_pWidget->m_pPrivInputControl = m_pControl;
        g_signal_connect (G_OBJECT (m_pControl),
                          "monitor-value",
                          G_CALLBACK (onStreamControlMonitorValue),
                          m_pWidget);
    }

    /* Enable the port selector if the stream has one */
    portSwitch = findStreamPortSwitch (m_pWidget,stream);
    if (portSwitch != nullptr) {
        const GList *options;
        options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(portSwitch));
        while (options != nullptr) {
            MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
            QString label = mate_mixer_switch_option_get_label(opt);
            QString name = mate_mixer_switch_option_get_name(opt);
//            m_pWidget->m_pInputPortList->append(name);
            m_pWidget->m_pInputWidget->m_pInputPortCombobox->addItem(label);
            options = options->next;
        }
        MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(portSwitch));
        QString label = mate_mixer_switch_option_get_label(option);
        if (m_pWidget->m_pInputPortList->count() > 0) {
//            m_pWidget->m_pInputWidget->inputWidgetAddPort();
            m_pWidget->m_pInputWidget->m_pInputPortCombobox->setCurrentText(label);
        }
        connect(m_pWidget->m_pInputWidget->m_pInputPortCombobox,SIGNAL(currentIndexChanged(int)),m_pWidget,SLOT(inputPortComboxChangedSlot(int)));
    }

}

MateMixerSwitch* UkmediaMainWidget::findStreamPortSwitch (UkmediaMainWidget *widget,MateMixerStream *stream)
{
    const GList *switches;
//    stream = mate_mixer_context_get_default_input_stream(widget->m_pContext);
    switches = mate_mixer_stream_list_switches (stream);
    while (switches != nullptr) {
        MateMixerStreamSwitch *swtch = MATE_MIXER_STREAM_SWITCH (switches->data);
        if (!MATE_MIXER_IS_STREAM_TOGGLE (swtch) &&
                mate_mixer_stream_switch_get_role (swtch) == MATE_MIXER_STREAM_SWITCH_ROLE_PORT) {
            return MATE_MIXER_SWITCH (swtch);
        }
        switches = switches->next;
    }
    return NULL;
}

void UkmediaMainWidget::onStreamControlMonitorValue (MateMixerStream *m_pStream,gdouble value,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    g_debug("on stream control monitor value");
    value = value*100;
    if (value >= 0) {
        m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->setValue(value);
    }
    else {
        m_pWidget->m_pInputWidget->m_pInputLevelProgressBar->setValue(0);
    }
}

/*
    输入stream control add
*/
void UkmediaMainWidget::onInputStreamControlAdded (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on input stream control added");
    MateMixerStreamControl *m_pControl;
    m_pControl = mate_mixer_stream_get_control (m_pStream, m_pName);
    if G_LIKELY (m_pControl != nullptr) {
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (m_pControl);

        /* Non-application input control doesn't affect the icon */
        if (role != MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            return;
        }
    }

    /* Either an application control has been added or we couldn't
     * read the control, this shouldn't happen but let's revalidate the
     * icon to be sure if it does */
    updateIconInput (m_pWidget);
}

/*
    输入stream control removed
*/
void UkmediaMainWidget::onInputStreamControlRemoved (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    Q_UNUSED(m_pName);
    g_debug("on input stream control removed");
    updateIconInput (m_pWidget);
}

/*
    更新默认的输入stream
*/
gboolean UkmediaMainWidget::updateDefaultInputStream (UkmediaMainWidget *m_pWidget)
{
    g_debug("update default input stream");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pWidget->m_pContext);

    m_pWidget->m_pInput = (m_pStream == nullptr) ? nullptr : m_pStream;
    if (m_pWidget->m_pInput != nullptr) {
        g_signal_connect (G_OBJECT (m_pWidget->m_pInput),
                          "control-added",
                          G_CALLBACK (onInputStreamControlAdded),
                          m_pWidget);
        g_signal_connect (G_OBJECT (m_pWidget->m_pInput),
                          "control-removed",
                          G_CALLBACK (onInputStreamControlRemoved),
                          m_pWidget);
    }

    /* Return TRUE if the default input stream has changed */
    return TRUE;
}

gboolean UkmediaMainWidget::saveAlertSounds (QComboBox *combox,const char *id)
{
    const char *sounds[3] = { "bell-terminal", "bell-window-system", NULL };
    char *path;

    if (strcmp (id, DEFAULT_ALERT_ID) == 0) {
        deleteOldFiles (sounds);
        deleteDisabledFiles (sounds);
    } else {
        deleteOldFiles (sounds);
        deleteDisabledFiles (sounds);
        addCustomFile (sounds, id);
    }

    /* And poke the directory so the theme gets updated */
    path = customThemeDirPath(NULL);
    if (utime (path, NULL) != 0) {
        g_warning ("Failed to update mtime for directory '%s': %s",
                   path, g_strerror (errno));
    }
    g_free (path);

    return FALSE;
}

void UkmediaMainWidget::deleteOldFiles (const char **sounds)
{
    guint i;
    for (i = 0; sounds[i] != NULL; i++) {
        deleteOneFile (sounds[i], "%s.ogg");
    }
}

void UkmediaMainWidget::deleteOneFile (const char *sound_name, const char *pattern)
{
        GFile *file;
        char *name, *filename;

        name = g_strdup_printf (pattern, sound_name);
        filename = customThemeDirPath(name);
        g_free (name);
        file = g_file_new_for_path (filename);
        g_free (filename);
        cappletFileDeleteRecursive (file, NULL);
        g_object_unref (file);
}


void UkmediaMainWidget::deleteDisabledFiles (const char **sounds)
{
    guint i;
    for (i = 0; sounds[i] != NULL; i++) {
        deleteOneFile (sounds[i], "%s.disabled");
    }
}

void UkmediaMainWidget::addCustomFile (const char **sounds, const char *filename)
{
    guint i;

    for (i = 0; sounds[i] != NULL; i++) {
        GFile *file;
        char *name, *path;

        /* We use *.ogg because it's the first type of file that
                 * libcanberra looks at */
        name = g_strdup_printf ("%s.ogg", sounds[i]);
        path = customThemeDirPath(name);
        g_free (name);
        /* In case there's already a link there, delete it */
        g_unlink (path);
        file = g_file_new_for_path (path);
        g_free (path);

        /* Create the link */
        g_file_make_symbolic_link (file, filename, NULL, NULL);
        g_object_unref (file);
    }
}

/**
 * capplet_file_delete_recursive :
 * @file :
 * @error  :
 *
 * A utility routine to delete files and/or directories,
 * including non-empty directories.
 **/
gboolean UkmediaMainWidget::cappletFileDeleteRecursive (GFile *file, GError **error)
{
    GFileInfo *info;
    GFileType type;

    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    info = g_file_query_info (file,
                              G_FILE_ATTRIBUTE_STANDARD_TYPE,
                              G_FILE_QUERY_INFO_NONE,
                              NULL, error);
    if (info == NULL) {
        return FALSE;
    }

    type = g_file_info_get_file_type (info);
    g_object_unref (info);

    if (type == G_FILE_TYPE_DIRECTORY) {
        return directoryDeleteRecursive (file, error);
    }
    else {
        return g_file_delete (file, NULL, error);
    }
}

gboolean UkmediaMainWidget::directoryDeleteRecursive (GFile *directory, GError **error)
{
    GFileEnumerator *enumerator;
    GFileInfo *info;
    gboolean success = TRUE;

    enumerator = g_file_enumerate_children (directory,
                                            G_FILE_ATTRIBUTE_STANDARD_NAME ","
                                            G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                            G_FILE_QUERY_INFO_NONE,
                                            NULL, error);
    if (enumerator == NULL)
        return FALSE;

    while (success &&
           (info = g_file_enumerator_next_file (enumerator, NULL, NULL))) {
        GFile *child;

        child = g_file_get_child (directory, g_file_info_get_name (info));

        if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY) {
            success = directoryDeleteRecursive (child, error);
        }
        g_object_unref (info);

        if (success)
            success = g_file_delete (child, NULL, error);
    }
    g_file_enumerator_close (enumerator, NULL, NULL);

    if (success)
        success = g_file_delete (directory, NULL, error);

    return success;
}

void UkmediaMainWidget::createCustomTheme (const char *parent)
{
    GKeyFile *keyfile;
    char     *data;
    char     *path;

    /* Create the custom directory */
    path = customThemeDirPath(NULL);
    g_mkdir_with_parents (path, 0755);
    g_free (path);

    /* Set the data for index.theme */
    keyfile = g_key_file_new ();
    g_key_file_set_string (keyfile, "Sound Theme", "Name", _("Custom"));
    g_key_file_set_string (keyfile, "Sound Theme", "Inherits", parent);
    g_key_file_set_string (keyfile, "Sound Theme", "Directories", ".");
    data = g_key_file_to_data (keyfile, NULL, NULL);
    g_key_file_free (keyfile);

    /* Save the index.theme */
    path = customThemeDirPath ("index.theme");
    g_file_set_contents (path, data, -1, NULL);
    g_free (path);
    g_free (data);

    customThemeUpdateTime ();
}

/* This function needs to be called after each individual
 * changeset to the theme */
void UkmediaMainWidget::customThemeUpdateTime (void)
{
    char *path;
    path = customThemeDirPath (NULL);
    utime (path, NULL);
    g_free (path);
}

gboolean UkmediaMainWidget::customThemeDirIsEmpty (void)
{
    char            *dir;
    GFile           *file;
    gboolean         is_empty;
    GFileEnumerator *enumerator;
    GFileInfo       *info;
    GError          *error = NULL;

    dir = customThemeDirPath(NULL);
    file = g_file_new_for_path (dir);
    g_free (dir);

    is_empty = TRUE;

    enumerator = g_file_enumerate_children (file,
                                            G_FILE_ATTRIBUTE_STANDARD_NAME ","
                                            G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                            G_FILE_QUERY_INFO_NONE,
                                            NULL, &error);
    if (enumerator == NULL) {
        g_warning ("Unable to enumerate files: %s", error->message);
        g_error_free (error);
        goto out;
    }

    while (is_empty &&
           (info = g_file_enumerator_next_file (enumerator, NULL, NULL))) {

        if (strcmp ("index.theme", g_file_info_get_name (info)) != 0) {
            is_empty = FALSE;
        }

        g_object_unref (info);
    }
    g_file_enumerator_close (enumerator, NULL, NULL);

out:
    g_object_unref (file);

    return is_empty;
}

int UkmediaMainWidget::caPlayForWidget(UkmediaMainWidget *w, uint32_t id, ...)
{
    va_list ap;
    int ret;
    ca_proplist *p;

    if ((ret = ca_proplist_create(&p)) < 0)
        return ret;

    if ((ret = caProplistSetForWidget(p, w)) < 0)
        return -1;

    va_start(ap, id);
    ret = caProplistMergeAp(p, ap);
    va_end(ap);

    if (ret < 0)
        return -1;
    ca_context *c ;
    ca_context_create(&c);
    ret = ca_context_play_full(c, id, p, NULL, NULL);

    return ret;
}

int UkmediaMainWidget::caProplistMergeAp(ca_proplist *p, va_list ap)
{
    int ret;
    for (;;) {
        const char *key, *value;

        if (!(key = va_arg(ap, const char*)))
            break;

        if (!(value = va_arg(ap, const char*)))
            return CA_ERROR_INVALID;

        if ((ret = ca_proplist_sets(p, key, value)) < 0)
            return ret;
    }

    return CA_SUCCESS;
}

int UkmediaMainWidget::caProplistSetForWidget(ca_proplist *p, UkmediaMainWidget *widget)
{
    int ret;
    const char *t;
    QScreen *screen;
    gint x = -1;
    gint y = -1;
    gint width = -1;
    gint height = -1;
    gint screen_width = -1;
    gint screen_height = -1;

    if ((t = widget->windowTitle().toLatin1().data()))
        if ((ret = ca_proplist_sets(p, CA_PROP_WINDOW_NAME, t)) < 0)
            return ret;

    if (t)
        if ((ret = ca_proplist_sets(p, CA_PROP_WINDOW_ID, t)) < 0)
            return ret;

    if ((t = widget->windowIconText().toLatin1().data()))
        if ((ret = ca_proplist_sets(p, CA_PROP_WINDOW_ICON_NAME, t)) < 0)
            return ret;
    if (screen = qApp->primaryScreen()) {
        if ((ret = ca_proplist_setf(p, CA_PROP_WINDOW_X11_SCREEN, "%i", 0)) < 0)
            return ret;
    }

    width = widget->size().width();
    height = widget->size().height();

    if (width > 0)
        if ((ret = ca_proplist_setf(p, CA_PROP_WINDOW_WIDTH, "%i", width)) < 0)
            return ret;
    if (height > 0)
        if ((ret = ca_proplist_setf(p, CA_PROP_WINDOW_HEIGHT, "%i", height)) < 0)
            return ret;

    if (x >= 0 && width > 0) {
        screen_width = qApp->primaryScreen()->size().width();

        x += width/2;
        x = CA_CLAMP(x, 0, screen_width-1);

        /* We use these strange format strings here to avoid that libc
                         * applies locale information on the formatting of floating
                         * numbers. */

        if ((ret = ca_proplist_setf(p, CA_PROP_WINDOW_HPOS, "%i.%03i",
                                    (int) (x/(screen_width-1)), (int) (1000.0*x/(screen_width-1)) % 1000)) < 0)
            return ret;
    }

    if (y >= 0 && height > 0) {
        screen_height = qApp->primaryScreen()->size().height();

        y += height/2;
        y = CA_CLAMP(y, 0, screen_height-1);

        if ((ret = ca_proplist_setf(p, CA_PROP_WINDOW_VPOS, "%i.%03i",
                                    (int) (y/(screen_height-1)), (int) (1000.0*y/(screen_height-1)) % 1000)) < 0)
            return ret;
    }
    return CA_SUCCESS;
}

QList<char *> UkmediaMainWidget::listExistsPath()
{
    char ** childs;
    int len;

    DConfClient * client = dconf_client_new();
    childs = dconf_client_list (client, KEYBINDINGS_CUSTOM_DIR, &len);
    g_object_unref (client);

    QList<char *> vals;

    for (int i = 0; childs[i] != NULL; i++){
        if (dconf_is_rel_dir (childs[i], NULL)){
            char * val = g_strdup (childs[i]);

            vals.append(val);
        }
    }
    g_strfreev (childs);
    return vals;
}

QString UkmediaMainWidget::findFreePath(){
    int i = 0;
    char * dir;
    bool found;
    QList<char *> existsdirs;

    existsdirs = listExistsPath();

    for (; i < MAX_CUSTOM_SHORTCUTS; i++){
        found = true;
        dir = QString("custom%1/").arg(i).toLatin1().data();
        for (int j = 0; j < existsdirs.count(); j++)
            if (!g_strcmp0(dir, existsdirs.at(j))){
                found = false;
                break;
            }
        if (found)
            break;
    }

    if (i == MAX_CUSTOM_SHORTCUTS){
        qDebug() << "Keyboard Shortcuts" << "Too many custom shortcuts";
        return "";
    }

    return QString("%1%2").arg(KEYBINDINGS_CUSTOM_DIR).arg(QString(dir));
}

void UkmediaMainWidget::addValue(QString name,QString filename)
{
    //在创建setting表时，先判断是否存在该设置，存在时不创建
    QList<char *> existsPath = listExistsPath();

    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();

            g_warning("full path: %s", allpath);
            qDebug() << filenameStr << FILENAME_KEY <<NAME_KEY << nameStr;
            if (nameStr == name) {
                return;
            }
            delete settings;
        }
        else {
            continue;
        }

    }
    QString availablepath = findFreePath();

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toUtf8().data());
    if(QGSettings::isSchemaInstalled(id))
    {
        QGSettings * settings = new QGSettings(id, idd);
        settings->set(FILENAME_KEY, filename);
        settings->set(NAME_KEY, name);
    }
}

MateMixerSwitch * UkmediaMainWidget::findDeviceProfileSwitch (UkmediaMainWidget *w,MateMixerDevice *device)
{
    const GList *switches;
    const gchar *profileLabel = nullptr;
    const gchar *devName = nullptr;
    devName = mate_mixer_device_get_name(device);
    switches = mate_mixer_device_list_switches (device);
    while (switches != nullptr) {
        MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);
        MateMixerSwitchOption *active;
        active = mate_mixer_switch_get_active_option (MATE_MIXER_SWITCH (swtch));
//        w->m_pOutputWidget->m_pProfileCombobox->setCurrentText(profileLabel);
        if (G_LIKELY (active != NULL)) {
            profileLabel = mate_mixer_switch_option_get_label (active);
        }
//        if (w->m_pOutputWidget->m_pProfileCombobox)
        int devIndex = w->m_pOutputWidget->m_pSelectCombobox->currentIndex();
        QString deviceStr = w->m_pDeviceNameList->at(devIndex);
        QByteArray bba = deviceStr.toLatin1();
        const gchar * deviceName = bba.data();
        qDebug() << "profilelabel :" << devName << "device name :" << mate_mixer_device_get_name(device) <<deviceName;
        if (strcmp(deviceName,devName) == 0) {
//            w->m_pOutputWidget->m_pProfileCombobox->setCurrentText(profileLabel);
        }
        if (mate_mixer_device_switch_get_role (swtch) == MATE_MIXER_DEVICE_SWITCH_ROLE_PROFILE)
            return MATE_MIXER_SWITCH (swtch);

        switches =  switches->next;
    }
    return nullptr;
}

gchar * UkmediaMainWidget::deviceStatus (MateMixerDevice *device)
{
    guint        inputs = 0;
    guint        outputs = 0;
    gchar       *inputs_str = NULL;
    gchar       *outputs_str = NULL;
    const GList *streams;

    /* Get number of input and output streams in the device */
    streams = mate_mixer_device_list_streams (device);
    while (streams != NULL) {
        MateMixerStream   *stream = MATE_MIXER_STREAM (streams->data);
        MateMixerDirection direction;

        direction = mate_mixer_stream_get_direction (stream);

        if (direction == MATE_MIXER_DIRECTION_INPUT)
            inputs++;
        else if (direction == MATE_MIXER_DIRECTION_OUTPUT)
            outputs++;

        streams = streams->next;
    }

    if (inputs == 0 && outputs == 0) {
        /* translators:
                 * The device has been disabled */
        return g_strdup (_("Disabled"));
    }

    if (outputs > 0) {
        /* translators:
                 * The number of sound outputs on a particular device */
        outputs_str = g_strdup_printf (ngettext ("%u Output",
                                                 "%u Outputs",
                                                 outputs),
                                       outputs);
    }

    if (inputs > 0) {
        /* translators:
                 * The number of sound inputs on a particular device */
        inputs_str = g_strdup_printf (ngettext ("%u Input",
                                                "%u Inputs",
                                                inputs),
                                      inputs);
    }

    if (inputs_str != NULL && outputs_str != NULL) {
        gchar *ret = g_strdup_printf ("%s / %s",
                                      outputs_str,
                                      inputs_str);
        g_free (outputs_str);
        g_free (inputs_str);
        return ret;
    }

    if (inputs_str != NULL)
        return inputs_str;

    return outputs_str;
}

void UkmediaMainWidget::updateProfileOption()
{
    int index = m_pOutputWidget->m_pSelectCombobox->currentIndex();
    if (index < 0)
        return;
    QString deviceStr = m_pDeviceNameList->at(index);
    QByteArray ba = deviceStr.toLatin1();
    const gchar *deviceName = ba.data();
    const gchar *profileLabel = nullptr;
    const gchar *profileName = nullptr;
    const gchar *setProfileLabel = nullptr;
    MateMixerSwitchOption *activeOption;
    MateMixerDevice *pDevice = mate_mixer_context_get_device(m_pContext,deviceName);
    const GList *switches;
    switches = mate_mixer_device_list_switches (MATE_MIXER_DEVICE(pDevice));
    m_pOutputWidget->m_pProfileCombobox->clear();
    m_pProfileNameList->clear();
    while (switches != nullptr) {
        MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);
        const GList *options;
        options = mate_mixer_switch_list_options ( MATE_MIXER_SWITCH(swtch));
        activeOption = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(swtch));
        setProfileLabel  = mate_mixer_switch_option_get_label(activeOption) ;

        while (options != NULL) {
            MateMixerSwitchOption *option = MATE_MIXER_SWITCH_OPTION (options->data);
            profileLabel = mate_mixer_switch_option_get_label (option);
            profileName = mate_mixer_switch_option_get_name(option);
            m_pProfileNameList->append(profileName);
            m_pOutputWidget->m_pProfileCombobox->addItem(profileLabel);
            /* Select the currently active option of the switch */
            options = options->next;
        }
        switches = switches->next;
    }
}

void UkmediaMainWidget::updateDeviceInfo (UkmediaMainWidget *w, MateMixerDevice *device)
{
    const gchar     *label;
    const gchar     *profileLabel = NULL;
    gchar           *status;
    MateMixerSwitch *profileSwitch;

    label = mate_mixer_device_get_label (device);
    profileSwitch = findDeviceProfileSwitch (w,device);
    if (profileSwitch != NULL) {
        MateMixerSwitchOption *active;

        active = mate_mixer_switch_get_active_option (profileSwitch);
        if (G_LIKELY (active != NULL))
            profileLabel = mate_mixer_switch_option_get_label (active);

        //qDebug() << "update device info ,设置combobox profile:" << profileLabel;
        w->m_pOutputWidget->m_pProfileCombobox->setCurrentText(profileLabel);
    }

    status = deviceStatus (device);
    g_free (status);
}

void UkmediaMainWidget::onSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w)
{
    MateMixerSwitchOption *action = mate_mixer_switch_get_active_option(swtch);
    mate_mixer_switch_option_get_label(action);
    const gchar *outputPortLabel = mate_mixer_switch_option_get_label(action);
    qDebug() << "update active option  notify" << outputPortLabel;
    w->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
    w->m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(outputPortLabel);
    w->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
}

void UkmediaMainWidget::onDeviceProfileActiveOptionNotify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w)
{
    MateMixerDevice *device;
    device = mate_mixer_device_switch_get_device (swtch);
    w->updateInputDevicePort();
    w->updateOutputDevicePort();
    updateDeviceInfo (w, device);
}

void UkmediaMainWidget::updateOutputDevicePort()
{
    MateMixerSwitch *outputPortSwitch;
    const GList  *options ;
    const gchar *outputPortLabel = nullptr;
    const gchar *outputPortName = nullptr;

    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(m_pContext);
    if (outputStream == nullptr) {
        return;
    }
    outputPortSwitch = findStreamPortSwitch(this,outputStream);
    options = mate_mixer_switch_list_options(outputPortSwitch);

    MateMixerSwitchOption *outputActivePort;
    outputActivePort = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (outputPortSwitch));
    if (G_LIKELY (outputActivePort != NULL)) {
        outputPortLabel = mate_mixer_switch_option_get_label(outputActivePort);
        outputPortName = mate_mixer_switch_option_get_name(outputActivePort);
    }
    if (outputPortSwitch != NULL) {
        if (G_LIKELY (outputActivePort != NULL))
            outputPortLabel = mate_mixer_switch_option_get_label(outputActivePort);
        if (MATE_MIXER_IS_SWITCH_OPTION (outputActivePort)) {
            m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
            m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(outputPortLabel);
            m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
        }

        g_signal_connect (G_OBJECT (outputPortSwitch),
                          "notify::active-option",
                          G_CALLBACK(onOutputSwitchActiveOptionNotify),
                          this);
    }
}

void UkmediaMainWidget::updateInputDevicePort()
{
    MateMixerSwitch *inputPortSwitch;
    const GList  *inputOptions ;
    const gchar *inputPortLabel = nullptr;
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(m_pContext);
    if (inputStream == nullptr) {
        return;
    }
    inputPortSwitch = findStreamPortSwitch(this,inputStream);

    inputOptions = mate_mixer_switch_list_options(inputPortSwitch);
    MateMixerSwitchOption *inputActiveOption = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(inputPortSwitch));

    MateMixerSwitchOption *inputActivePort;
    inputActivePort = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (inputPortSwitch));
    if (G_LIKELY (inputActiveOption != NULL))
        inputPortLabel = mate_mixer_switch_option_get_label(inputActivePort);

    if (inputPortSwitch != NULL) {
        if (G_LIKELY (inputActiveOption != NULL))
            inputPortLabel = mate_mixer_switch_option_get_label(inputActivePort);
        if (MATE_MIXER_IS_SWITCH_OPTION (inputActivePort)) {
            m_pInputWidget->m_pInputPortCombobox->blockSignals(true);
            m_pInputWidget->m_pInputPortCombobox->setCurrentText(inputPortLabel);
            m_pInputWidget->m_pInputPortCombobox->blockSignals(false);
        }
        g_signal_connect (G_OBJECT (inputPortSwitch),
                          "notify::active-option",
                          G_CALLBACK(onInputSwitchActiveOptionNotify),
                          this);
    }
}

void UkmediaMainWidget::onInputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w)
{
    MateMixerSwitchOption *action = mate_mixer_switch_get_active_option(swtch);
    mate_mixer_switch_option_get_label(action);
    const gchar *inputPortLabel = mate_mixer_switch_option_get_label(action);
    w->m_pInputWidget->m_pInputPortCombobox->blockSignals(true);
    w->m_pInputWidget->m_pInputPortCombobox->setCurrentText(inputPortLabel);
    w->m_pInputWidget->m_pInputPortCombobox->blockSignals(false);
}

void UkmediaMainWidget::onOutputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w)
{
    MateMixerSwitchOption *action = mate_mixer_switch_get_active_option(swtch);
    mate_mixer_switch_option_get_label(action);
    const gchar *outputPortLabel = mate_mixer_switch_option_get_label(action);
    w->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(true);
    w->m_pOutputWidget->m_pOutputPortCombobox->setCurrentText(outputPortLabel);
    w->m_pOutputWidget->m_pOutputPortCombobox->blockSignals(false);
}

void UkmediaMainWidget::setConnectingMessage(const char *string) {
    QByteArray markup = "<i>";
    if (!string)
        markup += tr("Establishing connection to PulseAudio. Please wait...").toUtf8().constData();
    else
        markup += string;
    markup += "</i>";
}

gboolean UkmediaMainWidget::connect_to_pulse(gpointer userdata)
{
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, QObject::tr("PulseAudio Volume Control").toUtf8().constData());
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "org.PulseAudio.pavucontrol");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_VERSION, "PACKAGE_VERSION");

    context = pa_context_new_with_proplist(api, nullptr, proplist);
    g_assert(context);

    pa_proplist_free(proplist);

    pa_context_set_state_callback(context, context_state_callback, w);
    if (pa_context_connect(context, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        if (pa_context_errno(context) == PA_ERR_INVALID) {
            qDebug() << "connect error pulseaudio disconnect ";
            w->setConnectingMessage(QObject::tr("Connection to PulseAudio failed. Automatic retry in 5s\n\n"
                "In this case this is likely because PULSE_SERVER in the Environment/X11 Root Window Properties\n"
                "or default-server in client.conf is misconfigured.\n"
                "This situation can also arrise when PulseAudio crashed and left stale details in the X11 Root Window.\n"
                "If this is the case, then PulseAudio should autospawn again, or if this is not configured you should\n"
                "run start-pulseaudio-x11 manually.").toUtf8().constData());
        }
    }

    return false;
}

void UkmediaMainWidget::createEventRole()
{
    pa_channel_map cm = {
        1, { PA_CHANNEL_POSITION_MONO }
    };
    channelMap = cm;
    executeVolumeUpdate(false);
}

void UkmediaMainWidget::context_state_callback(pa_context *c, void *userdata) {
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);
    g_assert(c);

    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY: {
            pa_operation *o;

            /* Create event widget immediately so it's first in the list */
            w->createEventRole();
            pa_context_set_subscribe_callback(c, subscribe_cb, w);
            if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                           (PA_SUBSCRIPTION_MASK_SINK|
                                            PA_SUBSCRIPTION_MASK_SOURCE|
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                            PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT|
                                            PA_SUBSCRIPTION_MASK_CLIENT|
                                            PA_SUBSCRIPTION_MASK_SERVER|
                                            PA_SUBSCRIPTION_MASK_CARD), nullptr, nullptr))) {
                w->show_error(QObject::tr("pa_context_subscribe() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);
            if (!(o = pa_context_get_card_info_list(c, card_cb, w))) {
                w->show_error(QObject::tr("pa_context_get_card_info_list() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);
            /* These calls are not always supported */
            if ((o = pa_ext_stream_restore_read(c, ext_stream_restore_read_cb, w))) {
                pa_operation_unref(o);

                if ((o = pa_ext_stream_restore_subscribe(c, 1, nullptr, nullptr)))
                    pa_operation_unref(o);

            } else
                g_debug(QObject::tr("Failed to initialize stream_restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(w->context)));
            break;
        }
        case PA_CONTEXT_FAILED:
        if (w->reconnectTime > 0) {
            g_debug("%s", QObject::tr("Connection failed, attempting reconnect").toUtf8().constData());
            qDebug() << "connect failed ,wait to reconnect";
            g_timeout_add_seconds(w->reconnectTime, connectContext, w);
        }
        return;
        case PA_CONTEXT_TERMINATED:
        default:
            return;
    }
}

void UkmediaMainWidget::ext_stream_restore_subscribe_cb(pa_context *c, void *userdata)
{
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);
    pa_operation *o;
    if (!(o = pa_ext_stream_restore_read(c, w->ext_stream_restore_read_cb, w))) {
        w->show_error(QObject::tr("pa_ext_stream_restore_read() failed").toUtf8().constData());
        return;
    }

    pa_operation_unref(o);
}

void UkmediaMainWidget::ext_stream_restore_read_cb(pa_context *,const pa_ext_stream_restore_info *i,int eol,void *userdata)
{
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);

    if (eol < 0) {
        return;
    }

    if (eol > 0) {
        qDebug() << "Failed to initialize stream_restore extension";
        return;
    }

    w->updateRole(*i);
}

void UkmediaMainWidget::executeVolumeUpdate(bool isMuted)
{
    info.name = role;
    info.channel_map.channels = 1;
    info.channel_map.map[0] = PA_CHANNEL_POSITION_MONO;
    volume.channels = 1;
    volume.values[0] = m_pSoundWidget->m_pAlertSlider->value()*65536/100;
    info.volume = volume;
    info.device = device == "" ? nullptr : device.constData();
    info.mute = isMuted;

    pa_operation* o;
    if (!(o = pa_ext_stream_restore_write(get_context(), PA_UPDATE_REPLACE, &info, 1, true, nullptr, nullptr))) {
        show_error(tr("pa_ext_stream_restore_write() failed").toUtf8().constData());
        return;
    }
    pa_operation_unref(o);
}



void UkmediaMainWidget::subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_CARD:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa_operation *o;
//                if ((o = pa_context_get_card_info_by_index(c, index, card_cb, w))) {
//                    w->show_error(QObject::tr("pa_context_get_card_info_by_index() failed").toUtf8().constData());
//                }
                 w->currentInputPortLabelMap.clear();
                 w->m_pCurrentOutputPortLabelList->clear();
                 w->m_pCurrentInputPortLabelList->clear();
                 w->currentOutputPortLabelMap.clear();
                 int removeIndex = index;
                //将移除的声卡信息在map删除
                QMap<int,QString>::iterator it;
                QMap<int,QList<QString>>::iterator temp;
                QMap<int, QMap<QString,int>>::iterator profilePriorityMap;
                for(it = w->cardMap.begin();it!=w->cardMap.end();)
                {
                    if(it.key() == index)
                    {
                        it = w->cardMap.erase(it);

                        continue;
                    }
                    ++it;
                }

                for(temp=w->cardProfileMap.begin();temp!=w->cardProfileMap.end();) {

                    if (it.key() == index) {
                        temp= w->cardProfileMap.erase(temp);
                        continue;
                    }
                    ++temp;
                }

                for(profilePriorityMap=w->cardProfilePriorityMap.begin();profilePriorityMap!=w->cardProfilePriorityMap.end();) {

                    if (profilePriorityMap.key() == index) {
                        profilePriorityMap= w->cardProfilePriorityMap.erase(profilePriorityMap);
                        continue;
                    }
                    ++profilePriorityMap;
                }

                //移除输入端口名
                for(it = w->inputPortNameMap.begin();it!=w->inputPortNameMap.end();)
                {
                    if(it.key() == index)
                    {
                        it = w->inputPortNameMap.erase(it);
                        //qDebug() << "remove input port name map index" << index << w->inputPortNameMap.count();
                        continue;
                    }
                    ++it;
                }

                for(it = w->outputPortNameMap.begin();it!=w->outputPortNameMap.end();)
                {
                    if(it.key() == index)
                    {
                        it = w->outputPortNameMap.erase(it);
                        //qDebug() << "remove output port name map index" << index << w->outputPortNameMap.count();
                        continue;
                    }
                    ++it;
                }
                for(it = w->inputPortLabelMap.begin();it!=w->inputPortLabelMap.end();)
                {
                    if(it.key() == index)
                    {
                        QString removePortLabel = it.value();
                        QMap<QString,QString>::iterator removeProfileMap;
                        for (removeProfileMap = w->inputPortProfileNameMap.begin();removeProfileMap!= w->inputPortProfileNameMap.end();) {
                            if (removeProfileMap.key() == removePortLabel) {
                                removeProfileMap = w->inputPortProfileNameMap.erase(removeProfileMap);
                                continue;
                            }
                            ++removeProfileMap;
                        }
                        it = w->inputPortLabelMap.erase(it);
                        qDebug() << "remove input port label map index" << index << w->inputPortLabelMap.count();
                        continue;
                    }
                    ++it;
                }

                for(it = w->outputPortLabelMap.begin();it!=w->outputPortLabelMap.end();)
                {
                    if(it.key() == index)
                    {
                        QString removePortLabel = it.value();
                        QMap<QString,QString>::iterator removeProfileMap;
                        for (removeProfileMap = w->profileNameMap.begin();removeProfileMap!= w->profileNameMap.end();) {
                            if (removeProfileMap.key() == removePortLabel) {
                                removeProfileMap = w->profileNameMap.erase(removeProfileMap);
                                continue;
                            }
                            ++removeProfileMap;
                        }
                        //qDebug() << "remove output port label map index" << it.value() << w->outputPortLabelMap.count();
                        it = w->outputPortLabelMap.erase(it);
                        continue;
                    }
                    ++it;
                }

                qDebug() << "remove cards" << index << w->inputPortLabelMap.count() << w->outputPortLabelMap.count() ;
                for (int i=0;i<w->m_pOutputWidget->m_pOutputListWidget->count();i++) {

                    QListWidgetItem *item = w->m_pOutputWidget->m_pOutputListWidget->item(i);
                    UkuiListWidgetItem *wid = (UkuiListWidgetItem *)w->m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
                    int index;
                    int count = 0;
                    for (it=w->cardMap.begin();it!=w->cardMap.end();) {
                        if (wid->deviceLabel->text() == it.value()) {
                            index = it.key();
                            break;
                        }
                        ++it;
                        ++count;
                    }
                    if (count < w->cardMap.count()) {
                        w->currentOutputPortLabelMap.insertMulti(index,wid->portLabel->text());
                        w->m_pCurrentOutputPortLabelList->append(wid->portLabel->text());
                        w->m_pCurrentOutputCardList->append(wid->deviceLabel->text());
                    }
                    else { //在cardMap没找到，已经被删除
                        w->currentOutputPortLabelMap.insertMulti(removeIndex,wid->portLabel->text());
                        w->m_pCurrentOutputPortLabelList->append(wid->portLabel->text());
                        w->m_pCurrentOutputCardList->append(wid->deviceLabel->text());
                    }
                    //qDebug() << index << "current output item ************" << item->text() <<wid->portLabel->text() << w->m_pOutputPortLabelList->count() ;//<< w->m_pOutputPortLabelList->at(i);
                }

                for (int i=0;i<w->m_pInputWidget->m_pInputListWidget->count();i++) {

                    QListWidgetItem *item = w->m_pInputWidget->m_pInputListWidget->item(i);
                    UkuiListWidgetItem *wid = (UkuiListWidgetItem *)w->m_pInputWidget->m_pInputListWidget->itemWidget(item);
                    int index;
                    int count = 0;
                    for (it=w->cardMap.begin();it!=w->cardMap.end();) {
                        if (wid->deviceLabel->text() == it.value()) {
                            index = it.key();
                            break;
                        }

                        ++count;
                        ++it;
                    }
                    if (count < w->cardMap.count()) {
                        w->currentInputPortLabelMap.insertMulti(index,wid->portLabel->text());
                        w->m_pCurrentInputPortLabelList->append(wid->portLabel->text());
                        w->m_pCurrentInputCardList->append(wid->deviceLabel->text());
                    }
                    else { //在cardMap中没找到意味着已经被移除了
                        w->currentInputPortLabelMap.insertMulti(removeIndex,wid->portLabel->text());
                        w->m_pCurrentInputPortLabelList->append(wid->portLabel->text());
                        w->m_pCurrentInputCardList->append(wid->deviceLabel->text());
                    }
                    qDebug() <<"current input port label insert" << index << wid->deviceLabel->text() << w->cardMap.count() << "count= " << count;

                    //qDebug() << i << "current input item ************" << item->text() <<wid->portLabel->text() ;
                }
                w->deleteNotAvailableInputPort();
                w->addAvailableInputPort();
                w->deleteNotAvailableOutputPort();
                w->addAvailableOutputPort();

            }
            else {
                pa_operation *o;
                if (!(o = pa_context_get_card_info_by_index(c, index, card_cb, w))) {
                    w->show_error(QObject::tr("pa_context_get_card_info_by_index() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;
    }
}

void UkmediaMainWidget::card_cb(pa_context *, const pa_card_info *i, int eol, void *userdata) {
    UkmediaMainWidget *w = static_cast<UkmediaMainWidget*>(userdata);

    if (eol < 0) {
        if (pa_context_errno(w->context) == PA_ERR_NOENTITY)
            return;

        w->show_error(QObject::tr("Card callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
//        dec_outstanding(w);
        return;
    }
//    bool alreadyInclude = false;
//    QMap <int,QString>::iterator it;
//    for(it=w->cardMap.begin();it!=w->cardMap.end();) {
//        if (it.key() == i->index) {
//            alreadyInclude = true;
//            break;
//        }
//    }
//    if (alreadyInclude)
    w->cardMap.insert(i->index,i->name);
    qDebug() << "update card" << i->name << i->index << i->ports << "card count: "<< w->cardMap.count()<<i->active_profile;
    w->updateCard(*i);

}

void UkmediaMainWidget::updateCard(const pa_card_info &info) {
    const char *name;
    const char *description, *icon;
    std::set<pa_card_profile_info2 *, profile_prio_compare> profile_priorities;

    description = pa_proplist_gets(info.proplist, PA_PROP_DEVICE_DESCRIPTION);
    name = description ? description : info.name;

    icon = pa_proplist_gets(info.proplist, PA_PROP_DEVICE_ICON_NAME);

    this->hasSinks = false;
    this->hasSources = false;
    profile_priorities.clear();

    QList<QString> profileName;
    QMap<QString,int> profilePriorityMap;
    for (pa_card_profile_info2 ** p_profile = info.profiles2; *p_profile != nullptr; ++p_profile) {
        this->hasSinks = this->hasSinks || ((*p_profile)->n_sinks > 0);
        this->hasSources = this->hasSources || ((*p_profile)->n_sources > 0);
        profile_priorities.insert(*p_profile);
        profileName.append((*p_profile)->name);
        profilePriorityMap.insertMulti((*p_profile)->name,(*p_profile)->priority);
    }
    cardProfilePriorityMap.insertMulti(info.index,profilePriorityMap);

    removeOutputPortName(info);
    removeOutputPortLabel(info);
    removeInputPortName(info);
    removeInputPortLabel(info);

    this->ports.clear();
    for (uint32_t i = 0; i < info.n_ports; ++i) {
        PortInfo p;

        p.name = info.ports[i]->name;
        p.description = info.ports[i]->description;
        p.priority = info.ports[i]->priority;
        p.available = info.ports[i]->available;
        p.direction = info.ports[i]->direction;
        p.latency_offset = info.ports[i]->latency_offset;
        if (info.ports[i]->profiles2 != nullptr)
        for (pa_card_profile_info2 ** p_profile = info.ports[i]->profiles2; *p_profile != nullptr; ++p_profile) {
            p.profiles.push_back((*p_profile)->name);

        }
        if (p.direction == 1 && p.available != PA_PORT_AVAILABLE_NO) {

            outputPortNameMap.insertMulti(info.index,p.name);
            outputPortLabelMap.insertMulti(info.index,p.description.data());

            QList<QString> portProfileName;
            for (auto p_profile : p.profiles) {
                portProfileName.append(p_profile.data());
                profileNameMap.insertMulti(p.description.data(),p_profile.data());
            }
            cardProfileMap.insertMulti(info.index,portProfileName);
        }
        else if (p.direction == 2 && p.available != PA_PORT_AVAILABLE_NO){
            inputPortNameMap.insertMulti(info.index,p.name);
            inputPortLabelMap.insertMulti(info.index,p.description.data());
            for (auto p_profile : p.profiles) {
                inputPortProfileNameMap.insertMulti(p.description.data(),p_profile.data());
            }
        }
        this->ports[p.name] = p;
    }

    this->profiles.clear();

    for (auto p_profile : profile_priorities) {
        bool hasNo = false, hasOther = false;
        std::map<QByteArray, PortInfo>::iterator portIt;
        QByteArray desc = p_profile->description;

        for (portIt = this->ports.begin(); portIt != this->ports.end(); portIt++) {
            PortInfo port = portIt->second;

            if (std::find(port.profiles.begin(), port.profiles.end(), p_profile->name) == port.profiles.end())
                continue;

            if (port.available == PA_PORT_AVAILABLE_NO)
                hasNo = true;
            else {
                hasOther = true;
                break;
            }
        }
        if (hasNo && !hasOther)
            desc += tr(" (unplugged)").toUtf8().constData();

        if (!p_profile->available)
            desc += tr(" (unavailable)").toUtf8().constData();

        this->profiles.push_back(std::pair<QByteArray,QByteArray>(p_profile->name, desc));
        if (p_profile->n_sinks == 0 && p_profile->n_sources == 0)
            this->noInOutProfile = p_profile->name;
    }

    this->activeProfile = info.active_profile ? info.active_profile->name : "";

    qDebug() << "this->active profile -----------------" << info.name <<info.active_profile->name << info.active_profile2->name;
    /* Because the port info for sinks and sources is discontinued we need
     * to update the port info for them here. */
    if (this->hasSinks || this->hasSources) {
        updatePorts(this, info, this->ports);
    }
}

void UkmediaMainWidget::show_error(const char *txt) {
    char buf[256];

    snprintf(buf, sizeof(buf), "%s: %s", txt, pa_strerror(pa_context_errno(context)));
    qDebug() << "show error:" << QString::fromUtf8(buf);
}

pa_context* UkmediaMainWidget::get_context()
{
    return context;
}

void UkmediaMainWidget::updateRole(const pa_ext_stream_restore_info &info)
{
    if (strcmp(info.name, "sink-input-by-media-role:event") != 0)
        return;
    createEventRole();
}

UkmediaMainWidget::~UkmediaMainWidget()
{
//    delete player;
}


void UkmediaMainWidget::updatePorts(UkmediaMainWidget *w, const pa_card_info &info, std::map<QByteArray, PortInfo> &ports) {
    std::map<QByteArray, PortInfo>::iterator it;
    PortInfo p;

    w->updatePort = false;
    w->m_pCurrentOutputPortLabelList->clear();
    w->currentOutputPortLabelMap.clear();
    w->currentInputPortLabelMap.clear();
    w->m_pCurrentInputPortLabelList->clear();
    int ii;
    for (ii=0;ii<w->m_pOutputPortList->count();ii++) {
        if (ii == w->m_pOutputPortList->count())
            break;
    }
    for (ii = 0;ii<w->m_pPrivOutputStreamList->count();ii++) {
        if (ii == w->m_pPrivOutputStreamList->count())
            break;
    }


    w->m_pInputWidget->m_pInputPortCombobox->clear();
    w->m_pOutputWidget->m_pOutputPortCombobox->clear();
    w->m_pCurrentInputCardList->clear();

    int i = 0;
    w->m_pOutputPortList->clear();
    w->m_pOutputPortLabelList->clear();
    w->m_pInputPortLabelList->clear();
    w->m_pInputPortList->clear();
    for (auto & port : w->ports) {
        QByteArray desc;
        it = ports.find(port.first);

        if (it == ports.end())
            continue;

        p = it->second;
        desc = p.description;

        if (p.available == PA_PORT_AVAILABLE_YES) {
            desc +=  UkmediaMainWidget::tr(" (plugged in)").toUtf8().constData();

        }
        else if (p.available == PA_PORT_AVAILABLE_NO) {
            if (p.name == "analog-output-speaker" ||
                p.name == "analog-input-microphone-internal")
                desc += UkmediaMainWidget::tr(" (unavailable)").toUtf8().constData();
            else
                desc += UkmediaMainWidget::tr(" (unplugged)").toUtf8().constData();
        }

    }

    QMap<int,QString>::iterator at;
    QMap<int,QString>::iterator cardNameMap;
    if (w->firstEntry == true) {

        for(at = w->outputPortLabelMap.begin();at!=w->outputPortLabelMap.end();)
        {

            UkuiListWidgetItem *itemW = new UkuiListWidgetItem(w);

            QListWidgetItem * item = new QListWidgetItem(w->m_pOutputWidget->m_pOutputListWidget);
            item->setSizeHint(QSize(200,50)); //QSize(120, 40) spacing: 12px;
            w->m_pOutputWidget->m_pOutputListWidget->setItemWidget(item, itemW);
            QString cardName;
            for(cardNameMap = w->cardMap.begin();cardNameMap!=w->cardMap.end();)
            {
                if (cardNameMap.key() == at.key()) {
                    cardName = cardNameMap.value();
                    break;
                }
                ++cardNameMap;
            }

            itemW->setLabelText(at.value(),cardName);
            w->m_pOutputWidget->m_pOutputListWidget->insertItem(i++,item);

            ++at;
        }
        for(at = w->inputPortLabelMap.begin();at!=w->inputPortLabelMap.end();)
        {
            UkuiListWidgetItem *itemW = new UkuiListWidgetItem(w);

            QListWidgetItem * item = new QListWidgetItem(w->m_pInputWidget->m_pInputListWidget);
            item->setSizeHint(QSize(200,50)); //QSize(120, 40) spacing: 12px;
            w->m_pInputWidget->m_pInputListWidget->setItemWidget(item, itemW);
            QString cardName;
            for(cardNameMap = w->cardMap.begin();cardNameMap!=w->cardMap.end();)
            {
                if (cardNameMap.key() == at.key()) {
                    cardName = cardNameMap.value();
                    break;
                }
                ++cardNameMap;
            }
            itemW->setLabelText(at.value(),cardName);
            w->m_pInputWidget->m_pInputListWidget->insertItem(i++,item);

            ++at;
        }

    }
    else {
        //记录上一次output label
        for (i=0;i<w->m_pOutputWidget->m_pOutputListWidget->count();i++) {

            QListWidgetItem *item = w->m_pOutputWidget->m_pOutputListWidget->item(i);
            UkuiListWidgetItem *wid = (UkuiListWidgetItem *)w->m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
            int index;
            for (at=w->cardMap.begin();at!=w->cardMap.end();) {
                if (wid->deviceLabel->text() == at.value()) {
                    index = at.key();
                    break;
                }
                ++at;
            }
            w->currentOutputPortLabelMap.insertMulti(index,wid->portLabel->text());
            w->m_pCurrentOutputPortLabelList->append(wid->portLabel->text());
            w->m_pCurrentOutputCardList->append(wid->deviceLabel->text());
            //qDebug() << index << "current output item ************" << item->text() <<wid->portLabel->text() << w->m_pOutputPortLabelList->count() ;//<< w->m_pOutputPortLabelList->at(i);
        }

        for (i=0;i<w->m_pInputWidget->m_pInputListWidget->count();i++) {

            QListWidgetItem *item = w->m_pInputWidget->m_pInputListWidget->item(i);
            UkuiListWidgetItem *wid = (UkuiListWidgetItem *)w->m_pInputWidget->m_pInputListWidget->itemWidget(item);
            int index;
            int count;
            for (at=w->cardMap.begin();at!=w->cardMap.end();) {
                if (wid->deviceLabel->text() == at.value()) {
                    index = at.key();
                    break;
                }
                ++at;
                ++count;
            }
            w->currentInputPortLabelMap.insertMulti(index,wid->portLabel->text());
            w->m_pCurrentInputPortLabelList->append(wid->portLabel->text());
            w->m_pCurrentInputCardList->append(wid->deviceLabel->text());
            qDebug() <<"current input port label insert " << index << wid->deviceLabel->text();

            //qDebug() << i << "current input item ************" << item->text() <<wid->portLabel->text() ;
        }

        w->deleteNotAvailableOutputPort();
        w->addAvailableOutputPort();
        w->deleteNotAvailableInputPort();
        w->addAvailableInputPort();
    }

    if (w->m_pOutputWidget->m_pOutputListWidget->count() > 0 || w->m_pInputWidget->m_pInputListWidget->count()) {

        w->firstEntry = false;
    }

    if (w->firstEntry == false) {
        w->m_pPrivOutputStreamList = w->m_pOutputPortList;
    } 
}

void UkmediaMainWidget::deleteNotAvailableOutputPort()
{
    //删除不可用的输出端口
    QMap<int,QString>::iterator at;
    QMap<int,QString>::iterator it;
    int i;
    for(i=0,it=currentOutputPortLabelMap.begin();it!=currentOutputPortLabelMap.end();) {

        int count = 0;
        for(at = outputPortLabelMap.begin();at!=outputPortLabelMap.end();)
        {
            if (it.key() == at.key() ){
                if (it.value() == at.value()) {
                    break;
                }
            }
            ++at;
            count ++;
        }
        //没找到，需要删除
        if (count == outputPortLabelMap.count()) {
            int index = indexOfOutputPortInOutputListWidget(it.value());
            if (index == -1)
                return;
            QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->takeItem(index);
            m_pOutputWidget->m_pOutputListWidget->removeItemWidget(item);
            m_pCurrentOutputPortLabelList->removeAt(index);
            it = currentOutputPortLabelMap.erase(it);
            continue;
        }
        ++it;
        ++i;
    }
}

void UkmediaMainWidget::deleteNotAvailableInputPort()
{
    //删除不可用的输入端口
    QMap<int,QString>::iterator at;
    QMap<int,QString>::iterator it;
    int i;
    for(i=0,it=currentInputPortLabelMap.begin();it!=currentInputPortLabelMap.end();) {

        int count = 0;
        for(at = inputPortLabelMap.begin();at!=inputPortLabelMap.end();)
        {
            if (it.key() == at.key() ){
                if (it.value() == at.value()) {
                    break;
                }
            }
            ++at;
            count ++;
        }
        //没找到，需要删除

        qDebug() << "input label" <<it.key() << it.value();
        if (count == inputPortLabelMap.count()) {
            int index = indexOfInputPortInInputListWidget(it.value());
            if (index == -1)
                return;
            QListWidgetItem *item = m_pInputWidget->m_pInputListWidget->takeItem(index);
            m_pInputWidget->m_pInputListWidget->blockSignals(true);
            m_pInputWidget->m_pInputListWidget->removeItemWidget(item);
            m_pInputWidget->m_pInputListWidget->blockSignals(false);
            m_pCurrentInputPortLabelList->removeAt(index);
            qDebug() <<"remove current output label" <<it.key() << it.value() <<inputPortLabelMap.count();
            it = currentInputPortLabelMap.erase(it);
            continue;
        }
        ++it;
        ++i;
    }
}

void UkmediaMainWidget::addAvailableOutputPort()
{
    QMap<int,QString>::iterator at;
    QMap<int,QString>::iterator it;
    QMap<int,QString>::iterator cardNameMap;
    int i;
    //增加端口
    for(at = outputPortLabelMap.begin();at!=outputPortLabelMap.end();)
    {
        for(i=0,it=currentOutputPortLabelMap.begin();it!=currentOutputPortLabelMap.end();i++) {
            if ( at.key() == it.key() && at.value() == it.value()) {

                break;
            }
            ++it;
        }
        //需添加到list widget
        if (i == currentOutputPortLabelMap.count()) {
            UkuiListWidgetItem *itemW = new UkuiListWidgetItem(this);

            QListWidgetItem * item = new QListWidgetItem(m_pOutputWidget->m_pOutputListWidget);
            item->setSizeHint(QSize(200,50)); //QSize(120, 40) spacing: 12px;
            m_pOutputWidget->m_pOutputListWidget->setItemWidget(item, itemW);
            QString cardName;
            for(cardNameMap = cardMap.begin();cardNameMap!=cardMap.end();)
            {
                if (cardNameMap.key() == at.key()) {
                    cardName = cardNameMap.value();
                    break;
                }
                ++cardNameMap;
            }
            itemW->setLabelText(at.value(),cardName);
            m_pCurrentOutputPortLabelList->append(at.value());
            currentOutputPortLabelMap.insertMulti(at.key(),at.value());
            m_pOutputWidget->m_pOutputListWidget->insertItem(i++,item);

        }

        ++at;
    }
}

void UkmediaMainWidget::addAvailableInputPort()
{
    QMap<int,QString>::iterator it;
    QMap<int,QString>::iterator at;
    QMap<int,QString>::iterator cardNameMap;
    int i;
    //增加端口
    for(it = inputPortLabelMap.begin();it!=inputPortLabelMap.end();)
    {

        for(i=0,at=currentInputPortLabelMap.begin();at!=currentInputPortLabelMap.end();i++) {
            if ( at.key() == it.key() && at.value() == it.value()) {
                break;
            }
            ++at;
        }
        //需添加到list widget
        if (i == currentInputPortLabelMap.count()) {
            UkuiListWidgetItem *itemW = new UkuiListWidgetItem(this);

            QListWidgetItem * item = new QListWidgetItem(m_pInputWidget->m_pInputListWidget);
            item->setSizeHint(QSize(200,50)); //QSize(120, 40) spacing: 12px;
            m_pInputWidget->m_pInputListWidget->setItemWidget(item, itemW);
            QString cardName;
            int count;
            for(cardNameMap = cardMap.begin();cardNameMap!=cardMap.end();)
            {
                if (cardNameMap.key() == it.key()) {
                    cardName = cardNameMap.value();
                    break;
                }
                ++count;
                ++cardNameMap;
            }
            itemW->setLabelText(it.value(),cardName);
            m_pCurrentInputPortLabelList->append(it.value());
            currentInputPortLabelMap.insertMulti(it.key(),it.value());
            qDebug() <<"current input port label insert " << it.value()<< it.key();
            m_pInputWidget->m_pInputListWidget->insertItem(i++,item);
        }

        ++it;
    }
}

//查找指定声卡名的索引
int UkmediaMainWidget::findCardIndex(QString cardName)
{
    QMap<int, QString>::iterator it;

    for(it=cardMap.begin();it!=cardMap.end();) {
        if (it.value() == cardName) {
            return it.key();
        }
        ++it;
    }
    return -1;
}

QString UkmediaMainWidget::findHighPriorityProfile(int index,QString profile)
{
    QMap<int, QMap<QString,int>>::iterator it;
    int priority = 0;
    QString profileName = "";
    QMap<QString,int> profileNameMap;
    QMap<QString,int>::iterator tempMap;
    for (it=cardProfilePriorityMap.begin();it!=cardProfilePriorityMap.end();) {
        if (it.key() == index) {
            profileNameMap = it.value();
            for (tempMap=profileNameMap.begin();tempMap!=profileNameMap.end();) {
                if ( tempMap.key().contains(profile) && tempMap.value() > priority) {
                    priority = tempMap.value();
                    profileName = tempMap.key();
                }
                ++tempMap;
            }
        }
        ++it;
    }
    return profileName;
}

void UkmediaMainWidget::findOutputListWidgetItem(QString cardName,MateMixerStream *stream)
{
    MateMixerSwitch *portSwitch = findStreamPortSwitch(this,stream);
    MateMixerSwitchOption *activePort = mate_mixer_switch_get_active_option(portSwitch);
    const gchar *portLabel = mate_mixer_switch_option_get_label(activePort);
    if (cardName == "") {
        MateMixerDevice *device = mate_mixer_stream_get_device(stream);
        cardName = mate_mixer_device_get_name(device);
    }
    for (int row=0;row<m_pOutputWidget->m_pOutputListWidget->count();row++) {

        QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->item(row);
        UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
//        qDebug() << "findOutputListWidgetItem" << "card name:" << cardName << "portLabel:" << wid->portLabel->text() << "deviceLabel:" << wid->deviceLabel->text();
        if (wid->deviceLabel->text() == cardName && wid->portLabel->text() == portLabel) {
            m_pOutputWidget->m_pOutputListWidget->blockSignals(true);
//            qDebug() << "set output list widget" << row;
            m_pOutputWidget->m_pOutputListWidget->setCurrentRow(row);
            m_pOutputWidget->m_pOutputListWidget->blockSignals(false);
            break;
        }
    }
}

void UkmediaMainWidget::findInputListWidgetItem(QString cardName,MateMixerStream *stream)
{
    MateMixerSwitch *portSwitch = findStreamPortSwitch(this,stream);
    MateMixerSwitchOption *activePort = mate_mixer_switch_get_active_option(portSwitch);

    const gchar *portLabel = mate_mixer_switch_option_get_label(activePort);

    for (int row=0;row<m_pInputWidget->m_pInputListWidget->count();row++) {
        QListWidgetItem *item = m_pInputWidget->m_pInputListWidget->item(row);
        UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pInputWidget->m_pInputListWidget->itemWidget(item);
         qDebug() << "findInputListWidgetItem" << "card name:" << cardName << "portLabel:" << wid->portLabel->text() << "deviceLabel:" << wid->deviceLabel->text() << "port" << portLabel;
        if (wid->deviceLabel->text() == cardName && wid->portLabel->text() == portLabel) {
            m_pInputWidget->m_pInputListWidget->blockSignals(true);
            m_pInputWidget->m_pInputListWidget->setCurrentRow(row);
            m_pInputWidget->m_pInputListWidget->blockSignals(false);
            if (wid->deviceLabel->text().contains("bluez_card"))
                isCheckBluetoothInput = true;
            qDebug() << "set input list widget" << row;
            break;
        }
    }
}

bool UkmediaMainWidget::inputCardListContainBluetooth()
{
    for (int i=0;i<m_pInputCardNameList->count();i++) {
        QString cardName = m_pInputCardNameList->at(i);
        if (strstr(cardName.toLocal8Bit().data(),"bluez"))
            return true;
    }
    return false;
}

QString UkmediaMainWidget::blueCardName()
{
    for (int i=0;i<m_pInputCardNameList->count();i++) {
        QString cardName = m_pInputCardNameList->at(i);
        if (strstr(cardName.toLocal8Bit().data(),"bluez"))
            return cardName;
    }
    return "";
}

int UkmediaMainWidget::indexOfOutputPortInOutputListWidget(QString portName)
{
    for (int row=0;row<m_pOutputWidget->m_pOutputListWidget->count();row++) {

        QListWidgetItem *item = m_pOutputWidget->m_pOutputListWidget->item(row);
        UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pOutputWidget->m_pOutputListWidget->itemWidget(item);
        if (wid->portLabel->text() == portName) {
            return row;
        }
    }
    return -1;
}

int UkmediaMainWidget::indexOfInputPortInInputListWidget(QString portName)
{
    for (int row=0;row<m_pInputWidget->m_pInputListWidget->count();row++) {

        QListWidgetItem *item = m_pInputWidget->m_pInputListWidget->item(row);
        UkuiListWidgetItem *wid = (UkuiListWidgetItem *)m_pInputWidget->m_pInputListWidget->itemWidget(item);
        if (wid->portLabel->text() == portName) {
            return row;
        }
    }
    return -1;
}

/*
    记录输入stream的card name
*/
void UkmediaMainWidget::inputStreamMapCardName (QString streamName,QString cardName)
{
    if (inputCardStreamMap.count() == 0) {
        inputCardStreamMap.insertMulti(streamName,cardName);
    }
    QMap<QString, QString>::iterator it;

    for (it=inputCardStreamMap.begin();it!=inputCardStreamMap.end();) {
        if (it.value() == cardName) {
            break;
        }
        if (it == inputCardStreamMap.end()-1) {
            qDebug() << "inputCardSreamMap " << streamName << cardName;
            inputCardStreamMap.insertMulti(streamName,cardName);
        }
        ++it;
    }
}

/*
    记录输出stream的card name
*/
void UkmediaMainWidget::outputStreamMapCardName(QString streamName, QString cardName)
{
    if (outputCardStreamMap.count() == 0) {
        outputCardStreamMap.insertMulti(streamName,cardName);
    }
    QMap<QString, QString>::iterator it;

    for (it=outputCardStreamMap.begin();it!=outputCardStreamMap.end();) {
        if (it.value() == cardName) {
            break;
        }
        if (it == outputCardStreamMap.end()-1) {
            qDebug() << "outputCardStreamMap " << streamName << cardName;
            outputCardStreamMap.insertMulti(streamName,cardName);
        }
        ++it;
    }
}

/*
    找输入stream对应的card name
*/
QString UkmediaMainWidget::findInputStreamCardName(QString streamName)
{
    QString cardName;
    QMap<QString, QString>::iterator it;
    for (it=inputCardStreamMap.begin();it!=inputCardStreamMap.end();) {
        if (it.key() == streamName) {
            cardName = it.value();
            qDebug() << "findInputStreamCardName:" << cardName;
            break;
        }
        ++it;
    }
    return cardName;
}

/*
    找输出stream对应的card name
*/
QString UkmediaMainWidget::findOutputStreamCardName(QString streamName)
{
    QString cardName;
    QMap<QString, QString>::iterator it;
    for (it=outputCardStreamMap.begin();it!=outputCardStreamMap.end();) {
        if (it.key() == streamName) {
            cardName = it.value();
            break;
        }
        ++it;
    }
    return cardName;
}


/*
    移除不可用的输出端口name
*/
void UkmediaMainWidget::removeOutputPortName(const pa_card_info &info)
{
    //拔插耳机的时候删除端口
    QMap<int,QString>::iterator it;
    for(it = outputPortNameMap.begin();it!=outputPortNameMap.end();)
    {
        if(it.key() == info.index)
        {
            qDebug() << "remove output port name map index" << info.index << outputPortNameMap.count() << it.value();
            it = outputPortNameMap.erase(it);
            continue;
        }
        ++it;
    }
}

/*
    移除不可用的输出端口label
*/
void UkmediaMainWidget::removeOutputPortLabel(const pa_card_info &info)
{
    QMap<int,QString>::iterator it;
    for(it = outputPortLabelMap.begin();it!=outputPortLabelMap.end();)
    {
        if(it.key() == info.index)
        {
            QString removePortLabel = it.value();
            QMap<QString,QString>::iterator removeProfileMap;
            for (removeProfileMap = profileNameMap.begin();removeProfileMap!= profileNameMap.end();) {
                if (removeProfileMap.key() == removePortLabel) {
                    removeProfileMap = profileNameMap.erase(removeProfileMap);
                    continue;
                }
                ++removeProfileMap;
            }
            it = outputPortLabelMap.erase(it);

            continue;
        }
        ++it;
    }
}

/*
    移除不可用的输出端口Name
*/
void UkmediaMainWidget::removeInputPortName(const pa_card_info &info)
{
    QMap<int,QString>::iterator it;
    for(it = inputPortNameMap.begin();it!=inputPortNameMap.end();)
    {
        if(it.key() == info.index)
        {
            it = inputPortNameMap.erase(it);
            //qDebug() << "remove input port map index" << info.index << inputPortNameMap.count();
            continue;
        }
        ++it;
    }
}

void UkmediaMainWidget::removeInputPortLabel(const pa_card_info &info)
{
    QMap<int,QString>::iterator it;
    for(it = inputPortLabelMap.begin();it!=inputPortLabelMap.end();)
    {
        if(it.key() == info.index)
        {
            QString removePortLabel = it.value();
            QMap<QString,QString>::iterator removeProfileMap;
            for (removeProfileMap = inputPortProfileNameMap.begin();removeProfileMap!= inputPortProfileNameMap.end();) {
                if (removeProfileMap.key() == removePortLabel) {
                    removeProfileMap = inputPortProfileNameMap.erase(removeProfileMap);
                    continue;
                }
                ++removeProfileMap;
            }
            it = inputPortLabelMap.erase(it);
            qDebug() << "remove input port Label map index" << info.index << inputPortLabelMap.count();
            continue;
        }
        ++it;
    }
}
