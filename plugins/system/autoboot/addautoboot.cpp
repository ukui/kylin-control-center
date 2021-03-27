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
#include "addautoboot.h"
#include "ui_addautoboot.h"
#include "CloseButton/closebutton.h"

#include <QDebug>
#include <QFileInfo>

//#define DESKTOPPATH "/etc/xdg/autostart/"
#define DESKTOPPATH "/usr/share/applications/"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

AddAutoBoot::AddAutoBoot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAutoBoot) {

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    initStyle();
    initConnection();
}

void AddAutoBoot::resetBeforeClose() {
    ui->certainBtn->setEnabled(false);
    ui->hintLabel->clear();
    ui->nameLineEdit->setText(QString());
    ui->commentLineEdit->setText(QString());
    ui->execLineEdit->setText(QString());
    close();
}

void AddAutoBoot::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 10, 10);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}

void AddAutoBoot::initStyle() {
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    selectFile = "";

    ui->nameLineEdit->setPlaceholderText(tr("Program name"));
    ui->execLineEdit->setPlaceholderText(tr("Program exec"));
    ui->commentLineEdit->setPlaceholderText(tr("Program comment"));

    ui->hintLabel->setAlignment(Qt::AlignCenter);
    ui->hintLabel->setStyleSheet("color:red;");
    ui->certainBtn->setEnabled(false);
}

void AddAutoBoot::initConnection() {

    connect(ui->openBtn, SIGNAL(clicked(bool)), this, SLOT(open_desktop_dir_slots()));
    connect(ui->cancelBtn, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->execLineEdit, SIGNAL(textEdited(QString)), this, SLOT(execLinEditSlot(QString)));

    connect(ui->cancelBtn, &QPushButton::clicked, [=] {
        resetBeforeClose();
    });
    connect(ui->certainBtn, &QPushButton::clicked, this, [=] {
        emit autoboot_adding_signals(selectFile, ui->nameLineEdit->text(), mDesktopExec, ui->commentLineEdit->text(), mDesktopIcon);
        resetBeforeClose();
    });
}

AddAutoBoot::~AddAutoBoot() {
    delete ui;
    ui = nullptr;
}

void AddAutoBoot::open_desktop_dir_slots() {
    QString filters = tr("Desktop files(*.desktop)");
    QFileDialog fd;
    fd.setDirectory(DESKTOPPATH);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("select autoboot desktop"));
    fd.setLabelText(QFileDialog::Accept, tr("Select"));
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();
    selectFile = selectedfile;

    QByteArray ba;
    ba = selectedfile.toUtf8();

    //解析desktop文件
    GKeyFile * keyfile;
    char *name, * comment;

    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)){
        g_key_file_free (keyfile);
        return;
    }

    name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
    comment = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL, NULL);
    mDesktopExec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
    mDesktopIcon = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);

    if (ui->nameLineEdit->text().isEmpty()) {
        ui->nameLineEdit->setText(QString(name));
    }

    ui->execLineEdit->setText(QString(selectedfile));
    ui->commentLineEdit->setText(QString(comment));

    emit ui->execLineEdit->textEdited(QString(selectedfile));
    g_key_file_free(keyfile);
}

void AddAutoBoot::execLinEditSlot(const QString &fileName) {

    selectFile = fileName;
    QFileInfo fileInfo(fileName);
    if (fileInfo.isFile() && fileName.endsWith("desktop")) {
        ui->hintLabel->clear();
        ui->certainBtn->setEnabled(true);

        QByteArray ba;
        ba = fileName.toUtf8();

        //解析desktop文件
        GKeyFile * keyfile;
        char *name, * comment;

        keyfile = g_key_file_new();
        if (!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)){
            g_key_file_free (keyfile);
            return;
        }

        name = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL);
        mDesktopExec = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
        mDesktopIcon = g_key_file_get_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
        comment = g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL, NULL);

        if (ui->nameLineEdit->text().isEmpty()) {
            ui->nameLineEdit->setText(QString(name));
        }

        ui->execLineEdit->setText(QString(fileName));
        ui->commentLineEdit->setText(QString(comment));

        g_key_file_free(keyfile);
    } else {
        ui->hintLabel->setText(tr("desktop file not exist"));
        ui->hintLabel->setAlignment(Qt::AlignCenter);
        ui->hintLabel->setStyleSheet("color:red;");
        ui->certainBtn->setEnabled(false);
    }
}
