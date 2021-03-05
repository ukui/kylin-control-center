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
#include "ukccabout.h"
#include "../utils/xatom-helper.h"

#include <QVariant>
#include <QDesktopServices>
#include <QUrl>

UkccAbout::UkccAbout(QWidget *parent)
    : QDialog(parent) {

    setFixedSize(420, 560);

    XAtomHelper::getInstance()->setUKUIDecoraiontHint(this->winId(), true);
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);
    initUI();
    initConnection();
}

void UkccAbout::initUI() {

    mMainVLayout = new QVBoxLayout();
    mTitleLayout = new QHBoxLayout();
    mCenterIconLayout = new QHBoxLayout();
    mCenterTitleLayout = new QHBoxLayout();
    mCenterVersionLayout = new QHBoxLayout();
    mUkccDetailLayout = new QHBoxLayout();
    mUkccDeveloperLayout = new QHBoxLayout();

    mMainVLayout->setContentsMargins(8, 4, 4, 4);
    mTitleLayout->setSpacing(0);

    QIcon titleIcon = QIcon::fromTheme("ukui-control-center");

    mUkccIcon  = new QLabel();
    mUkccIcon->setPixmap(titleIcon.pixmap(titleIcon.actualSize(QSize(24, 24))));

    mUkccTitle = new QLabel(tr("UKCC"));

    mUkccCloseBtn = new QPushButton();
    mUkccCloseBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    mUkccCloseBtn->setProperty("isWindowButton", 0x02);
    mUkccCloseBtn->setProperty("useIconHighlightEffect", 0x08);
    mUkccCloseBtn->setFlat(true);
    mUkccCloseBtn->setFixedSize(30, 30);

    mTitleLayout->addWidget(mUkccIcon);
    mTitleLayout->addSpacing(8);
    mTitleLayout->addWidget(mUkccTitle);
    mTitleLayout->addStretch();
    mTitleLayout->addWidget(mUkccCloseBtn);

    mUkccCenterIcon = new QLabel();
    mUkccCenterIcon->setPixmap(titleIcon.pixmap(titleIcon.actualSize(QSize(96, 96))));

    mCenterIconLayout->addStretch();
    mCenterIconLayout->addWidget(mUkccCenterIcon);
    mCenterIconLayout->addStretch();

    mUkccCenterTitle = new QLabel(tr("UKCC"));
    mUkccCenterTitle->setFixedHeight(28);

    mCenterTitleLayout->addStretch();
    mCenterTitleLayout->addWidget(mUkccCenterTitle);
    mCenterTitleLayout->addStretch();

    mUkccVersion = new QLabel(tr("Version: ") + getUkccVersion());
    mUkccVersion->setFixedHeight(24);

    mCenterVersionLayout->addStretch();
    mCenterVersionLayout->addWidget(mUkccVersion);
    mCenterVersionLayout->addStretch();

    mUkccDetail = new QTextEdit(tr("The control panel provides a friendly graphical user interface to manage common configuration items of the operating system. "
                                "System configuration provides system, equipment, personalization, network, account, time and date, account, time and date, update, notification and operation module operations. "));

    mUkccDetail->setReadOnly(true);
    mUkccDetailLayout->addSpacing(32);
    mUkccDetailLayout->addWidget(mUkccDetail);
    mUkccDetailLayout->addSpacing(32);

    mUkccDeveloper = new QLabel(tr("Service and Support:"));

    mUkccDeveloperLayout->addSpacing(32);
    mUkccDeveloperLayout->addWidget(mUkccDeveloper);

    mUkccDeveloperEmailBtn = new QPushButton("support@kylinos.cn");
    mUkccDeveloperEmailBtn->setFixedSize(185, 50);
    mUkccDeveloperEmailBtn->setFocusPolicy(Qt::NoFocus);
    mUkccDeveloperEmailBtn->setContentsMargins(0,0,0,0);
    mUkccDeveloperEmailBtn->setCursor(QCursor(Qt::PointingHandCursor));
    mUkccDeveloperEmailBtn->setStyleSheet("QPushButton{background: transparent;border-radius: 4px;text-decoration: underline;} ");

    mUkccDeveloperLayout->addWidget(mUkccDeveloperEmailBtn);
    mUkccDeveloperLayout->setAlignment(Qt::AlignLeft);
    connect(mUkccDeveloperEmailBtn, &QPushButton::clicked, this,[=] {
        QDesktopServices::openUrl(QUrl(QLatin1String("mailto:support@kylinos.cn")));
    });

    mMainVLayout->addLayout(mTitleLayout);
    mMainVLayout->addSpacing(42);
    mMainVLayout->addLayout(mCenterIconLayout);
    mMainVLayout->addSpacing(16);
    mMainVLayout->addLayout(mCenterTitleLayout);
    mMainVLayout->addSpacing(12);
    mMainVLayout->addLayout(mCenterVersionLayout);
    mMainVLayout->addSpacing(12);
    mMainVLayout->addLayout(mUkccDetailLayout);
    mMainVLayout->addSpacing(24);
    mMainVLayout->addLayout(mUkccDeveloperLayout);
    mMainVLayout->addSpacing(40);

    this->setLayout(mMainVLayout);
}

void UkccAbout::initConnection() {
    connect(mUkccCloseBtn, &QPushButton::clicked, this, [=]() {
       this->close();
    });
}

QString UkccAbout::getUkccVersion() {
    FILE *pp = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *q = NULL;
    QString version = "none";

    pp = popen("dpkg -l  ukui-control-center | grep  ukui-control-center", "r");
    if(NULL == pp)
        return version;

    while((read = getline(&line, &len, pp)) != -1){
        q = strrchr(line, '\n');
        *q = '\0';

        QString content = line;
        QStringList list = content.split(" ");

        list.removeAll("");

        if (list.size() >= 3)
            version = list.at(2);
    }

    free(line);
    line = NULL;
    pclose(pp);
    return version;
}
