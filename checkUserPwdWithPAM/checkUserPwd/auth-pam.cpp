/*
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
**/
#include "auth-pam.h"
#include <string.h>
#include <QDebug>
#include <sys/prctl.h>
#include <unistd.h>
#include <wait.h>

#include <QTimer>


#define PAM_SERVICE_NAME "control-center"

//通信管道的文件描述符
int toParent[2], toChild[2];

static void writeData(int fd, const void *buf, ssize_t count);
static void writeString(int fd, const char *data);
static int readData(int fd, void *buf, size_t count);
static char * readString(int fd);
static int pam_conversation(int msgLength, const struct pam_message **msg,
                PAM_RESPONSE **resp, void *appData);
static void sigchld_handler(int signo);

AuthPAM::AuthPAM(QObject *parent)
    : Auth(parent),
      pid(0),
      nPrompts(0),
      _isAuthenticated(false),
      _isAuthenticating(false)
{
    signal(SIGCHLD, sigchld_handler);
}

void AuthPAM::authenticate(const QString &userName, const QString &userPwd)
{
    stopAuth();

    if(pipe(toParent) || pipe(toChild))
        qDebug()<< "create pipe failed: " << strerror(errno);
    if((pid = fork()) < 0)
    {
        qDebug() << "fork error: " << strerror(errno);
    }
    else if(pid == 0)
    {
        int arg1_int = toParent[1];
        int arg2_int = toChild[0];
        char arg1[128];
        char arg2[128];
        snprintf(arg1,128,"%d",arg1_int);
        snprintf(arg2,128,"%d",arg2_int);
        //_authenticate(userName.toLocal8Bit().data());
        prctl(PR_SET_PDEATHSIG,SIGHUP);
        execlp ("childCheckpwdwithPAM",
                "childCheckpwdwithPAM",
                arg1, arg2,userName.toLocal8Bit().data(), NULL);
        _exit (EXIT_FAILURE);
    }
    else
    {
        _isAuthenticating = true;
        notifier = new QSocketNotifier(toParent[0], QSocketNotifier::Read);
        connect(notifier, &QSocketNotifier::activated, this, &AuthPAM::onSockRead);
    }

    QTimer::singleShot(10, this, [=]{respond(userPwd);});

}

void AuthPAM::stopAuth()
{
//    qDebug()<<"pppppppppppppppppid  = "<<pid;
    if(pid != 0)
    {
        messageList.clear();
        responseList.clear();
        _isAuthenticating = false;
        _isAuthenticated = false;
        nPrompts = 0;

        ::kill(pid, SIGKILL);

        pid = 0;
    }
}

void AuthPAM::respond(const QString &response)
{
    nPrompts--;
    responseList.push_back(response);

//    for(auto msg : messageList)
//        qDebug() << msg.msg;
//    qDebug() << responseList;
//    qDebug() << nPrompts;

    if(nPrompts == 0)
    {
        //发送响应到子进程
        int j = 0;
        PAM_RESPONSE *resp = (PAM_RESPONSE*)calloc(messageList.size(), sizeof(PAM_RESPONSE));
        //响应的数量和消息的数量一致，如果消息类型不是PROMPT，则响应是空的
        for(int i = 0; i < messageList.size(); i++)
        {
            struct pam_message message = messageList[i];
            PAM_RESPONSE *r = &resp[i];
            if(message.msg_style == PAM_PROMPT_ECHO_OFF
                    || message.msg_style == PAM_PROMPT_ECHO_ON)
            {
                int respLength = responseList[j].length() + 1;
                r->resp = (char *)malloc(sizeof(char) * respLength);
                memcpy(r->resp, responseList[j].toLocal8Bit().data(), respLength);
                j++;
            }
        }
         _respond(resp);
         free(resp);
         resp = NULL;
         messageList.clear();
         responseList.clear();
    }
}

bool AuthPAM::isAuthenticated()
{
    return _isAuthenticated;
}

bool AuthPAM::isAuthenticating()
{
    return _isAuthenticating;
}


