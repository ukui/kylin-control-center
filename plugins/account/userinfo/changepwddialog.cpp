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
#include <QGraphicsDropShadowEffect>

#include <QDebug>

#define PWD_LOW_LENGTH 6
#define PWD_HIGH_LENGTH 20

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangePwdDialog::ChangePwdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePwdDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

//    ui->frame->setStyleSheet(".QFrame{background:rgba(255,255,255,1); border:1px solid rgba(201,201,201,1); border-radius:16px;}");
//    QGraphicsDropShadowEffect *frame_shadow = new QGraphicsDropShadowEffect(this);
//    frame_shadow->setYOffset(0);
//    frame_shadow->setColor("rgba(47,56,64,0.15)");
//    frame_shadow->setBlurRadius(16);
//    ui->frame->setGraphicsEffect(frame_shadow);

//    顶边栏样式，此控件样式应由主题控制
//    ui->topWidget->setStyleSheet(".QWidget{background:rgba(246,246,246,1);}");

//    ui->titleLabel->setStyleSheet("QLabel{font-size: 14px; color: palette(windowText);}");
//    ui->label_5->setStyleSheet("QLabel{font-size: 16px; color: palette(windowText);}");
//    ui->pwdLabel->setStyleSheet("QLabel{font-size: 16px; color: palette(windowText);}");
//    ui->pwdsureLabel->setStyleSheet("QLabel{font-size: 16px; color: palette(windowText);}");
//    ui->pwdLineEdit->setStyleSheet("QLineEdit{border-radius:12px;background:rgba(245,245,245,1);}");
//    ui->pwdsureLineEdit->setStyleSheet("QLineEdit{border-radius:12px;background:rgba(245,245,245,1);}");
//    ui->pwdFrame->setStyleSheet("QFrame{border: 0px; border-radius:12px;background:rgba(235,235,235,1);}");
//    ui->cancelPushBtn->setStyleSheet(ui->cancelPushBtn->styleSheet().append(QString("border-radius:12px;")));
//    ui->confirmPushBtn->setStyleSheet("QPushButton{border-radius:12px;}");

    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);
    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    ui->pwdFrame->setFrameShape(QFrame::Shape::Box);

    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    initPwdChecked();
    setupComponent();
    setupConnect();
}

ChangePwdDialog::~ChangePwdDialog()
{
    delete ui;
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

//    ElipseMaskWidget * cpMaskWidget = new ElipseMaskWidget(ui->faceLabel);
//    cpMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    ui->pwdtypeComboBox->setText(tr("General Pwd"));

    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->pwdsureLineEdit->setEchoMode(QLineEdit::Password);

    ui->pwdLineEdit->setPlaceholderText(tr("New Password"));
    ui->pwdsureLineEdit->setPlaceholderText(tr("New Password Identify"));

    ui->pwdLineEdit->setTextMargins(16,0,32,0);
    ui->pwdsureLineEdit->setTextMargins(16,0,32,0);
    pwdLineEditHLayout = new QHBoxLayout();
    pwdsureLineEditHLayout = new QHBoxLayout();

    showPwdBtn = new QPushButton();
    showPwdBtn->setFixedSize(QSize(16, 16));

    pwdLineEditHLayout->addStretch();
    pwdLineEditHLayout->addWidget(showPwdBtn);
    pwdLineEditHLayout->setContentsMargins(0,0,16,0);
    ui->pwdLineEdit->setLayout(pwdLineEditHLayout);

    showSurePwdBtn = new QPushButton();
    showSurePwdBtn->setFixedSize(QSize(16, 16));

    pwdsureLineEditHLayout->addStretch();
    pwdsureLineEditHLayout->addWidget(showSurePwdBtn);
    pwdsureLineEditHLayout->setContentsMargins(0,0,16,0);
    ui->pwdsureLineEdit->setLayout(pwdsureLineEditHLayout);

    showPwdBtn->setCursor(QCursor(Qt::ArrowCursor));
    showSurePwdBtn->setCursor(QCursor(Qt::ArrowCursor));

    showPwdBtn->setStyleSheet("QPushButton{border-image: url(:/img/plugins/userinfo/pwdEncrypted.png);}"
                              "QPushButton:pressed{border-image: url(:/img/plugins/userinfo/pwdUnscramble.png);}");
    showSurePwdBtn->setStyleSheet("QPushButton{border-image: url(:/img/plugins/userinfo/pwdEncrypted.png);}"
                              "QPushButton:pressed{border-image: url(:/img/plugins/userinfo/pwdUnscramble.png);}");

    connect(showPwdBtn, &QPushButton::pressed, this, [=]{
        ui->pwdLineEdit->setEchoMode(QLineEdit::Normal);
    });
    connect(showPwdBtn, &QPushButton::released, this, [=]{
        ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
        ui->pwdLineEdit->setFocus();
    });
    connect(showSurePwdBtn, &QPushButton::pressed, this, [=]{
        ui->pwdsureLineEdit->setEchoMode(QLineEdit::Normal);
    });
    connect(showSurePwdBtn, &QPushButton::released, this, [=]{
        ui->pwdsureLineEdit->setEchoMode(QLineEdit::Password);
        ui->pwdsureLineEdit->setFocus();
    });
    refreshConfirmBtnStatus();
}

void ChangePwdDialog::setupConnect(){
    connect(ui->closeBtn, &QPushButton::clicked, [=]{
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
            pwdTip.isEmpty() ? ui->tipLabel->setText(nameTip) : ui->tipLabel->setText(pwdTip);
        }

        refreshConfirmBtnStatus();
    });

    connect(ui->cancelPushBtn, &QPushButton::clicked, [=]{
        reject();
    });
    connect(ui->confirmPushBtn, &QPushButton::clicked, [=]{
        this->accept();
        emit passwd_send(ui->pwdLineEdit->text(), ui->usernameLabel->text());
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
        if (pwd.length() < PWD_LOW_LENGTH) {
            if (pwd.length() == 0) pwdTip_2 = "";
            else pwdTip_2 = tr("Password length needs to more than %1 character!").arg(PWD_LOW_LENGTH - 1);
        } else if (pwd.length() > PWD_HIGH_LENGTH) {
            pwdTip_2 = tr("Password length needs to less than %1 character!").arg(PWD_HIGH_LENGTH + 1);
        } else {
            pwdTip_2 = "";
        }
    };
    ui->tipLabel_2->setText(pwdTip_2);

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
        pwdSureTip.isEmpty() ? ui->tipLabel->setText(nameTip) : ui->tipLabel->setText(pwdSureTip);
    }

    refreshConfirmBtnStatus();
}


void ChangePwdDialog::refreshConfirmBtnStatus(){
    if (!ui->tipLabel->text().isEmpty() || \
            ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit->text() == tr("New Password") || \
            ui->pwdsureLineEdit->text().isEmpty() || ui->pwdsureLineEdit->text() == tr("New Password Identify") ||
            !nameTip.isEmpty() || !pwdTip.isEmpty() || !pwdSureTip.isEmpty() || !pwdTip_2.isEmpty())
        ui->confirmPushBtn->setEnabled(false);
    else
        ui->confirmPushBtn->setEnabled(true);
}
