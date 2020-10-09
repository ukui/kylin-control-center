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
#include "sharemain.h"

#include <QHBoxLayout>
#include <QAbstractButton>

ShareMain::ShareMain(QWidget *parent)
    : QWidget(parent) {

    mVlayout = new QVBoxLayout(this);
    mVlayout->setContentsMargins(0, 0, 32, 0);
    initUI();
    initConnection();
}

ShareMain::~ShareMain() {

}

void ShareMain::initUI() {
    mBtnGroup = new QButtonGroup(this);

    mShareTitleLabel = new QLabel(tr("Share"), this);
    mShareTitleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    mViewFrame = new QFrame(this);
    mViewFrame->setFrameShape(QFrame::Shape::Box);
    mViewFrame->setMinimumSize(550, 50);
    mViewFrame->setMaximumSize(960, 50);

    QHBoxLayout * viewHLayout = new QHBoxLayout();

    mViewBox = new QCheckBox(this);
    mViewLabel = new QLabel(tr("Allow others to view your desktop"), this);
    viewHLayout->addWidget(mViewBox);
    viewHLayout->addWidget(mViewLabel);
    viewHLayout->addStretch();

    mViewFrame->setLayout(viewHLayout);

    mSecurityTitleLabel = new QLabel(tr("Security"), this);

    mSecurityFrame = new QFrame(this);
    mSecurityFrame->setFrameShape(QFrame::Shape::Box);
    mSecurityFrame->setMinimumSize(550, 50);
    mSecurityFrame->setMaximumSize(960, 50);

    QHBoxLayout * secHLayout = new QHBoxLayout();

    mAccessBox = new QRadioButton(this);
    mAccessLabel = new QLabel(tr("You must confirm every visit for this machine"), this);
    secHLayout->addWidget(mAccessBox);
    secHLayout->addWidget(mAccessLabel);
    secHLayout->addStretch();

    mSecurityFrame->setLayout(secHLayout);

    mSecurityPwdFrame = new QFrame(this);
    mSecurityPwdFrame->setFrameShape(QFrame::Shape::Box);
    mSecurityPwdFrame->setMinimumSize(550, 50);
    mSecurityPwdFrame->setMaximumSize(960, 50);

    QHBoxLayout * pwdHLayout = new QHBoxLayout();

    mPwdBox = new QRadioButton(this);
    mPwdsLabel = new QLabel(tr("Require user to enter this password: "), this);
    mPwdLineEdit = new QLineEdit(this);
    pwdHLayout->addWidget(mPwdBox);
    pwdHLayout->addWidget(mPwdsLabel);
    pwdHLayout->addWidget(mPwdLineEdit);

    mSecurityPwdFrame->setLayout(pwdHLayout);

    mVlayout->addWidget(mShareTitleLabel);
    mVlayout->addWidget(mViewFrame);

    mVlayout->addWidget(mSecurityTitleLabel);
    mVlayout->addWidget(mSecurityFrame);
    mVlayout->addWidget(mSecurityPwdFrame);

    mVlayout->addStretch();

    mBtnGroup->addButton(mAccessBox);
    mBtnGroup->addButton(mPwdBox);

    mBtnGroup->setId(mAccessBox, RequestPwd::NOPWD);
    mBtnGroup->setId(mPwdBox, RequestPwd::NEEDPWD);
}

void ShareMain::initConnection() {
    QByteArray id(kVinoSchemas);
    if (QGSettings::isSchemaInstalled(id)) {
        mVinoGsetting = new QGSettings(kVinoSchemas, QByteArray(), this);

        connect(mViewBox, &QCheckBox::clicked, this, &ShareMain::viewBoxSlot);
        connect(mPwdLineEdit, &QLineEdit::textChanged, this, &ShareMain::pwdInputSlot);
        connect(mBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            [=](int index) {
            accessSlot(index);
        });

        bool isShared = mVinoGsetting->get(kVinoViewOnlyKey).toBool();
        bool secPwd = mVinoGsetting->get(kVinoPromptKey).toBool();
        initShareStatus(!isShared, secPwd);
    }
}

void ShareMain::initShareStatus(bool isConnnect, bool isPwd) {
    mViewBox->setChecked(isConnnect);
    if (isPwd) {
        mAccessBox->setChecked(isPwd);
    } else {
        mPwdBox->setChecked(true);
    }
}

void ShareMain::viewBoxSlot(bool status) {
    Q_UNUSED(status);
    if (status) {
        mVinoGsetting->set(kVinoViewOnlyKey, status);
    }
}

void ShareMain::accessSlot(int index) {
    Q_UNUSED(index);
    if (NOPWD == index) {
        mPwdsLabel->setEnabled(false);
        mPwdLineEdit->setEnabled(false);
        mVinoGsetting->set(kVinoPromptKey, true);
        mVinoGsetting->reset(kAuthenticationKey);
        mVinoGsetting->reset(kVncPwdKey);
    } else {
        mPwdsLabel->setEnabled(true);
        mPwdLineEdit->setEnabled(true);
        mVinoGsetting->set(kVinoPromptKey, false);
        mVinoGsetting->reset(kAuthenticationKey), 'vnc';
    }
}

void ShareMain::pwdInputSlot(QString pwd) {
    Q_UNUSED(pwd);
    QByteArray text = pwd.toLocal8Bit();
    QByteArray secPwd = text.toBase64();
    mVinoGsetting->set(kVncPwdKey, secPwd);
}
