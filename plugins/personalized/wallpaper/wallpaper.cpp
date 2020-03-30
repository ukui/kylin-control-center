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
#include "wallpaper.h"
#include "ui_wallpaper.h"

#include <QDBusReply>
#include <QDBusInterface>

#include "pictureunit.h"
#include "MaskWidget/maskwidget.h"

#include <QDebug>

enum{
    PICTURE, //图片背景
    COLOR, //纯色背景
    SLIDESHOW //幻灯片背景
};

#define ITEMWIDTH 182
#define ITEMHEIGH 126

#define COLORITEMWIDTH 56
#define COLORITEMHEIGH 56

Wallpaper::Wallpaper()
{
    ui = new Ui::Wallpaper;
    itemDelege = new QStyledItemDelegate();
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("background");
    pluginType = PERSONALIZED;

    settingsCreate = false;

    //设置样式
    setupQStylesheet();
    //初始化控件
    setupComponent();
    //初始化gsettings
    const QByteArray id(BACKGROUND);
    if (QGSettings::isSchemaInstalled(id)){
        settingsCreate = true;

        bgsettings = new QGSettings(id);
        setupConnect();
        initBgFormStatus();
    }
    //构建xmlhandle对象
    xmlhandleObj = new XmlHandle();

}

Wallpaper::~Wallpaper()
{
    delete ui;
    if (settingsCreate){
        delete bgsettings;
    }

    delete xmlhandleObj;

}

QString Wallpaper::get_plugin_name(){
    return pluginName;
}

int Wallpaper::get_plugin_type(){
    return pluginType;
}

QWidget *Wallpaper::get_plugin_ui(){
    return pluginWidget;
}

void Wallpaper::plugin_delay_control(){
}

void Wallpaper::setupQStylesheet(){
    pluginWidget->setStyleSheet("background: #ffffff;");

    ui->previewLabel->setStyleSheet("QLabel#previewLabel{border-radius: 6px;}");

    ui->switchWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

//    ui->listWidget->setStyleSheet("QListWidget#listWidget{background: #ffffff; border: none;}");
//    ui->colorListWidget->setStyleSheet("QListWidget#colorListWidget{background: #ffffff; border: none;}");

    QString btnQss = QString("QPushButton{background: #E9E9E9; border-radius: 4px;}"
                             "QPushButton:hover:!pressed{background: #3d6be5; border: none; border-radius: 4px;}"
                             "QPushButton:hover:pressed{background: #415FC4; border: none; border-radius: 4px;}");
    ui->browserLocalwpBtn->setStyleSheet(btnQss);
    ui->browserOnlinewpBtn->setStyleSheet(btnQss);

    ui->resetBtn->setStyleSheet("QPushButton{border: none;}");
}

void Wallpaper::setupComponent(){

    ui->browserLocalwpBtn->hide();
    ui->browserOnlinewpBtn->hide();
    //背景形式
    QStringList formList;
    formList << tr("picture") << tr("color")/* << tr("slideshow")*/ ;
    ui->formComBox->setItemDelegate(itemDelege);
    ui->formComBox->setMaxVisibleItems(5);
//    ui->formComBox->addItems(formList);
    ui->formComBox->addItem(formList.at(0), PICTURE);
    ui->formComBox->addItem(formList.at(1), COLOR);
//    ui->formComBox->addItem(formList.at(2), SLIDESHOW);

    ui->picOptionsComBox->setItemDelegate(itemDelege);
    ui->picOptionsComBox->setMaxVisibleItems(5);

    //预览遮罩
    MaskWidget * maskWidget = new MaskWidget(ui->previewLabel);
    maskWidget->setGeometry(0, 0, ui->previewLabel->width(), ui->previewLabel->height());

    ///图片背景
    picFlowLayout = new FlowLayout(ui->picListWidget);
    picFlowLayout->setContentsMargins(0, 0, 0, 0);
    ui->picListWidget->setLayout(picFlowLayout);
    //纯色背景
    colorFlowLayout = new FlowLayout(ui->colorListWidget);
    colorFlowLayout->setContentsMargins(0, 0, 0, 0);
    ui->colorListWidget->setLayout(colorFlowLayout);

    //壁纸放置方式
    ui->picOptionsComBox->addItem(tr("wallpaper"), "wallpaper");
    ui->picOptionsComBox->addItem(tr("centered"), "centered");
    ui->picOptionsComBox->addItem(tr("scaled"), "scaled");
    ui->picOptionsComBox->addItem(tr("stretched"), "stretched");
    ui->picOptionsComBox->addItem(tr("zoom"), "zoom");
    ui->picOptionsComBox->addItem(tr("spanned"), "spanned");
}

