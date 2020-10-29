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
#include "creategroupdialog.h"
#include "editgroupdialog.h"
#include "delgroupdialog.h"
#include "CloseButton/closebutton.h"

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

void ChangeGroupDialog::connectToServer()
{
    serviceInterface = new QDBusInterface("org.ukui.groupmanager",
                                          "/org/ukui/groupmanager",
                                          "org.ukui.groupmanager.interface",
                                          QDBusConnection::systemBus());
    if (!serviceInterface->isValid())
    {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    // 将以后所有DBus调用的超时设置为 milliseconds
    serviceInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void ChangeGroupDialog::loadGroupInfo()
{
    qDebug() << "loadGroupInfo";
    //check_permission();
    QDBusMessage msg = serviceInterface->call("get");
    if(msg.type() == QDBusMessage::ErrorMessage) {
        printf("get group info fail.\n");
    }
    QDBusArgument argument = msg.arguments().at(0).value<QDBusArgument>();
    QList<QVariant> infos;
    argument >> infos;

    value = new QList<custom_struct *>();
    for (int i = 0; i < infos.size(); i++)
    {
        custom_struct *dbus_struct = new custom_struct;
        infos.at(i).value<QDBusArgument>() >> *dbus_struct;
        value->push_back(dbus_struct);
    }
}

void ChangeGroupDialog::refreshList()
{
    qDebug() << "refresh list";
    int count = ui->listWidget->count();
    for(int i = count; i >= 0; i--){
        QListWidgetItem *item = ui->listWidget->item(i);
        ui->listWidget->takeItem(i);
        ui->listWidget->removeItemWidget(item);
        delete item;
    }
    loadGroupInfo();
    loadAllGroup();
}

bool ChangeGroupDialog::polkit()
{
    PolkitQt1::Authority::Result result;
    //PolkitQt1::SystemBusNameSubject subject(message().service());

    result = PolkitQt1::Authority::instance()->checkAuthorizationSync(
                "org.ukui.groupmanager.action",
                PolkitQt1::UnixProcessSubject(QCoreApplication::applicationPid()),
                PolkitQt1::Authority::AllowUserInteraction);
    if (result == PolkitQt1::Authority::Yes) { //认证通过
        qDebug() << QString("operation authorized");
        return true;
    } else {
        qDebug() << QString("not authorized");
        return false;
    }
}

void ChangeGroupDialog::loadAllGroup()
{
    for(int i = 0; i < value->size(); i++){
        DefineGroupItem * singleWidget = new DefineGroupItem(value->at(i)->groupname);
        if(value->at(i)->groupname == "root"){
            singleWidget->setDeleteable(false);
            singleWidget->setEditable(false);
        } else{
            singleWidget->setDeleteable(true);
            singleWidget->setUpdateable(true);
            singleWidget->setEditable(true);
        }
        singleWidget->setFrameShape(QFrame::Shape::Box);
        singleWidget->setProperty("userData", true);

        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(ui->listWidget->width() - 5, 50));
        item->setData(Qt::UserRole, "");
        ui->listWidget->setItemWidget(item, singleWidget);

        QPushButton *itemDelBtn = singleWidget->delBtnComponent();
        QPushButton *itemEditBtn = singleWidget->editBtnComponent();

        connect(itemDelBtn, &QPushButton::clicked, [=](){
            bool reply = polkit();
            qDebug() << "call polkit " << reply;
            if(reply){
                DelGroupDialog *delDialog = new DelGroupDialog(value->at(i)->groupname);
                QPushButton *delBtn = delDialog->delBtnComponent();
                connect(delBtn, &QPushButton::clicked, [=](){
                    QDBusReply<bool> reply = serviceInterface->call("del",value->at(i)->groupname);
                    if (reply.isValid()){
                        // use the returned value
                        qDebug() << "get call value" << reply.value();
    //                    qDebug() << "current index" << ui->listWidget->currentIndex();
                        ui->listWidget->removeItemWidget(item);
                        delete item;
                        ui->listWidget->scrollTo(ui->listWidget->currentIndex());
                        delDialog->close();
                    } else {
                        // call failed. Show an error condition.
                        qDebug() << "call failed" << reply.error();
                    }
                    refreshList();
                });
                delDialog->exec();
            }
        });
        connect(itemEditBtn, &QPushButton::clicked, [=](){
            bool reply = polkit();
            qDebug() << "call polkit " << reply;
            if(reply){
                EditGroupDialog *editDialog = new EditGroupDialog(value->at(i)->usergroup,value->at(i)->groupid);
                connect(editDialog, &EditGroupDialog::needRefresh, this, &ChangeGroupDialog::needRefreshSlot);
                QLineEdit *lineName = editDialog->lineNameComponent();
                QLineEdit *lineId = editDialog->lineIdComponent();
                lineName->setText(value->at(i)->groupname);
                lineId->setText(value->at(i)->groupid);
                editDialog->exec();
            }

        });
    }
//    ui->listWidget->setSortingEnabled(true);
}

void ChangeGroupDialog::setupInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    //ui->titleLabel_2->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
//    ui->closeBtn->setProperty("useIconHighlightEffect", true);
//    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
//    ui->closeBtn->setFlat(true);

//    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
//                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");


    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setSpacing(1);

    connectToServer();
    initNewGroupBtn();
    loadGroupInfo();
    loadAllGroup();
}

