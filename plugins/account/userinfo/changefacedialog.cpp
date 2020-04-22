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
#include "changefacedialog.h"
#include "ui_changefacedialog.h"

#include "FlowLayout/flowlayout.h"
#include "elipsemaskwidget.h"

#include <QDebug>

#define FACEPATH "/usr/share/ukui/faces/"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeFaceDialog::ChangeFaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeFaceDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

//    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
//    ui->closeBtn->setStyleSheet("QPushButton{background: #ffffff; border: none;}");


    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.png"));

    ElipseMaskWidget * cfMaskWidget = new ElipseMaskWidget(ui->faceLabel);
//    cfMaskWidget->setBgColor("#F4F4F4");
    cfMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    loadSystemFaces();


    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->customfaceBtn, &QPushButton::clicked, [=]{
        showLocalFaceDialog();
    });
}

ChangeFaceDialog::~ChangeFaceDialog()
{
    delete ui;
}

void ChangeFaceDialog::loadSystemFaces(){

    FlowLayout * facesFlowLayout = new FlowLayout(ui->facesWidget);
    ui->facesWidget->setLayout(facesFlowLayout);

    //遍历头像目录
    QStringList facesList;
    QDir facesDir = QDir(FACEPATH);
    foreach (QString filename, facesDir.entryList(QDir::Files)){
//        facesList.append(FACEPATH + filename);
        QString fullface = QString("%1%2").arg(FACEPATH).arg(filename);
        if (fullface.endsWith(".svg"))
            continue;
        if (fullface.endsWith("default.png"))
            continue;

        QPushButton * button = new QPushButton;
        button->setAttribute(Qt::WA_DeleteOnClose);
        button->setFixedSize(QSize(48, 48));
//        button->setStyleSheet("QPushButton{border: none;}");

        QHBoxLayout * mainHorLayout = new QHBoxLayout(button);
        mainHorLayout->setSpacing(0);
        mainHorLayout->setMargin(0);
        QLabel * iconLabel = new QLabel(button);
        iconLabel->setScaledContents(true);
        iconLabel->setPixmap(QPixmap(fullface));

        mainHorLayout->addWidget(iconLabel);

        button->setLayout(mainHorLayout);

        connect(button, &QPushButton::clicked, [=]{
            //show dialog更新头像
            setFace(fullface);

            emit face_file_send(fullface, ui->usernameLabel->text());
        });

        facesFlowLayout->addWidget(button);
    }
}

void ChangeFaceDialog::setFace(QString iconfile){
    ui->faceLabel->setPixmap(QPixmap(iconfile));
}

void ChangeFaceDialog::setUsername(QString username){
    ui->usernameLabel->setText(username);
}

void ChangeFaceDialog::setAccountType(QString atype){
    ui->typeLabel->setText(atype);
}

void ChangeFaceDialog::showLocalFaceDialog(){
    QString filters = "Face files(*.png *.jpg *.svg)";
    QFileDialog fd;
    fd.setDirectory(QString(const_cast<char *>(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES))));
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("selsect custom face file"));
    fd.setLabelText(QFileDialog::Accept, tr("Select"));
    fd.setLabelText(QFileDialog::LookIn, tr("Position: "));
    fd.setLabelText(QFileDialog::FileName, tr("FileName: "));
    fd.setLabelText(QFileDialog::FileType, tr("FileType: "));
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();

    setFace(selectedfile);
    emit face_file_send(selectedfile, ui->usernameLabel->text());

}

void ChangeFaceDialog::paintEvent(QPaintEvent *event) {
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
