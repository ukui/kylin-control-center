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
#include <QWidget>
#include <QDebug>

#include "defaultapp.h"
#include "ui_defaultapp.h"
#include "addappdialog.h"

#define BROWSERTYPE "x-scheme-handler/http"
#define MAILTYPE "x-scheme-handler/mailto"
#define IMAGETYPE "image/png"
#define AUDIOTYPE "audio/x-vorbis+ogg"
#define VIDEOTYPE "video/x-ogm+ogg"
#define TEXTTYPE "text/plain"

#define DESKTOPPATH "/usr/share/applications/"

DefaultApp::DefaultApp() {
    ui = new Ui::DefaultAppWindow;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Default App");
    pluginType = SYSTEM;
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    initUI();

    connect(ui->ResetBtn, SIGNAL(clicked(bool)), this, SLOT(resetDefaultApp()));
}

DefaultApp::~DefaultApp() {
    delete ui;
}

QString DefaultApp::get_plugin_name() {
    return pluginName;
}

int DefaultApp::get_plugin_type() {
    return pluginType;
}

QWidget *DefaultApp::get_plugin_ui() {
    return pluginWidget;
}

void DefaultApp::plugin_delay_control() {

}

const QString DefaultApp::name() const {

    return QStringLiteral("defaultapp");
}

