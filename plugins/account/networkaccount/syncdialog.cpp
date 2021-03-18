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
#include "syncdialog.h"
#include <QDesktopWidget>
#include <QPaintEngine>
#include "configfile.h"
#include <QApplication>
#include <QDebug>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

SyncDialog::SyncDialog(QString name,QString path, QWidget *parent) : QDialog(parent)
{
    mTitle = new QLabel(this);
    mTips = new QLabel(this);
    mListWidget = new QListWidget(this);
    mSyncButton = new QPushButton(tr("Sync"),this);
    mCancelButton = new QPushButton(tr("Do not"),this);
    setAttribute(Qt::WA_DeleteOnClose);
    mMainLayout = new QVBoxLayout;
    mHBoxLayout = new QHBoxLayout;

    connect(mSyncButton, &QPushButton::clicked,this,[=] {
        if (m_List.isEmpty()) {
            emit coverMode();
            return ;
        }

        emit sendKeyMap(m_List);
    });
    connect(mCancelButton, &QPushButton::clicked,this,[=] {
        emit coverMode();
    });
    initUI();

}

void SyncDialog::checkOpt() {
    mDate = m_List.at(0);
    //qDebug() << m_List.size()<<m_List.at(0) << m_List.at(1);
    m_List.removeAt(0);
    m_List.removeAt(1);
    mTips->setText(tr("Last sync at %1").arg(mDate));
    //qDebug() << m_List.size()<<"mList";
    for(const QString &item : qAsConst(m_szItemlist)) {
        if (m_List.contains(item)) {
            QCheckBox * checkBox = new QCheckBox(m_szItemNameList.at(m_szItemlist.indexOf(item)),this);
            QListWidgetItem * items = new QListWidgetItem(mListWidget,0);
            mListWidget->addItem(items);
            mListWidget->setItemWidget(items,checkBox);
            items->setSizeHint(QSize(mListWidget->size().width(), 20));
            QStringList filter;
            filter << "indicator-china-weather" << "kylin-video" << "terminal"
            << "editor" << "peony";

            connect(checkBox, &QCheckBox::toggled, this, [=] (bool status) {
               if (status == true) {
                   m_List.removeAll(item);
               } else {
                   m_List.append(item);
               }
            });
            if (filter.contains(item)) {
                checkBox->setChecked(false);
            } else {
                m_List.removeAll(item);
                checkBox->setChecked(true);
            }
        }
    }
}

void SyncDialog::initUI() {
    setFixedSize(400,380);
    setContentsMargins(32,32,32,23);
    mTitle->setStyleSheet("font-size:18px;font-weight:500");

    mListWidget->setFixedHeight(160);
    mSyncButton->setFixedSize(100,36);
    mCancelButton->setFixedSize(100,36);
    mListWidget->setContentsMargins(0,0,0,0);
    mListWidget->setSpacing(8);

    mTitle->setText(tr("Sync now?"));
    mTips->setText(tr("Last sync at %1").arg(mDate));

    mMainLayout->setContentsMargins(0,0,0,0);
    mHBoxLayout->setContentsMargins(0,0,0,0);

    mMainLayout->setSpacing(0);
    mHBoxLayout->setSpacing(16);

    mHBoxLayout->addWidget(mSyncButton);
    mHBoxLayout->addWidget(mCancelButton);
    mHBoxLayout->setAlignment(Qt::AlignRight);

    mMainLayout->addWidget(mTitle);
    mMainLayout->addSpacing(8);
    mMainLayout->addWidget(mTips);
    mMainLayout->addSpacing(16);
    mMainLayout->addWidget(mListWidget);
    mMainLayout->addSpacing(40);
    mMainLayout->addLayout(mHBoxLayout);
    setLayout(mMainLayout);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::Tool);
    setModal(true);
    hide();
}

/* 窗口重绘，阴影设置 */
void SyncDialog::paintEvent(QPaintEvent *event)
{
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

