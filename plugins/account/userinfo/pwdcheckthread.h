#ifndef PWDCHECKTHREAD_H
#define PWDCHECKTHREAD_H

#include <QObject>
#include <QThread>

class PwdCheckThread : public QThread
{
    Q_OBJECT

public:
    explicit PwdCheckThread();
    ~PwdCheckThread();

public:
    void setArgs(const QString &userName, const QString &userPwd);
    void run();

private:
    QString uname;
    QString upwd;

Q_SIGNALS:
    void complete(bool result);


};

#endif // PWDCHECKTHREAD_H
