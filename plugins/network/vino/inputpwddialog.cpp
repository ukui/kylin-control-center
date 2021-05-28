#include "inputpwddialog.h"

#include <QDebug>
#include <QLineEdit>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include "sharemain.h"


InputPwdDialog::InputPwdDialog(QGSettings *Keygsettiings,QWidget *parent) :
    QDialog(parent),
    mgsettings(Keygsettiings)
{

    setupInit();
    initConnect();
}

InputPwdDialog::~InputPwdDialog()
{

}

void InputPwdDialog::setupInit()
{
    setWindowTitle(tr("Set Password"));
    this->resize(380, 161);
    this->setMinimumSize(QSize(380, 161));
    this->setMaximumSize(QSize(380, 161));


    mpwd = new QLineEdit(this);
    mpwd->setGeometry(32, 25, 316,42);


    mHintLabel = new QLabel(this);
    mHintLabel->setGeometry(32,67,316,28);
    mHintLabel->setContentsMargins(8,2,8,2);
    mHintLabel->setText(tr("Password can not be blank"));
    mHintLabel->setStyleSheet("color:red;");

    mCancelBtn = new QPushButton(this);
    mCancelBtn->setContentsMargins(36,6,36,6);
    mCancelBtn->setGeometry(132,99,100,33);
    mCancelBtn->setText("Cancel");

    mConfirmBtn = new QPushButton(this);
    mConfirmBtn->setContentsMargins(36,6,36,6);
    mConfirmBtn->setGeometry(248,99,100,33);
    mConfirmBtn->setText("confirm");

}

void InputPwdDialog::mpwdInputSlot(const QString &pwd)
{
    Q_UNUSED(pwd);

    if (pwd.length() <= 7 && !pwd.isEmpty()) {
        QByteArray text = pwd.toLocal8Bit();
        secPwd = text.toBase64();
        mHintLabel->setVisible(false);
    } else if (pwd.isEmpty()) {
        mHintLabel->setText(tr("Password can not be blank"));
        mHintLabel->setStyleSheet("color:red;");
        mHintLabel->setVisible(true);
    } else {
        mHintLabel->setText(tr("less than or equal to 8"));
        mHintLabel->setStyleSheet("color:red;");
        mHintLabel->setVisible(true);
        mpwd->setText(pwd.mid(0, 8));
        QByteArray text = pwd.mid(0, 8).toLocal8Bit();
        secPwd = text.toBase64();
    }
}
void InputPwdDialog::initConnect() {

    connect(mCancelBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        this->close();
    });

    connect(mConfirmBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        if (secPwd.length() == 0) {
            return;
        } else {
            mgsettings->set(kVncPwdKey, secPwd);
            this->close();
        }
    });
    connect(mpwd, &QLineEdit::textChanged, this, &InputPwdDialog::mpwdInputSlot);
}
