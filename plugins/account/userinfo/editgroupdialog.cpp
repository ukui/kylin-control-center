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

#include "editgroupdialog.h"
#include "ui_editgroupdialog.h"
#include "userinfo.h"
#include "changegroupdialog.h"
#include "CloseButton/closebutton.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

EditGroupDialog::EditGroupDialog(QString usergroup, QString groupid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditGroupDialog),
    _nameHasModified(false),
    _idHasModified(false),
    _boxModified(false),
    userGroup(usergroup),
    groupId(groupid),
    cgDialog(new ChangeGroupDialog)
{
    ui->setupUi(this);
    qDebug() << "new EditGroupDialog" << userGroup << groupId;
    setupInit();
    getUsersList(userGroup);
    signalsBind();
}

EditGroupDialog::~EditGroupDialog()
{
    delete cgDialog;
    delete ui;
}

void EditGroupDialog::limitInput()
{
    QIntValidator *intValidator = new QIntValidator;
    //QRegExp rx("^[a-zA-z]+$");// 首字符为字母
    QRegExp rx("[a-zA-z]{40}");
    QRegExpValidator *regValidator = new QRegExpValidator(rx);
    //intValidator->setRange(0, 65535);
    intValidator->setBottom(0);
    // 整形输入限制
    ui->lineEdit_id->setValidator(intValidator);
    // 字母输入限制
    ui->lineEdit_name->setValidator(regValidator);
    // 字符长度限制
    //ui->lineEdit_name->setMaxLength(4);
}

void EditGroupDialog::refreshCertainBtnStatus(){
    if (ui->lineEdit_name->text().isEmpty() ||
            ui->lineEdit_id->text().isEmpty())
        ui->certainBtn->setEnabled(false);
    else
        ui->certainBtn->setEnabled(_nameHasModified || _idHasModified || _boxModified);
}

void EditGroupDialog::getUsersList(QString usergroup)
{
    UserInfo * userinfo = new UserInfo;
    QStringList usersList = userinfo->getUsersList();
    qDebug() << "EditGroupDialog::getUsersList";
    QStringList usergroupList = usergroup.split(",");

    for(int i = 0; i < usersList.size(); i++){
        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(ui->listWidget->width(), 36));
        item->setData(Qt::UserRole, "");
        QCheckBox * box = new QCheckBox(usersList.at(i));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, box);

        for (int j = 0; j < usergroupList.size(); j ++){
            if(usergroupList.at(j) == usersList.at(i)){
                box->setChecked(true);
            }
        }


        connect(box, &QCheckBox::clicked, this, [=](bool checked){
            Q_UNUSED(checked);
            qDebug() << "checkbox clicked";
            _boxModified = true;
            refreshCertainBtnStatus();
        });
    }
}

QLineEdit *EditGroupDialog::lineNameComponent()
{
    return ui->lineEdit_name;
}

QLineEdit *EditGroupDialog::lineIdComponent()
{
    return ui->lineEdit_id;
}

QListWidget *EditGroupDialog::listWidgetComponent()
{
    return ui->listWidget;
}

void EditGroupDialog::signalsBind()
{
    connect(ui->closeBtn, &CloseButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });
    connect(ui->cancelBtn,&QPushButton::clicked,[=](){
        close();
    });
    connect(ui->lineEdit_name,&QLineEdit::textChanged,[=](QString txt){
        refreshCertainBtnStatus();
    });
    connect(ui->lineEdit_id,&QLineEdit::textChanged,[=](QString txt){
        refreshCertainBtnStatus();
    });
    connect(ui->lineEdit_id, &QLineEdit::textEdited,[=](){
//        if(ui->lineEdit_id->text() != groupId){
//            qDebug() << "ui->lineEdit_id->text() != groupId";
//            _idHasModified = true;
//        } else {
//            _idHasModified = false;
//        }
        for (int j = 0; j < cgDialog->value->size(); j++){
            if(ui->lineEdit_id->text() == cgDialog->value->at(j)->groupid){
                _idHasModified = false;
                return;
            }
        }
        _idHasModified = true;
    });
    connect(ui->lineEdit_name, &QLineEdit::textEdited,[=](){
        _nameHasModified = true;
    });
    connect(ui->certainBtn, &QPushButton::clicked, this, [=](){
        ChangeGroupDialog *cgDialog = new ChangeGroupDialog;
        for (int i = 0; i < ui->listWidget->count(); i++){
            if(_idHasModified){
                for (int j = 0; j < cgDialog->value->size(); j++){
                    if(ui->lineEdit_id->text() == cgDialog->value->at(j)->groupid){
                        QMessageBox invalid(QMessageBox::Question, tr("Tips"), tr("Invalid Id!"));
                        invalid.setIcon(QMessageBox::Warning);
                        invalid.setStandardButtons(QMessageBox::Ok);
                        invalid.setButtonText(QMessageBox::Ok, QString(tr("OK")));
                        invalid.exec();
                        return;
                    }
                }
            }
            QListWidgetItem *item = ui->listWidget->item(i);
            QCheckBox *box = static_cast<QCheckBox *> (ui->listWidget->itemWidget(item));

            QDBusReply<bool> reply = cgDialog->serviceInterface->call("set",
                                            ui->lineEdit_name->text(),ui->lineEdit_id->text());
            if (reply.isValid()){
                // use the returned value
                qDebug() << "set get call value" << reply.value();
            } else {
                // call failed. Show an error condition.
                qDebug() << "set call failed" << reply.error();
            }

            if(box->isChecked()){
                QDBusReply<bool> reply = cgDialog->serviceInterface->call("addUserToGroup",
                                                ui->lineEdit_name->text(),box->text());
                if (reply.isValid()){
                    // use the returned value
                    qDebug() << "addUserToGroup get call value" << reply.value();
                } else {
                    // call failed. Show an error condition.
                    qDebug() << "addUserToGroup call failed" << reply.error();
                }
            } else {
                QDBusReply<bool> reply = cgDialog->serviceInterface->call("delUserFromGroup",
                                                ui->lineEdit_name->text(),box->text());
                if (reply.isValid()){
                    // use the returned value
                    qDebug() << "delUserFromGroup get call value" << reply.value();
                } else {
                    // call failed. Show an error condition.
                    qDebug() << "delUserFromGroup call failed" << reply.error();
                }
            }
        }
        emit needRefresh();
        delete cgDialog;
        close();
    });
}

void EditGroupDialog::setupInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
//    ui->closeBtn->setProperty("useIconHighlightEffect", true);
//    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
//    ui->closeBtn->setFlat(true);

//    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
//                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");
    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setSpacing(5);
    ui->lineEdit_name->setEnabled(false);
    ui->lineEdit_name->setStyleSheet("QLineEdit{background:#EEEEEE;}");
    ui->lineEdit_id->setStyleSheet("QLineEdit{background:#EEEEEE;}");
    ui->listWidget->setStyleSheet("QListWidget{background:#EEEEEE; border-radius: 4px;}"
                                  "QListWidget::Item:hover{background:#FFE1E1E1}"
                                  "QListWidget::Item{padding-left:20px;}");

    // 设置确认按钮
    refreshCertainBtnStatus();
    // 限制组名输入规则
    limitInput();
    //
}

void EditGroupDialog::paintEvent(QPaintEvent *event) {
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
