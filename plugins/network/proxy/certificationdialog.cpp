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
#include "certificationdialog.h"
#include "ui_certificationdialog.h"

#include <QGSettings>
#include "SwitchButton/switchbutton.h"

#include <QDebug>

CertificationDialog::CertificationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CertificationDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Certification"));

    const QByteArray id(HTTP_PROXY_SCHEMA);
    cersettings = new QGSettings(id);

    component_init();
    status_init();
}

CertificationDialog::~CertificationDialog()
{
    delete ui;
    ui = nullptr;
    delete cersettings;
    cersettings = nullptr;
}

void CertificationDialog::component_init(){
    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);

    activeSwitchBtn = new SwitchButton;
    activeSwitchBtn->setAttribute(Qt::WA_DeleteOnClose);
    ui->activeHLayout->addWidget(activeSwitchBtn);
    ui->activeHLayout->addStretch();
}

void CertificationDialog::status_init(){
    //获取认证状态
    bool status = cersettings->get(HTTP_AUTH_KEY).toBool();
    activeSwitchBtn->setChecked(status);
    ui->widget->setEnabled(status);

    //获取用户名密码
    QString user = cersettings->get(HTTP_AUTH_USER_KEY).toString();
    ui->userLineEdit->setText(user);
    QString pwd = cersettings->get(HTTP_AUTH_PASSWD_KEY).toString();
    ui->pwdLineEdit->setText(pwd);

    connect(activeSwitchBtn, SIGNAL(checkedChanged(bool)), this, SLOT(active_status_changed_slot(bool)));
    connect(ui->closePushBtn, SIGNAL(released()), this, SLOT(close()));

    connect(ui->userLineEdit, SIGNAL(textChanged(QString)), this, SLOT(user_edit_changed_slot(QString)));
    connect(ui->pwdLineEdit, SIGNAL(textChanged(QString)), this, SLOT(pwd_edit_changed_slot(QString)));
}

void CertificationDialog::active_status_changed_slot(bool status){
    ui->widget->setEnabled(status);
    cersettings->set(HTTP_AUTH_KEY, QVariant(status));
}

void CertificationDialog::user_edit_changed_slot(QString edit){
    cersettings->set(HTTP_AUTH_USER_KEY, QVariant(edit));
}

void CertificationDialog::pwd_edit_changed_slot(QString edit){
    cersettings->set(HTTP_AUTH_PASSWD_KEY, QVariant(edit));
}