void ChangeGroupDialog::signalsBind()
{
    connect(ui->closeBtn, &CloseButton::clicked, [=]{
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

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](){
        bool reply = polkit();
        qDebug() << "call polkit " << reply;
        if(reply){
            CreateGroupDialog *dialog = new CreateGroupDialog();
            QPushButton *certainBtn = dialog->certainBtnComponent();
            QLineEdit *lineId = dialog->lineIdComponent();
            QLineEdit *lineName = dialog->lineNameComponent();
            QListWidget *cglist = dialog->listWidgetComponent();

            connect(certainBtn, &QPushButton::clicked, this, [=](){
                for (int j = 0; j < value->size(); j++){
                    if(lineId->text() == value->at(j)->groupid){
                        QMessageBox invalid(QMessageBox::Question, tr("Tips"), tr("Invalid Id!"));
                        invalid.setIcon(QMessageBox::Warning);
                        invalid.setStandardButtons(QMessageBox::Ok);
                        invalid.setButtonText(QMessageBox::Ok, QString(tr("OK")));
                        invalid.exec();
                        return;
                    }
                    if(lineName->text() == value->at(j)->groupname){
                        QMessageBox invalid(QMessageBox::Question, tr("Tips"), tr("Invalid Group Name!"));
                        invalid.setIcon(QMessageBox::Warning);
                        invalid.setStandardButtons(QMessageBox::Ok);
                        invalid.setButtonText(QMessageBox::Ok, QString(tr("OK")));
                        invalid.exec();
                        return;
                    }
                }

                QDBusReply<bool> reply = serviceInterface->call("add",lineName->text(),lineId->text());
                if (reply.isValid()){
                    // use the returned value
                    qDebug() << "get call value" << reply.value();
                } else {
                    // call failed. Show an error condition.
                    qDebug() << "call failed" << reply.error();
                }

                for (int i = 0; i < cglist->count(); i++){
                    QListWidgetItem *item = cglist->item(i);
                    QCheckBox *box = static_cast<QCheckBox *> (cglist->itemWidget(item));
                    if(box->isChecked()){
                        QDBusReply<bool> reply = serviceInterface->call("addUserToGroup",
                                                        lineName->text(),box->text());
                        if (reply.isValid()){
                            // use the returned value
                            qDebug() << "addUserToGroupget call value" << reply.value() << lineName->text() << box->text();
                        } else {
                            // call failed. Show an error condition.
                            qDebug() << "addUserToGroup call failed" << reply.error();
                        }
                    } else {
                        QDBusReply<bool> reply = serviceInterface->call("delUserFromGroup",
                                                        lineId->text(),box->text());
                        if (reply.isValid()){
                            // use the returned value
                            qDebug() << "delUserFromGroup get call value" << reply.value() << lineName->text() << box->text();;
                        } else {
                            // call failed. Show an error condition.
                            qDebug() << "delUserFromGroup call failed" << reply.error() << lineName->text() << box->text();;
                        }
                    }
                }
                refreshList();
                ui->listWidget->scrollToBottom();
                dialog->close();
            });
            dialog->exec();
        }
    });
    ui->addLyt->addWidget(addWgt);
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

void ChangeGroupDialog::needRefreshSlot()
{
    refreshList();
}
