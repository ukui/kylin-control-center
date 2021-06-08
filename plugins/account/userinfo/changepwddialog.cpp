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


#define PWD_LOW_LENGTH 6
#define PWD_HIGH_LENGTH 20


extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);


ChangePwdDialog::ChangePwdDialog(bool _isCurrentUser, QString _username, QWidget *parent) :
    QDialog(parent),
    isCurrentUser(_isCurrentUser),
    ui(new Ui::ChangePwdDialog),
    currentUserName(_username)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Change pwd"));

    curPwdTip = "";
    pwdChecking = false;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->pwdFrame->setFrameShape(QFrame::Shape::Box);
    ui->tipLabel->setAlignment(Qt::AlignCenter);
    ui->tipLabel->setStyleSheet("color:red;");

//    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

//    isCurrentUser = true;

    pcThread = new PwdCheckThread();


    initPwdChecked();
    setupComponent();
    setupConnect();
}

ChangePwdDialog::~ChangePwdDialog()
{
    delete ui;
    ui = nullptr;

//    pcThread->terminate();
//    delete pcThread;
}

bool ChangePwdDialog::checkOtherPasswd(QString name, QString pwd){
    FILE * stream;
    char command[128];
    char output[256];

    QByteArray ba1 = name.toLatin1();

    //
    if (pwd.contains("'")){
        snprintf(command, 128, "/usr/bin/checkTest %s \"%s\"", ba1.data(), pwd.toLatin1().data());
    } else {

        snprintf(command, 128, "/usr/bin/checkTest %s '%s'", ba1.data(), pwd.toLatin1().data());
    }

    if ((stream = popen(command, "r")) == NULL){
        return false;
    }

    while(fgets(output, 256, stream) != NULL){
        qDebug() << "output:" << QString(output).simplified();
    }

//    if (fread(output, sizeof(char), 128, stream) > 0){
//        pclose(stream);
//        return true;
//    }



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

    connect(pcThread, &PwdCheckThread::complete, this, [=](bool re){
        curPwdTip = re ? "" : tr("Pwd input error, re-enter!");

        if (pwdTip.isEmpty() && pwdSureTip.isEmpty()){
            ui->tipLabel->setText(curPwdTip);
        }

        if (curPwdTip.isEmpty()){
            pwdTip.isEmpty() ? ui->tipLabel->setText(pwdSureTip) : ui->tipLabel->setText(pwdTip);
        }

        if (re){ //密码校验成功

            this->accept();

            emit passwd_send(ui->curPwdLineEdit->text(), ui->pwdLineEdit->text());

        } else {
            ui->curPwdLineEdit->setText("");

            refreshConfirmBtnStatus();
        }

        pwdChecking = false;

        refreshCancelBtnStatus();



    });

    if (isCurrentUser){

//        connect(timerForCheckPwd, &QTimer::timeout, [=]{
//            /* 密码为空不检测 */
//            if (ui->curPwdLineEdit->text().isEmpty()){
//                return;
//            }

//            pcThread->setArgs(currentUserName, ui->curPwdLineEdit->text());

//            pcThread->start();

//        });

        connect(ui->curPwdLineEdit, &QLineEdit::textChanged, [=](QString txt){
//            pwdLegalityCheck();

//            ui->confirmPushBtn->setEnabled(false);

//            timerForCheckPwd->start();

            if (!txt.isEmpty()){
                curPwdTip = "";
            }

            if (pwdTip.isEmpty() && pwdSureTip.isEmpty()){
                ui->tipLabel->setText(curPwdTip);
            }

            if (curPwdTip.isEmpty()){
                pwdTip.isEmpty() ? ui->tipLabel->setText(pwdSureTip) : ui->tipLabel->setText(pwdTip);
            }


            refreshConfirmBtnStatus();
        });

        connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{

            if (pwdChecking)
                return;

            pcThread->setArgs(currentUserName, ui->curPwdLineEdit->text());

            pcThread->start();

            pwdChecking = true;

            refreshCancelBtnStatus();

//            this->accept();

//            emit passwd_send(ui->curPwdLineEdit->text(), ui->pwdLineEdit->text());
        });
    } else {
        connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{
            this->accept();

            emit passwd_send2(ui->pwdLineEdit->text());
        });
    }



    connect(ui->pwdLineEdit, &QLineEdit::textChanged, [=]{
        pwdLegalityCheck();

        refreshConfirmBtnStatus();
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
//        reject();
        close();
    });
}

void ChangePwdDialog::setFace(QString iconfile){
    ui->faceLabel->setPixmap(QPixmap(iconfile).scaled(QSize(80, 80)));

}

void ChangePwdDialog::setUsername(QString realname){
    ui->usernameLabel->setText(realname);
}

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

void ChangePwdDialog::pwdLegalityCheck(){
    //
    if (!checkCharLegitimacy(ui->pwdLineEdit->text())){
        pwdTip = tr("Contains illegal characters!");
    } else if (QString::compare(ui->pwdLineEdit->text(), ui->curPwdLineEdit->text()) == 0 && !ui->pwdLineEdit->text().isEmpty()){
        pwdTip = tr("Same with old pwd");
    } else {
        if (enablePwdQuality){
#ifdef ENABLEPQ
            void * auxerror;
            int ret;
            const char * msg;
            char buf[256];

            QByteArray ba = ui->pwdLineEdit->text().toLatin1();
            QByteArray ba1 = ui->curPwdLineEdit->text().toLatin1();

            if (isCurrentUser){
               ret = pwquality_check(settings, ba.data(), ba1.data(), currentUserName.toLatin1().data(), &auxerror);
            } else {
               ret = pwquality_check(settings, ba.data(), NULL, currentUserName.toLatin1().data(), &auxerror);
            }

            if (ret < 0 && ui->pwdLineEdit->text().length() > 0){
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
}

bool ChangePwdDialog::checkCharLegitimacy(QString password){
    foreach (QChar ch, password){
        if (int(ch.toLatin1() <= 0 || int(ch.toLatin1()) > 127)){
            return false;
        }
    }
    return true;
}

void ChangePwdDialog::refreshCancelBtnStatus(){
    if (pwdChecking){
        ui->cancelPushBtn->setEnabled(false);
    } else {
        ui->cancelPushBtn->setEnabled(true);
    }
}


void ChangePwdDialog::refreshConfirmBtnStatus(){

    if (getuid() && isCurrentUser){
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
                !pwdTip.isEmpty() || !pwdSureTip.isEmpty())
            ui->confirmPushBtn->setEnabled(false);
        else
            ui->confirmPushBtn->setEnabled(true);
    }
}


void ChangePwdDialog::keyPressEvent(QKeyEvent * event){
    switch (event->key())
    {
    case Qt::Key_Escape:
        break;
    case Qt::Key_Enter:
        break;
    case Qt::Key_Return:
        if (ui->confirmPushBtn->isEnabled())
            ui->confirmPushBtn->clicked();
        break;
    default:
        QDialog::keyPressEvent(event);
    }
}
