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
#include "userinfo.h"
#include "ui_userinfo.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>

#include <polkit-qt5-1/polkitqt1-authority.h>

#include "SwitchButton/switchbutton.h"
#include "ImageUtil/imageutil.h"
#include "elipsemaskwidget.h"
#include "passwdcheckutil.h"
#include "loginedusers.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
}

#ifdef WITHKYSEC
#include <kysec/libkysec.h>
#include <kysec/status.h>
#endif

#define DEFAULTFACE "/usr/share/ukui/faces/default.png"
#define ITEMHEIGH 52

UserInfo::UserInfo() : mFirstLoad(true)
{
    pluginName = tr("User Info");
    pluginType = ACCOUNT;
}

UserInfo::~UserInfo()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
        delete autoSettings;
        autoSettings = nullptr;
    }
}

QString UserInfo::get_plugin_name() {
    return pluginName;
}

int UserInfo::get_plugin_type() {
    return pluginType;
}

QWidget *UserInfo::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::UserInfo;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        // 构建System dbus调度对象
        sysdispatcher = new SystemDbusDispatcher(this);

        // 获取系统全部用户信息，用户Uid大于等于1000的
        _acquireAllUsersInfo();

        initTitleLabel();
        initSearchText();
        readCurrentPwdConf();
        initComponent();
        initAllUserStatus();
        // 设置界面用户信息
        _refreshUserInfoUI();
    }
    return pluginWidget;
}

void UserInfo::plugin_delay_control() {

}

const QString UserInfo::name() const {

    return QStringLiteral("userinfo");
}

void UserInfo::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
    ui->title2Label->setFont(font);
    ui->bioPasswordLabel->setFont(font);
}

void UserInfo::initSearchText() {
    //~ contents_path /userinfo/Password
    ui->changePwdBtn->setText(tr("Password"));
    //~ contents_path /userinfo/Type
    ui->changeTypeBtn->setText(tr("Type"));
    //~ contents_path /userinfo/Login no passwd
    ui->loginpwdLabel->setText(tr("Login no passwd"));
    //~ contents_path /userinfo/enable autoLogin
    ui->autologinLabel->setText(tr("enable autoLogin"));
}

QString UserInfo::_accountTypeIntToString(int type){
    QString atype;
    if (type == STANDARDUSER)
        atype = tr("Standard");
    else if (type == ADMINISTRATOR)
        atype = tr("Admin");
    else if (type == ROOT)
        atype = tr("root");

    return atype;
}

void UserInfo::_acquireAllUsersInfo(){

    mUserName = qgetenv("USER");
    if (mUserName.isEmpty()) {
        mUserName = qgetenv("USERNAME");
    }

    QStringList objectpaths = sysdispatcher->list_cached_users();

    //初始化用户信息QMap
    allUserInfoMap.clear();
    //初始化管理员数目为0
//    adminnum = 0;

    //root
    if (!getuid()){
        UserInfomation root;
        root.username = g_get_user_name();
        root.realname = g_get_real_name();
        root.current = true;
        root.logined = true;
        root.autologin = false;
        root.uid = 0;
        root.accounttype = ADMINISTRATOR;
        root.iconfile = DEFAULTFACE;
        allUserInfoMap.insert(root.username, root);
    }

    for (QString objectpath : objectpaths){
        UserInfomation user;
        user = _acquireUserInfo(objectpath);
        allUserInfoMap.insert(user.username, user);
    }

    if (allUserInfoMap.isEmpty()) {
        ui->currentUserFrame->setVisible(false);
        ui->autoLoginFrame->setVisible(false);
        ui->liveFrame->setVisible(true);
    } else {
        ui->currentUserFrame->setVisible(true);
        ui->autoLoginFrame->setVisible(true);
        ui->liveFrame->setVisible(false);
    }
    initUserPropertyConnection(objectpaths);
}

int UserInfo::_userCanDel(QString user){
    QString cmd = QString("cat /etc/group | grep sudo | awk -F: '{ print $NF}'");
    QString output;

    FILE   *stream;
    char buf[256];

    if ((stream = popen(cmd.toLatin1().data(), "r" )) == NULL){
        return -1;
    }

    while(fgets(buf, 256, stream) != NULL){
        output = QString(buf).simplified();
    }

    pclose(stream);

    QStringList users = output.split(",");
    int num = users.length();

    if (users.contains(user)){
        if (num > 1){
            return 1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}

UserInfomation UserInfo::_acquireUserInfo(QString objpath){
    UserInfomation user;

    //默认值
    user.current = false;
    user.logined = false;
    user.autologin = false;

    QDBusInterface * iproperty = new QDBusInterface("org.freedesktop.Accounts",
                                            objpath,
                                            "org.freedesktop.DBus.Properties",
                                            QDBusConnection::systemBus());
    QDBusReply<QMap<QString, QVariant> > reply = iproperty->call("GetAll", "org.freedesktop.Accounts.User");
    if (reply.isValid()){
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        user.username = propertyMap.find("UserName").value().toString();
        user.realname = propertyMap.find("RealName").value().toString();

        if (user.realname.isEmpty()){
            user.realname = propertyMap.find("UserName").value().toString();
        }

        if (user.username == QString(g_get_user_name())) {
            user.current = true;
            user.logined = true;
            user.noPwdLogin = getNoPwdStatus();
        }
        user.accounttype = propertyMap.find("AccountType").value().toInt();
        user.iconfile = propertyMap.find("IconFile").value().toString();
        user.passwdtype = propertyMap.find("PasswordMode").value().toInt();
        user.uid = propertyMap.find("Uid").value().toInt();
        user.autologin = getAutomaticLogin().contains(user.username, Qt::CaseSensitive);
        user.objpath = objpath;
    }
    else
        qDebug() << "reply failed";

    delete iproperty;
    iproperty = nullptr;

    return user;
}

void UserInfo::readCurrentPwdConf(){
#ifdef ENABLEPQ
    int ret, status;
    void *auxerror;
    char buf[255];

    pwdMsg = "";

    pwdconf = pwquality_default_settings();
    if (pwdconf == NULL) {
        enablePwdQuality = false;
        qDebug() << "init pwquality settings failed";
    } else {
        enablePwdQuality = true;
    }

    ret = pwquality_read_config(pwdconf, PWCONF, &auxerror);
    if (ret != 0){
        enablePwdQuality = false;
        qDebug() << "Reading pwquality configuration file failed: " << pwquality_strerror(buf, sizeof(buf), ret, auxerror);
    } else {
        enablePwdQuality = true;
    }

    if (PasswdCheckUtil::getCurrentPamState()){
        enablePwdQuality = true;
    } else {
        enablePwdQuality = false;
    }

    if (enablePwdQuality){
        int minLen;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_MIN_LENGTH, &minLen);
        if (!status && minLen > 0){
            pwdOption.min_length = minLen;
            pwdMsg += QObject::tr("min length %1\n").arg(minLen);

        } else {
            pwdMsg += "";
        }

        int digCredit;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_DIG_CREDIT, &digCredit);
        if (!status && digCredit > 0){
            pwdOption.dig_credit = digCredit;
            pwdMsg += QObject::tr("min digit num %1\n").arg(digCredit);
        } else {
            pwdMsg += "";
        }

        int upCredit;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_UP_CREDIT, &upCredit);
        if (!status && upCredit > 0){
            pwdOption.up_credit = upCredit;
            pwdMsg += QObject::tr("min upper num %1\n").arg(upCredit);
        } else {
            pwdMsg += "";
        }

        int lowCredit;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_LOW_CREDIT, &lowCredit);
        if (!status && lowCredit > 0){
            pwdOption.low_credit = lowCredit;
            pwdMsg += QObject::tr("min lower num %1\n").arg(lowCredit);
        } else {
            pwdMsg += "";
        }

        int othCredit;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_OTH_CREDIT, &othCredit);
        if (!status && othCredit > 0){
            pwdOption.oth_credit = othCredit;
            pwdMsg += QObject::tr("min other num %1\n").arg(othCredit);
        } else {
            pwdMsg += "";
        }


        int minClass;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_MIN_CLASS, &minClass);
        if (!status && minClass > 0){
            pwdOption.min_class = minClass;
            pwdMsg += QObject::tr("min char class %1\n").arg(minClass);
        } else {
            pwdMsg += "";
        }

        int maxRepeat;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_MAX_REPEAT, &maxRepeat);
        if (!status && maxRepeat > 0){
            pwdOption.max_repeat = maxRepeat;
            pwdMsg += QObject::tr("max repeat %1\n").arg(maxRepeat);
        } else {
            pwdMsg += "";
        }

        int maxClassRepeat;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_MAX_CLASS_REPEAT, &maxClassRepeat);
        if (!status && maxClassRepeat > 0){
            pwdOption.max_class_repeat = maxClassRepeat;
            pwdMsg += QObject::tr("max class repeat %1\n").arg(maxClassRepeat);
        } else {
            pwdMsg += "";
        }

        int maxSequence;
        status = pwquality_get_int_value(pwdconf, PWQ_SETTING_MAX_SEQUENCE, &maxSequence);
        if (!status && maxSequence > 0){
            pwdOption.max_class_repeat = maxSequence;
            pwdMsg += QObject::tr("max sequence %1\n").arg(maxSequence);
        } else {
            pwdMsg += "";
        }
    }

    qDebug() << "pwquality:" << pwdOption.min_length << pwdOption.min_class << pwdOption.dig_credit << pwdOption.low_credit << pwdOption.up_credit;
    qDebug() << "pwquality msg:" << pwdMsg;
