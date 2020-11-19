/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include <QtPlugin>

#include <QSignalMapper>
#include <QMouseEvent>
#include <QSettings>

#include "shell/interface.h"

#include "qtdbus/systemdbusdispatcher.h"
#include "qtdbus/userdispatcher.h"

#include "changegroupdialog.h"
#include "changepwddialog.h"
#include "changefacedialog.h"
#include "changetypedialog.h"
#include "changevaliddialog.h"
#include "deluserdialog.h"
#include "createuserdialog.h"
#include "HoverWidget/hoverwidget.h"

#ifdef ENABLEPQ
extern "C" {
#include <pwquality.h>
}
#endif

enum {
    STANDARDUSER,
    ADMINISTRATOR,
    ROOT
};

typedef struct _UserInfomation {
    QString objpath;
    QString username;
    QString iconfile;
    QString passwd;
    int accounttype;
    int passwdtype;
    bool current;
    bool logined;
    bool autologin;
    bool noPwdLogin;
    qint64 uid;
}UserInfomation;

typedef struct _PwdQualityOption {

    int diff_ok;
    int min_length;
    int dig_credit;
    int up_credit;
    int low_credit;
    int oth_credit;
    int min_class;
    int max_repeat;
    int max_class_repeat;
    int max_sequence;
    int gecos_check;
    int dict_check;
    int user_check;
    int enforcing;
    int retry_times;
    int enforce_for_root;
    int local_users_only;
    int palindrome;
    int no_similar_check;
    char *bad_words;
    char *dict_path;

}PwdQualityOption;

namespace Ui {
class UserInfo;
}

class QDBusInterface;
class SwitchButton;

class UserInfo : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    UserInfo();
    ~UserInfo();

    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget *get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initSearchText();
    void initComponent();
    void initAllUserStatus();

    QStringList getLoginedUsers();
    void _acquireAllUsersInfo();
    UserInfomation _acquireUserInfo(QString objpath);
    QString _accountTypeIntToString(int type);
    void _buildWidgetForItem(UserInfomation user);
    void _resetListWidgetHeigh();

    void _refreshUserInfoUI();

    void showCreateUserDialog();
    void createUser(QString username, QString pwd, QString pin, int atype);
    void createUserDone(QString objpath);

    void showDeleteUserDialog(QString username);
    void deleteUser(bool removefile, QString username);
    void deleteUserDone(QString objpath);

    void showChangePwdDialog(QString username);
    void changeUserPwd(QString pwd, QString username);

    void showChangeTypeDialog(QString username);
    void changeUserType(int atype, QString username);

    void showChangeFaceDialog(QString username);
    void changeUserFace(QString facefile, QString username);

    void showChangeValidDialog(QString username);

    void showChangeGroupDialog();

    void get_all_users();
    UserInfomation init_user_info(QString objpath);
    void setup_otherusers_ui();
    void build_item_with_widget(UserInfomation user);
    void ui_component_init();

    QString accounttype_enum_to_string(int id);
    QString login_status_bool_to_string(bool status);

    void readCurrentPwdConf();
    QStringList getUsersList();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::UserInfo *ui;

    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;
    HoverWidget *addWgt;

    SwitchButton * nopwdSwitchBtn;
    SwitchButton * autoLoginSwitchBtn;

    SystemDbusDispatcher * sysdispatcher;
    QSettings * autoSettings = nullptr;

    QMap<QString, UserInfomation> allUserInfoMap;
    QMap<QString, QListWidgetItem *> otherUserItemMap;

    QMap<QString, QListWidgetItem *> otherItemMap;

    QSignalMapper * pwdSignalMapper;
    QSignalMapper * faceSignalMapper;
    QSignalMapper * typeSignalMapper;
    QSignalMapper * delSignalMapper;

    QSize faceSize;
    QSize itemSize;
    QSize btnSize;

    QString pwdcreate;
    QString _newUserPwd;
    QString mUserName;

    QStringList m_loginedUser;

    QDBusInterface *sysinterface;
    QDBusInterface *mUserproperty;

    int adminnum;
    bool enablePwdQuality;
    bool mFirstLoad;

#ifdef ENABLEPQ
    pwquality_settings_t * pwdconf;
#endif

    PwdQualityOption pwdOption;

    QString pwdMsg;

private:
    bool getAutomaticLogin(QString username);
    bool getNoPwdStatus();
    void initUserPropertyConnection(const QStringList &objPath);

private slots:
    void delete_user_slot(bool removefile, QString username);
    void propertyChangedSlot(QString, QMap<QString, QVariant>, QStringList);
    void pwdAndAutoChangedSlot(QString key);
};

#endif // USERINFO_H
