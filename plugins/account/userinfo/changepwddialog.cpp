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
#include "changepwddialog.h"
#include "ui_changepwddialog.h"

#include "elipsemaskwidget.h"
#include "passwdcheckutil.h"

#include <QStyledItemDelegate>

#include <QDebug>

#include "CloseButton/closebutton.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>

}

#include "run-passwd.h"

#define PWD_LOW_LENGTH 6
#define PWD_HIGH_LENGTH 20

QString ChangePwdDialog::curPwdTip = "";

ChangePwdDialog * cpdGlobalObj = new ChangePwdDialog(false);

PasswdHandler * passwd_handler = NULL;

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

static void chpasswd_cb(PasswdHandler * passwd_handler, GError * error, gpointer user_data);
static void auth_cb(PasswdHandler * passwd_handler, GError * error, gpointer user_data);

ChangePwdDialog::ChangePwdDialog(bool _isCurrentUser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePwdDialog),
    isCurrentUser(_isCurrentUser)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Change pwd"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->pwdFrame->setFrameShape(QFrame::Shape::Box);
    ui->tipLabel->setAlignment(Qt::AlignCenter);
    ui->tipLabel->setStyleSheet("color:red;");

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

//    isCurrentUser = true;

    //初始化passwd对象
    if (isCurrentUser){
        passwd_handler = passwd_init();
        connect(ui->curPwdLineEdit, &QLineEdit::editingFinished, [=]{

            if (isCurrentUser){
                if (!ui->curPwdLineEdit->text().isEmpty()){
                    curPwdTip = tr("Cur pwd checking!");
                    cpdGlobalObj->helpEmitSignal();

                    std::string str1 = ui->curPwdLineEdit->text().toStdString();
                    const char * old_passwd = str1.c_str();
                    passwd_authenticate(passwd_handler, old_passwd, auth_cb, NULL);
                } else {
                    curPwdTip = "";
                    cpdGlobalObj->helpEmitSignal();
                }
            }
        });
    } else {
        connect(ui->curPwdLineEdit, &QLineEdit::editingFinished, [=]{

            if (checkOtherPasswd(ui->usernameLabel->text(), ui->curPwdLineEdit->text())){
                curPwdTip = "";
            } else {
                curPwdTip = QObject::tr("Pwd input error, re-enter!");
            }
            cpdGlobalObj->helpEmitSignal();

        });

    }

    connect(cpdGlobalObj, &ChangePwdDialog::pwdCheckOver, this, [=]{

        ui->tipLabel->setText(curPwdTip);
        if (curPwdTip.isEmpty()){
            pwdTip.isEmpty() ? ui->tipLabel->setText(pwdTip) : ui->tipLabel->setText(pwdSureTip);
        }

        refreshConfirmBtnStatus();
    });

    if (isCurrentUser){
        connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{
            this->accept();
            std::string str2 = ui->pwdLineEdit->text().toStdString();
            const char * passwd = str2.c_str();
            passwd_change_password(passwd_handler, passwd, chpasswd_cb, NULL);
        });
    } else {
        connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{
            this->accept();
            emit passwd_send(ui->pwdLineEdit->text(), ui->usernameLabel->text());

        });
    }
//    connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{
//        this->accept();
//        if (_isCurrentUser){//当前用户使用passwd修改密码，免除polikit验证
//            qDebug() << "is Current";
//            std::string str2 = ui->pwdLineEdit->text().toStdString();
//            const char * passwd = str2.c_str();
//            passwd_change_password(passwd_handler, passwd, chpasswd_cb, NULL);
//        } else {
//            emit (ui->pwdLineEdit->text(), ui->usernameLabel->text());
//        }
//    });

    initPwdChecked();
    setupComponent();
    setupConnect();
}

ChangePwdDialog::~ChangePwdDialog()
{
    delete ui;
//    delete cpdGlobalObj;

}

bool ChangePwdDialog::checkOtherPasswd(QString name, QString pwd){
    FILE * stream;
    char command[128];
    char output[128];

    QByteArray ba1 = name.toLatin1();
    QByteArray ba2 = pwd.toLatin1();

    sprintf(command, "/usr/bin/checkuserpwd %s %s", ba1.data(), ba2.data());

    if ((stream = popen(command, "r")) == NULL){
        return false;
    }

    if (fread(output, sizeof(char), 128, stream) > 0){
        pclose(stream);
        return true;
    }
    pclose(stream);
    return false;
}

void ChangePwdDialog::initPwdChecked(){
#ifdef ENABLEPQ
    int ret;
    void *auxerror;
    char buf[255];

    settings = pwquality_default_settings();
    if (settings == NULL) {
        enablePwdQuality = false;
        qDebug() << "init pwquality settings failed";
    } else {
        enablePwdQuality = true;
    }

    ret = pwquality_read_config(settings, PWCONF, &auxerror);
    if (ret != 0){
        enablePwdQuality = false;
        qDebug() << "Reading pwquality configuration file failed: " << pwquality_strerror(buf, sizeof(buf), ret, auxerror);
    } else {
        enablePwdQuality = true;
    }

    if (PasswdCheckUtil::getCurrentPamState())
        enablePwdQuality = true;
    else
        enablePwdQuality = false;

#else
    enablePwdQuality = false;
#endif
}

void ChangePwdDialog::setupComponent(){

    ElipseMaskWidget * cpMaskWidget = new ElipseMaskWidget(ui->faceLabel);
    cpMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    ui->pwdtypeComboBox->setText(tr("General Pwd"));

    ui->curPwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwdsureLineEdit->setEchoMode(QLineEdit::Password);

    ui->curPwdLineEdit->setPlaceholderText(tr("Current Password"));
    ui->pwdLineEdit->setPlaceholderText(tr("New Password"));
    ui->pwdsureLineEdit->setPlaceholderText(tr("New Password Identify"));

    refreshConfirmBtnStatus();
}

