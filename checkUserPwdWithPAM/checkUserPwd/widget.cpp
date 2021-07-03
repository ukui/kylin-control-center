#include "widget.h"

#include "auth-pam.h"

#include <QDebug>


Widget::Widget()
{

    auth = new AuthPAM(this);

    connect(auth, &Auth::showMessage, this, &Widget::onShowMessage);
    connect(auth, &Auth::showPrompt, this, &Widget::onShowPrompt);
    connect(auth, &Auth::authenticateComplete, this, &Widget::onAuthComplete);

}

Widget::~Widget()
{

    auth->stopAuth();

    delete auth;
}

void Widget::pwdCheck(QString userName, QString userPwd){
    auth->authenticate(userName, userPwd);
}

void Widget::onShowMessage(const QString &message, Auth::MessageType type)
{
//    qDebug() << message;
    printf("%s\n", message.toUtf8().data());

}

void Widget::onShowPrompt(const QString &prompt, Auth::PromptType type)
{
//    qDebug() << "prompt: " << prompt;
}

void Widget::onAuthComplete()
{

//    if(auth->isAuthenticated()){
//        qDebug() << "Succes!\n";
//        printf("Succes!\n");
//    } else {
//        printf("Failed!\n");
//        qDebug() << "Failed!";
//    }

    exit(0);

}