#endif
}

void UserInfo::initComponent(){

    //root需要屏蔽部分功能
    if (!getuid()){
        ui->changeTypeBtn->setEnabled(false);
//        ui->changeGroupBtn->setEnabled(false);
        ui->autoLoginFrame->setVisible(false);
        ui->noPwdLoginFrame->setVisible(false);
    }

#ifdef WITHKYSEC
    if (!kysec_is_disabled() && kysec_get_3adm_status() && (getuid() || geteuid())){
        ui->addUserWidget->hide();
    }
#endif

#ifdef __sw_64__
    ui->changeValidBtn->show();
#else
    ui->changeValidBtn->hide();
#endif

    ui->listWidget->setStyleSheet("QListWidget::Item:hover{background:palette(base);}");

    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add new user"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    iconLabel->setProperty("useIconHighlightEffect", true);
    iconLabel->setProperty("iconHighlightEffectMode", 1);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname) {
        Q_UNUSED(mname);
        showCreateUserDialog();
    });

    ui->addLyt->addWidget(addWgt);

    ui->nopwdHorLayout->setSpacing(0);
    ui->nopwdHorLayout->setMargin(0);

    nopwdSwitchBtn = new SwitchButton(ui->noPwdLoginFrame);
    ui->nopwdHorLayout->addWidget(nopwdSwitchBtn);

    autoLoginSwitchBtn = new SwitchButton(ui->autoLoginFrame);
    ui->autoLoginHorLayout->addWidget(autoLoginSwitchBtn);

    ui->listWidget->setSpacing(0);

    ElipseMaskWidget * mainElipseMaskWidget = new ElipseMaskWidget(ui->currentUserFaceLabel);
    mainElipseMaskWidget->setGeometry(0, 0, ui->currentUserFaceLabel->width(), ui->currentUserFaceLabel->height());

    if (getuid()){
        ui->currentUserFaceLabel->installEventFilter(this);
        ui->nameChangeWidget->installEventFilter(this);
    }

    //修改当前用户密码的回调
    connect(ui->changePwdBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        UserInfomation user = allUserInfoMap.value(g_get_user_name());

        showChangePwdDialog(user.username);
    });

    //修改当前用户类型的回调
    if (getuid())
        connect(ui->changeTypeBtn, &QPushButton::clicked, this, [=](bool checked){
            Q_UNUSED(checked)
            UserInfomation user = allUserInfoMap.value(g_get_user_name());

            showChangeTypeDialog(user.username);
        });

    connect(ui->changeGroupBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showChangeGroupDialog();
    });

    connect(ui->changeValidBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        UserInfomation user = allUserInfoMap.value(g_get_user_name());

        showChangeValidDialog(user.username);

    });

    //修改当前用户免密登录
    if (getuid())
        connect(nopwdSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){

            UserInfomation user = allUserInfoMap.value(g_get_user_name());
            //免密登录状态改变

            QDBusInterface * tmpSysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                                                  "/",
                                                                  "com.control.center.interface",
                                                                  QDBusConnection::systemBus());

            if (!tmpSysinterface->isValid()){
                qCritical() << "Create Client Interface Failed When execute gpasswd: " << QDBusConnection::systemBus().lastError();
                return;
            }
            tmpSysinterface->call("setNoPwdLoginStatus", checked, user.username);

            delete tmpSysinterface;
            tmpSysinterface = nullptr;

        });

    //修改当前用户自动登录
    if (getuid())
        connect(autoLoginSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked) {

            UserInfomation user = allUserInfoMap.value(g_get_user_name());

            UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);

            bool status = getAutomaticLogin().contains(user.username, Qt::CaseSensitive);

            if (checked && !isOpenAutoLogin(user.username)) {
                autoLoginSwitchBtn->blockSignals(true);
                autoLoginSwitchBtn->setChecked(false);
                autoLoginSwitchBtn->blockSignals(false);
                return ;
            }

            if ((checked != status)) {
                if (checked) {
                    userdispatcher->change_user_autologin(user.username);
                } else {
                    userdispatcher->change_user_autologin("");
                }
            }
        });

    //成功删除用户的回调
    connect(sysdispatcher, &SystemDbusDispatcher::deleteuserdone, this, [=](QString objPath){
        deleteUserDone(objPath);
    });

    //新建用户的回调
