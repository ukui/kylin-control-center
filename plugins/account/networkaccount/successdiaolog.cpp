#include "successdiaolog.h"

SuccessDiaolog::SuccessDiaolog(QWidget *parent) : QWidget(parent)
{
    bkg = new QLabel(this);
    text_msg = new QLabel(this);
    back_login = new QPushButton(this);
    layout = new QVBoxLayout;

    bkg->setStyleSheet("background-image:url(:/new/image/success.png);width:148px;height:148px;");
    bkg->setFixedSize(148,148);

    text_msg->setStyleSheet("color:rgba(0,0,0,0.85);font-size:24px");

    text_msg->adjustSize();

    back_login->setFixedSize(338,36);
    back_login->setText(tr("返回登录"));
    back_login->setStyleSheet("QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                              "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                              "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    back_login->setContentsMargins(0,16,0,0);

    layout->setContentsMargins(41,100,41,126);
    layout->setSpacing(0);
    layout->addWidget(bkg,0,Qt::AlignCenter);
    layout->addSpacing(16);
    layout->addWidget(text_msg,0,Qt::AlignCenter);
    layout->addWidget(back_login,0,Qt::AlignCenter | Qt::AlignBottom);

    setLayout(layout);
}

void SuccessDiaolog::set_mode_text(int mode) {
    if(mode == 0) {
        text_msg->setText(tr("注册账号成功！"));
    } else if(mode == 1) {
        text_msg->setText(tr("重置密码成功！"));
    } else if(mode == 2) {
        text_msg->setText(tr("登录帐号成功！"));
        back_login->setText("确认");
    }
}
