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

#include "changegroupdialog.h"
#include "ui_changegroupdialog.h"
#include "definegroupitem.h"
#include "ImageUtil/imageutil.h"
//#include "group_manager_client.h"
#include "creategroupdialog.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeGroupDialog::ChangeGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeGroupDialog)
{
    ui->setupUi(this);
    setupInit();
    signalsBind();
}

ChangeGroupDialog::~ChangeGroupDialog()
{
    delete ui;
}

void ChangeGroupDialog::showCreateGroupDialog()
{
    CreateGroupDialog *dialog = new CreateGroupDialog;
    dialog->exec();

}

void ChangeGroupDialog::loadGroupInfo()
{
//    group_manager_client demo;
//    demo.value = demo.get_group_info();
//    qDebug() << "load group info" << demo.value->at(0)->groupname << demo.value->at(0)->groupid;
    //设置ListWidget是否可以自动排序,默认是false
    //list_widget->setSortingEnabled(true);
}

void ChangeGroupDialog::setupInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    initNewGroupBtn();

    loadGroupInfo();

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    //ui->titleLabel_2->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");


    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setSpacing(1);

    DefineGroupItem * singleWidget = new DefineGroupItem("aaa");
    singleWidget->setDeleteable(true);
    singleWidget->setUpdateable(true);
    singleWidget->setEditable(true);
    singleWidget->setFrameShape(QFrame::Shape::Box);
    singleWidget->setProperty("userData", true);

    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width() - 5, 50));
    item->setData(Qt::UserRole, "");
    ui->listWidget->setItemWidget(item, singleWidget);
}

void ChangeGroupDialog::signalsBind()
{
    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });
}

void ChangeGroupDialog::initNewGroupBtn()
{
    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(454, 50));
    addWgt->setMaximumSize(QSize(454, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add user group"));
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
        showCreateGroupDialog();
    });
    ui->addLyt->addWidget(addWgt);
}

void ChangeGroupDialog::initGeneralItemsStyle(){
    initItemsStyle(ui->listWidget);
}

void ChangeGroupDialog::initItemsStyle(QListWidget *listWidget){
    int total = listWidget->count();
    for (int row = 0; row < total; row++){
        QString style;
        QString subStyle;
        if (1 == total){ //总数为1
            style = "QWidget{background: #F4F4F4; border: none; border-radius: 6px;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (0 == row && (row % 2 == 0)){ //首位
            style = "QWidget{background: #F4F4F4; border: none; border-top-left-radius: 6px; border-top-right-radius: 6px;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (total - 1 == row){ //末位
            if (0 == row % 2){
                style = "QWidget{background: #F4F4F4; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}";
                subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
            } else {
                style = "QWidget{background: #EEEEEE; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}";
                subStyle = "background: #EEEEEE; border: none; border-radius: 4px;";
            }
        } else if (row % 2 == 0){
            style = "QWidget{background: #F4F4F4; border: none;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (row % 2 != 0){
            style = "QWidget{background: #EEEEEE; border: none;}";
            subStyle = "background: #EEEEEE; border: none; border-radius: 4px;";
        }

        QWidget * widget = listWidget->itemWidget(listWidget->item(row));
        DefineGroupItem * pShortcutItem = dynamic_cast<DefineGroupItem *>(widget);
//        pShortcutItem->widgetComponent()->setStyleSheet(style);
//        pShortcutItem->btnComponent()->setStyleSheet(subStyle);
    }
}

void ChangeGroupDialog::paintEvent(QPaintEvent * event){
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