//    connect(ui->addBtn, &QPushButton::clicked, this, [=](bool checked){
//        Q_UNUSED(checked)
//        showCreateUserDialog();
//    });

    //成功新建用户的回调
    connect(sysdispatcher, &SystemDbusDispatcher::createuserdone, this, [=](QString objPath){
        createUserDone(objPath);
    });

    //初始化生物密码控件
    if(isShowBiometric())
        initBioComonent();
    else
        setBiometricDeviceVisible(false);
}

void UserInfo::_resetListWidgetHeigh(){
    //设置其他用户控件的总高度
    ui->listWidget->setFixedHeight((allUserInfoMap.count() - 1 ) * (ITEMHEIGH) + 4);
}

void UserInfo::initAllUserStatus(){
    _resetListWidgetHeigh();

    //每次初始化用户状态清空其他用户QMap
    otherUserItemMap.clear();

    QMap<QString, UserInfomation>::iterator it = allUserInfoMap.begin();
    for (; it != allUserInfoMap.end(); it++){
        UserInfomation user = it.value();

        //当前用户
        if (user.username == QString(g_get_user_name())){

        } else { //其他用户
            _buildWidgetForItem(user);

        }
    }
}

QStringList UserInfo::getLoginedUsers() {
    m_loginedUser.clear();
    qRegisterMetaType<LoginedUsers>("LoginedUsers");
    qDBusRegisterMetaType<LoginedUsers>();
    QDBusInterface loginInterface("org.freedesktop.login1",
                                  "/org/freedesktop/login1",
                                  "org.freedesktop.login1.Manager",
                                  QDBusConnection::systemBus());

    if (loginInterface.isValid()) {
        qDebug() << "create interface sucess";
    }

    QDBusMessage result = loginInterface.call("ListUsers");
    QList<QVariant> outArgs = result.arguments();
    QVariant first = outArgs.at(0);
    QDBusArgument dbvFirst = first.value<QDBusArgument>();
    QVariant vFirst = dbvFirst.asVariant();
    const QDBusArgument &dbusArgs = vFirst.value<QDBusArgument>();

    QVector<LoginedUsers> loginedUsers;

    dbusArgs.beginArray();
    while (!dbusArgs.atEnd()) {
        LoginedUsers user;
        dbusArgs >> user;
        loginedUsers.push_back(user);
    }
    dbusArgs.endArray();

    for (LoginedUsers user : loginedUsers) {

        QDBusInterface userPertyInterface("org.freedesktop.login1",
                                          user.objpath.path(),
                                          "org.freedesktop.DBus.Properties",
                                          QDBusConnection::systemBus());

        QDBusReply<QVariant> reply = userPertyInterface.call("Get", "org.freedesktop.login1.User", "State");
        if (reply.isValid()) {
            QString status = reply.value().toString();
            if ("closing" != status) {
                m_loginedUser.append(user.userName);
            }
        }
    }
    return m_loginedUser;
}

void UserInfo::_refreshUserInfoUI(){
    QMap<QString, UserInfomation>::iterator it = allUserInfoMap.begin();
    for (; it != allUserInfoMap.end(); it++){
        UserInfomation user = it.value();

        //用户头像为.face且.face文件不存在
        char * iconpath = user.iconfile.toLatin1().data();
        if (!g_file_test(iconpath, G_FILE_TEST_EXISTS)){
            user.iconfile = DEFAULTFACE;
            //更新用户数据
            allUserInfoMap.find(it.key()).value().iconfile = DEFAULTFACE;
        }

        //当前用户
        if (user.username == QString(g_get_user_name())){
            //设置用户头像
            QPixmap iconPixmap = QPixmap(user.iconfile).scaled(ui->currentUserFaceLabel->size());
            ui->currentUserFaceLabel->setPixmap(iconPixmap);

            //设置用户名
            ui->userNameLabel->setText(user.realname);
            ui->userNameChangeLabel->setPixmap(QIcon::fromTheme("document-edit-symbolic").pixmap(ui->userNameChangeLabel->size()));
            //设置用户类型
            ui->userTypeLabel->setText(_accountTypeIntToString(user.accounttype));
            //设置登录状态
            autoLoginSwitchBtn->blockSignals(true);
            autoLoginSwitchBtn->setChecked(user.autologin);
            autoLoginSwitchBtn->blockSignals(false);
            //设置免密登录状态
            nopwdSwitchBtn->setChecked(user.noPwdLogin);

        } else { //其他用户
            QListWidgetItem * item = otherUserItemMap.value(user.objpath); //是否需要判断？？

            QWidget * widget = ui->listWidget->itemWidget(item);

            QPushButton * faceBtn = widget->findChild<QPushButton *>("faceBtn");
            faceBtn->setIcon(QIcon(user.iconfile));

        }
    }
}

void UserInfo::_buildWidgetForItem(UserInfomation user){
    HoverWidget * baseWidget = new HoverWidget(user.username);
    baseWidget->setMinimumSize(550,50);
    baseWidget->setMaximumSize(960,50);
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);

    //ui->currentUserFrame->setContentsMargins(16,0,16,0);

    QHBoxLayout * baseVerLayout = new QHBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setMargin(0);

    QHBoxLayout * baseHorLayout = new QHBoxLayout();
    baseHorLayout->setSpacing(16);
    baseHorLayout->setMargin(0);

    QFrame * widget = new QFrame(baseWidget);
    widget->setFrameShape(QFrame::Shape::Box);
    widget->setFixedHeight(50);

    QHBoxLayout * mainHorLayout = new QHBoxLayout(widget);
    mainHorLayout->setSpacing(16);
    mainHorLayout->setContentsMargins(16, 0, 16, 0);

    QPushButton * faceBtn = new QPushButton(widget);
    faceBtn->setObjectName("faceBtn");
    faceBtn->setFixedSize(40, 40);
    faceBtn->setIcon(QIcon(user.iconfile));
    faceBtn->setIconSize(QSize(32, 32));
    connect(faceBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        showChangeFaceDialog(user.username);
    });

    ElipseMaskWidget * otherElipseMaskWidget = new ElipseMaskWidget(faceBtn);
    otherElipseMaskWidget->setGeometry(0, 0, faceBtn->width(), faceBtn->height());

    QLabel * nameLabel = new QLabel(widget);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setText(user.realname);

    QString btnQss = QString("QPushButton{background: #ffffff; border-radius: 4px;}");

    QPushButton * typeBtn = new QPushButton(widget);
//    typeBtn->setFixedSize(88, 36);
    typeBtn->setFixedHeight(36);
    typeBtn->setMinimumWidth(88);
    typeBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    typeBtn->setText(tr("Change type"));