void AuthPAM::onSockRead()
{
//    qDebug() << "has message";
    int msgLength;
    int authComplete;
    readData(toParent[0], &authComplete, sizeof(authComplete));

    if(authComplete)
    {
        int authRet;
        if(readData(toParent[0], (void*)&authRet, sizeof(authRet)) <= 0)
            qDebug() << "get authentication result failed: " << strerror(errno);
//        qDebug() << "result: " << authRet;
        _isAuthenticated = (authRet == PAM_SUCCESS);
        _isAuthenticating = false;
        Q_EMIT authenticateComplete();

    }
    else
    {
        readData(toParent[0], &msgLength, sizeof(msgLength));
//        qDebug() << "message length: " << msgLength;

        for(int i = 0; i < msgLength; i++)
        {
            //读取message
            struct pam_message message;
            readData(toParent[0], &message.msg_style, sizeof(message.msg_style));
            message.msg = readString(toParent[0]);

//            qDebug() << message.msg;

            messageList.push_back(message);

            switch (message.msg_style)
            {
            case PAM_PROMPT_ECHO_OFF:
                nPrompts++;
                Q_EMIT showPrompt(message.msg, Auth::PromptTypeSecret);
                break;
            case PAM_PROMPT_ECHO_ON:
                nPrompts++;
                Q_EMIT showPrompt(message.msg, Auth::PromptTypeQuestion);
                break;
            case PAM_ERROR_MSG:
                Q_EMIT showMessage(message.msg, Auth::MessageTypeInfo);
                break;
            case PAM_TEXT_INFO:
                Q_EMIT showMessage(message.msg, Auth::MessageTypeError);
                break;
            }
        }

        if(nPrompts == 0)
        {
            //不需要响应，发送一个空的
            PAM_RESPONSE *response = (PAM_RESPONSE*)calloc(messageList.size(), sizeof(PAM_RESPONSE));
            _respond(response);
            free(response);
            response = NULL;
            messageList.clear();
        }
    }
}

static void
writeData(int fd, const void *buf, ssize_t count)
{
    if(write(fd, buf, count) != count)
        qDebug() << "write to parent failed: " << strerror(errno);
}

static void
writeString(int fd, const char *data)
{
    int length = data ? strlen(data) : -1;
    writeData(fd, &length, sizeof(length));
    if(data)
        writeData(fd, data, sizeof(char) * length);
}

static int
readData(int fd, void *buf, size_t count)
{
    ssize_t nRead = read(fd, buf, count);
    if(nRead < 0)
        qDebug() << "read data failed: " << strerror(errno);
    return nRead;
}

static char *
readString(int fd)
{
    int length;

    if(readData(fd, &length, sizeof(length)) <= 0)
        return NULL;
    if(length <= 0)
        return NULL;

    char *value = (char *)malloc(sizeof(char) * (length + 1));
    readData(fd, value, length);
    value[length] = '\0';

    return value;
}

void AuthPAM::_authenticate(const char *userName)
{
//    qDebug() << "authenticate " << userName;

    pam_handle_t *pamh = NULL;
    char *newUser;
    int ret;
    int authRet;
    struct pam_conv conv;

    conv.conv = pam_conversation;
    conv.appdata_ptr = NULL;

    ret = pam_start(PAM_SERVICE_NAME, userName, &conv, &pamh);
    if(ret != PAM_SUCCESS)
    {
        qDebug() << "failed to start PAM: " << pam_strerror(NULL, ret);
    }

    authRet = pam_authenticate(pamh, 0);

    ret = pam_get_item(pamh, PAM_USER, (const void **)&newUser);
    if(ret != PAM_SUCCESS)
    {
        pam_end(pamh, 0);
        qDebug() << "failed to get username";
    }
    free(newUser);
    newUser = NULL;
//    fprintf(stderr, "authentication result: %d\n", authRet);

    // 发送认证结果
    int authComplete = 1;
    writeData(toParent[1], (const void*)&authComplete, sizeof(authComplete));
    writeData(toParent[1], (const void *)&authRet, sizeof(authRet));
//    qDebug() << "--- 认证完成";
    _exit(EXIT_SUCCESS);
}

void AuthPAM::_respond(const PAM_RESPONSE *response)
{
    for(int i = 0; i < messageList.size(); i++)
    {
        const PAM_RESPONSE *resp = &response[i];
        writeData(toChild[1], (const void *)&resp->resp_retcode,
                sizeof(resp->resp_retcode));
        writeString(toChild[1], resp->resp);
    }
}


static int
pam_conversation(int msgLength, const struct pam_message **msg,
                PAM_RESPONSE **resp, void */*appData*/)
{
    PAM_RESPONSE *response = (PAM_RESPONSE*)calloc(msgLength,sizeof(PAM_RESPONSE));

    int authComplete = 0;
    writeData(toParent[1], (const void*)&authComplete, sizeof(authComplete));
    writeData(toParent[1], (const void*)&msgLength, sizeof(msgLength));
    //发送pam消息
    for(int i = 0; i < msgLength; i++)
    {
        const struct pam_message *m = msg[i];
        writeData(toParent[1], (const void *)&m->msg_style, sizeof(m->msg_style));
        writeString(toParent[1], m->msg);
    }
    //读取响应
    for(int i = 0; i < msgLength; i++)
    {
        PAM_RESPONSE *r = &response[i];
        readData(toChild[0], &r->resp_retcode, sizeof(r->resp_retcode));
        r->resp = readString(toChild[0]);
    }
    *resp = response;
    return PAM_SUCCESS;
}

void sigchld_handler(int signo)
{
    if(signo == SIGCHLD)
    {
        ::waitpid(-1, NULL, WNOHANG);
    }
}
