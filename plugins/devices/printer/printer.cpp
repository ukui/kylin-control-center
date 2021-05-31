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

Printer::Printer() : mFirstLoad(true)
{
    //~ contents_path /printer/Printer
    pluginName = tr("Printer");
    pluginType = DEVICES;
}

Printer::~Printer()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString Printer::get_plugin_name()
{
    return pluginName;
}

int Printer::get_plugin_type()
{
    return pluginType;
}

QWidget *Printer::get_plugin_ui()
{
    if (mFirstLoad) {
        mFirstLoad = false;
        ui = new Ui::Printer;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        //~ contents_path /printer/Add Printers And Scanners
        ui->titleLabel->setText(tr("Add Printers And Scanners"));

        // 禁用选中效果
        ui->listWidget->setFocusPolicy(Qt::NoFocus);
        ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);

        initTitleLabel();
        initComponent();

        refreshPrinterDevSlot();
    }
    return pluginWidget;
}

void Printer::plugin_delay_control()
{
}

const QString Printer::name() const
{
    return QStringLiteral("printer");
}

void Printer::initTitleLabel()
{
    ui->listWidget->setSpacing(1);
}

void Printer::initComponent()
{
    mAddWgt = new HoverWidget("", pluginWidget);
    mAddWgt->setObjectName("addwgt");
    mAddWgt->setMinimumSize(QSize(580, 50));
    mAddWgt->setMaximumSize(QSize(960, 50));
    mAddWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button);border-radius: 4px;}"
                           "HoverWidget:hover:!pressed#addwgt{background: rgb(64,169,251); border-radius: 4px;}");

    ui->listWidget->setStyleSheet("QListWidget::Item:hover{background:palette(base);}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel *iconLabel = new QLabel();
    QLabel *textLabel = new QLabel(tr("Add printers and scanners"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    //iconLabel->setProperty("useIconHighlightEffect", true);
    //iconLabel->setProperty("iconHighlightEffectMode", 1);

    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    mAddWgt->setLayout(addLyt);

    connect(mAddWgt, &HoverWidget::widgetClicked, this, [=](QString mname) {
        Q_UNUSED(mname)
        runExternalApp();
    });

    // 悬浮改变Widget状态
    connect(mAddWgt, &HoverWidget::enterWidget, this, [=](){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });

    // 还原状态
    connect(mAddWgt, &HoverWidget::leaveWidget, this, [=](){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });
    ui->addLyt->addWidget(mAddWgt);
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, [=] {
        refreshPrinterDevSlot();
    });

    mTimer->start(1000);
}

void Printer::refreshPrinterDevSlot()
{
    QStringList printer = QPrinterInfo::availablePrinterNames();

    for (int num = 0; num < printer.count(); num++) {
        QStringList env = QProcess::systemEnvironment();

        env << "LANG=en_US.UTF-8";

        QProcess *process = new QProcess;
        process->setEnvironment(env);
        process->start("lpstat -p "+printer.at(num));
        process->waitForFinished();

        QString ba = process->readAllStandardOutput();
        delete process;
        QString printer_stat = QString(ba.data());

        HoverBtn *printerItem = new HoverBtn(printer.at(num), pluginWidget);
        printerItem->mPitLabel->setText(printer.at(num));
        printerItem->mAbtBtn->setText(tr("Attrs"));
        QIcon printerIcon = QIcon::fromTheme("printer");
        printerItem->mPitIcon->setPixmap(printerIcon.pixmap(printerIcon.actualSize(QSize(24, 24))));
        connect(printerItem->mAbtBtn, &QPushButton::clicked, this, [=] {
            runExternalApp();
        });

        // 标志位flag用来判断该打印机是否可用，flag1用来决定是否新增窗口(为真则加)
        bool flag = printer_stat.contains("disable", Qt::CaseSensitive)
                    || printer_stat.contains("Unplugged or turned off", Qt::CaseSensitive);

        bool flag1 = true;

        // 遍历窗口列表，判断列表中是否已经存在该打印机，若存在，便判断该打印机是否可用，不可用则从列表中删除该打印机窗口
        for (int j = 0; j < ui->listWidget->count(); j++) {
            QString itemData = ui->listWidget->item(j)->data(Qt::UserRole).toString();
            if (!itemData.compare(printer.at(num))) {
                if (flag) {
                    ui->listWidget->takeItem(j);
                    flag1 = false;
                    break;
                }
                flag1 = false;
                break;
            }
        }

        //
        if (!flag && flag1) {
            QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
            item->setData(Qt::UserRole, printer.at(num));

            item->setSizeHint(QSize(QSizePolicy::Expanding, 50));
            ui->listWidget->setItemWidget(item, printerItem);
        }
    }
}

void Printer::runExternalApp()
{
    QString cmd = "system-config-printer";

    QProcess process(this);
    process.startDetached(cmd);
}
