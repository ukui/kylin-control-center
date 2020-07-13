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
#include "changevailddialog.h"
#include "ui_changevailddialog.h"

#include <QProcess>
#include <QDBusInterface>

#include <QDebug>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeVaildDialog::ChangeVaildDialog(QString userName, QWidget *parent) :
    QDialog(parent),
    _name(userName),
    ui(new Ui::ChangeVaildDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    _getCurrentPwdStatus();

    setupCurrentVaild();
    setupConnect();
    setupComponent();

}

ChangeVaildDialog::~ChangeVaildDialog()
{
    delete ui;
}

void ChangeVaildDialog::setupConnect(){
    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        reject();
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->yearCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
#else
    connect(ui->yearCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
#endif
        setupMonthCombo();
        setupDayCombo();

    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->monthCombox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
#else
    connect(ui->monthCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
#endif
        setupDayCombo();

    });

    connect(ui->certainBtn, &QPushButton::clicked, [=]{
        int year = ui->yearCombox->currentData().toInt();
        QString cmd;
        if (year == 0){
            cmd = QString("chage -M %1 %2").arg(99999).arg(_name);
        } else {
            int month = ui->monthCombox->currentData().toInt();
            int day = ui->dayCombox->currentData().toInt();

            QDate selected = QDate(year, month, day);

            int setDays = lastChangeDate.daysTo(selected);

            cmd = QString("chage -M %1 %2").arg(setDays).arg(_name);

        }

        QDBusInterface * tmpSysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                                              "/",
                                                              "com.control.center.interface",
                                                              QDBusConnection::systemBus());

        if (!tmpSysinterface->isValid()){
            qCritical() << "Create Client Interface Failed When execute chage: " << QDBusConnection::systemBus().lastError();
            return;
        }
        tmpSysinterface->call("systemRun", cmd);
        delete tmpSysinterface;

        close();
    });
}

void ChangeVaildDialog::setUserLogo(QString iconfile){
    ui->faceLabel->setPixmap(QPixmap(iconfile));
}

void ChangeVaildDialog::setUserName(){
    ui->nameLabel->setText(_name);
}

void ChangeVaildDialog::setUserType(QString atype){
    ui->typeLabel->setText(atype);
}

void ChangeVaildDialog::_getCurrentPwdStatus(){
    //
    QString cmd = "passwd -S " + _name;

    QProcess * process = new QProcess;
    process->start(cmd);
    process->waitForFinished();

    QByteArray ba = process->readAllStandardOutput();
    QString vaild = QString(ba.data()).simplified();

    if (vaild.startsWith(_name)){
        QStringList vaildList = vaild.split(" ");
        QString lastChangeStr = vaildList.at(2);
        QStringList lastChangeList = lastChangeStr.split("/");
        lastChangeDate = QDate(QString(lastChangeList.at(2)).toInt(), QString(lastChangeList.at(0)).toInt(), QString(lastChangeList.at(1)).toInt());
        delayDays = QString(vaildList.at(4)).toInt();
    } else {
        delayDays = -1;
    }
}

void ChangeVaildDialog::setupCurrentVaild(){
    if (lastChangeDate.isValid() && delayDays >= 0){
        if (delayDays >= 10000){
            ui->vaildDateLabel->setText(QObject::tr("Never"));
        } else {
            QDate invaild = lastChangeDate.addDays(delayDays);
            ui->vaildDateLabel->setText(invaild.toString("yyyy-MM-dd"));
        }
    } else {
        ui->vaildDateLabel->setText(QObject::tr("Unknown"));
    }
}

void ChangeVaildDialog::setupComponent(){
    //chage源码中超过10000天会显示从不，10000/365 = 27.3，为了方便显示，取整，界面显示26年
    if (lastChangeDate.isValid()){
        setupYearCombo();


        setupMonthCombo();
        ui->monthCombox->blockSignals(true);
        ui->monthCombox->setCurrentIndex(ui->monthCombox->findData(lastChangeDate.addDays(delayDays).month()));
        ui->monthCombox->blockSignals(false);

        setupDayCombo();
        ui->dayCombox->blockSignals(true);
        ui->dayCombox->setCurrentIndex(ui->dayCombox->findData(lastChangeDate.addDays(delayDays).day()));
        ui->dayCombox->blockSignals(false);
    } else {

    }
}

void ChangeVaildDialog::setupYearCombo(){
    ui->yearCombox->blockSignals(true);
    ui->yearCombox->clear();

    QDate begin = QDate::currentDate().addDays(1);
    QDate canSelect = lastChangeDate.addYears(26);

    ui->yearCombox->addItem(QObject::tr("Never"), 0);
    for (int year = begin.year(); year <= canSelect.year(); year++){
        ui->yearCombox->addItem(QString::number(year)+QObject::tr("Year"), year);
    }

    if (delayDays > 10000)
        ui->yearCombox->setCurrentIndex(0);
    else
        ui->yearCombox->setCurrentIndex(ui->yearCombox->findData(lastChangeDate.addDays(delayDays).year()));

    ui->yearCombox->blockSignals(false);
}

void ChangeVaildDialog::setupMonthCombo(){
    ui->monthCombox->blockSignals(true);

    ui->monthCombox->clear();

    int year = ui->yearCombox->currentData().toInt();
    if (year > 0){
        ui->monthCombox->addItem(QObject::tr("Jan"), 1);
        ui->monthCombox->addItem(QObject::tr("Feb"), 2);
        ui->monthCombox->addItem(QObject::tr("Mar"), 3);
        ui->monthCombox->addItem(QObject::tr("Apr"), 4);
        ui->monthCombox->addItem(QObject::tr("May"), 5);
        ui->monthCombox->addItem(QObject::tr("Jun"), 6);
        ui->monthCombox->addItem(QObject::tr("Jul"), 7);
        ui->monthCombox->addItem(QObject::tr("Aug"), 8);
        ui->monthCombox->addItem(QObject::tr("Sep"), 9);
        ui->monthCombox->addItem(QObject::tr("Oct"), 10);
        ui->monthCombox->addItem(QObject::tr("Nov"), 11);
        ui->monthCombox->addItem(QObject::tr("Dec"), 12);
    }



    ui->monthCombox->blockSignals(false);
}

void ChangeVaildDialog::setupDayCombo(){
    ui->dayCombox->blockSignals(true);

    ui->dayCombox->clear();

    int year = ui->yearCombox->currentData().toInt();

    if (year > 0){
        int month = ui->monthCombox->currentData().toInt();
        if (month){
            QDate selected = QDate(year, month, 1);
            int days = selected.daysInMonth();
            for (int d = 1; d <= days; d++){
                ui->dayCombox->addItem(QString::number(d)+QObject::tr("Day"), d);
            }
        }
    } else {

    }

    ui->dayCombox->blockSignals(false);
}


void ChangeVaildDialog::paintEvent(QPaintEvent * event){
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