//    typeBtn->setStyleSheet(btnQss);
    connect(typeBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showChangeTypeDialog(user.username);
    });
    typeBtn->hide();

    QPushButton * pwdBtn = new QPushButton(widget);
//    pwdBtn->setFixedSize(88, 36);
    pwdBtn->setFixedHeight(36);
    pwdBtn->setMinimumWidth(88);
    pwdBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    pwdBtn->setText(tr("Change pwd"));
//    pwdBtn->setStyleSheet(btnQss);
    connect(pwdBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showChangePwdDialog(user.username);
    });
    pwdBtn->hide();

    mainHorLayout->addWidget(faceBtn);
    mainHorLayout->addWidget(nameLabel);
    mainHorLayout->addStretch();
    mainHorLayout->addWidget(typeBtn);
    mainHorLayout->addWidget(pwdBtn);

    widget->setLayout(mainHorLayout);

    QPushButton * delBtn = new QPushButton(baseWidget);
    delBtn->setFixedSize(60, 36);
    delBtn->setText(tr("Del"));
//    delBtn->setStyleSheet("QPushButton{background: #FA6056; border-radius: 4px}");
    delBtn->hide();
    connect(delBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showDeleteUserDialog(user.username);
    });

    connect(baseWidget, &HoverWidget::enterWidget, this, [=](QString name){

        //不允许删除最后一个管理员
        if (_userCanDel(name) == 1){
            delBtn->setEnabled(true);
        } else if (_userCanDel(name) == 0) {
            delBtn->setEnabled(false);
        }

#ifdef WITHKYSEC
        if (!kysec_is_disabled() && kysec_get_3adm_status()){
            if (user.username == "secadm" || user.username == "auditadm"){
                delBtn->setEnabled(false);
            }
        }
#endif

        typeBtn->show();
        pwdBtn->show();
        delBtn->show();
    });
    connect(baseWidget, &HoverWidget::leaveWidget, this, [=](QString name){
        Q_UNUSED(name)
        typeBtn->hide();
        pwdBtn->hide();
        delBtn->hide();
    });

    baseHorLayout->addWidget(widget);
    baseHorLayout->addWidget(delBtn, Qt::AlignVCenter);
    //baseHorLayout->addSpacing(4);

    baseVerLayout->addLayout(baseHorLayout);
//    baseVerLayout->addStretch();

    baseWidget->setLayout(baseVerLayout);

#ifdef WITHKYSEC
    if (!kysec_is_disabled() && kysec_get_3adm_status()){
        if (user.username == "secadm" || user.username == "auditadm"){
            pwdBtn->setEnabled(false);
            typeBtn->setEnabled(false);
        }
    }
#endif

    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
//    item->setSizeHint(QSize(ui->listWidget->width() - 4, ITEMHEIGH));
    item->setSizeHint(QSize(QSizePolicy::Expanding, ITEMHEIGH));
    item->setData(Qt::UserRole, QVariant(user.objpath));
    ui->listWidget->setItemWidget(item, baseWidget);

    otherUserItemMap.insert(user.objpath, item);

}

void UserInfo::showCreateUserDialog(){
    //获取系统所有用户名列表，创建时判断重名
    QStringList usersStringList;
    for (QVariant tmp : allUserInfoMap.keys()){
        usersStringList << tmp.toString();
    }

    CreateUserDialog * dialog = new CreateUserDialog(usersStringList);
    dialog->setRequireLabel(pwdMsg);
    connect(dialog, &CreateUserDialog::newUserWillCreate, this, [=](QString uName, QString pwd, QString pin, int aType){
        createUser(uName, pwd, pin, aType);
    });
    dialog->exec();
}

QStringList UserInfo::getUsersList()
{
    QStringList usersStringList;
    for (QVariant tmp : allUserInfoMap.keys()){
        usersStringList << tmp.toString();
    }
    return usersStringList;
}

void UserInfo::createUser(QString username, QString pwd, QString pin, int atype){
    Q_UNUSED(pin);
    sysdispatcher->create_user(username, "", atype);

    //使用全局变量传递新建用户密码
    _newUserPwd = pwd;
}

void UserInfo::createUserDone(QString objpath){
    UserDispatcher * userdispatcher  = new UserDispatcher(objpath);
    //设置默认头像
    userdispatcher->change_user_face(DEFAULTFACE);
    //设置默认密码
    userdispatcher->change_user_pwd(_newUserPwd, "");

    //刷新全部用户信息
    _acquireAllUsersInfo();
    //重建其他用户ListWidget高度
    _resetListWidgetHeigh();

    //获取新建用户信息
    UserInfomation user;
    user = _acquireUserInfo(objpath);

    //构建Item
    _buildWidgetForItem(user);
}

void UserInfo::showDeleteUserDialog(QString username){

    QStringList loginedusers = getLoginedUsers();
    if (loginedusers.contains(username)) {
        QMessageBox::warning(pluginWidget, tr("Warning"), tr("The user is logged in, please delete the user after logging out"));
        return;
    }
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    DelUserDialog * dialog = new DelUserDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setUsername(user.username);
    connect(dialog, &DelUserDialog::removefile_send, this, [=](bool removeFile){
        deleteUser(removeFile, user.username);
    });
    dialog->exec();
}

void UserInfo::deleteUser(bool removefile, QString username){
    qDebug() << allUserInfoMap.keys() << username;

    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    // hidden the item when click delete user button
//    QListWidgetItem *item =  otherUserItemMap.find(user.objpath).value();
//    ui->listWidget->setItemHidden(item, true);

    sysdispatcher->delete_user(user.uid, removefile);
}

void UserInfo::delete_user_slot(bool removefile, QString username){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    sysdispatcher->delete_user(user.uid, removefile);
}

void UserInfo::pwdAndAutoChangedSlot(QString key) {
    if ("option" == key) {
        autoLoginSwitchBtn->blockSignals(true);
        autoLoginSwitchBtn->setChecked(getAutomaticLogin().contains(mUserName, Qt::CaseSensitive));
        autoLoginSwitchBtn->blockSignals(false);
        nopwdSwitchBtn->setChecked(getNoPwdStatus());
    } else if( "avatar" == key) {
        //重新获取全部用户QMap
        _acquireAllUsersInfo();

        //更新界面显示
        _refreshUserInfoUI();
    }
}

void UserInfo::propertyChangedSlot(QString property, QMap<QString, QVariant> propertyMap, QStringList propertyList) {
    Q_UNUSED(property);
    Q_UNUSED(propertyList);
    if (propertyMap.keys().contains("IconFile") && getuid() &&
            propertyMap.value("RealName").toString() == mUserName) {
        if (propertyMap.keys().contains("AccountType")) {
            int type = propertyMap.value("AccountType").toInt();
            ui->userTypeLabel->setText(_accountTypeIntToString(type));
        }
    }
}

