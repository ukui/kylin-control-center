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
#include <QDBusMetaType>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDebug>
#include <QMessageBox>

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
        delete autoSettings;
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

        ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
        ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

        // 构建System dbus调度对象
        sysdispatcher = new SystemDbusDispatcher(this);

        // 获取系统全部用户信息，用户Uid大于等于1000的
        _acquireAllUsersInfo();

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

void UserInfo::initSearchText() {
    //~ contents_path /userinfo/Change pwd
    ui->changePwdBtn->setText(tr("Change pwd"));
    //~ contents_path /userinfo/Change type
    ui->changeTypeBtn->setText(tr("Change type"));
    //~ contents_path /userinfo/Change valid
    ui->changeValidBtn->setText(tr("Change valid"));
    //~ contents_path /userinfo/Login no passwd
    ui->loginpwdLabel->setText(tr("Login no passwd"));
    //~ contents_path /userinfo/enable autoLogin
    ui->autologinLabel->setText(tr("enable autoLogin"));
}

QString UserInfo::_accountTypeIntToString(int type){
    QString atype;
    if (type == STANDARDUSER)
        atype = tr("standard user");
    else if (type == ADMINISTRATOR)
        atype = tr("administrator");
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
    adminnum = 0;

    //root
    if (!getuid()){
        UserInfomation root;
        root.username = g_get_user_name();
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
        if (user.username == QString(g_get_user_name())) {
            user.current = true;
            user.logined = true;
            user.noPwdLogin = getNoPwdStatus();
        }
        user.accounttype = propertyMap.find("AccountType").value().toInt();
        if (user.accounttype == ADMINISTRATOR)
            adminnum++;
        user.iconfile = propertyMap.find("IconFile").value().toString();
        user.passwdtype = propertyMap.find("PasswordMode").value().toInt();
        user.uid = propertyMap.find("Uid").value().toInt();
//        user.autologin = propertyMap.find("AutomaticLogin").value().toBool();
        user.autologin = this->getAutomaticLogin(user.username);
        user.objpath = objpath;
    }
    else
        qDebug() << "reply failed";

    delete iproperty;

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
        ui->changeGroupBtn->setEnabled(false);
        ui->autoLoginFrame->setVisible(false);
        ui->autoLoginFrame_2->setVisible(false);
    }

#ifdef WITHKYSEC
    if (!kysec_is_disabled() && kysec_get_3adm_status() && (getuid() || geteuid())){
        ui->addUserWidget->hide();
    }
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
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    // 悬浮改变Widget状态
    connect(addWgt, &HoverWidget::enterWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(base);");

    });
    // 还原状态
    connect(addWgt, &HoverWidget::leaveWidget, this, [=](QString mname) {
        Q_UNUSED(mname);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        iconLabel->setPixmap(pixgray);
        textLabel->setStyleSheet("color: palette(windowText);");
    });

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname) {
        Q_UNUSED(mname);
        showCreateUserDialog();
    });

    ui->addLyt->addWidget(addWgt);

    ui->nopwdHorLayout->setSpacing(0);
    ui->nopwdHorLayout->setMargin(0);
    nopwdSwitchBtn = new SwitchButton(ui->nopwdLoginWidget);
    ui->nopwdHorLayout->addWidget(nopwdSwitchBtn);

    autoLoginSwitchBtn = new SwitchButton(ui->autoLoginFrame);
    ui->autoLoginHorLayout->addWidget(autoLoginSwitchBtn);


//    ui->listWidget->setStyleSheet("QListWidget{background: #00ffff}");
//    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setSpacing(0);

    ElipseMaskWidget * mainElipseMaskWidget = new ElipseMaskWidget(ui->currentUserFaceLabel);
    mainElipseMaskWidget->setGeometry(0, 0, ui->currentUserFaceLabel->width(), ui->currentUserFaceLabel->height());

    //设置添加用户的图标
//    ui->addBtn->setIcon(QIcon("://img/plugins/userinfo/add.png"));
//    ui->addBtn->setIconSize(ui->addBtn->size());
//    ui->addBtn->setStyleSheet("QPushButton{background-color:transparent;}");

    if (getuid())
        ui->currentUserFaceLabel->installEventFilter(this);
//    ui->addUserFrame->installEventFilter(this);

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

    connect(ui->changeValidBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        UserInfomation user = allUserInfoMap.value(g_get_user_name());

        showChangeValidDialog(user.username);

    });

    if (getuid())
        connect(ui->changeGroupBtn, &QPushButton::clicked, this, [=](bool checked){
            Q_UNUSED(checked)
            showChangeGroupDialog();
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

        });

    //修改当前用户自动登录
    if (getuid())
        connect(autoLoginSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
            UserInfomation user = allUserInfoMap.value(g_get_user_name());


            UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);


            //        bool status = userdispatcher->get_autoLogin_status();

            bool status = this->getAutomaticLogin(user.username);


            if ((checked != status)) {
                if (checked) {
                    userdispatcher->change_user_autologin(user.username);
                } else {
                    userdispatcher->change_user_autologin("");
                }
            }

            //        bool lstStatus = userdispatcher->get_autoLogin_status();

            //        bool lstStatus = this->getAutomaticLogin(user.username);
            //        autoLoginSwitchBtn->setChecked(lstStatus);
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
}

