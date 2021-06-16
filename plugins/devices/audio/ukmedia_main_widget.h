
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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "ukmedia_output_widget.h"
#include "ukmedia_input_widget.h"
#include "ukmedia_sound_effects_widget.h"
#include "ukui_list_widget_item.h"
#include <QMediaPlayer>
#include <gio/gio.h>
#include <libxml/tree.h>
#include <libmatemixer/matemixer.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gobject/gparamspecs.h>
#include <glib/gstdio.h>
extern "C" {
#include <dconf/dconf.h>
#include <canberra.h>
#include <glib/gmain.h>
#include <pulse/ext-stream-restore.h>
#include <pulse/glib-mainloop.h>
#include <pulse/error.h>
#include <pulse/subscribe.h>
#include <pulse/introspect.h>
}
#include <utime.h>
#include <a.out.h>
#include <QScreen>
#include <QApplication>
#include <QDomDocument>
#include <QGSettings>
#include <QAudioInput>
#include <set>

#define UKUI_THEME_SETTING "org.ukui.style"
#define UKUI_THEME_NAME "style-name"
#define UKUI_THEME_WHITE "ukui-white"
#define UKUI_THEME_BLACK "ukui-black"

#define UKUI_INPUT_REAR_MIC "analog-input-rear-mic"  //后置麦克风
#define UKUI_INPUT_FRONT_MIC "analog-input-front-mic" //前置麦克风
#define UKUI_OUTPUT_HEADPH "analog-output-headphones" //模拟耳机

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.media.sound"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/sound/keybindings/"
#define MAX_CUSTOM_SHORTCUTS 1000
#define FILENAME_KEY "filename"
#define NAME_KEY "name"

#define KEY_SOUNDS_SCHEMA "org.ukui.sound"
#define UKUI_SWITCH_SETTING "org.ukui.session"
#define UKUI_STARTUP_MUSIC_KEY "startup-music"
#define UKUI_POWEROFF_MUSIC_KEY "poweroff-music"
#define UKUI_LOGOUT_MUSIC_KEY "logout-music"
#define UKUI_WAKEUP_MUSIC_KEY "weakup-music"

#define EVENT_SOUNDS_KEY "event-sounds"
#define INPUT_SOUNDS_KEY "input-feedback-sounds"
#define SOUND_THEME_KEY "theme-name"

#define DEFAULT_ALERT_ID "__default"
#define CUSTOM_THEME_NAME "__custom"
#define NO_SOUNDS_THEME_NAME "__no_sounds"

#ifdef __GNUC__
#define CA_CLAMP(x, low, high)                                          \
        __extension__ ({ typeof(x) _x = (x);                            \
                        typeof(low) _low = (low);                       \
                        typeof(high) _high = (high);                    \
                        ((_x > _high) ? _high : ((_x < _low) ? _low : _x)); \
                })
#else
#define CA_CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

typedef enum
{
    GVC_LEVEL_SCALE_LINEAR,
    GVC_LEVEL_SCALE_LOG
} LevelScale;

class PortInfo {
public:

      QByteArray name;
      QByteArray description;
      uint32_t priority;
      int available;
      int direction;
      int64_t latency_offset;
      std::vector<QByteArray> profiles;

};

class UkmediaMainWidget : public QWidget
{
    Q_OBJECT

public:
    UkmediaMainWidget(QWidget *parent = nullptr);
    ~UkmediaMainWidget();

    static int connectContext(gpointer userdata);
    static int caProplistMergeAp(ca_proplist *p, va_list ap);
    static int caPlayForWidget(UkmediaMainWidget *w, uint32_t id, ...);
    static int caProplistSetForWidget(ca_proplist *p, UkmediaMainWidget *widget);

    QPixmap drawDarkColoredPixmap(const QPixmap &source);
    QPixmap drawLightColoredPixmap(const QPixmap &source);
    void updateProfileOption();
    void alertIconButtonSetIcon(bool state,int value);
    void createAlertSound(UkmediaMainWidget *w);
    void inputVolumeDarkThemeImage(int value,bool status);
    void outputVolumeDarkThemeImage(int value,bool status);
    int getInputVolume();
    int getOutputVolume();
    bool getInputMuteStatus();
    bool getOutputMuteStatus();
    void comboboxCurrentTextInit();
    QList<char *> listExistsPath();
    QString findFreePath();
    void addValue(QString name,QString filename);

    static void listDevice(UkmediaMainWidget *w,MateMixerContext *context);
    static void streamStatusIconSetControl (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void contextSetProperty(UkmediaMainWidget *object);//guint prop_id,const GValue *value,GParamSpec *pspec);
    static void onContextStateNotify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget	*w);