void DefaultApp::initUI() {

    //~ contents_path /defaultapp/Browser
    ui->browserLabel->setText(tr("Browser"));
    //~ contents_path /defaultapp/Mail
    ui->mailLabel->setText(tr("Mail"));
    //~ contents_path /defaultapp/Image Viewer
    ui->imageLabel->setText(tr("Image Viewer"));
    //~ contents_path /defaultapp/Audio Player
    ui->audioLabel->setText(tr("Audio Player"));
    //~ contents_path /defaultapp/Video Player
    ui->videoLabel->setText(tr("Video Player"));
    //~ contents_path /defaultapp/Text Editor
    ui->textLabel->setText(tr("Text Editor"));


    // BROWSER
    int browserindex = -1;
    QString currentbrowser(getDefaultAppId(BROWSERTYPE)); //获取当前
    AppList * list = getAppIdList(BROWSERTYPE); //获取可选列表
    if (list) {
        for (int i = 0; list[i].appid != NULL; i++) {
            QString single(list[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * browserinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(browserinfo));
            //        qDebug() << appname ;
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(browserinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            ui->browserComBoBox->addItem(appicon, appname, single);
            if (currentbrowser == single)
                browserindex = i;
            free(list[i].appid);
        }
        free(list);
    }
    ui->browserComBoBox->setCurrentIndex(browserindex);
    browserComBoBox_changed_cb(browserindex);
    connect(ui->browserComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(browserComBoBox_changed_cb(int)));

    // MAIL
    int mailindex = -1;
    QString currentmail(getDefaultAppId(MAILTYPE));

    AppList * maillist = getAppIdList(MAILTYPE);
    if (maillist) {
        for (int i = 0; maillist[i].appid != NULL; i++) {
            QString single(maillist[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * mailinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(mailinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(mailinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            ui->mailComBoBox->addItem(appicon, appname, single);
            if (currentmail == single)
                mailindex = i;
            free(maillist[i].appid);
        }
        free(maillist);
    }
    ui->mailComBoBox->setCurrentIndex(mailindex);
    mailComBoBox_changed_cb(mailindex);
    connect(ui->mailComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(mailComBoBox_changed_cb(int)));

    // IMAGE
    int imageindex = -1;
    QString currentimage(getDefaultAppId(IMAGETYPE));
    QStringList browserList;
    AppList * imagelist = getAppIdList(IMAGETYPE);

    for(int i = 0; i < ui->browserComBoBox->count(); i++) {
        browserList << ui->browserComBoBox->itemText(i);
    }
    if (imagelist) {
        for (int i = 0; imagelist[i].appid != NULL; i++) {
            QString single(imagelist[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * imageinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(imageinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(imageinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            if(!browserList.contains(appname)){
                ui->imageComBoBox->addItem(appicon, appname, single);
                free(imagelist[i].appid);
            }
        }
        free(imagelist);
    }
    for(int i = 0; i < ui->imageComBoBox->count(); i++) {
        if(currentimage == ui->imageComBoBox->itemData(i)) {
            imageindex = i;
        }
    }
    ui->imageComBoBox->setCurrentIndex(imageindex);
    imageComBoBox_changed_cb(imageindex);
    connect(ui->imageComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(imageComBoBox_changed_cb(int)));

    // AUDIO
    int audioindex = -1;
    QString currentaudio(getDefaultAppId(AUDIOTYPE));

    AppList * audiolist = getAppIdList(AUDIOTYPE);
    if (audiolist) {
        for (int i = 0; audiolist[i].appid != NULL; i++) {
            QString single(audiolist[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * audioinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(audioinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(audioinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            ui->audioComBoBox->addItem(appicon, appname, single);
            if (currentaudio == single)
                audioindex = i;
            free(audiolist[i].appid);
        }
        free(audiolist);
    }
    ui->audioComBoBox->setCurrentIndex(audioindex);
    audioComBoBox_changed_cb(audioindex);
    connect(ui->audioComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(audioComBoBox_changed_cb(int)));

    // VIDEO
    int videoindex =-1;
    QString currentvideo(getDefaultAppId(VIDEOTYPE));

    AppList * videolist = getAppIdList(VIDEOTYPE);
    if (videolist) {
        for (int i = 0; videolist[i].appid != NULL; i++) {
            QString single(videolist[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * videoinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(videoinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(videoinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            ui->videoComBoBox->addItem(appicon, appname, single);
            if (currentvideo == single)
                videoindex = i;
            free(videolist[i].appid);
        }
        free(videolist);
    }
    ui->videoComBoBox->setCurrentIndex(videoindex);
    videoComBoBox_changed_cb(videoindex);
    connect(ui->videoComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(videoComBoBox_changed_cb(int)));

    // TEXT
    int textindex = -1;
    QString currenttext(getDefaultAppId(TEXTTYPE));

    AppList * textlist = getAppIdList(TEXTTYPE);
    if (textlist){
        for (int i = 0; textlist[i].appid != NULL; i++) {
            QString single(textlist[i].appid);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * textinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(textinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(textinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));

            ui->textComBoBox->addItem(appicon, appname, single);
            if (currenttext == single)
                textindex = i;
            free(textlist[i].appid);
        }
        free(textlist);
    }
    ui->textComBoBox->setCurrentIndex(textindex);
    textComBoBox_changed_cb(textindex);
    connect(ui->textComBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(textComBoBox_changed_cb(int)));
}

void DefaultApp::browserComBoBox_changed_cb(int index) {
//    QString appid = ui->browserComBoBox->currentData().toString();
    QString appid = ui->browserComBoBox->itemData(index).toString();
    if (appid == "add") {
//        AddAppDialog * dialog = new AddAppDialog();
//        dialog->show();
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setWebBrowsersDefaultProgram(ba.data());
    }
}

void DefaultApp::mailComBoBox_changed_cb(int index) {
    QString appid = ui->mailComBoBox->itemData(index).toString();
    if (appid == "add"){
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setMailReadersDefaultProgram(ba.data());
    }
}

void DefaultApp::imageComBoBox_changed_cb(int index) {
    QString appid = ui->imageComBoBox->itemData(index).toString();
    if (appid == "add"){
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setImageViewersDefaultProgram(ba.data());
    }
}

void DefaultApp::audioComBoBox_changed_cb(int  index) {
    qDebug() << "this is audio:" << endl;

    QString appid = ui->audioComBoBox->itemData(index).toString();
    if (appid == "add"){
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setAudioPlayersDefaultProgram(ba.data());
    }
}

void DefaultApp::videoComBoBox_changed_cb(int index) {
    QString appid = ui->videoComBoBox->itemData(index).toString();
    if (appid == "add") {
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setVideoPlayersDefaultProgram(ba.data());
    }
}

void DefaultApp::textComBoBox_changed_cb(int index) {
    QString appid = ui->textComBoBox->itemData(index).toString();
    if (appid == "add"){
        qDebug() << "add clicked";
    } else {
        QByteArray ba = appid.toUtf8(); // QString to char *
        setTextEditorsDefautlProgram(ba.data());
    }
}

void DefaultApp::resetDefaultApp() {
    ui->browserComBoBox->setCurrentIndex(0);
    ui->mailComBoBox->setCurrentIndex(0);
    ui->imageComBoBox->setCurrentIndex(0);
    ui->audioComBoBox->setCurrentIndex(0);
    ui->videoComBoBox->setCurrentIndex(0);
    ui->textComBoBox->setCurrentIndex(0);
}

char * DefaultApp::getDefaultAppId(const char * contentType) {
    GAppInfo * app = g_app_info_get_default_for_type(contentType, false);
    if(app != NULL){
        const char * id = g_app_info_get_id(app);
        if(id != NULL){
            gint len = strlen(id);
            char * appid = (char *)malloc(sizeof(char)*(len+1));
            strcpy(appid,id);
            return appid;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

AppList * DefaultApp::getAppIdList(const char *contentType) {
    Appinfo *appinfo = _getAppList(contentType);
    if(appinfo != NULL){
        int i = 0;
        while(appinfo[i].item != NULL)
            i++;
        AppList *list = (AppList *)malloc(sizeof(AppList)*(i+1));
        int count = i;
        int index = 0;
        for(gint j = 0;appinfo[j].item != NULL;j++) {
            const char *id = g_app_info_get_id(appinfo[j].item);
            if (id != NULL) {
                int len = strlen(id);
                list[index].appid = (char *)malloc(sizeof(char)*(len+1));
                strcpy(list[index].appid,id);
                index++;
            } else {
                free(list+count);
                count--;
            }
        }
        list[count].appid=NULL;
        free(appinfo);
        return list;
    } else {
        return NULL;
    }
}

Appinfo * DefaultApp::_getAppList(const char *contentType) {
    GList *applist;
    applist = g_app_info_get_all_for_type(contentType);
    GAppInfo * item;

    if (applist != NULL) {
        int len = g_list_length(applist);
        Appinfo * appinfo=(Appinfo *)malloc(sizeof(Appinfo)*(len+1));

        //获取应用列表
        for (int index=0; index < len; index++) {
            item = (GAppInfo*) g_list_nth_data(applist, index);
            appinfo[index].item=item;
        }
        appinfo[len].item=NULL;
        return appinfo;

    } else {
        return NULL;
    }
}

bool DefaultApp::setWebBrowsersDefaultProgram(char * appid) {
    const char * content_type = "x-scheme-handler/http";
    Appinfo * appinfo = _getAppList(content_type);
    bool judge = false;
    if (appinfo != NULL) {
        for(int i = 0; appinfo[i].item != NULL; i++) {
            const char *id = g_app_info_get_id(appinfo[i].item);
            int result = strcmp(id,appid);
            if (0 == result) {
                GAppInfo *appitem=appinfo[i].item;
                gboolean ret1=g_app_info_set_as_default_for_type(appitem, "x-scheme-handler/http", NULL);
                gboolean ret2=g_app_info_set_as_default_for_type(appitem, "x-scheme-handler/https", NULL);
                gboolean ret3=g_app_info_set_as_default_for_type(appitem, "x-scheme-handler/about", NULL);
                gboolean ret4=g_app_info_set_as_default_for_type(appitem, "text/html", NULL);
                if(ret1 && ret2 && ret3 && ret4)
                    judge=true;
                break;
            }
        }
        free(appinfo);
    }
    return judge;
}

bool DefaultApp::setMailReadersDefaultProgram(char *appid) {
    const char *content_type="x-scheme-handler/mailto";
    Appinfo *appinfo=_getAppList(content_type);

    bool judge = false;
    if (appinfo != NULL) {
        for (int i = 0; appinfo[i].item != NULL; i++) {
            const char * id = g_app_info_get_id(appinfo[i].item);
            int result=strcmp(id,appid);
            if (0 == result) {
                GAppInfo *appitem=appinfo[i].item;
                gboolean ret1=g_app_info_set_as_default_for_type(appitem, "x-scheme-handler/mailto", NULL);
                gboolean ret2=g_app_info_set_as_default_for_type(appitem, "application/x-extension-eml", NULL);
                gboolean ret3=g_app_info_set_as_default_for_type(appitem, "message/rfc822", NULL);
                if(ret1 && ret2 && ret3)
                    judge=true;
                break;
            }
        }
        free(appinfo);
    }
    return judge;
}

bool DefaultApp::setImageViewersDefaultProgram(char *appid) {
    const char *content_type="image/png";
    Appinfo *appinfo = _getAppList(content_type);

    bool judge = false;
    if (appinfo != NULL) {
        for (int i=0;appinfo[i].item!=NULL;i++){
            const char *id = g_app_info_get_id(appinfo[i].item);
            int result = strcmp(id, appid);
            if (0 == result) {
                GAppInfo *appitem=appinfo[i].item;
                gboolean ret1 = g_app_info_set_as_default_for_type(appitem, "image/bmp", NULL);
                gboolean ret2 = g_app_info_set_as_default_for_type(appitem, "image/gif", NULL);
                gboolean ret3 = g_app_info_set_as_default_for_type(appitem, "image/jpeg", NULL);
                gboolean ret4 = g_app_info_set_as_default_for_type(appitem, "image/png", NULL);
                gboolean ret5 = g_app_info_set_as_default_for_type(appitem, "image/tiff", NULL);
                if(ret1 && ret2 && ret3 && ret4 && ret5)
                    judge=true;
                break;
            }
        }
        free(appinfo);
    }

    return judge;
}

bool DefaultApp::setVideoPlayersDefaultProgram(char *appid) {
    const char *content_type = "video/x-ogm+ogg";
    Appinfo * appinfo = _getAppList(content_type);

    bool judge = false;
    if (appinfo != NULL) {
        for(int i = 0; appinfo[i].item != NULL; i++) {
            const char *id = g_app_info_get_id(appinfo[i].item);
            int result = strcmp(id,appid);
            if (0 == result) {
                GAppInfo *appitem = appinfo[i].item;
                gboolean ret1 = g_app_info_set_as_default_for_type(appitem, "video/mp4", NULL);
                gboolean ret2 = g_app_info_set_as_default_for_type(appitem, "video/mpeg", NULL);
                gboolean ret3 = g_app_info_set_as_default_for_type(appitem, "video/mp2t", NULL);
                gboolean ret4 = g_app_info_set_as_default_for_type(appitem, "video/msvideo", NULL);
                gboolean ret5 = g_app_info_set_as_default_for_type(appitem, "video/quicktime", NULL);
                gboolean ret6 = g_app_info_set_as_default_for_type(appitem, "video/webm", NULL);
                gboolean ret7 = g_app_info_set_as_default_for_type(appitem, "video/x-avi", NULL);
                gboolean ret8 = g_app_info_set_as_default_for_type(appitem, "video/x-flv", NULL);
                gboolean ret9 = g_app_info_set_as_default_for_type(appitem, "video/x-matroska", NULL);
                gboolean ret10 = g_app_info_set_as_default_for_type(appitem, "video/x-mpeg", NULL);
                gboolean ret11 = g_app_info_set_as_default_for_type(appitem, "video/x-ogm+ogg", NULL);
                if (ret1 && ret2 && ret3 && ret4 && ret5 && ret6 && ret7 && ret8 && ret9 && ret10 && ret11)
                    judge=true;
                break;
            }
        }
        free(appinfo);
    }
    return judge;
}

bool DefaultApp::setAudioPlayersDefaultProgram(char *appid) {
    const char *content_type = "audio/x-vorbis+ogg";
    Appinfo * appinfo = _getAppList(content_type);

    bool judge = false;
    if (appinfo != NULL) {
        for(int i = 0; appinfo[i].item != NULL ;i++) {
            const char *id = g_app_info_get_id(appinfo[i].item);
            int result = strcmp(id,appid);
            if (0 == result) {
                GAppInfo *appitem=appinfo[i].item;
                gboolean ret1 = g_app_info_set_as_default_for_type(appitem, "audio/mpeg", NULL);
                gboolean ret2 = g_app_info_set_as_default_for_type(appitem, "audio/x-mpegurl", NULL);
                gboolean ret3 = g_app_info_set_as_default_for_type(appitem, "audio/x-scpls", NULL);
                gboolean ret4 = g_app_info_set_as_default_for_type(appitem, "audio/x-vorbis+ogg", NULL);
                gboolean ret5 = g_app_info_set_as_default_for_type(appitem, "audio/x-wav", NULL);
                gboolean ret6 = g_app_info_set_as_default_for_type(appitem, "audio/x-ms-wma", NULL);
                gboolean ret7 = g_app_info_set_as_default_for_type(appitem, "audio/x-flac", NULL);
                gboolean ret8 = g_app_info_set_as_default_for_type(appitem, "audio/ac3", NULL);
                gboolean ret9 = g_app_info_set_as_default_for_type(appitem, "audio/acc", NULL);
                gboolean ret10 = g_app_info_set_as_default_for_type(appitem, "audio/aac", NULL);
                gboolean ret11 = g_app_info_set_as_default_for_type(appitem, "audio/mp4", NULL);
                gboolean ret12 = g_app_info_set_as_default_for_type(appitem, "audio/x-m4r", NULL);
                gboolean ret13 = g_app_info_set_as_default_for_type(appitem, "audio/midi", NULL);
                gboolean ret14 = g_app_info_set_as_default_for_type(appitem, "audio/mp2", NULL);
                gboolean ret15 = g_app_info_set_as_default_for_type(appitem, "audio/x-wavpack", NULL);
                gboolean ret16 = g_app_info_set_as_default_for_type(appitem, "audio/x-ape", NULL);
                if(ret1 && ret2 && ret3 && ret4 && ret5 && ret6 && ret7 && ret8 && ret9 && ret10 &&
                   ret11 && ret12 && ret13 && ret14 && ret15 && ret16) {
                    judge=true;
                }
                break;
            }
        }
        free(appinfo);
    }
    return judge;
}

bool DefaultApp::setTextEditorsDefautlProgram(char *appid) {
    const char * content_type = "text/plain";
    Appinfo * appinfo = _getAppList(content_type);

    bool judge = false;
    if (appinfo != NULL) {
        for (int i = 0; appinfo[i].item != NULL; i++) {
            const char * id = g_app_info_get_id(appinfo[i].item);
            int result = strcmp(id,appid);
            if (0 == result) {
                GAppInfo *appitem = appinfo[i].item;
                gboolean ret1 = g_app_info_set_as_default_for_type(appitem, "text/plain", NULL);
                if(ret1)
                    judge=true;
                break;
            }
        }
        free(appinfo);
    }

    return judge;
}