void UserInfo::_resetListWidgetHeigh(){
    //设置其他用户控件的总高度
    ui->listWidget->setFixedHeight((allUserInfoMap.count()) * ITEMHEIGH - allUserInfoMap.count() * 6);
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
//            ui->currentUserFaceLabel->setScaledContents(true);
            ui->currentUserFaceLabel->setPixmap(iconPixmap);

            //设置用户名
            ui->userNameLabel->setText(user.username);
            //设置用户类型
            ui->userTypeLabel->setText(_accountTypeIntToString(user.accounttype));
            //设置登录状态
            autoLoginSwitchBtn->setChecked(user.autologin);
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
    faceBtn->setFixedSize(32, 32);
    faceBtn->setIcon(QIcon(user.iconfile));
    faceBtn->setIconSize(faceBtn->size());
    connect(faceBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        showChangeFaceDialog(user.username);
    });

    QLabel * nameLabel = new QLabel(widget);
    QSizePolicy nameSizePolicy = nameLabel->sizePolicy();
    nameSizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    nameSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    nameLabel->setSizePolicy(nameSizePolicy);
    nameLabel->setText(user.username);

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
    delBtn->setText(tr("Delete"));
//    delBtn->setStyleSheet("QPushButton{background: #FA6056; border-radius: 4px}");
    delBtn->hide();
    connect(delBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showDeleteUserDialog(user.username);
    });

    connect(baseWidget, &HoverWidget::enterWidget, this, [=](QString name){
        Q_UNUSED(name)

        //不允许删除最后一个管理员
        if (user.accounttype > 0 && adminnum == 1){
            delBtn->setEnabled(false);
        } else {
            delBtn->setEnabled(true);
        }
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
    baseHorLayout->addSpacing(4);

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
    dialog->setFace(user.iconfile);
    dialog->setUsername(user.username);
    connect(dialog, &DelUserDialog::removefile_send, this, [=](bool removeFile, QString userName){
        deleteUser(removeFile, userName);
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
        autoLoginSwitchBtn->setChecked(getAutomaticLogin(mUserName));
        nopwdSwitchBtn->setChecked(getNoPwdStatus());
    }
}

void UserInfo::propertyChangedSlot(QString property, QMap<QString, QVariant> propertyMap, QStringList propertyList) {
    Q_UNUSED(property);
    Q_UNUSED(propertyList);
    if (propertyMap.keys().contains("IconFile") && getuid() &&
            propertyMap.value("UserName").toString() == mUserName) {
        QString iconFile = propertyMap.value("IconFile").toString();
        QPixmap iconPixmap = QPixmap(iconFile).scaled(ui->currentUserFaceLabel->size());
        ui->currentUserFaceLabel->setPixmap(iconPixmap);
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
//    qDebug() << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__ << QDateTime::currentDateTime();
    ChangeGroupDialog * dialog = new ChangeGroupDialog();
    dialog->exec();
//    qDebug() << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__ << QDateTime::currentDateTime();
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
        dialog->setUsername(user.username);
        dialog->setCurrentAccountTypeLabel(_accountTypeIntToString(user.accounttype));
        dialog->setCurrentAccountTypeBtn(user.accounttype);
        dialog->forbidenChange(adminnum);
//        connect(dialog, SIGNAL(type_send(int,QString,bool)), this, SLOT(change_accounttype_slot(int,QString,bool)));
        connect(dialog, &ChangeTypeDialog::type_send, this, [=](int atype, QString userName){
            changeUserType(atype, userName);
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


void UserInfo::showChangeFaceDialog(QString username){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    ChangeFaceDialog * dialog = new ChangeFaceDialog;
    dialog->setFace(user.iconfile);
    dialog->setUsername(user.username);
    dialog->setAccountType(_accountTypeIntToString(user.accounttype));
//    dialog->set_face_list_status(user.iconfile);
    connect(dialog, &ChangeFaceDialog::face_file_send, [=](QString faceFile, QString userName){
        changeUserFace(faceFile, userName);
    });
    dialog->exec();
}

void UserInfo::changeUserFace(QString facefile, QString username){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);
    userdispatcher->change_user_face(facefile);
//    userdispatcher->change_user_face(QString("/home/%1/.face").arg(user.username));

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
        ChangePwdDialog * dialog = new ChangePwdDialog(user.current);
        dialog->setFace(user.iconfile);
        dialog->setUsername(user.username);
        dialog->setAccountType(_accountTypeIntToString(user.accounttype));
        if (!getuid() && user.current)
            dialog->haveCurrentPwdEdit(false);
        connect(dialog, &ChangePwdDialog::passwd_send, this, [=](QString pwd, QString userName){
            changeUserPwd(pwd, userName);
        });
        dialog->exec();

    } else {
        qDebug() << "User Info Data Error When Change User type";
    }
}


void UserInfo::changeUserPwd(QString pwd, QString username){
    //上层已做判断，这里不去判断而直接获取
    UserInfomation user = allUserInfoMap.value(username);

    UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath); //继承QObject不再删除
    QString result = userdispatcher->change_user_pwd(pwd, "");

    Q_UNUSED(result)
}


bool UserInfo::eventFilter(QObject *watched, QEvent *event){
    if (watched == ui->currentUserFaceLabel){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton ){
                if(watched == ui->currentUserFaceLabel){
                    showChangeFaceDialog(ui->userNameLabel->text());
                }
                return true;
            } else {
                return false;
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

bool UserInfo::getAutomaticLogin(QString username) {

    QString filename = "/etc/lightdm/lightdm.conf";
    autoSettings = new QSettings(filename, QSettings::IniFormat);
    autoSettings->beginGroup("SeatDefaults");

    QString autoUser = autoSettings->value("autologin-user", "").toString();

    autoSettings->endGroup();

    return autoUser == username ? true : false;
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