    static void onContextStreamAdded (MateMixerContext *context,const gchar *name,UkmediaMainWidget  *w);
    static void onContextStreamRemoved (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void removeStream (UkmediaMainWidget *w, const gchar *name);

    static void addStream (UkmediaMainWidget *w, MateMixerStream *stream,MateMixerContext *context);
    static void addApplicationControl (UkmediaMainWidget *w, MateMixerStreamControl *control);
    static void onStreamControlAdded (MateMixerStream *stream,const gchar *name,UkmediaMainWidget  *w);
    static void onStreamControlRemoved (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);

    static void onContextStoredControlAdded (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void onContextDeviceAdded (MateMixerContext *context, const gchar *name, UkmediaMainWidget *w);
    static void addDevice (UkmediaMainWidget *w, MateMixerDevice *device);
    static void onContextDeviceRemoved (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);

    static void onContextDefaultInputStreamNotify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget *w);
    static void setInputStream (UkmediaMainWidget *w, MateMixerStream *stream);
    static void onStreamControlMuteNotify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);

    static void onContextDefaultOutputStreamNotify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget *w);
    static void onContextStoredControlRemoved (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void setContext(UkmediaMainWidget *w,MateMixerContext *context);

    static void updateIconInput (UkmediaMainWidget *w);
    static void updateIconOutput (UkmediaMainWidget *w);
    static void onStreamControlVolumeNotify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    static void onControlMuteNotify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    //平衡
    static void ukuiBalanceBarSetProperty (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void ukuiBalanceBarSetControl (UkmediaMainWidget *w, MateMixerStreamControl *control);

    static void onBalanceValueChanged (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    static void updateOutputSettings (UkmediaMainWidget *w,MateMixerStreamControl *control);

    static void onKeyChanged (GSettings *settings,gchar *key,UkmediaMainWidget *w);
    static void updateTheme (UkmediaMainWidget *w);

    static void setupThemeSelector (UkmediaMainWidget *w);
    static void soundThemeInDir (UkmediaMainWidget *w,GHashTable *hash,const char *dir);
    static char *loadIndexThemeName (const char *index,char **parent);

    static void setComboxForThemeName (UkmediaMainWidget *w,const char *name);
    static void updateAlertsFromThemeName (UkmediaMainWidget *w,const gchar *name);
    static void updateAlert (UkmediaMainWidget *w,const char *alert_id);
    static int getFileType (const char *sound_name,char **linked_name);
    static char *customThemeDirPath (const char *child);

    static void populateModelFromDir (UkmediaMainWidget *w,const char *dirname);
    static void populateModelFromFile (UkmediaMainWidget *w,const char *filename);
    static void populateModelFromNode (UkmediaMainWidget *w,xmlNodePtr node);
    static xmlChar *xmlGetAndTrimNames (xmlNodePtr node);

    static void playAlretSoundFromPath (UkmediaMainWidget *w,QString path);
    static void setOutputStream (UkmediaMainWidget *w, MateMixerStream *stream);
    static void updateOutputStreamList(UkmediaMainWidget *w,MateMixerStream *stream);
    static void ukuiBarSetStream (UkmediaMainWidget *w ,MateMixerStream *stream);
    static void ukuiBarSetStreamControl (UkmediaMainWidget *w,MateMixerDirection direction,MateMixerStreamControl *control);

    static void updateInputSettings (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void onStreamControlMonitorValue (MateMixerStream *stream,gdouble value,UkmediaMainWidget *w);
    void ukuiInputLevelSetProperty (UkmediaMainWidget *w);
    void ukuiInputLevelSetScale (UkmediaMainWidget *w, LevelScale scale);
    static void ukuiUpdatePeakValue (UkmediaMainWidget *w);

    static gdouble ukuiFractionFromAdjustment(UkmediaMainWidget  *w);
    static void onInputStreamControlAdded (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);
    static void onInputStreamControlRemoved (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);
    static gboolean updateDefaultInputStream (UkmediaMainWidget *w);

    static gboolean saveAlertSounds (QComboBox *combox,const char *id);
    static void deleteOldFiles (const char **sounds);
    static void deleteOneFile (const char *sound_name, const char *pattern);
    static void deleteDisabledFiles (const char **sounds);
    static void addCustomFile (const char **sounds, const char *filename);
    static gboolean cappletFileDeleteRecursive (GFile *file, GError **error);
    static gboolean directoryDeleteRecursive (GFile *directory, GError **error);
    static void createCustomTheme (const char *parent);
    static void customThemeUpdateTime (void);
    static gboolean customThemeDirIsEmpty (void);
    static MateMixerSwitch *findStreamPortSwitch (UkmediaMainWidget *widget,MateMixerStream *stream);
    static MateMixerSwitch *findDeviceProfileSwitch (UkmediaMainWidget *widget,MateMixerDevice *device);
    static void onSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w);
    static void onDeviceProfileActiveOptionNotify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w);
    static gchar *deviceStatus (MateMixerDevice *device);
    static void updateDeviceInfo (UkmediaMainWidget *w, MateMixerDevice *device);
    void updateOutputDevicePort();
    void updateInputDevicePort();
    static void onInputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w);
    static void onOutputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,UkmediaMainWidget *w);

    //为一些不能更改提示音音量的机器做一些初始化操作
    void executeVolumeUpdate(bool status);
    pa_context* get_context(void);
    void show_error(const char *txt);
    static void context_state_callback(pa_context *c, void *userdata);
    gboolean connect_to_pulse(gpointer userdata);
    void setConnectingMessage(const char *string);
    void createEventRole();
    void updateRole(const pa_ext_stream_restore_info &info);
    static void ext_stream_restore_read_cb(pa_context *,const pa_ext_stream_restore_info *i,int eol,void *userdata);
    static void ext_stream_restore_subscribe_cb(pa_context *c, void *userdata);
    static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata);
    void updateCard(const pa_card_info &info);
    static void card_cb(pa_context *, const pa_card_info *i, int eol, void *userdata);

    static void updatePorts(UkmediaMainWidget *w,const pa_card_info &info, std::map<QByteArray, PortInfo> &ports);

    void deleteNotAvailableOutputPort();
    void deleteNotAvailableInputPort();
    void addAvailableOutputPort();
    void addAvailableInputPort();

    void removeOutputPortName(const pa_card_info &info);  //移除不可用输出端口name
    void removeOutputPortLabel(const pa_card_info &info);  //移除不可用的输出端口label
    void removeInputPortName(const pa_card_info &info);  //移除不可用输入端口Name
    void removeInputPortLabel(const pa_card_info &info);  //移除不可用输入端口Label

    int findCardIndex(QString cardName);
    QString findHighPriorityProfile(int index,QString profile);
    void findOutputListWidgetItem(QString cardName,MateMixerStream *stream);
    void findInputListWidgetItem(QString cardName,MateMixerStream *stream);
    bool inputCardListContainBluetooth();
    int indexOfOutputPortInOutputListWidget(QString portName);
    int indexOfInputPortInInputListWidget(QString portName);
    void inputStreamMapCardName(QString streamName,QString cardName);
    void outputStreamMapCardName(QString streamName,QString cardName);
    QString findInputStreamCardName(QString streamName);
    QString findOutputStreamCardName(QString streamName);
    void setOutputListWidgetRow(); //设置输出设备
    bool exitBluetoochDevice();
    QString blueCardName(); //记录蓝牙声卡名称

