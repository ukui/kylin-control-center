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
#include <XdgIcon>
#include <XdgDesktopFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QStringList>
#include <QSpacerItem>
#include <QListView>
#include <QScrollBar>
#include <QGSettings>
#include <qmath.h>
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_DIALOG_DBUS_NAME "org.mate.VolumeControl"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"
#define GVC_SOUND_SOUND    (xmlChar *) "sound"
#define GVC_SOUND_NAME     (xmlChar *) "name"
#define GVC_SOUND_FILENAME (xmlChar *) "filename"
#define SOUND_SET_DIR "/usr/share/ukui-media/sounds"
guint appnum = 0;
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

UkmediaMainWidget::UkmediaMainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pOutputWidget = new UkmediaOutputWidget();
    m_pInputWidget = new UkmediaInputWidget();
    m_pSoundWidget = new UkmediaSoundEffectsWidget();

    mThemeName = UKUI_THEME_WHITE;
    QVBoxLayout *m_pvLayout = new QVBoxLayout();
    m_pvLayout->addWidget(m_pOutputWidget);
    m_pvLayout->addWidget(m_pInputWidget);
    m_pvLayout->addWidget(m_pSoundWidget);
    m_pvLayout->setSpacing(48);
    this->setLayout(m_pvLayout);
    this->setMinimumWidth(582);
    this->setMaximumWidth(910);
//    this->setStyleSheet("QWidget{background: white;}");

    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }

    m_pSoundList = new QStringList;
    m_pThemeNameList = new QStringList;
    m_pThemeDisplayNameList = new QStringList;
    m_pDeviceNameList = new QStringList;
    m_pOutputStreamList = new QStringList;
    m_pInputStreamList = new QStringList;
    m_pAppVolumeList = new QStringList;
    m_pStreamControlList = new QStringList;
    m_pAppNameList = new QStringList;
    //创建context
    m_pContext = mate_mixer_context_new();

    mate_mixer_context_set_app_name (m_pContext,_("Volume Control"));//设置app名
    mate_mixer_context_set_app_icon(m_pContext,"multimedia-volume-control");

    //打开context
    if G_UNLIKELY (mate_mixer_context_open(m_pContext) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }

    g_param_spec_object ("context",
                        "Context",
                        "MateMixer context",
                        MATE_MIXER_TYPE_CONTEXT,
                        (GParamFlags)(G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_STATIC_STRINGS));

    if (mate_mixer_context_get_state (m_pContext) == MATE_MIXER_STATE_CONNECTING) {

    }

    //当出现获取输入输出异常时，使用默认的输入输出stream
    m_pInputStream = mate_mixer_context_get_default_input_stream(m_pContext);
    m_pOutputStream = mate_mixer_context_get_default_output_stream(m_pContext);
    contextSetProperty(this);
//    点击输出设备
    connect(m_pOutputWidget->m_pOutputDeviceCombobox,SIGNAL(currentIndexChanged(QString)),this,SLOT(outputDeviceComboxIndexChangedSlot(QString)));
