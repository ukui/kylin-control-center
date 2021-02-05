#ifndef WIDGET_H
#define WIDGET_H


#include "auth-pam.h"


class Widget : public QObject
{
    Q_OBJECT

public:
    Widget();
    ~Widget();

public:
    void pwdCheck(QString userName, QString userPwd);

private:
    Auth * auth;

private Q_SLOTS:
    void onShowMessage(const QString &message, Auth::MessageType type);
    void onShowPrompt(const QString &prompt, Auth::PromptType type);
    void onAuthComplete();
};
#endif // WIDGET_H