void UserInfo::deleteUserDone(QString objpath){
    QListWidgetItem * item = otherUserItemMap.value(objpath);

    //删除Item
    ui->listWidget->takeItem(ui->listWidget->row(item));

    //更新其他用户QMap
    otherUserItemMap.remove(objpath);

    //更新所有用户信息
    _acquireAllUsersInfo();

    //重置其他用户ListWidget高度
    _resetListWidgetHeigh();
}

void UserInfo::showChangeGroupDialog(){
    ChangeGroupDialog * dialog = new ChangeGroupDialog();
    dialog->exec();
}

void UserInfo::showChangeValidDialog(QString username){
    if (allUserInfoMap.keys().contains(username)){
        UserInfomation user = allUserInfoMap.value(username);

        ChangeValidDialog * dialog = new ChangeValidDialog(user.username);
        dialog->setUserName();
        dialog->setUserLogo(user.iconfile);
        dialog->setUserType(_accountTypeIntToString(user.accounttype));
        dialog->exec();

    } else {
        qDebug() << "User Data Error When Change User type";
    }
}


void UserInfo::showChangeTypeDialog(QString username){
    if (allUserInfoMap.keys().contains(username)){
        UserInfomation user = allUserInfoMap.value(username);

        ChangeTypeDialog * dialog = new ChangeTypeDialog;
        dialog->setFace(user.iconfile);
        dialog->setUsername(user.realname);
        dialog->setCurrentAccountTypeLabel(_accountTypeIntToString(user.accounttype));
        dialog->setCurrentAccountTypeBtn(user.accounttype);
        dialog->forbidenChange(_userCanDel(username));
//        connect(dialog, SIGNAL(type_send(int,QString,bool)), this, SLOT(change_accounttype_slot(int,QString,bool)));
        connect(dialog, &ChangeTypeDialog::type_send, this, [=](int atype){
            changeUserType(atype, username);
        });
        dialog->exec();

    } else {
        qDebug() << "User Data Error When Change User type";
    }
}

void UserInfo::changeUserType(int atype, QString username){
    UserInfomation user = allUserInfoMap.value(username);

    //构建dbus调度对象
    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath); //继承QObject不再删除

    //更改用户类型
    userdispatcher->change_user_type(atype);

    //重新获取全部用户QMap
    _acquireAllUsersInfo();

    //更新界面显示
    _refreshUserInfoUI();
}

void UserInfo::showChangeNameDialog(){
    ChangeUserName * dialog = new ChangeUserName;
    connect(dialog, &ChangeUserName::sendNewName, [=](QString name){
        changeUserName(name);
    });
    dialog->exec();
}

void UserInfo::changeUserName(QString newName){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(QString(g_get_user_name())).value());

    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);

    userdispatcher->change_user_name(newName);

    //重新获取全部用户QMap
    _acquireAllUsersInfo();

    //更新界面显示
    _refreshUserInfoUI();
}


void UserInfo::showChangeFaceDialog(QString username){
    if (allUserInfoMap.keys().contains(username)){
        UserInfomation user = allUserInfoMap.value(username);

        ChangeFaceDialog * dialog = new ChangeFaceDialog;
        dialog->setFace(user.iconfile);
        dialog->setUsername(user.realname);
        dialog->setAccountType(_accountTypeIntToString(user.accounttype));
    //    dialog->set_face_list_status(user.iconfile);
        connect(dialog, &ChangeFaceDialog::face_file_send, [=](QString faceFile){
            changeUserFace(faceFile, user.username);
        });
        dialog->exec();
    } else {
        qDebug() << "User Data Error When Change User Face!";
    }

}

void UserInfo::changeUserFace(QString facefile, QString username){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);
    userdispatcher->change_user_face(facefile);

    //拷贝设置的头像文件到~/.face
    sysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                     "/",
                                     "com.control.center.interface",
                                     QDBusConnection::systemBus());

    if (!sysinterface->isValid()){
        qCritical() << "Create Client Interface Failed When Copy Face File: " << QDBusConnection::systemBus().lastError();
        return;
    }

    QString cmd = QString("cp %1 /home/%2/.face").arg(facefile).arg(user.username);

    QProcess::execute(cmd);

    //重新获取全部用户QMap
    _acquireAllUsersInfo();

    //更新界面显示
    _refreshUserInfoUI();
}

void UserInfo::showChangePwdDialog(QString username){

    if (allUserInfoMap.keys().contains(username)){
        UserInfomation user = allUserInfoMap.value(username);
        ChangePwdDialog * dialog = new ChangePwdDialog(user.current, user.username);
        dialog->setFace(user.iconfile);
        dialog->setUsername(user.realname);
        dialog->setAccountType(_accountTypeIntToString(user.accounttype));
        if (!getuid() || !user.current)
            dialog->haveCurrentPwdEdit(false);

        connect(dialog, &ChangePwdDialog::passwd_send, this, [=](QString oldpwd, QString pwd){

//                changeUserPwd(pwd, username);

            QString output;

            char * cmd = g_strdup_printf("/usr/bin/changeuserpwd %s %s", oldpwd.toLatin1().data(), pwd.toLatin1().data());

            FILE   *stream;
            char buf[256];

            if ((stream = popen(cmd, "r" )) == NULL){
                return -1;
            }

            while(fgets(buf, 256, stream) != NULL){
                output = QString(buf).simplified();
            }

            pclose(stream);

        });
        connect(dialog, &ChangePwdDialog::passwd_send2, this, [=](QString pwd){

            PolkitQt1::Authority::Result result;

            result = PolkitQt1::Authority::instance()->checkAuthorizationSync(
                        "org.control.center.qt.systemdbus.action",
                        PolkitQt1::UnixProcessSubject(QCoreApplication::applicationPid()),
                        PolkitQt1::Authority::AllowUserInteraction);

            if (result == PolkitQt1::Authority::Yes){
                changeUserPwd(pwd, username);
            }


        });
        dialog->exec();

    } else {
        qDebug() << "User Info Data Error When Change User type";
    }
}


void UserInfo::changeUserPwd(QString pwd, QString username){
    //上层已做判断，这里不去判断而直接获取
    UserInfomation user = allUserInfoMap.value(username);

//    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath); //继承QObject不再删除
//    QString result = userdispatcher->change_user_pwd(pwd, "");

    QDBusInterface * tmpSysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                                          "/",
                                                          "com.control.center.interface",
                                                          QDBusConnection::systemBus());

    if (!tmpSysinterface->isValid()){
        qCritical() << "Create Client Interface Failed When : " << QDBusConnection::systemBus().lastError();
        return;
    }
    tmpSysinterface->call("changeOtherUserPasswd", username, pwd);

    delete tmpSysinterface;
    tmpSysinterface = nullptr;
}