//    点击输入设备
    connect(m_pInputWidget->m_pInputDeviceCombobox,SIGNAL(currentIndexChanged(QString)),this,SLOT(inputDeviceComboxIndexChangedSlot(QString)));

    g_signal_connect (G_OBJECT (m_pContext),
                     "notify::state",
                     G_CALLBACK (onContextStateNotify),
                     this);

    //设置滑动条的最大值为100
    m_pInputWidget->m_pIpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMinimum(-100);
    m_pOutputWidget->m_pOpBalanceSlider->setSingleStep(100);
    m_pInputWidget->m_pInputLevelSlider->setMaximum(100);
    m_pInputWidget->m_pInputLevelSlider->setEnabled(false);
    //设置声音主题
    //获取声音gsettings值
    m_pSoundSettings = g_settings_new (KEY_SOUNDS_SCHEMA);

    g_signal_connect (G_OBJECT (m_pSoundSettings),
                             "changed",
                             G_CALLBACK (onKeyChanged),
                             this);
    setupThemeSelector(this);
    updateTheme(this);

    //报警声音,从指定路径获取报警声音文件
    populateModelFromDir(this,SOUND_SET_DIR);

    //检测系统主题
    if (QGSettings::isSchemaInstalled(UKUI_THEME_SETTING)){
        m_pThemeSetting = new QGSettings(UKUI_THEME_SETTING);
    }
    if (m_pThemeSetting->keys().contains("styleName")) {
        mThemeName = m_pThemeSetting->get(UKUI_THEME_NAME).toString();
    }
    connect(m_pThemeSetting, SIGNAL(changed(const QString &)),this,SLOT(ukuiThemeChangedSlot(const QString &)));

    //检测设计开关机音乐
    if (QGSettings::isSchemaInstalled(UKUI_SWITCH_SETTING)) {
        m_pBootSetting = new QGSettings(UKUI_SWITCH_SETTING);
    }
    if (m_pBootSetting->keys().contains("bootMusic")) {
        m_hasMusic = m_pBootSetting->get(UKUI_BOOT_MUSIC_KEY).toBool();
    }

    connect(m_pBootSetting,SIGNAL(changed(const QString &)),this,SLOT(bootMusicSettingsChanged()));
    connect(m_pSoundWidget->m_pBootButton,SIGNAL(checkedChanged(bool)),this,SLOT(bootButtonSwitchChangedSlot(bool)));
    //输出音量控制
    //输出滑动条音量控制
    connect(m_pOutputWidget->m_pOpVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(outputWidgetSliderChangedSlot(int)));
    //输入滑动条音量控制
    connect(m_pInputWidget->m_pIpVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(inputWidgetSliderChangedSlot(int)));

    //点击报警音量时播放报警声音
    connect(m_pSoundWidget->m_pShutdownCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pLagoutCombobox ,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pSoundThemeCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(themeComboxIndexChangedSlot(int)));
    connect(m_pInputWidget->m_pInputLevelSlider,SIGNAL(valueChanged(int)),this,SLOT(inputLevelValueChangedSlot()));
    //输入等级
    ukuiInputLevelSetProperty(this);
}

