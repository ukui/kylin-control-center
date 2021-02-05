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
#ifndef AUTH_H
#define AUTH_H

#ifndef QT_NO_KEYWORDS
#define QT_NO_KEYWORDS
#endif

#include <QObject>

class Auth : public QObject
{
    Q_OBJECT

    Q_ENUMS(PromptType MessageType)
public:
    explicit Auth(QObject *parent = nullptr)
        : QObject(parent)
    {

    }

    enum PromptType {
        PromptTypeQuestion,
        PromptTypeSecret
    };
    enum MessageType {
        MessageTypeInfo,
        MessageTypeError
    };


Q_SIGNALS:
    void showPrompt(const QString &prompt, Auth::PromptType type);
    void showMessage(const QString &message, Auth::MessageType type);
    void authenticateComplete();

public:
    virtual void authenticate(const QString &userName, const QString &userPwd) = 0;
    virtual void stopAuth() = 0;
    virtual void respond(const QString &response) = 0;
    virtual bool isAuthenticating() = 0;
    virtual bool isAuthenticated() = 0;
};

#endif // AUTH_H