void Wallpaper::setupConnect(){
    //使用线程构建本地壁纸文件；获取壁纸压缩QPixmap
    pThread = new QThread;
    pObject = new WorkerObject;
    connect(pObject, &WorkerObject::pixmapGenerate, this, [=](QPixmap pixmap, QString filename){
        PictureUnit * picUnit = new PictureUnit;
        picUnit->setPixmap(pixmap);
        picUnit->setFilenameText(filename);
        connect(picUnit, &PictureUnit::clicked, [=](QString fn){
            ui->previewLabel->setPixmap(pixmap.scaled(ui->previewLabel->size()));
            bgsettings->set(FILENAME, fn);
            ui->previewStackedWidget->setCurrentIndex(PICTURE);
        });

        picFlowLayout->addWidget(picUnit);

    });
    connect(pObject, &WorkerObject::workComplete, this, [=](QMap<QString, QMap<QString, QString>> wpInfoMaps){
        wallpaperinfosMap = wpInfoMaps;
        pThread->quit(); //退出事件循环
        pThread->wait(); //释放资源
    });

    pObject->moveToThread(pThread);
    connect(pThread, &QThread::started, pObject, &WorkerObject::run);
    connect(pThread, &QThread::finished, this, [=]{
//        if (ui->formComBox->currentIndex() == PICTURE){
            //设置当前壁纸放置方式
//            if (wallpaperinfosMap.contains(filename)){
//                QMap<QString, QString> currentwpMap = wallpaperinfosMap.value(filename);
//                if (currentwpMap.contains("options")){
//                    QString opStr = QString::fromLocal8Bit("%1").arg(currentwpMap.value("options"));
//                    ui->picOptionsComBox->blockSignals(true);
//                    ui->picOptionsComBox->setCurrentText(tr("%1").arg(opStr));
//                    ui->picOptionsComBox->blockSignals(false);
//                }
//            }
//        }
    });
    connect(pThread, &QThread::finished, pObject, &WorkerObject::deleteLater);

    pThread->start();

    connect(ui->picOptionsComBox, SIGNAL(currentTextChanged(QString)), this, SLOT(wpOptionsChangedSlot(QString)));
    connect(ui->resetBtn, SIGNAL(clicked(bool)), this, SLOT(resetDefaultWallpaperSlot()));

    ///纯色背景
    QStringList colors;

    colors << "#2d7d9a" << "#018574" << "#107c10" << "#10893e" << "#038387" << "#486860" << "#525e54" << "#7e735f" << "#4c4a48" << "#000000";
    colors << "#ff8c00" << "#e81123" << "#d13438" << "#c30052" << "#bf0077" << "#9a0089" << "#881798" << "#744da9" << "#8764b8" << "#e9e9e9";

    for (QString color : colors){
        QPushButton * button = new QPushButton(ui->colorListWidget);
        button->setFixedSize(QSize(48, 48));
        QString btnQss = QString("QPushButton{background: %1; border: none; border-radius: 4px;}").arg(color);
        button->setStyleSheet(btnQss);

        connect(button, &QPushButton::clicked, [=]{

            QString widgetQss = QString("QWidget{background: %1; border-radius: 6px;}").arg(color);
            ui->previewWidget->setStyleSheet(widgetQss);

            ///设置系统纯色背景
            bgsettings->set(FILENAME, "");
            bgsettings->set(PRIMARY, QVariant(color));

            ui->previewStackedWidget->setCurrentIndex(COLOR);
        });
        colorFlowLayout->addWidget(button);
    }

    connect(ui->formComBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        Q_UNUSED(index)
        //切换
        int currentPage = ui->formComBox->currentData(Qt::UserRole).toInt();
        ui->substackedWidget->setCurrentIndex(currentPage);

        if (currentPage == PICTURE){

        } else if (currentPage == COLOR){
        }

    });
    //壁纸变动后改变用户属性
    connect(bgsettings, &QGSettings::changed, [=](QString key){


        //GSettings key picture-filename 这里收到 pictureFilename的返回值
        if (!QString::compare(key, "pictureFilename")){
            QString curPicname = bgsettings->get(key).toString();

            QDBusInterface * interface = new QDBusInterface("org.freedesktop.Accounts",
                                             "/org/freedesktop/Accounts",
                                             "org.freedesktop.Accounts",
                                             QDBusConnection::systemBus());

            if (!interface->isValid()){
                qCritical() << "Create /org/freedesktop/Accounts Client Interface Failed " << QDBusConnection::systemBus().lastError();
                return;
            }

            QDBusReply<QDBusObjectPath> reply =  interface->call("FindUserByName", g_get_user_name());
            QString userPath;
            if (reply.isValid()){
                userPath = reply.value().path();
            }
            else {
                qCritical() << "Call 'GetComputerInfo' Failed!" << reply.error().message();
                return;
            }

            QDBusInterface * useriFace = new QDBusInterface("org.freedesktop.Accounts",
                                                            userPath,
                                                            "org.freedesktop.Accounts.User",
                                                            QDBusConnection::systemBus());

            if (!useriFace->isValid()){
                qCritical() << QString("Create %1 Client Interface Failed").arg(userPath) << QDBusConnection::systemBus().lastError();
                return;
            }

            QDBusMessage msg = useriFace->call("SetBackgroundFile", curPicname);
            if (!msg.errorMessage().isEmpty())
                qDebug() << "update user background file error: " << msg.errorMessage();
        }
    });
}