/*
    是否播放开关机音乐
*/
void UkmediaMainWidget::bootButtonSwitchChangedSlot(bool status)
{
    bool bBootStatus = true;
    if (m_pBootSetting->keys().contains("bootMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_BOOT_MUSIC_KEY).toBool();
        if (bBootStatus != status) {
            m_pBootSetting->set(UKUI_BOOT_MUSIC_KEY,status);
        }
    }
}

void UkmediaMainWidget::bootMusicSettingsChanged()
{
    bool bBootStatus = true;
    bool status = m_pSoundWidget->m_pBootButton->isChecked();
    if (m_pBootSetting->keys().contains("bootMusic")) {
        bBootStatus = m_pBootSetting->get(UKUI_BOOT_MUSIC_KEY).toBool();
        if (status != bBootStatus ) {
            m_pSoundWidget->m_pBootButton->setChecked(bBootStatus);
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
        updateDefaultInputStream (m_pWidget);
        updateIconOutput(m_pWidget);
        updateIconInput(m_pWidget);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {
        UkuiMessageBox::critical(m_pWidget,tr("sound error"),tr("load sound failed"),UkuiMessageBox::Yes | UkuiMessageBox::No,UkuiMessageBox::Yes);
        g_debug(" mate mixer state failed");
    }
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
    g_debug("on conext stored control add");
    MateMixerStreamControl *m_pControl;
    MateMixerStreamControlMediaRole mediaRole;
    m_pControl = MATE_MIXER_STREAM_CONTROL (mate_mixer_context_get_stored_control (m_pContext, m_pName));
    if (G_UNLIKELY (m_pControl == nullptr))
        return;

    mediaRole = mate_mixer_stream_control_get_media_role (m_pControl);
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT)
        ukuiBarSetStreamControl (m_pWidget,MATE_MIXER_DIRECTION_UNKNOWN, m_pControl);
}

/*
    当其他设备插入时添加这个stream
*/
void UkmediaMainWidget::onContextStreamAdded (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on context stream added");
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

    const GList *switchList;
    MateMixerSwitch *swt;
    switchList = mate_mixer_stream_list_switches(m_pStream);
    while (switchList != nullptr) {
        swt = MATE_MIXER_SWITCH(switchList->data);
        //            MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(optionList->data);
        MateMixerSwitchOption *opt = mate_mixer_switch_get_active_option(swt);
        const char *name = mate_mixer_switch_option_get_name(opt);
        const char *label = mate_mixer_switch_option_get_label(opt);
        qDebug() << "opt name:" << name << "opt label:" << label;
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
        if (m_pStream == m_pInput) {
            ukuiBarSetStream (m_pWidget, m_pStream);
            m_pControl = mate_mixer_stream_get_default_control(m_pStream);
            updateInputSettings (m_pWidget,m_pControl);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        m_pWidget->m_pInputStreamList->append(m_pName);
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->addItem(m_pLabel);
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *m_pOutput;
        MateMixerStreamControl *m_pControl;
        m_pOutput = mate_mixer_context_get_default_output_stream (m_pContext);
        m_pControl = mate_mixer_stream_get_default_control (m_pStream);
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        if (m_pStream == m_pOutput) {
            updateOutputSettings(m_pWidget,m_pControl);
            ukuiBarSetStream (m_pWidget, m_pStream);
        }

        if (m_pStream == m_pOutput) {
            updateOutputSettings(m_pWidget,m_pControl);
            ukuiBarSetStream (m_pWidget, m_pStream);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        m_pWidget->m_pOutputStreamList->append(m_pName);
        m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->addItem(m_pLabel);
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
    g_debug("remove stream");
    int index;
    index = m_pWidget->m_pInputStreamList->indexOf(m_pName);
    if (index >= 0) {
        m_pWidget->m_pInputStreamList->removeAt(index);
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->removeItem(index);
    }
    else {
        index = m_pWidget->m_pOutputStreamList->indexOf(m_pName);
        if (index >= 0) {
            m_pWidget->m_pOutputStreamList->removeAt(index);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->removeItem(index);
        }
    }
    if (m_pWidget->m_pAppVolumeList != nullptr) {
        ukuiBarSetStream(m_pWidget,nullptr);
    }
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
}

/*
    添加设备
*/
void UkmediaMainWidget::addDevice (UkmediaMainWidget *m_pWidget, MateMixerDevice *m_pDevice)
{
    g_debug("add device");
    const gchar *m_pName;
    /*
     * const gchar *m_pLabel;
     * m_pLabel = mate_mixer_device_get_label (m_pDevice);
    */
    m_pName  = mate_mixer_device_get_name (m_pDevice);
    m_pWidget->m_pDeviceNameList->append(m_pName);
}

/*
    移除设备
*/
void UkmediaMainWidget::onContextDeviceRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    g_debug("on context device removed");
    int index = m_pWidget->m_pDeviceNameList->indexOf(m_pName);
    if (index >= 0)
        m_pWidget->m_pDeviceNameList->removeAt(index);
}

/*
    默认输入流通知
*/
void UkmediaMainWidget::onContextDefaultInputStreamNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug ("on context default input stream notify");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pContext);
    if (m_pStream == nullptr) {
        //当输入流更改异常时，使用默认的输入流，不应该发生这种情况
        m_pStream = m_pWidget->m_pInputStream;
    }
    QString deviceName = mate_mixer_stream_get_label(m_pStream);
    int index = m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->findText(deviceName);
    if (index < 0)
        return;
    m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->setCurrentIndex(index);
    updateIconInput(m_pWidget);

    setInputStream(m_pWidget, m_pStream);
}

void UkmediaMainWidget::setInputStream(UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream)
{
    g_debug("set input stream");
    if (m_pStream == nullptr) {
        return;
    }

    MateMixerStreamControl *m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    if (m_pControl != nullptr) {
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, false);
    }
    ukuiBarSetStream (m_pWidget, m_pStream);

    if (m_pStream != nullptr) {
        const GList *m_pControls;
        m_pControls = mate_mixer_context_list_stored_controls (m_pWidget->m_pContext);

        /* Move all stored controls to the newly selected default stream */
        while (m_pControls != nullptr) {
            MateMixerStream *parent;

            m_pControl = MATE_MIXER_STREAM_CONTROL (m_pControls->data);
            parent  = mate_mixer_stream_control_get_stream (m_pControl);

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
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, false);
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
    m_pStream = mate_mixer_context_get_default_output_stream (m_pContext);

    if (m_pStream == nullptr) {
        qDebug() << "on context default output steam notify:" << "stream is null";
        //当输出流更改异常时，使用默认的输入流，不应该发生这种情况
        m_pStream = m_pWidget->m_pOutputStream;
    }
    QString deviceName = mate_mixer_stream_get_label(m_pStream);
    int index = m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->findText(deviceName);
    if (index < 0)
        return;
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->setCurrentIndex(index);
    updateIconOutput(m_pWidget);
    setOutputStream (m_pWidget, m_pStream);
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
    if (mThemeName == UKUI_THEME_WHITE) {
        color = QColor(0,0,0,216);
    }
    else if (mThemeName == UKUI_THEME_BLACK) {
        color = QColor(255,255,255,216);
    }
    m_pOutputWidget->m_pOutputIconBtn->mColor = color;
    if (status) {
        image  = QImage("/usr/share/ukui-media/img/audio-volume-muted.svg");
        m_pOutputWidget->m_pOutputIconBtn->mImage = image;
    }
    else if (value <= 0) {
        image  = QImage("/usr/share/ukui-media/img/audio-volume-muted.svg");
        m_pOutputWidget->m_pOutputIconBtn->mImage = image;
    }
    else if (value > 0 && value <= 33) {
        image = QImage("/usr/share/ukui-media/img/audio-volume-low.svg");
        m_pOutputWidget->m_pOutputIconBtn->mImage = image;
    }
    else if (value >33 && value <= 66) {
        image = QImage("/usr/share/ukui-media/img/audio-volume-medium.svg");
        m_pOutputWidget->m_pOutputIconBtn->mImage = image;
    }
    else {
        image = QImage("/usr/share/ukui-media/img/audio-volume-high.svg");
        m_pOutputWidget->m_pOutputIconBtn->mImage = image;
    }

}

/*
    输入音量图标
*/
void UkmediaMainWidget::inputVolumeDarkThemeImage(int value,bool status)
{
    QImage image;
    QColor color = QColor(0,0,0,190);
    if (mThemeName == UKUI_THEME_WHITE) {
        color = QColor(0,0,0,190);
    }
    else if (mThemeName == UKUI_THEME_BLACK) {
        color = QColor(255,255,255,190);
    }
    m_pInputWidget->m_pInputIconBtn->mColor = color;
    if (status) {
        image  = QImage("/usr/share/ukui-media/img/microphone-mute.svg");
        m_pInputWidget->m_pInputIconBtn->mImage = image;
    }
    else if (value <= 0) {
        image  = QImage("/usr/share/ukui-media/img/microphone-mute.svg");
        m_pInputWidget->m_pInputIconBtn->mImage = image;
    }
    else if (value > 0 && value <= 33) {
        image = QImage("/usr/share/ukui-media/img/microphone-low.svg");
        m_pInputWidget->m_pInputIconBtn->mImage = image;
    }
    else if (value >33 && value <= 66) {
        image = QImage("/usr/share/ukui-media/img/microphone-medium.svg");
        m_pInputWidget->m_pInputIconBtn->mImage = image;
    }
    else {
        image = QImage("/usr/share/ukui-media/img/microphone-high.svg");
        m_pInputWidget->m_pInputIconBtn->mImage = image;
    }
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
    const GList *m_pInputs =mate_mixer_stream_list_controls(m_pStream);
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);

    m_pWidget->m_pStream = m_pStream;
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    bool status = mate_mixer_stream_control_get_mute(m_pControl);
    int value = volume *100 /65536.0+0.5;
    m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setFocusPolicy(Qt::NoFocus);
//    m_pWidget->m_pInputWidget->m_pInputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

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
                show = TRUE;
                break;
            }
            if (strcmp (m_pAppId, "org.mate.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.gnome.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.PulseAudio.pavucontrol") != 0) {
                g_debug ("Found a recording application %s", m_pAppId);

                if G_UNLIKELY (m_pControl == nullptr)
                    m_pControl = input;

                show = TRUE;
                break;
            }
        }
        m_pInputs = m_pInputs->next;
    }

    if (show == TRUE) {
        g_debug ("Input icon enabled");
    }
    else {
        g_debug ("There is no recording application, input icon disabled");
    }
    streamStatusIconSetControl(m_pWidget, m_pControl);

    if (m_pControl != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }
    //开始监听输入等级
    if (show == TRUE ) {
        flags = mate_mixer_stream_control_get_flags(m_pControl);
        if (m_pWidget->m_pDeviceStr == UKUI_INPUT_REAR_MIC || m_pWidget->m_pDeviceStr == UKUI_INPUT_FRONT_MIC || m_pWidget->m_pDeviceStr == UKUI_OUTPUT_HEADPH) {
            mate_mixer_stream_control_set_monitor_enabled(m_pControl,true);
        }
        /* Enable level bar only if supported by the control */
        if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
        }
    }
    else if(show == FALSE) {
        mate_mixer_stream_control_set_monitor_enabled(m_pControl,false);
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

    streamStatusIconSetControl(m_pWidget, m_pControl);
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    bool status = mate_mixer_stream_control_get_mute(m_pControl);
    int value = volume *100 /65536.0+0.5;

    m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setFocusPolicy(Qt::NoFocus);
//    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    const QSize icon_size = QSize(24,24);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIconSize(icon_size);
    m_pWidget->outputVolumeDarkThemeImage(value,status);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->repaint();
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
    MateMixerStreamControlFlags flags;
    guint volume = 0;
    QString decscription;

    if (m_pControl != nullptr)
        flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags&MATE_MIXER_STREAM_CONTROL_VOLUME_READABLE) {
        volume = mate_mixer_stream_control_get_volume(m_pControl);
    }

    decscription = mate_mixer_stream_control_get_label(m_pControl);
    MateMixerDirection direction;
    MateMixerStream *m_pStream = mate_mixer_stream_control_get_stream(m_pControl);

    if (MATE_MIXER_IS_STREAM(m_pStream)) {

        qDebug() << "get stream correct" << mate_mixer_stream_control_get_label(m_pControl) << mate_mixer_stream_get_label(m_pStream);
    }
    else {
        m_pStream = m_pWidget->m_pStream;
        direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(m_pStream));
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
//            mate_mixer_context_set_default_output_stream(m_pWidget->m_pContext,m_pStream);
            setOutputStream(m_pWidget,m_pStream);
            qDebug() << "从control 获取的stream不为output stream" << mate_mixer_stream_get_label(m_pStream);
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
//            mate_mixer_context_set_default_input_stream(m_pWidget->m_pContext,m_pStream);
            qDebug() << "从control 获取的stream不为input stream" << mate_mixer_stream_get_label(m_pStream);
            setInputStream(m_pWidget,m_pStream);
        }
    }

    direction = mate_mixer_stream_get_direction(m_pStream);
    //设置输出滑动条的值
    int value = volume*100/65536.0 + 0.5;
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
    }
}

