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
#include "vpn.h"
#include "ui_vpn.h"

#include <QProcess>
#include <QMouseEvent>

Vpn::Vpn() : mFirstLoad(true)
{
    pluginName = tr("Vpn");
    pluginType = NETWORK;
}

Vpn::~Vpn()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString Vpn::get_plugin_name(){
    return pluginName;
}

int Vpn::get_plugin_type(){
    return pluginType;
}

QWidget *Vpn::get_plugin_ui(){
    if (mFirstLoad) {
        mFirstLoad = false;
        ui = new Ui::Vpn;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        initComponent();
    }
    return pluginWidget;
}

void Vpn::plugin_delay_control(){

}

const QString Vpn::name() const {

    return QStringLiteral("vpn");
}

void Vpn::initComponent(){
    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: rgb(64,169,251); border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    //~ contents_path /vpn/Add vpn connect
    QLabel * textLabel = new QLabel(tr("Add vpn connect"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    //iconLabel->setProperty("useIconHighlightEffect", true);
    //iconLabel->setProperty("iconHighlightEffectMode", 1);

    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        runExternalApp();
    });

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });

    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](){
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    ui->addLyt->addWidget(addWgt);

}

void Vpn::runExternalApp(){
    QString cmd = "nm-connection-editor";
    QProcess process(this);
    process.startDetached(cmd);
}