int Wallpaper::_getCurrentBgForm(){
    QString filename = bgsettings->get(FILENAME).toString();

    int current = 0;

    //设置当前背景形式
    if (filename.isEmpty()){
        current = COLOR;
    } else if (filename.endsWith("xml")){
        current = SLIDESHOW;
    } else {
        current = PICTURE;
    }

    return current;
}

void Wallpaper::initBgFormStatus(){
    initPreviewStatus();

    int currentIndex = _getCurrentBgForm();
    //设置当前背景形式
    ui->formComBox->blockSignals(true);
    ui->formComBox->setCurrentIndex(currentIndex);
    ui->formComBox->blockSignals(false);
    ui->substackedWidget->setCurrentIndex(currentIndex);
    ui->previewStackedWidget->setCurrentIndex(currentIndex);

    //屏蔽背景放置方式无效
    ui->picOptionsComBox->hide();
    ui->picOptionsLabel->hide();

    //屏蔽纯色背景的确定按钮
    ui->cancelBtn->hide();
    ui->certainBtn->hide();

    //根据背景形式选择显示组件
    showComponent(currentIndex);
}

void Wallpaper::showComponent(int index){
    if (PICTURE == index){ //图片
//        ui->picOptionsComBox->show();
//        ui->picOptionsLabel->show();
    } else if (COLOR == index){ //纯色
//        ui->picOptionsComBox->hide();
//        ui->picOptionsLabel->hide();
    } else { //幻灯片

    }
}

void Wallpaper::initPreviewStatus(){
    //设置图片背景的预览效果
    QString filename = bgsettings->get(FILENAME).toString();
    QByteArray ba = filename.toLatin1();
    if (g_file_test(ba.data(), G_FILE_TEST_EXISTS)){
        ui->previewLabel->setPixmap(QPixmap(filename).scaled(ui->previewLabel->size()));
    }

    //设置纯色背景的预览效果
    QString color = bgsettings->get(PRIMARY).toString();
    if (!color.isEmpty()){
        QString widgetQss = QString("QWidget{background: %1; border-radius: 6px;}").arg(color);
        ui->previewWidget->setStyleSheet(widgetQss);
    }
}

void Wallpaper::wpOptionsChangedSlot(QString op){
    //获取当前选中的壁纸
//    QListWidgetItem * currentitem = ui->listWidget->currentItem();
//    QString filename = currentitem->data(Qt::UserRole).toString();

    bgsettings->set(OPTIONS, ui->picOptionsComBox->currentData().toString());

    //更新xml数据
//    if (wallpaperinfosMap.contains(filename)){
//        wallpaperinfosMap[filename]["options"] = op;
//    }

    //将改动保存至文件
    xmlhandleObj->xmlUpdate(wallpaperinfosMap);
}