/*
    设置平衡属性
*/
void UkmediaMainWidget::ukuiBalanceBarSetProperty (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("ukui balance bar set property");
    ukuiBalanceBarSetControl(m_pWidget,m_pControl);
}

/*
    平衡设置control
*/
void UkmediaMainWidget::ukuiBalanceBarSetControl (UkmediaMainWidget *m_pWidget, MateMixerStreamControl *m_pControl)
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
    MateMixerStreamControlFlags flags;
    if (m_pControl == nullptr) {
        return;
    }

    flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags & MATE_MIXER_STREAM_CONTROL_CAN_BALANCE) {
        ukuiBalanceBarSetProperty(m_pWidget,m_pControl);
    }

    connect(m_pWidget->m_pOutputWidget->m_pOpBalanceSlider,&QSlider::valueChanged,[=](int volume){
        gdouble value = volume/100.0;
        mate_mixer_stream_control_set_balance(m_pControl,value);
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
        //设置主题到combox中
        qDebug() << "sound theme in dir" << "displayname:" << m_pIndexName << "theme name:" << m_pName;
        m_pWidget->m_pThemeDisplayNameList->append(m_pIndexName);
        m_pWidget->m_pThemeNameList->append(m_pName);
        m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->addItem(m_pIndexName);
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
    qDebug() << "set combox for theme name" << name;
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
//        if (index == -1) {
//            return;
//        }
        value = m_pWidget->m_pThemeNameList->at(index);
        m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->setCurrentIndex(index);
    }
    /* When we can't find the theme we need to set, try to set the default
     * one "freedesktop" */
/*    if (found) {
    }*/ else if (strcmp (name, "freedesktop") != 0) {//设置为默认的主题
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

    qDebug() << "namestr:" << nameStr << "themeStr:" << themeStr << "parent:" << parent << "theme:" << theme;
    if (! is_custom && is_default) {
        /* remove custom just in case */
        remove_custom = TRUE;
    } else if (! is_custom && ! is_default) {
        create_custom_theme (parent);
        saveAlertSounds(pWidget->m_pSoundWidget->m_pSoundThemeCombobox, alertId);
        add_custom = TRUE;
    } else if (is_custom && is_default) {
        saveAlertSounds(pWidget->m_pSoundWidget->m_pSoundThemeCombobox, alertId);
        /* after removing files check if it is empty */
        if (custom_theme_dir_is_empty ()) {
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
    d = g_dir_open (dirname, 0, nullptr);
    if (d == nullptr) {
        return;
    }
    while ((name = g_dir_read_name (d)) != nullptr) {
        char *path;

        if (! g_str_has_suffix (name, ".xml")) {
            continue;
        }
        path = g_build_filename (dirname, name, nullptr);
        populateModelFromFile (m_pWidget, path);
        g_free (path);
    }
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

    //将找到的声音文件名设置到combox中
    if (filename != nullptr && name != nullptr) {
        m_pWidget->m_pSoundList->append((const char *)filename);
        m_pWidget->m_pSoundWidget->m_pShutdownCombobox->addItem((char *)name);
        m_pWidget->m_pSoundWidget->m_pLagoutCombobox->addItem((char *)name);
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
void UkmediaMainWidget::playAlretSoundFromPath (QString path)
{
    g_debug("play alert sound from path");
   QMediaPlayer *player = new QMediaPlayer;
   player->setMedia(QUrl::fromLocalFile(path));
   player->play();
   qDebug() << path << player->state() << player->mediaStatus();
//   player->deleteLater();
   connect(player,&QMediaPlayer::stateChanged,[=](QMediaPlayer::State state){
        switch (state) {
        case QMediaPlayer::StoppedState:
            break;
        case QMediaPlayer::PlayingState:
            break;
        default:
            break;
        }
        player->deleteLater() ;
   });
}

/*
    点击combox播放声音
*/
void UkmediaMainWidget::comboxIndexChangedSlot(int index)
{
    g_debug("combox index changed slot");
    QString sound_name = m_pSoundList->at(index);
    playAlretSoundFromPath(sound_name);

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
    qDebug() << "index changed:" << index << m_pThemeNameList->at(index) << m_pThemeName;
    g_settings_set_string (m_pSoundSettings, SOUND_THEME_KEY, m_pThemeName);

    /* special case for no sounds */
    if (strcmp (m_pThemeName, NO_SOUNDS_THEME_NAME) == 0) {
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
    g_debug("output device combox index changed slot");
    MateMixerBackendFlags flags;
    int index = m_pOutputWidget->m_pOutputDeviceCombobox->findText(str);
    if (index == -1)
        return;
    const QString str1 =  m_pOutputStreamList->at(index);
    const gchar *name = str1.toLocal8Bit();
    MateMixerStream *stream = mate_mixer_context_get_stream(m_pContext,name);
    if (G_UNLIKELY (stream == nullptr)) {
       g_warn_if_reached ();
//       g_free (name);
       return;
    }

    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {

        mate_mixer_context_set_default_output_stream (m_pContext, stream);
        m_pStream = stream;
        MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
        int(mate_mixer_stream_control_get_volume(c) *100 /65536.0+0.5);
        /*miniWidget->masterVolumeSlider->setValue(volume);*/
    }
    else {
        setOutputStream(this, stream);
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
    if (G_UNLIKELY (stream == nullptr)) {
       g_warn_if_reached ();
//       g_free (name);
       return;
    }

    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {
        m_pStream = stream;
        mate_mixer_context_set_default_input_stream (m_pContext, stream);
        MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
        /*int volume = */int(mate_mixer_stream_control_get_volume(c) *100 /65536.0+0.5);
//        miniWidget->masterVolumeSlider->setValue(volume);
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
    m_pStream = mate_mixer_context_get_default_output_stream(m_pContext);
    if (m_pStream != nullptr)
        m_pControl = mate_mixer_stream_get_default_control(m_pStream);

    QString percent;
    bool status = false;
    percent = QString::number(value);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(m_pControl,guint(volume));
    if (value <= 0) {
        status = true;
        mate_mixer_stream_control_set_mute(m_pControl,status);
        mate_mixer_stream_control_set_volume(m_pControl,0);
        percent = QString::number(0);
    }

    outputVolumeDarkThemeImage(value,status);
    mate_mixer_stream_control_set_mute(m_pControl,status);
    percent.append("%");
    m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
    m_pOutputWidget->m_pOutputIconBtn->repaint();

}

/*
    滚动输入滑动条
*/
void UkmediaMainWidget::inputWidgetSliderChangedSlot(int value)
{
    m_pStream = mate_mixer_context_get_default_input_stream(m_pContext);
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);

    QString percent;
    bool status = false;
    if (value <= 0) {
        status = true;
        mate_mixer_stream_control_set_mute(m_pControl,status);
        mate_mixer_stream_control_set_volume(m_pControl,0);
        percent = QString::number(0);
    }
    //输入图标修改成深色主题

    inputVolumeDarkThemeImage(value,status);
    m_pInputWidget->m_pInputIconBtn->repaint();
    percent = QString::number(value);
    value = value * 65536 / 100;
    mate_mixer_stream_control_set_mute(m_pControl,status);
    mate_mixer_stream_control_set_volume(m_pControl,value);
    percent.append("%");
    m_pInputWidget->m_pInputIconBtn->repaint();
    m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
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

    level = m_pWidget->m_pInputWidget->m_pInputLevelSlider->value();
    min = m_pWidget->m_pInputWidget->m_pInputLevelSlider->minimum();
    max = m_pWidget->m_pInputWidget->m_pInputLevelSlider->maximum();

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

    /* Get the control currently associated with the input slider */
    if (m_pControl == nullptr)
        return;

    flags = mate_mixer_stream_control_get_flags (m_pControl);

    /* Enable level bar only if supported by the control */
    if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
        g_signal_connect (G_OBJECT (m_pControl),
                          "monitor-value",
                          G_CALLBACK (onStreamControlMonitorValue),
                          m_pWidget);
    }

    /* Get owning stream of the control */
    stream = mate_mixer_stream_control_get_stream (m_pControl);
    if (G_UNLIKELY (stream == nullptr))
        return;
}

void UkmediaMainWidget::onStreamControlMonitorValue (MateMixerStream *m_pStream,gdouble value,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    g_debug("on stream control monitor value");
    value = value*100;
    if (value >= 0) {
//        qDebug() << "设置输入等级的值为:" << value;
        m_pWidget->m_pInputWidget->m_pInputLevelSlider->setValue(value);
    }
    else {
        m_pWidget->m_pInputWidget->m_pInputLevelSlider->setValue(0);
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
        delete_old_files (sounds);
        delete_disabled_files (sounds);
    } else {
        delete_old_files (sounds);
        delete_disabled_files (sounds);
        add_custom_file (sounds, id);
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

void UkmediaMainWidget::delete_old_files (const char **sounds)
{
    guint i;
    for (i = 0; sounds[i] != NULL; i++) {
        delete_one_file (sounds[i], "%s.ogg");
    }
}

void UkmediaMainWidget::delete_one_file (const char *sound_name, const char *pattern)
{
        GFile *file;
        char *name, *filename;

        name = g_strdup_printf (pattern, sound_name);
        filename = customThemeDirPath(name);
        g_free (name);
        file = g_file_new_for_path (filename);
        g_free (filename);
        capplet_file_delete_recursive (file, NULL);
        g_object_unref (file);
}


void UkmediaMainWidget::delete_disabled_files (const char **sounds)
{
    guint i;
    for (i = 0; sounds[i] != NULL; i++) {
        delete_one_file (sounds[i], "%s.disabled");
    }
}

void UkmediaMainWidget::add_custom_file (const char **sounds, const char *filename)
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
gboolean UkmediaMainWidget::capplet_file_delete_recursive (GFile *file, GError **error)
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
        return directory_delete_recursive (file, error);
    }
    else {
        return g_file_delete (file, NULL, error);
    }
}

gboolean UkmediaMainWidget::directory_delete_recursive (GFile *directory, GError **error)
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
            success = directory_delete_recursive (child, error);
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

void UkmediaMainWidget::create_custom_theme (const char *parent)
{
    GKeyFile *keyfile;
    char     *data;
    char     *path;

    /* Create the custom directory */
    path = customThemeDirPath(NULL);
    g_mkdir_with_parents (path, 0755);
    g_free (path);

    qDebug() << "create_custom_theme" << parent;
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

    custom_theme_update_time ();
}

/* This function needs to be called after each individual
 * changeset to the theme */
void UkmediaMainWidget::custom_theme_update_time (void)
{
    char *path;
    path = customThemeDirPath (NULL);
    utime (path, NULL);
    g_free (path);
}

gboolean UkmediaMainWidget::custom_theme_dir_is_empty (void)
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

UkmediaMainWidget::~UkmediaMainWidget()
{
//    delete player;
}