Q_SIGNALS:
    void appVolumeChangedSignal(bool is_mute,int volume,const QString app_name);

private Q_SLOTS:
    void themeComboxIndexChangedSlot(int index);
    void comboxIndexChangedSlot(int index);
    void outputDeviceComboxIndexChangedSlot(QString str);
    void inputDeviceComboxIndexChangedSlot(QString str);
    void inputLevelValueChangedSlot();
    void outputWidgetSliderChangedSlot(int value);
    void timeSliderSlot();
    void inputWidgetSliderChangedSlot(int value);
    void ukuiThemeChangedSlot(const QString &);
    void startupButtonSwitchChangedSlot(bool status);
    void poweroffButtonSwitchChangedSlot(bool status);
    void logoutMusicButtonSwitchChangedSlot(bool status);
    void wakeButtonSwitchChangedSlot(bool status);
    void alertSoundButtonSwitchChangedSlot(bool status);
    void bootMusicSettingsChanged();
    void inputPortComboxChangedSlot(int index);
    void outputPortComboxChangedSlot(int index);
    void windowClosedComboboxChangedSlot(int index);
    void volumeChangedComboboxChangeSlot(int index);
    void settingMenuComboboxChangedSlot(int index);
    void profileComboboxChangedSlot(int index);
    void selectComboboxChangedSlot(int index);
    void inputMuteButtonSlot();
    void outputMuteButtonSlot();
    void alertVolumeSliderChangedSlot(int value);
    void alertSoundVolumeChangedSlot();
    void outputListWidgetCurrentRowChangedSlot(int row);
    void inputListWidgetCurrentRowChangedSlot(int row);