bool UserInfo::eventFilter(QObject *watched, QEvent *event){
    if (watched == ui->currentUserFaceLabel){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton ){
                if(watched == ui->currentUserFaceLabel){
                    showChangeFaceDialog(g_get_user_name());
                }
                return true;
            } else {
                return false;
            }
        }
    } else if (watched == ui->nameChangeWidget){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton ){
                showChangeNameDialog();
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

QString UserInfo::getAutomaticLogin() {

    QString filename = "/etc/lightdm/lightdm.conf";
    autoSettings = new QSettings(filename, QSettings::IniFormat);
    autoSettings->beginGroup("SeatDefaults");

    QString autoUser = autoSettings->value("autologin-user", "").toString();

    autoSettings->endGroup();

    return autoUser;
}

bool UserInfo::getNoPwdStatus() {
    // 获取当前用户免密登录属性
    QDBusInterface tmpSysinterface("com.control.center.qt.systemdbus",
                                     "/",
                                     "com.control.center.interface",
                                     QDBusConnection::systemBus());
    // 获取免密登录状态
    QDBusReply<QString> noPwdres;
    noPwdres  = tmpSysinterface.call("getNoPwdLoginStatus");
    if (!noPwdres.isValid()) {
        qDebug() << noPwdres.error();
    }
    return (noPwdres.value().contains(mUserName) ? true : false);
}

bool UserInfo::isOpenAutoLogin(const QString &userName) {
    QString autoLoginedUser = this->getAutomaticLogin();
    bool res = true;
    int  ret;
    if (!autoLoginedUser.isEmpty() && userName != autoLoginedUser) {
        QMessageBox msg(this->pluginWidget);
        msg.setWindowTitle(tr("Hint"));
        msg.setText(tr("The system only allows one user to log in automatically."
                       "After it is turned on, the automatic login of other users will be turned off."
                       "Is it turned on?"));
        msg.addButton(tr("Trun on"), QMessageBox::AcceptRole);
        msg.addButton(tr("Close on"), QMessageBox::RejectRole);

        ret = msg.exec();

        switch (ret) {
          case QMessageBox::AcceptRole:
              res = true;
              break;
          case QMessageBox::RejectRole:
              res = false;
              break;
        }
    }
    return res;
}

void UserInfo::initUserPropertyConnection(const QStringList &objPath) {

    foreach (QString userPath, objPath) {
        QDBusInterface iproperty("org.freedesktop.Accounts",
                                 userPath,
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::systemBus());

        iproperty.connection().connect("org.freedesktop.Accounts", userPath, "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                        this, SLOT(propertyChangedSlot(QString, QMap<QString, QVariant>, QStringList)));
    }

    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface", "keyChanged", this, SLOT(pwdAndAutoChangedSlot(QString)));
}

bool UserInfo::isShowBiometric()
{

    QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);

    return sysSettings.value("isShownInControlCenter").toString() == "true";

}

void UserInfo::initBioComonent()
{
    m_biometricProxy = new BiometricProxy(this);


    serviceInterface = new QDBusInterface(DBUS_SERVICE,
                                          DBUS_PATH,
                                          DBUS_INTERFACE
                                          , QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647); /* 微秒 */

    addBioFeatureWidget = new HoverWidget("");
    addBioFeatureWidget->setObjectName("addBioFeatureWidget");
    addBioFeatureWidget->setMinimumSize(QSize(580, 50));
    addBioFeatureWidget->setMaximumSize(QSize(960, 50));
    addBioFeatureWidget->setStyleSheet("HoverWidget#addBioFeatureWidget{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addBioFeatureWidget{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addBioFeatureLayout = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add biometric feature"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    addBioFeatureLayout->addWidget(iconLabel);
    addBioFeatureLayout->addWidget(textLabel);
    addBioFeatureLayout->addStretch();
    addBioFeatureWidget->setLayout(addBioFeatureLayout);

    // 悬浮改变Widget状态
    connect(addBioFeatureWidget, &HoverWidget::enterWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });
    // 还原状态
    connect(addBioFeatureWidget, &HoverWidget::leaveWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addBioFeatureWidget, &HoverWidget::widgetClicked, this, [=](QString mname) {
        Q_UNUSED(mname);
        showEnrollDialog();
    });

    ui->addFeatureLayout->addWidget(addBioFeatureWidget);

    ui->bioFeatureListWidget->setStyleSheet("QListWidget::Item:hover{background:palette(base);}");
    ui->bioFeatureListWidget->setSpacing(0);

    ui->bioFeatureListWidget->setFixedHeight(biometricFeatureMap.count()*ITEMHEIGH - biometricFeatureMap.count()*6);

    connect(ui->biometrictypeBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onbiometricTypeBoxCurrentIndexChanged(int)));

    connect(ui->biometricDeviceBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onbiometricDeviceBoxCurrentIndexChanged(int)));

    connect(ui->bioSettings, &QPushButton::clicked, this, [=](){
        QProcess process(this);
        process.startDetached("/usr/bin/biometric-manager");
    });

    ui->biometricMoreBtn->setText("...");
    connect(ui->biometricMoreBtn, &QPushButton::clicked, this, [=](){
        biometricShowMoreInfoDialog();
    });

    updateDevice();

    if(m_biometricProxy && m_biometricProxy->isValid())
    {
        connect(m_biometricProxy, &BiometricProxy::USBDeviceHotPlug,
            this, &UserInfo::onBiometricUSBDeviceHotPlug);
    }

    enableBiometricBtn = new SwitchButton(ui->enableBiometricFrame);
    enableBiometricBtn->setChecked(getBioStatus());
    ui->enableBiometricLayout->addWidget(enableBiometricBtn);
    connect(enableBiometricBtn, &SwitchButton::checkedChanged, [=](bool checked){
        QProcess process;
        if(checked){
            process.start("bioctl enable");
            process.waitForFinished(3000);
        }else{
            process.start("bioctl disable");
            process.waitForFinished(3000);
        }
    });

    mBiometricWatcher = nullptr;
    if(!mBiometricWatcher){
        mBiometricWatcher = new QFileSystemWatcher(this);
        mBiometricWatcher->addPath(UKUI_BIOMETRIC_SYS_CONFIG_PATH);
        connect(mBiometricWatcher,&QFileSystemWatcher::fileChanged,this,[=](const QString &path){
            mBiometricWatcher->addPath(UKUI_BIOMETRIC_SYS_CONFIG_PATH);
            enableBiometricBtn->blockSignals(true);
            enableBiometricBtn->setChecked(getBioStatus());
            enableBiometricBtn->blockSignals(false);
        });
    }

}

bool UserInfo::getBioStatus()
{
    QProcess process;
    process.start("bioctl status");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    if (output.contains("enable", Qt::CaseInsensitive)) {
        return true;
    }
    else {
        return false;
    }
}

void UserInfo::setBioStatus(bool status)
{
    enableBiometricBtn->setChecked(true);
}

