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
#include "changtime.h"
#include "ui_changtime.h"

#include <QDebug>


const int BEGINYEAR = 1900;
const int BEGINMD = 1;

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangtimeDialog::ChangtimeDialog(bool hour,QWidget *parent) :m_isEFHour(hour),
    QDialog(parent),
    ui(new Ui::changtimedialog)
{

//    QFile QssFile("://combox.qss");
//    QssFile.open(QFile::ReadOnly);

//    if (QssFile.isOpen()){
//        qss = QLatin1String(QssFile.readAll());
//        QssFile.close();
//    }

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

//    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
    //关闭按钮在右上角，窗体radius 6px，所以按钮只得6px
    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.png"));
//    ui->closeBtn->setStyleSheet("QPushButton#closeBtn{background: #ffffff; border: none; border-radius: 6px;}"
//                                "QPushButton:hover:!pressed#closeBtn{background: #FA6056; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}"
//                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}");

    m_datetimeInterface = new QDBusInterface("org.freedesktop.timedate1",
                                       "/org/freedesktop/timedate1",
                                       "org.freedesktop.timedate1",
                                       QDBusConnection::systemBus());

    initUi();
    initStatus();

    m_chtimer = new QTimer();
    m_chtimer->start(1000);
    connect(m_chtimer, SIGNAL(timeout()), this, SLOT(datetimeUpdateSlot()));


    connect(ui->monthcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(dayUpdateSlot()));
    connect(ui->yearcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(dayUpdateSlot()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->confirmButton,SIGNAL(clicked()),this,SLOT(changtimeApplySlot()));
    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });

}

ChangtimeDialog::~ChangtimeDialog()
{
    m_chtimer->stop();
    delete ui;
    delete m_datetimeInterface;
}


void ChangtimeDialog::datetimeUpdateSlot(){
    QDateTime current = QDateTime::currentDateTime();
    QString currenthourStr = current.toString("hh");
    QString currentminStr = current.toString("mm");
    QString currentsecStr = current.toString("ss");

    ui->seccomboBox->setCurrentIndex(currentsecStr.toInt());
    if (currentsecStr.toInt() == 0) {
       ui->mincomboBox->setCurrentIndex(currentminStr.toInt());
    }
    if (currentsecStr.toInt() == 0 && currentminStr.toInt() == 0) {
        ui->hourcomboBox->setCurrentIndex(currenthourStr.toInt());
    }
}

void ChangtimeDialog::dayUpdateSlot(){
    ui->daycomboBox->clear();

    int year = ui->yearcomboBox->currentIndex() + BEGINYEAR;
    int month = ui->monthcomboBox->currentIndex() + BEGINMD;

    bool f_year;
    if((year%4==0 && year%100!=0) || year%400==0){
        f_year = true;
    } else {
        f_year = false;
    }

    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        for(int i = 1; i <= 31; i++){
            ui->daycomboBox->addItem(QString::number(i));
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        for(int i = 1; i <= 30; i++){
            ui->daycomboBox->addItem(QString::number(i));
        }
        break;
    case 2:
        if(f_year){
            for(int i = 1; i <= 29; i++)
                ui->daycomboBox->addItem(QString::number(i));
        } else {
            for(int i = 1; i <= 28; i++)
                ui->daycomboBox->addItem(QString::number(i));
        }
        break;
    }
}

void ChangtimeDialog::changtimeApplySlot(){
//    qDebug()<<"时间应用------------》"<<endl;
    int year = ui->yearcomboBox->currentIndex() + BEGINYEAR;
    int month = ui->monthcomboBox->currentIndex() + BEGINMD;
    int day = ui->daycomboBox->currentIndex() + BEGINMD;

    QDate tmpdate(year,month,day);

    int hour;
    if (this->m_isEFHour) {
        hour = ui->hourcomboBox->currentIndex();
    } else {
        hour = ui->hourcomboBox->currentIndex();
    }

    QTime tmptime(hour, ui->mincomboBox->currentIndex(),ui->seccomboBox->currentIndex());

    QDateTime setdt(tmpdate,tmptime);
//    qDebug()<<"tmp time and hour is-->"<<setdt<<" "<<hour<<endl;

    for(int i=0; i < 2; i++){
        m_datetimeInterface->call("SetNTP", false, true);//先关闭网络同步
//        m_datetimeInterface->call("SetTime", QVariant::fromValue(setdt.toSecsSinceEpoch() * G_TIME_SPAN_SECOND), false, true);
    }
    this->close();
}

