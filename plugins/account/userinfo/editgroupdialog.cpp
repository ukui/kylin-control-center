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

EditGroupDialog::EditGroupDialog(QString usergroup, QString groupid, QString groupname, bool idSetEnable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditGroupDialog),
    cgDialog(new ChangeGroupDialog),
    _nameHasModified(false),
    _idHasModified(false),
    _boxModified(false),
    userGroup(usergroup),
    groupId(groupid),
    groupName(groupname)
{
    ui->setupUi(this);
    setupInit();
    idSetEnabled(idSetEnable);
    getUsersList(userGroup);
    signalsBind();
}

EditGroupDialog::~EditGroupDialog()
{
    delete cgDialog;
    cgDialog = nullptr;
    delete ui;
    ui = nullptr;
}

void EditGroupDialog::limitInput()
{
    QIntValidator *intValidator = new QIntValidator;
    //QRegExp rx("^[a-zA-z]+$");// 首字符为字母
    QRegExp rx("[a-zA-z]{32}");
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

UserInfomations EditGroupDialog::_acquireUserInfo(QString objpath){
    UserInfomations user;

    //默认值
    user.current = false;
    user.logined = false;
    user.autologin = false;

    QDBusInterface * iproperty = new QDBusInterface("org.freedesktop.Accounts",
                                            objpath,
                                            "org.freedesktop.DBus.Properties",
                                            QDBusConnection::systemBus());
    QDBusReply<QMap<QString, QVariant> > reply = iproperty->call("GetAll", "org.freedesktop.Accounts.User");
    if (reply.isValid()){
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        user.username = propertyMap.find("UserName").value().toString();
        if (user.username == QString(g_get_user_name())) {
            user.current = true;
            user.logined = true;
        }
    }
    else
        qDebug() << "reply failed";

    delete iproperty;
    iproperty = nullptr;

    return user;
}


void EditGroupDialog::getUsersList(QString usergroup)
{
    QStringList allUsers;
    sysdispatcher = new SystemDbusDispatcher(this);

    QStringList objectpaths = sysdispatcher->list_cached_users();
    allUserInfoMap.clear();
    //root
    if (!getuid()){
        UserInfomations root;
        root.username = g_get_user_name();
        root.current = true;
        root.logined = true;
        root.autologin = false;
        root.uid = 0;
        root.accounttype = ADMINISTRATOR;
        //        root.iconfile = DEFAULTFACE;
        allUserInfoMap.insert(root.username, root);
    }
    for (QString objectpath : objectpaths){
        UserInfomations user;
        user = _acquireUserInfo(objectpath);
        allUserInfoMap.insert(user.username, user);
    }
    for (QVariant tmp : allUserInfoMap.keys()){
        allUsers << tmp.toString();

    }
    QStringList usersList = allUsers;
    QStringList usergroupList = usergroup.split(",");

    for(int i = 0; i < usersList.size(); i++){
        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(ui->listWidget->width(), 36));
        item->setData(Qt::UserRole, "");
        QCheckBox * box = new QCheckBox(usersList.at(i));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, box);
        if(usersList.at(i) == groupName){
            box->setChecked(true);
            box->setDisabled(true);
        } else{
            for (int j = 0; j < usergroupList.size(); j ++){
                if(usergroupList.at(j) == usersList.at(i)){
                    box->setChecked(true);
                }
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

void EditGroupDialog::idSetEnabled(bool idSetEnable)
{
    ui->lineEdit_id->setEnabled(idSetEnable);
}

void EditGroupDialog::nameSetEnabled()
{
    ui->lineEdit_name->setEnabled(false);
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
        for (int j = 0; j < cgDialog->groupList->size(); j++){
            if(ui->lineEdit_id->text() == cgDialog->groupList->at(j)->groupid){
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
                for (int j = 0; j < cgDialog->groupList->size(); j++){
                    if(ui->lineEdit_id->text() == cgDialog->groupList->at(j)->groupid){
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
        cgDialog = nullptr;
        close();
    });
}

void EditGroupDialog::setupInit()
{
    setWindowTitle(tr("Edit user group"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setStyleSheet("QListWidget{border-radius: 4px;}"
                                  "QListWidget{background-color: palette(button);}"
                                  "QListWidget::Item{padding-left:20px;}");

    // 设置确认按钮
    refreshCertainBtnStatus();
    // 限制组名输入规则
    limitInput();
    nameSetEnabled();
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
