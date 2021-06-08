#ifndef PWDCHANGETHREAD_H
#define PWDCHANGETHREAD_H

#include <QObject>
#include <QThread>


class PwdChangeThread : public QThread
{
    Q_OBJECT

public:
    explicit PwdChangeThread();
    ~PwdChangeThread();

public:
    void setArgs(QString &currentpwd, QString &pwd);
    void run();

private:
    QString curpwd;
    QString newpwd;

Q_SIGNALS:
    void complete(QString result);
};

#endif // PWDCHANGETHREAD_H
