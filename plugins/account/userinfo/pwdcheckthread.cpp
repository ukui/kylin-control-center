#include "pwdcheckthread.h"

#include <QDebug>

PwdCheckThread::PwdCheckThread()
{
}

PwdCheckThread::~PwdCheckThread()
{
}

void PwdCheckThread::setArgs(const QString &userName, const QString &userPwd){
    uname = userName;
    upwd = userPwd;
}

void PwdCheckThread::run(){

    FILE * stream;
    char command[128];
    char output[128];

    bool result = false;

    QByteArray ba1 = uname.toLatin1();

    //
    if (upwd.contains("'")){
        sprintf(command, "/usr/bin/checkUserPwd %s \"%s\"", ba1.data(), upwd.toLatin1().data());
    } else {

        sprintf(command, "/usr/bin/checkUserPwd %s '%s'", ba1.data(), upwd.toLatin1().data());
    }

    if ((stream = popen(command, "r")) != NULL){

        while(fgets(output, 256, stream) != NULL){
            if (QString::compare(QString(output).simplified(), "Succes!") == 0){
                result = true;
            }

        }
        pclose(stream);
    }

    emit complete(result);
}
