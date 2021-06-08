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
#include "changetypedialog.h"
#include "ui_changetypedialog.h"
#include "CloseButton/closebutton.h"

#include "elipsemaskwidget.h"

#include <QDebug>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeTypeDialog::ChangeTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeTypeDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Change type"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
//    ui->closeBtn->setProperty("useIconHighlightEffect", true);
//    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
//    ui->closeBtn->setFlat(true);
//    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
//                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

//    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
//    ui->closeBtn->setStyleSheet("QPushButton{background: #ffffff; border: none;}");



//    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));


    setupComonpent();

}

ChangeTypeDialog::~ChangeTypeDialog()
{
    delete ui;
    ui = nullptr;
}

void ChangeTypeDialog::setupComonpent(){

    ElipseMaskWidget * ctMaskWidget = new ElipseMaskWidget(ui->faceLabel);
    ctMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    ui->buttonGroup->setId(ui->standardRadioButton, 0);
    ui->buttonGroup->setId(ui->adminRadioButton, 1);

    ui->confirmPushBtn->setEnabled(false);

//    connect(ui->closeBtn, &CloseButton::clicked, [=]{
//        close();
//    });
    connect(ui->cancelPushBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        reject();
    });
    connect(ui->confirmPushBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        this->accept();
        emit type_send(ui->buttonGroup->checkedId());
    });
}

void ChangeTypeDialog::setFace(QString faceFile){
    ui->faceLabel->setPixmap(QPixmap(faceFile));
}

void ChangeTypeDialog::setUsername(QString username){
    ui->usernameLabel->setText(username);
}

void ChangeTypeDialog::setCurrentAccountTypeLabel(QString atype){
    ui->typeLabel->setText(atype);
}

void ChangeTypeDialog::setCurrentAccountTypeBtn(int id){
    currenttype = id;
    if (id == 0)
        ui->standardRadioButton->setChecked(true);
    else
        ui->adminRadioButton->setChecked(true);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](int id){
#else
    connect(ui->buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id){
#endif
        if (id != currenttype)
            ui->confirmPushBtn->setEnabled(true);
        else
            ui->confirmPushBtn->setEnabled(false);
    });
}

void ChangeTypeDialog::forbidenChange(int can){
    if (can == -1)
        return;
    if (can){
        ui->standardRadioButton->setEnabled(true);
    } else {
        ui->standardRadioButton->setEnabled(false);
    }
}

void ChangeTypeDialog::paintEvent(QPaintEvent *event) {
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

void ChangeTypeDialog::keyPressEvent(QKeyEvent * event){
    switch (event->key())
    {
    case Qt::Key_Escape:
        break;
    case Qt::Key_Enter:
        break;
    case Qt::Key_Return:
        if (ui->confirmPushBtn->isEnabled())
            ui->confirmPushBtn->clicked();
        break;
    default:
        QDialog::keyPressEvent(event);
    }
}

