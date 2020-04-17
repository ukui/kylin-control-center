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
#include <QMediaPlayer>

#include <gio/gio.h>
#include <libxml/tree.h>
#include <libmatemixer/matemixer.h>
#include <glib-object.h>
#include <glib.h>
#include <gobject/gparamspecs.h>
#include <glib/gi18n.h>
#include <utime.h>
#include <glib/gstdio.h>
#include <QDomDocument>
#include <QGSettings>

#define UKUI_THEME_SETTING "org.ukui.style"
#define UKUI_THEME_NAME "style-name"
#define UKUI_THEME_WHITE "ukui-white"
#define UKUI_THEME_BLACK "ukui-black"

#define KEY_SOUNDS_SCHEMA "org.mate.sound"
#define EVENT_SOUNDS_KEY "event-sounds"
#define INPUT_SOUNDS_KEY "input-feedback-sounds"
#define SOUND_THEME_KEY "theme-name"

#define DEFAULT_ALERT_ID "__default"
#define CUSTOM_THEME_NAME "__custom"
#define NO_SOUNDS_THEME_NAME "__no_sounds"

typedef enum
{
    GVC_LEVEL_SCALE_LINEAR,
    GVC_LEVEL_SCALE_LOG
} LevelScale;
class UkmediaMainWidget : public QWidget
{
    Q_OBJECT

public:
    UkmediaMainWidget(QWidget *parent = nullptr);
    ~UkmediaMainWidget();
    void inputVolumeDarkThemeImage(int value);
    void outputVolumeDarkThemeIamge(int value);
    int getInputVolume();
    int getOutputVolume();

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

    static void playAlretSoundFromPath (QString path);
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
    static void delete_old_files (const char **sounds);
    static void delete_one_file (const char *sound_name, const char *pattern);
    static void delete_disabled_files (const char **sounds);
    static void add_custom_file (const char **sounds, const char *filename);
    static gboolean capplet_file_delete_recursive (GFile *file, GError **error);
    static gboolean directory_delete_recursive (GFile *directory, GError **error);
    static void create_custom_theme (const char *parent);
    static void custom_theme_update_time (void);
    static gboolean custom_theme_dir_is_empty (void);

Q_SIGNALS:
    void appVolumeChangedSignal(bool is_mute,int volume,const QString app_name);

private Q_SLOTS:
    void themeComboxIndexChangedSlot(int index);
    void comboxIndexChangedSlot(int index);
    void outputDeviceComboxIndexChangedSlot(QString str);
    void inputDeviceComboxIndexChangedSlot(QString str);
    void inputLevelValueChangedSlot();
    void outputWidgetSliderChangedSlot(int value);
    void inputWidgetSliderChangedSlot(int value);
    void ukuiThemeChangedSlot(const QString &);
private:

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

    QStringList *m_pSoundList;
    QStringList *m_pThemeDisplayNameList;
    QStringList *m_pThemeNameList;
    QStringList *m_pDeviceNameList;
    QStringList *m_pAppNameList;

    QStringList *m_pOutputStreamList;
    QStringList *m_pInputStreamList;
    QStringList *m_pAppVolumeList;
    QStringList *m_pStreamControlList;
    GSettings *m_pSoundSettings;
    LevelScale scale;
    gdouble peakFraction;
    gdouble maxPeak;
    guint maxPeakId;

    QGSettings *m_pThemeSetting;
    QString mThemeName;
};

#endif // WIDGET_H
