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
#ifndef AUTHPAM_H
#define AUTHPAM_H
#include "auth.h"
#include <QSocketNotifier>
#include <QList>

#include <security/pam_appl.h>

typedef struct pam_message PAM_MESSAGE;
typedef struct pam_response PAM_RESPONSE;

class AuthPAM : public Auth
{
    Q_OBJECT
public:
    AuthPAM(QObject *parent = nullptr);

    void authenticate(const QString &userName, const QString &userPwd);
    void stopAuth();
    void respond(const QString &response);
    bool isAuthenticated();
    bool isAuthenticating();

private:
    void _authenticate(const char *userName);
    void _respond(const struct pam_response *response);

private Q_SLOTS:
    void onSockRead();

private:
    QString userName;
    pid_t pid;
    QSocketNotifier *notifier;
    int nPrompts;
    QStringList responseList;
    QList<PAM_MESSAGE> messageList;
    bool _isAuthenticated;  //认证结果
    bool _isAuthenticating;
};

#endif // AUTHPAM_H