void Wallpaper::setlistview(){
    //初始化listview
//    ui->listView->setFocusPolicy(Qt::NoFocus);
//    ui->listView->setAutoFillBackground(true);
//    ui->listView->setIconSize(QSize(160, 100));
//    ui->listView->setResizeMode(QListView::Adjust);
//    ui->listView->setModel(&wpListModel);
//    ui->listView->setViewMode(QListView::IconMode);
//    ui->listView->setSpacing(5);
}

void Wallpaper::setModeldata(){
    QMap<QString, QMap<QString, QString> >::iterator iters = wallpaperinfosMap.begin();
    for (int row = 0; iters != wallpaperinfosMap.end(); iters++, row++){
        if (QString(iters.key()) == "head") //跳过xml的头部信息
            continue;
        QMap<QString, QString> wpMap = (QMap<QString, QString>)iters.value();
        QString delstatus = QString(wpMap.find("deleted").value());
        if (delstatus == "true") //跳过被删除的壁纸
            continue;

        QString filename = QString(iters.key());
        QPixmap pixmap(filename);

        wpListModel.insertRows(row, 1, QModelIndex());
        QModelIndex wpindex = wpListModel.index(row, 0, QModelIndex());
        wpListModel.setData(wpindex, QIcon(pixmap.scaled(QSize(160,100))), Qt::DecorationRole);
        wpListModel.setData(wpindex, QString("%1\nfolder: %2\n").arg(wpMap.find("name").value()).arg(filename), Qt::ToolTipRole);
    }
}

void Wallpaper::resetDefaultWallpaperSlot(){
    GSettings * wpgsettings;
    wpgsettings = g_settings_new(BACKGROUND);
    GVariant * variant = g_settings_get_default_value(wpgsettings, FILENAME);
    gsize size = g_variant_get_size(variant);
    const char * dwp = g_variant_get_string(variant, &size);
    g_object_unref(wpgsettings);

    bgsettings->set(FILENAME, QVariant(QString(dwp)));

    initPreviewStatus();
}

void Wallpaper::add_custom_wallpaper(){
    QString filters = "Wallpaper files(*.png *.jpg)";
    QFileDialog fd;
    fd.setDirectory(QString(const_cast<char *>(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES))));
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("selsect custom wallpaper file"));
    fd.setLabelText(QFileDialog::Accept, tr("Select"));
    fd.setLabelText(QFileDialog::LookIn, tr("Position: "));
    fd.setLabelText(QFileDialog::FileName, tr("FileName: "));
    fd.setLabelText(QFileDialog::FileType, tr("FileType: "));
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();

    QSize IMAGE_SIZE(160, 120);
    QPixmap pixmap = QPixmap(selectedfile).scaled(IMAGE_SIZE);
//    append_item(pixmap, selectedfile);

    if (wallpaperinfosMap.contains(selectedfile)){
        wallpaperinfosMap[selectedfile]["deleted"] = "false";
    }
    else{
        QMap<QString, QString> tmpinfo;
        tmpinfo.insert("artist", "(none)");
        tmpinfo.insert("deleted", "false");
        tmpinfo.insert("filename", selectedfile);
        tmpinfo.insert("name", selectedfile.split("/").last());
        tmpinfo.insert("options", "zoom");
        tmpinfo.insert("pcolor", "#000000");
        tmpinfo.insert("scolor", "#000000");
        tmpinfo.insert("shade_type", "solid");
        wallpaperinfosMap.insert(selectedfile, tmpinfo);

    }
    xmlhandleObj->xmlUpdate(wallpaperinfosMap);

    if (picWpItemMap.contains(selectedfile)){
//        ui->listWidget->setCurrentItem(picWpItemMap.find(selectedfile).value());
    }

}

void Wallpaper::del_wallpaper(){
    //获取当前选中的壁纸
//    QListWidgetItem * currentitem = ui->listWidget->currentItem();
//    QString filename = currentitem->data(Qt::UserRole).toString();

    //更新xml数据
//    if (wallpaperinfosMap.contains(filename)){
//        wallpaperinfosMap[filename]["deleted"] = "true";

//        int row = ui->listWidget->row(currentitem);

//        int nextrow = ui->listWidget->count() - 1 - row ? row + 1 : row - 1;

//        ui->listWidget->setCurrentItem(ui->listWidget->item(nextrow));

//        ui->listWidget->takeItem(row);

//    }

//    将改动保存至文件
    xmlhandleObj->xmlUpdate(wallpaperinfosMap);
}
