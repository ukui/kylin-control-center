#include "pwdchangethread.h"

PwdChangeThread::PwdChangeThread()
{
}

PwdChangeThread::~PwdChangeThread()
{
}

void PwdChangeThread::setArgs(QString &currentpwd, QString &pwd){
    curpwd = currentpwd;
    newpwd = pwd;
}

void PwdChangeThread::run(){

    FILE * stream;
    char command[256];
    char output[256] = "\0";

    snprintf(command, 256, "/usr/bin/changeuserpwd %s %s", curpwd.toLatin1().data(), newpwd.toLatin1().data());

    if ((stream = popen(command, "r")) != NULL){

        while(fgets(output, 256, stream) != NULL){
        }
        pclose(stream);
    }

    emit complete(QString(output));
}
