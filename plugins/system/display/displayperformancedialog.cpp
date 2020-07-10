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
#include "displayperformancedialog.h"
#include "ui_displayperformancedialog.h"

#include <QPainter>
#include <QPainterPath>

#define ADVANCED_SCHEMAS "org.mate.session.required-components"
#define ADVANCED_KEY "windowmanager"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

DisplayPerformanceDialog::DisplayPerformanceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayPerformanceDialog)
{
    ui->setupUi(this);

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    const QByteArray id(ADVANCED_SCHEMAS);
    settings = new QGSettings(id);

    setupComponent();
    setupConnect();
    initModeStatus();

}

DisplayPerformanceDialog::~DisplayPerformanceDialog()
{
    delete ui;
    delete settings;
}

void DisplayPerformanceDialog::setupComponent(){
    ui->performanceRadioBtn->setProperty("wm", "mutter");
    ui->compatibleRadioBtn->setProperty("wm", "marco");
    ui->autoRadioBtn->setProperty("wm", "kylin-wm-chooser");
}

void DisplayPerformanceDialog::setupConnect(){
    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){
#else
    connect(ui->buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){
#endif
        QString mode = button->property("wm").toString();
        settings->set(ADVANCED_KEY, mode);
    });

    connect(ui->autoRadioBtn, &QRadioButton::toggled, this, [=](bool checked){
        ui->lineEdit->setEnabled(checked);
        ui->applyBtn->setEnabled(checked);
        ui->resetBtn->setEnabled(checked);
    });
}

void DisplayPerformanceDialog::initModeStatus(){
    QString mode = settings->get(ADVANCED_KEY).toString();

    if (mode == ui->performanceRadioBtn->property("wm").toString()){
        ui->performanceRadioBtn->blockSignals(true);
        ui->performanceRadioBtn->setChecked(true);
        ui->performanceRadioBtn->blockSignals(false);
    } else if (mode == ui->compatibleRadioBtn->property("wm").toString()){
        ui->compatibleRadioBtn->blockSignals(true);
        ui->compatibleRadioBtn->setChecked(true);
        ui->compatibleRadioBtn->blockSignals(false);
    } else{
        ui->autoRadioBtn->blockSignals(true);
        ui->autoRadioBtn->setChecked(true);
        ui->autoRadioBtn->blockSignals(false);
    }

}

void DisplayPerformanceDialog::paintEvent(QPaintEvent *event){
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