void UserInfo::onBiometricUSBDeviceHotPlug(int drvid, int action, int deviceNum)
{
    int savedDeviceId = -1;
    if(currentDevice)
        savedDeviceId = currentDevice->id;

    int savedCount = 0;
    for(int type : deviceMap.keys())
        savedCount += deviceMap.value(type).count();

    switch(action)
    {
    case ACTION_ATTACHED:
    {
        //插入设备后，需要更新设备列表
        updateDevice();
        if(savedDeviceId >= 0)
            setCurrentDevice(savedDeviceId);
        break;
    }
    case ACTION_DETACHED:
    {
        updateDevice();
    }

    }
}

void UserInfo::setBiometricDeviceVisible(bool visible)
{
    if(!visible)
    {
        ui->bioTitleWidget->hide();
        ui->biometricWidget->hide();
    }else{
        ui->bioTitleWidget->show();
        ui->biometricWidget->show();
    }
}

void UserInfo::updateDevice()
{
    deviceMap.clear();
    DeviceList deviceList = m_biometricProxy->GetDevList();

    QString default_name = GetDefaultDevice(QString(qgetenv("USER")));

    for(auto pDeviceInfo : deviceList)
    {
        deviceMap[pDeviceInfo->deviceType].push_back(pDeviceInfo);
    }
    ui->biometrictypeBox->clear();
    for(int type : deviceMap.keys())
    {
        ui->biometrictypeBox->addItem(DeviceType::getDeviceType_tr(type), type);
    }
    if(deviceMap.size() > 0)
    {
        DeviceInfoPtr ptr = findDeviceByName(default_name);
        if(ptr)
        {
            setCurrentDevice(default_name);
        }else{
            int index = deviceMap.keys().at(0);
            setCurrentDevice(deviceMap[index].at(0));
        }
    }

    if(deviceMap.size()<=0)
        setBiometricDeviceVisible(false);
    else
        setBiometricDeviceVisible(true);
}

void UserInfo::setCurrentDevice(int drvid)
{
    DeviceInfoPtr pDeviceInfo = findDeviceById(drvid);
    if(pDeviceInfo)
    {
        setCurrentDevice(pDeviceInfo);
    }
}

void UserInfo::setCurrentDevice(const QString &deviceName)
{
    DeviceInfoPtr pDeviceInfo = findDeviceByName(deviceName);
    if(pDeviceInfo)
    {
        setCurrentDevice(pDeviceInfo);
    }
}

void UserInfo::setCurrentDevice(const DeviceInfoPtr &pDeviceInfo)
{
    this->currentDevice = pDeviceInfo;
    ui->biometrictypeBox->setCurrentText(DeviceType::getDeviceType_tr(pDeviceInfo->deviceType));
    ui->biometricDeviceBox->setCurrentText(pDeviceInfo->shortName);

}

DeviceInfoPtr UserInfo::findDeviceById(int drvid)
{
    for(int type : deviceMap.keys())
    {
        DeviceList &deviceList = deviceMap[type];
        auto iter = std::find_if(deviceList.begin(), deviceList.end(),
                                 [&](DeviceInfoPtr ptr){
            return ptr->id == drvid;
        });
        if(iter != deviceList.end())
        {
            return *iter;
        }
    }
    return DeviceInfoPtr();
}

DeviceInfoPtr UserInfo::findDeviceByName(const QString &name)
{
    for(int type : deviceMap.keys())
    {
        DeviceList &deviceList = deviceMap[type];
        auto iter = std::find_if(deviceList.begin(), deviceList.end(),
                                 [&](DeviceInfoPtr ptr){
            return ptr->shortName == name;
        });
        if(iter != deviceList.end())
        {
            return *iter;
        }
    }
    return DeviceInfoPtr();
}

bool UserInfo::deviceExists(int drvid)
{
    return (findDeviceById(drvid) != nullptr);
}

bool UserInfo::deviceExists(const QString &deviceName)
{
    return (findDeviceByName(deviceName) != nullptr);
}

void UserInfo::onbiometricTypeBoxCurrentIndexChanged(int index)
{
    if(index < 0 || index >= deviceMap.keys().size())
    {
        return;
    }

    int type = ui->biometrictypeBox->itemData(index).toInt();
    ui->biometricDeviceBox->clear();

    for(auto &deviceInfo : deviceMap.value(type))
    {
        ui->biometricDeviceBox->addItem(deviceInfo->shortName);
    }
}

void UserInfo::onbiometricDeviceBoxCurrentIndexChanged(int index)
{
    if(index < 0)
    {
        return;
    }

    int type = ui->biometrictypeBox->currentData().toInt();

    DeviceInfoPtr deviceInfo = deviceMap.value(type).at(index);
    QList<QVariant> args;

    currentDevice = deviceInfo;

    args << QVariant(deviceInfo->id)
        << QVariant((int)getuid()) << QVariant(0) << QVariant(-1);
    serviceInterface->callWithCallback("GetFeatureList", args, this,
                        SLOT(updateFeatureListCallback(QDBusMessage)),
                        SLOT(errorCallback(QDBusError)));
}

void UserInfo::updateFeatureListCallback(QDBusMessage callbackReply)
{
    QList<QDBusVariant> qlist;
    FeatureInfo *featureInfo;
    int listsize;

    ui->bioFeatureListWidget->clear();
    biometricFeatureMap.clear();

    QList<QVariant> variantList = callbackReply.arguments();
    listsize = variantList[0].value<int>();
    variantList[1].value<QDBusArgument>() >> qlist;
    for (int i = 0; i < listsize; i++) {
        featureInfo = new FeatureInfo;
        qlist[i].variant().value<QDBusArgument>() >> *featureInfo;
        addFeature(featureInfo);
    }
    updateFeatureList();
}

void UserInfo::updateFeatureList()
{
    ui->bioFeatureListWidget->setFixedHeight(biometricFeatureMap.count()*ITEMHEIGH + biometricFeatureMap.count()*6);
}

void UserInfo::errorCallback(QDBusError error)
{

}

void UserInfo::showEnrollDialog()
{
    if(ui->biometricDeviceBox->count() <= 0 || ui->biometrictypeBox->count() <= 0)
        return ;

    int index = ui->biometricDeviceBox->currentIndex();
    int type = ui->biometrictypeBox->currentData().toInt();

    if(index < 0|| type < 0)
        return ;

    DeviceInfoPtr deviceInfo = deviceMap.value(type).at(index);

    if(!deviceInfo)
        return ;

    BiometricEnrollDialog * dialog = new BiometricEnrollDialog(serviceInterface,deviceInfo->deviceType,deviceInfo->id,getuid());
    //gdxfp显示指纹图片
    if(deviceInfo->shortName == "gdxfp")
        dialog->setProcessed(true);

    int num=1;
    QStringList list = m_biometricProxy->getFeaturelist(deviceInfo->id,getuid(),0,-1);
    QString featurename;
    while(1){
        featurename = DeviceType::getDeviceType_tr(deviceInfo->deviceType) + QString::number(num);
        if(!list.contains(featurename))
            break;
        num++;
    }
    dialog->enroll(deviceInfo->id,getuid(),-1,featurename);

    onbiometricDeviceBoxCurrentIndexChanged(ui->biometricDeviceBox->currentIndex());

}