void ChangePwdDialog::setupConnect(){
    connect(ui->closeBtn, &CloseButton::clicked, [=]{
        close();
    });

    connect(ui->pwdLineEdit, &QLineEdit::textChanged, [=](QString text){
        pwdLegalityCheck(text);
    });
    connect(ui->pwdsureLineEdit, &QLineEdit::textChanged, [=](QString text){
        if (!text.isEmpty() && text != ui->pwdLineEdit->text()){
            pwdSureTip = tr("Inconsistency with pwd");
        } else {
            pwdSureTip = "";
        }

        ui->tipLabel->setText(pwdSureTip);
        if (pwdSureTip.isEmpty()){
            pwdTip.isEmpty() ? ui->tipLabel->setText(curPwdTip) : ui->tipLabel->setText(pwdTip);
        }

        refreshConfirmBtnStatus();
    });

    connect(ui->cancelPushBtn, &QPushButton::clicked, [=]{
        reject();
    });
}

void ChangePwdDialog::setFace(QString iconfile){
    ui->faceLabel->setPixmap(QPixmap(iconfile).scaled(QSize(80, 80)));

}

void ChangePwdDialog::setUsername(QString username){
    ui->usernameLabel->setText(username);
}

//void ChangePwdDialog::setPwdType(QString type){
//    ui->pwdtypeComboBox->setCurrentText(type);
//}

void ChangePwdDialog::setAccountType(QString aType){
    ui->aTypeLabel->setText(aType);
}

void ChangePwdDialog::haveCurrentPwdEdit(bool have){
    ui->curPwdLineEdit->setVisible(have);
    ui->label->setVisible(have);
}

void ChangePwdDialog::paintEvent(QPaintEvent *event) {
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
    pixmapPainter.setOpacity(0.65);
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

void ChangePwdDialog::pwdLegalityCheck(QString pwd){
    //
    if (!checkCharLegitimacy(pwd)){
        pwdTip = tr("Contains illegal characters!");
    } else if (QString::compare(ui->pwdLineEdit->text(), ui->curPwdLineEdit->text()) == 0){
        pwdTip = tr("Same with old pwd");
    } else {
        if (enablePwdQuality){
#ifdef ENABLEPQ
            void * auxerror;
            int ret;
            const char * msg;
            char buf[256];

            QByteArray ba = pwd.toLatin1();

            ret = pwquality_check(settings, ba.data(), NULL, NULL, &auxerror);
            if (ret < 0 && pwd.length() > 0){
                msg = pwquality_strerror(buf, sizeof(buf), ret, auxerror);
                pwdTip = QString(msg);
            } else {
                pwdTip = "";
            }
#endif

        } else { //系统未开启pwdquality模块
            pwdTip = "";
        }
    }

    //防止先输入确认密码，再输入密码后pwdsuretipLabel无法刷新
    if (!ui->pwdsureLineEdit->text().isEmpty()){
        if (ui->pwdLineEdit->text() == ui->pwdsureLineEdit->text()) {
            pwdSureTip = "";
        } else {
            pwdSureTip = tr("Inconsistency with pwd");
        }
    }

    ui->tipLabel->setText(pwdTip);
    if (pwdTip.isEmpty()){
        pwdSureTip.isEmpty() ? ui->tipLabel->setText(curPwdTip) : ui->tipLabel->setText(pwdSureTip);
    }

    refreshConfirmBtnStatus();
}

bool ChangePwdDialog::checkCharLegitimacy(QString password){
    foreach (QChar ch, password){
        if (int(ch.toLatin1() <= 0 || int(ch.toLatin1()) > 127)){
            return false;
        }
    }
    return true;
}


void ChangePwdDialog::refreshConfirmBtnStatus(){
    if (getuid()){
        if (!ui->tipLabel->text().isEmpty() || \
                ui->curPwdLineEdit->text().isEmpty() || ui->curPwdLineEdit->text() == tr("Current Password") || \
                ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit->text() == tr("New Password") || \
                ui->pwdsureLineEdit->text().isEmpty() || ui->pwdsureLineEdit->text() == tr("New Password Identify") ||
                !curPwdTip.isEmpty() || !pwdTip.isEmpty() || !pwdSureTip.isEmpty())
            ui->confirmPushBtn->setEnabled(false);
        else
            ui->confirmPushBtn->setEnabled(true);
    } else {
        if (!ui->tipLabel->text().isEmpty() || \
                ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit->text() == tr("New Password") || \
                ui->pwdsureLineEdit->text().isEmpty() || ui->pwdsureLineEdit->text() == tr("New Password Identify") ||
                !curPwdTip.isEmpty() || !pwdTip.isEmpty() || !pwdSureTip.isEmpty())
            ui->confirmPushBtn->setEnabled(false);
        else
            ui->confirmPushBtn->setEnabled(true);
    }
}

void ChangePwdDialog::helpEmitSignal(){
    emit this->pwdCheckOver();
}

void ChangePwdDialog::setCurPwdTip(){
    cpdGlobalObj->helpEmitSignal();
}

static void chpasswd_cb(PasswdHandler *passwd_handler, GError *error, gpointer user_data){
//    g_warning("error code: '%d'", error->code);
//    passwd_destroy(passwd_handler);
}

static void auth_cb(PasswdHandler *passwd_handler, GError *error, gpointer user_data){
    ChangePwdDialog::curPwdTip = error ? QObject::tr("Pwd input error, re-enter!") : "";
    ChangePwdDialog::setCurPwdTip();
}