private:
    UkmediaInputWidget *m_pInputWidget;
    UkmediaOutputWidget *m_pOutputWidget;
    UkmediaSoundEffectsWidget *m_pSoundWidget;

    MateMixerContext *m_pContext;
    MateMixerStream *m_pInputStream;
    MateMixerStream *m_pOutputStream;
    MateMixerStream *m_pInput;
    MateMixerStreamControl *m_pOutputBarStreamControl;
    MateMixerStreamControl *m_pInputBarStreamControl;
    MateMixerStreamControl *m_pControl;
    MateMixerStreamControl *m_pMediaRoleControl;
    MateMixerStreamControl *m_pPrivInputControl;
    MateMixerStream *m_pStream;
    MateMixerDevice *m_pDevice;
    MateMixerSwitch *m_pSwitch;

    QStringList *m_pSoundList;
    QStringList *m_pThemeDisplayNameList;
    QStringList *m_pThemeNameList;
    QStringList *m_pDeviceLabelList;
    QStringList *m_pDeviceNameList;
    QStringList *m_pInputDeviceLabelList;
    QStringList *m_pAppNameList;

    QStringList *m_pOutputStreamList;
    QStringList *m_pPrivOutputStreamList;
    QStringList *m_pInputStreamList;
    QStringList *m_pAppVolumeList;
    QStringList *m_pStreamControlList;
    QStringList *m_pInputPortList;
    QStringList *m_pOutputPortList;
    QStringList *m_pOutputPortLabelList;
    QStringList *m_pInputPortLabelList;
    QStringList *m_pProfileNameList;
    QStringList *m_pSoundThemeList;
    QStringList *m_pSoundThemeDirList;
    QStringList *m_pSoundThemeXmlNameList;
    QStringList *m_pListWidgetLabelList;

    QStringList *m_pCurrentOutputCardList;
    QStringList *m_pOutputCardList;
    QStringList *m_pCurrentOutputPortLabelList;
    QStringList *m_pCurrentInputPortLabelList;
    QStringList *m_pCurrentInputCardList;
    QStringList *m_pInputCardList;
    QStringList *m_pSoundNameList;
    QStringList *eventList;
    QStringList *eventIdNameList;
    QStringList *m_pCardNameList;
    QStringList *m_pInputCardNameList;
    QString m_pDeviceStr;

    GSettings *m_pSoundSettings;
    LevelScale scale;
    gdouble peakFraction;
    gdouble maxPeak;
    guint maxPeakId;

    QGSettings *m_pBootSetting;
    QGSettings *m_pThemeSetting;
//    QGSettings *m_pWindowClosedSetting;
    QString mThemeName;
    bool m_hasMusic;
    bool firstEnterSystem = true;

    const gchar* m_privOutputPortLabel = "";
    QByteArray role;
    QByteArray device;
    pa_channel_map channelMap;
    pa_cvolume volume;
    pa_context* context ;
    pa_mainloop_api* api;
    pa_ext_stream_restore_info info;

    int callBackCount = 0;
    bool firstEntry = true;
    bool hasSinks;
    bool hasSources;
    QByteArray activeProfile;
    QByteArray noInOutProfile;
    QByteArray lastActiveProfile;
    std::map<QByteArray, PortInfo> ports;
    std::vector< std::pair<QByteArray,QByteArray> > profiles;
    QMap<int, QString> cardMap;
    QMap<int, QString> outputPortNameMap;
    QMap<int, QString> inputPortNameMap;
    QMap<int, QString> outputPortLabelMap;
    QMap<int, QString> currentOutputPortLabelMap;
    QMap<int, QString> currentInputPortLabelMap;
    QMap<int, QString> inputPortLabelMap;
    QMap<QString, QString> profileNameMap;
    QMap<QString, QString> inputPortProfileNameMap;
    QMap<int, QList<QString>> cardProfileMap;
    QMap<int, QMap<QString,int>> cardProfilePriorityMap;
    QMap<QString,QString> inputCardStreamMap;
    QMap<QString,QString> outputCardStreamMap;
    bool updatePort = true;
    bool setDefaultstream = true;
    int reconnectTime;
    QTimer *time;

    QTimer *timeSlider;
    bool mousePress = false;
    bool mouseReleaseState = false;

    QTimer *timeSliderBlance;
    bool mousePressBlance = false;
    bool mouseReleaseStateBlance = false;
};

#endif // WIDGET_H