void UserInfo::showVerifyDialog(FeatureInfo *featureinfo)
{
    DeviceInfoPtr deviceInfoPtr = findDeviceByName(featureinfo->device_shortname);
    if(!deviceInfoPtr)
            return ;

    BiometricEnrollDialog * dialog = new BiometricEnrollDialog(serviceInterface,deviceInfoPtr->deviceType,deviceInfoPtr->id,getuid());

    if(deviceInfoPtr->shortName == "huawei")
        dialog->setProcessed(true);

    dialog->verify(deviceInfoPtr->id,getuid(),featureinfo->index);
}

void UserInfo::biometricShowMoreInfoDialog()
{
    if(ui->biometricDeviceBox->count() <= 0 || ui->biometrictypeBox->count() <= 0)
        return ;

    int index = ui->biometricDeviceBox->currentIndex();
    int type = ui->biometrictypeBox->currentData().toInt();

    if(index < 0|| type < 0)
        return ;

    DeviceInfoPtr deviceInfo = deviceMap.value(type).at(index);

    if(!deviceInfo)
        return ;

    BiometricMoreInfoDialog * dialog = new BiometricMoreInfoDialog(deviceInfo);
    dialog->exec();
}

void UserInfo::renameFeaturedone(FeatureInfo *featureinfo ,QString newname)
{
     QListWidgetItem *item = biometricFeatureMap.value(featureinfo->index_name);
     ui->bioFeatureListWidget->takeItem(ui->bioFeatureListWidget->row(item));
     biometricFeatureMap.remove(featureinfo->index_name);


     featureinfo->index_name = newname;
     addFeature(featureinfo);

}

void UserInfo::deleteFeaturedone(FeatureInfo *featureinfo)
{

    QListWidgetItem *item = biometricFeatureMap.value(featureinfo->index_name);

    ui->bioFeatureListWidget->takeItem(ui->bioFeatureListWidget->row(item));
    biometricFeatureMap.remove(featureinfo->index_name);

    updateFeatureList();
}

void UserInfo::addFeature(FeatureInfo *featureinfo)
{
    HoverWidget * baseWidget = new HoverWidget(featureinfo->index_name);
    baseWidget->setMinimumSize(550,50);
    baseWidget->setMaximumSize(960,50);
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);

    //ui->currentUserFrame->setContentsMargins(16,0,16,0);

    QHBoxLayout * baseVerLayout = new QHBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setMargin(0);

    QHBoxLayout * baseHorLayout = new QHBoxLayout();
    baseHorLayout->setSpacing(16);
    baseHorLayout->setMargin(0);

    QFrame * widget = new QFrame(baseWidget);
    widget->setFrameShape(QFrame::Shape::Box);
    widget->setFixedHeight(50);

    QHBoxLayout * mainHorLayout = new QHBoxLayout(widget);
    mainHorLayout->setSpacing(16);
    mainHorLayout->setContentsMargins(16, 0, 16, 0);

    QLabel * nameLabel = new QLabel(widget);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setText(featureinfo->index_name);

    QString btnQss = QString("QPushButton{background: #ffffff; border-radius: 4px;}");

    QLineEdit *renameEdit = new QLineEdit(widget);
    renameEdit->setFixedWidth(240);
    renameEdit->setText(featureinfo->index_name);
    renameEdit->hide();

    connect(renameEdit, &QLineEdit::editingFinished, this, [=](){
        renameEdit->hide();
        nameLabel->show();
        QString rename = renameEdit->text();
        if(rename == "" || rename == featureinfo->index_name)
            return;

        DeviceInfoPtr deviceInfoPtr = findDeviceByName(featureinfo->device_shortname);
        if(!deviceInfoPtr)
                return ;
        bool res = m_biometricProxy->renameFeature(deviceInfoPtr->id,getuid(),featureinfo->index,rename);
        renameFeaturedone(featureinfo,rename);
    });

    QPushButton * renameBtn = new QPushButton(widget);

    renameBtn->setFixedHeight(36);
    renameBtn->setMinimumWidth(88);
    renameBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    renameBtn->setText(tr("Rename"));

    connect(renameBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        nameLabel->hide();
        renameEdit->show();
    });
    renameBtn->hide();

    QPushButton * verifyBtn = new QPushButton(widget);

    verifyBtn->setFixedHeight(36);
    verifyBtn->setMinimumWidth(88);
    verifyBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    verifyBtn->setText(tr("Verify"));

    connect(verifyBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showVerifyDialog(featureinfo);
    });

    renameBtn->hide();
    verifyBtn->hide();

    mainHorLayout->addWidget(nameLabel);
    mainHorLayout->addWidget(renameEdit);
    mainHorLayout->addStretch();
    mainHorLayout->addWidget(renameBtn);
    mainHorLayout->addWidget(verifyBtn);

    widget->setLayout(mainHorLayout);

    QPushButton * delBtn = new QPushButton(baseWidget);
    delBtn->setFixedSize(60, 36);
    delBtn->setText(tr("Delete"));
//    delBtn->setStyleSheet("QPushButton{background: #FA6056; border-radius: 4px}");
    delBtn->hide();
    connect(delBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        DeviceInfoPtr deviceInfoPtr = findDeviceByName(featureinfo->device_shortname);
        if(!deviceInfoPtr)
                return ;
        bool res = m_biometricProxy->deleteFeature(deviceInfoPtr->id,getuid(),featureinfo->index,featureinfo->index);
        if(!res){
             deleteFeaturedone(featureinfo);
        }
    });

    connect(baseWidget, &HoverWidget::enterWidget, this, [=](QString name){
        Q_UNUSED(name)
        renameBtn->show();
	//驱动gdxfp不支持验证
        if(featureinfo->device_shortname != "gdxfp")
	    verifyBtn->show();
        delBtn->show();
    });
    connect(baseWidget, &HoverWidget::leaveWidget, this, [=](QString name){
        Q_UNUSED(name)
        renameBtn->hide();
        verifyBtn->hide();
        delBtn->hide();
    });

    baseHorLayout->addWidget(widget);
    baseHorLayout->addWidget(delBtn, Qt::AlignVCenter);
    baseHorLayout->addSpacing(4);

    baseVerLayout->addLayout(baseHorLayout);

    baseWidget->setLayout(baseVerLayout);

    QListWidgetItem * item = new QListWidgetItem(ui->bioFeatureListWidget);
    item->setSizeHint(QSize(QSizePolicy::Expanding, ITEMHEIGH));
    item->setData(Qt::UserRole, QVariant(featureinfo->index_name));
    ui->bioFeatureListWidget->setItemWidget(item, baseWidget);

    biometricFeatureMap.insert(featureinfo->index_name, item);
}
