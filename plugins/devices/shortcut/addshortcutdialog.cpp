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
#include "addshortcutdialog.h"
#include "ui_addshortcutdialog.h"

#include "realizeshortcutwheel.h"

#define DEFAULTPATH "/usr/share/applications/"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

addShortcutDialog::addShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addShortcutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(tr("Add custom shortcut"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    ui->closeBtn->setIcon(QIcon(QPixmap("://img/titlebar/close.svg")));
    ui->noteLabel->setPixmap(QPixmap("://img/plugins/shortcut/note.png"));

    ui->stackedWidget->setCurrentIndex(1);


    refreshCertainChecked();

    gsPath = "";

    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });
    connect(ui->openBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        openProgramFileDialog();
    });

    connect(ui->execLineEdit, &QLineEdit::textChanged, [=](QString text){
        if (text.endsWith("desktop") ||
                (!g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_DIR) && g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_EXECUTABLE))){
            ui->certainBtn->setChecked(true);
            ui->stackedWidget->setCurrentIndex(1);

        } else{
            ui->certainBtn->setChecked(false);
            ui->stackedWidget->setCurrentIndex(0);
        }

        refreshCertainChecked();
    });

    connect(ui->nameLineEdit, &QLineEdit::textChanged, [=](QString text){
        refreshCertainChecked();
    });

    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->certainBtn, &QPushButton::clicked, [=]{
        emit shortcutInfoSignal(gsPath, ui->nameLineEdit->text(), ui->execLineEdit->text());

        close();
    });

    connect(this, &addShortcutDialog::finished, [=]{
        gsPath = "";
        ui->nameLineEdit->clear();
        ui->execLineEdit->clear();
        ui->stackedWidget->setCurrentIndex(1);
        ui->nameLineEdit->setFocus(Qt::ActiveWindowFocusReason);

        refreshCertainChecked();
    });
}

addShortcutDialog::~addShortcutDialog()
{
    delete ui;
}

void addShortcutDialog::setTitleText(QString text){
    ui->titleLabel->setText(text);
}

void addShortcutDialog::setUpdateEnv(QString path, QString name, QString exec){
    gsPath = path;
    ui->nameLineEdit->setText(name);
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

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

void addShortcutDialog::openProgramFileDialog(){
    QString filters = "Desktop files(*.desktop)";
    QFileDialog fd;
    fd.setDirectory(DEFAULTPATH);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("select desktop"));
//    fd.setLabelText(QFileDialog::Accept, "Select");
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();

    QString exec = selectedfile.section("/", -1, -1);
//    exec.replace(".desktop", "");
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::refreshCertainChecked(){
    if (ui->nameLineEdit->text().isEmpty() || ui->execLineEdit->text().isEmpty() || ui->stackedWidget->currentIndex() == 0){
        ui->certainBtn->setEnabled(false);
    } else {
        ui->certainBtn->setEnabled(true);
    }
}
