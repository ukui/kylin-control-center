/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2020 KYLINOS Information Technology Co., Ltd.
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

#include "delgroupdialog.h"
#include "ui_delgroupdialog.h"
#include "CloseButton/closebutton.h"
#include <QPushButton>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

DelGroupDialog::DelGroupDialog(QString groupName, QWidget *parent) :
    QDialog(parent),
    mgroupname(groupName),
    ui(new Ui::DelGroupDialog)
{
    ui->setupUi(this);
    setupInit();
    signalsBind();
}

DelGroupDialog::~DelGroupDialog()
{
    delete ui;
    ui = nullptr;
}

void DelGroupDialog::signalsBind()
{
    connect(mCancelBtn,&QPushButton::clicked,[=](){
        close();
    });
}

void DelGroupDialog::setupInit()
{
    setWindowTitle(tr("Delete user group"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    mLabelpic = new QLabel(this);
    mLabelpic->setGeometry(32, 32, 22,22);
    QPixmap mPixmap("://img/plugins/userinfo/notice.png");
    mPixmap = mPixmap.scaled(mLabelpic->size());
    mLabelpic->setPixmap(mPixmap);

    titleLabel = new QLabel(this);
    titleLabel->setGeometry(62, 32, 336,48);
    QFont font ( "Microsoft YaHei", 14, 75);
    titleLabel->setFont(font);
    titleLabel->setText(tr("Are you sure to delete the group:   ")+mgroupname);
    titleLabel->setWordWrap(true);

    mHintLabel = new QLabel(this);
    mHintLabel->setGeometry(62, 98, 280,20);
    if (QLabelSetText(mHintLabel,tr("which will make some file components in the file system invalid!"))) {
        mHintLabel->setToolTip(tr("which will make some file components in the file system invalid!"));
    }


    mCancelBtn = new QPushButton(this);
    mCancelBtn->setContentsMargins(36,6,36,6);
    mCancelBtn->setGeometry(143, 150, 120,36);
    mCancelBtn->setText(tr("Cancel"));


    mDelBtn = new QPushButton(this);
    mDelBtn->setContentsMargins(36,6,36,6);
    mDelBtn->setGeometry(279, 150, 120,36);
    mDelBtn->setText(tr("Delete"));


    //ui->titleLabel->setWordWrap(true);
}

QPushButton * DelGroupDialog::delBtnComponent()
{
    return mDelBtn;
}

void DelGroupDialog::setNoticeText(QString txt)
{
    qDebug() << "setNoticeText" << txt;
}

void DelGroupDialog::paintEvent(QPaintEvent * event){
    Q_UNUSED(event)

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
bool DelGroupDialog::QLabelSetText(QLabel *label, QString string)
{
    bool is_over_length = false;
    QFontMetrics fontMetrics(label->font());
    int fontSize = fontMetrics.width(string);
    QString str = string;
    if (fontSize > (label->width()-5)) {
        str = fontMetrics.elidedText(string, Qt::ElideRight, label->width()-10);
        is_over_length = true;
    }
    label->setText(str);
    return is_over_length;
}
