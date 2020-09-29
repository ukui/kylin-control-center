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
#include "printer.h"
#include "ui_printer.h"

#include <QtPrintSupport/QPrinterInfo>
#include <QProcess>

#include <QDebug>
#include <QMouseEvent>

#define ITEMFIXEDHEIGH 58

Printer::Printer(){
    ui = new Ui::Printer;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Printer");
    pluginType = DEVICES;

    //~ contents_path /printer/Add Printers And Scanners
    ui->titleLabel->setText(tr("Add Printers And Scanners"));
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    ui->listWidget->setSpacing(0);
    //禁用选中效果
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    pTimer = new QTimer(this);
    pTimer->setInterval(1000);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(refreshPrinterDev()));

    initComponent();
}

Printer::~Printer()
{
    delete ui;
}

QString Printer::get_plugin_name(){
    return pluginName;
}

int Printer::get_plugin_type(){
    return pluginType;
}

QWidget *Printer::get_plugin_ui(){
    return pluginWidget;
}

void Printer::plugin_delay_control(){

}

const QString Printer::name() const {

    return QStringLiteral("printer");
}

void Printer::initComponent(){

//    ui->addBtn->setIcon(QIcon("://img/plugins/printer/add.png"));
//    ui->addBtn->setIconSize(QSize(48, 48));
//    ui->addBtn->setStyleSheet("QPushButton{background-color:transparent;}");

    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    //~ contents_path /printer/Add printers and scanners
    QLabel * textLabel = new QLabel(tr("Add printers and scanners"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });
    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](QString mname){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        runExternalApp();
    });
    ui->addLyt->addWidget(addWgt);

    pTimer->start();

//    connect(ui->addBtn, &QPushButton::clicked, this, [=]{
//        runExternalApp();
//    });
}

void Printer::refreshPrinterDev(){

    ui->listWidget->clear();

    QStringList printer = QPrinterInfo::availablePrinterNames();

    for (int num = 0; num < printer.count(); num++){

        QWidget * baseWidget = new QWidget;
        baseWidget->setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
        baseVerLayout->setSpacing(0);
        baseVerLayout->setContentsMargins(0, 0, 0, 8);

        QWidget * pdWidget = new QWidget(baseWidget);

        QHBoxLayout * pdHorLayout = new QHBoxLayout(pdWidget);
        pdHorLayout->setSpacing(8);
        pdHorLayout->setContentsMargins(16, 0, 0, 0);

        QLabel * pdIconLabel = new QLabel(pdWidget);
        QSizePolicy iconSizePolicy = pdIconLabel->sizePolicy();
        iconSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
        iconSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        pdIconLabel->setSizePolicy(iconSizePolicy);
        pdIconLabel->setFixedSize(QSize(24, 24));
        pdIconLabel->setScaledContents(true);
        pdIconLabel->setPixmap(QPixmap("://img/plugins/printer/printer.png"));

        QLabel * pdLabel = new QLabel(pdWidget);
        QSizePolicy txtSizePolicy = pdLabel->sizePolicy();
        txtSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
        txtSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
        pdLabel->setSizePolicy(txtSizePolicy);
        pdLabel->setScaledContents(true);
        pdLabel->setText(printer.at(num));

        pdHorLayout->addWidget(pdIconLabel);
        pdHorLayout->addWidget(pdLabel);
        pdHorLayout->addStretch();

        pdWidget->setLayout(pdHorLayout);

        //
        baseVerLayout->addWidget(pdWidget);
        baseVerLayout->addStretch();

        baseWidget->setLayout(baseVerLayout);


        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(ui->listWidget->width(), ITEMFIXEDHEIGH));
        ui->listWidget->setItemWidget(item, baseWidget);
    }
}

void Printer::runExternalApp(){
    QString cmd = "system-config-printer";

    QProcess process(this);
    process.startDetached(cmd);
}

//bool Printer::eventFilter(QObject *watched, QEvent *event)
//{
//    if (watched == ui->addFrame){
//        if (event->type() == QEvent::MouseButtonPress){
//            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
//            if (mouseEvent->button() == Qt::LeftButton){
//                runExternalApp();
//                return true;
//            } else
//                return false;
//        }
//    }
//    return QObject::eventFilter(watched, event);
//}