void ChangtimeDialog::initUi(){

//    ui->hourcomboBox->setStyleSheet(qss);
//    ui->hourcomboBox->setView(new QListView());
//    ui->hourcomboBox->setMaxVisibleItems(5);

//    ui->mincomboBox->setStyleSheet(qss);
//    ui->mincomboBox->setView(new QListView());
//    ui->mincomboBox->setMaxVisibleItems(5);

//    ui->seccomboBox->setView(new QListView());
//    ui->seccomboBox->setStyleSheet(qss);
//    ui->seccomboBox->setMaxVisibleItems(5);

//    ui->yearcomboBox->setStyleSheet(qss);
//    ui->yearcomboBox->setView(new QListView());
//    ui->yearcomboBox->setMaxVisibleItems(5);

//    ui->monthcomboBox->setStyleSheet(qss);
//    ui->monthcomboBox->setView(new QListView());
//    ui->monthcomboBox->setMaxVisibleItems(5);

//    ui->daycomboBox->setStyleSheet(qss);
//    ui->daycomboBox->setView(new QListView());
//    ui->daycomboBox->setMaxVisibleItems(4);

//    this->setStyleSheet("background: #ffffff;");
    ui->timelabel->setText(tr("time"));
//    ui->timelabel->setStyleSheet("QLabel#timelabel{background: #F4F4F4;}");

    ui->yearlabel->setText(tr("year"));
//    ui->yearlabel->setStyleSheet("QLabel#yearlabel{background: #F4F4F4;}");

    ui->monthlabel->setText(tr("month"));
//    ui->monthlabel->setStyleSheet("QLabel#monthlabel{background: #F4F4F4;}");

    ui->daylabel->setText(tr("day"));
//    ui->daylabel->setStyleSheet("QLabel#daylabel{background: #F4F4F4;}");

//    ui->timewidget->setStyleSheet("QWidget#timewidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->yearwidget->setStyleSheet("QWidget#yearwidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->monthwidget->setStyleSheet("QWidget#monthwidget{background: #F4F4F4; border-radius: 6px;}");
//    ui->daywidget->setStyleSheet("QWidget#daywidget{background: #F4F4F4; border-radius: 6px;}");


//    ui->cancelButton->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white};border-radius:4px");
//    ui->confirmButton->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white};border-radius:4px");

    hourComboxSetup();
    for(int m = 0; m < 60; m++){
        ui->mincomboBox->addItem(QString::number(m));
    }

    for(int s = 0; s < 60; s++){
        ui->seccomboBox->addItem(QString::number(s));
    }

    for(int year = 1900; year <= 2100; year++){
        ui->yearcomboBox->addItem(QString::number(year)+tr("year"));
    }
    for(int month = 1; month <= 12; month++){
        ui->monthcomboBox->addItem(QString::number(month)+tr("month"));
    }
    ymdComboxSetup();
}

void ChangtimeDialog::hourComboxSetup(){
    ui->hourcomboBox->clear();

    //获取时间制式，设置时间combobox
    for (int h = 0; h < 24; h++){
        ui->hourcomboBox->addItem(QString::number(h));
    }

//    if (this->m_isEFHour){
//        for (int h = 0; h < 24; h++)
//            ui->hourcomboBox->addItem(QString::number(h));
//    } else {
//        for (int h = 1; h <= 12; h++)
//            ui->hourcomboBox->addItem(QString::number(h));
//    }
}


void ChangtimeDialog::ymdComboxSetup(){
    QDateTime currentime = QDateTime::currentDateTime();
    int year = currentime.toString("yyyy").toInt();
    int month = currentime.toString("MM").toInt();
    int day = currentime.toString("dd").toInt();

    ui->yearcomboBox->setCurrentIndex(year - BEGINYEAR);
    ui->monthcomboBox->setCurrentIndex(month - BEGINMD);

    dayUpdateSlot();
    ui->daycomboBox->setCurrentIndex(day - BEGINMD);

}

void ChangtimeDialog::initStatus(){

    datetimeUpdateSlot();
    QDateTime current = QDateTime::currentDateTime();
    QString currenthourStr = current.toString("hh");
    QString currentminStr = current.toString("mm");

    //if date formate is 24 hour
    if(this->m_isEFHour) {
//        ui->hourcomboBox->setItemText(currenthourStr.toInt());
        ui->hourcomboBox->setCurrentIndex(currenthourStr.toInt());
    } else {
//        qDebug()<<"currenthourStr.toInt() is------------->"<<currenthourStr.toInt()<<endl;
        if (currenthourStr.toInt() > 12) {
            ui->hourcomboBox->setCurrentIndex(currenthourStr.toInt() - 12);
        } else {
            ui->hourcomboBox->setCurrentIndex((currenthourStr.toInt()));
        }
    }
    ui->mincomboBox->setCurrentIndex(currentminStr.toInt());
}


void ChangtimeDialog::paintEvent(QPaintEvent *event) {
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
//    p.fillPath(rectPath,QColor(0,0,0));
    p.restore();
}
