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

#include <QDebug>

#include "SwitchButton/switchbutton.h"
#include "HoverWidget/hoverwidget.h"
#include "elipsemaskwidget.h"

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


#define DEFAULTFACE "/usr/share/ukui/faces/default.png"
#define ITEMHEIGH 58

UserInfo::UserInfo()
{
    ui = new Ui::UserInfo;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Userinfo");
    pluginType = ACCOUNT;
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    //构建System dbus调度对象
    sysdispatcher = new SystemDbusDispatcher;


    //获取系统全部用户信息，用户Uid大于等于1000的
    _acquireAllUsersInfo();


    initComponent();
    initAllUserStatus();
    //设置界面用户信息
    _refreshUserInfoUI();


//    pwdSignalMapper = new QSignalMapper(this);
//    faceSignalMapper = new QSignalMapper(this);
//    typeSignalMapper = new QSignalMapper(this);
//    delSignalMapper = new QSignalMapper(this);

//    faceSize = QSize(64, 64);
//    itemSize = QSize(230, 106); //?需要比btnsize大多少？否则显示不全
//    btnSize = QSize(222, 92);


//    get_all_users();
//    ui_component_init();
//    ui_status_init();

}

UserInfo::~UserInfo()
{
    delete ui;
    delete autoSettings;
}

QString UserInfo::get_plugin_name(){
    return pluginName;
}

int UserInfo::get_plugin_type(){
    return pluginType;
}

QWidget *UserInfo::get_plugin_ui(){
    return pluginWidget;
}

void UserInfo::plugin_delay_control(){

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
    QStringList objectpaths = sysdispatcher->list_cached_users();

    //初始化用户信息QMap
    allUserInfoMap.clear();
    //初始化管理员数目为0
    adminnum = 0;

    for (QString objectpath : objectpaths){
        UserInfomation user;
        user = _acquireUserInfo(objectpath);
        allUserInfoMap.insert(user.username, user);
    }

    //处理root登录

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
        if (user.username == QString(g_get_user_name())){
            user.current = true;
            user.logined = true;

            //获取当前用户免密登录属性
            QDBusInterface *tmpSysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                             "/",
                                             "com.control.center.interface",
                                             QDBusConnection::systemBus());
            //获取免密登录状态
            QDBusReply<QString> noPwdres;
            noPwdres  = tmpSysinterface ->call("getNoPwdLoginStatus");
            //const QString &tmp=noPwdres;
            if(!noPwdres.isValid()){
                qDebug()<<"获取tmpSysinterface状态不合法---->"<< noPwdres.error();
            }
            delete tmpSysinterface;

            user.noPwdLogin = noPwdres.value().contains(user.username) ? true : false;
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

void UserInfo::initComponent(){
    //样式表
//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->currentUserWidget->setStyleSheet("QWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
//    ui->autoLoginWidget->setStyleSheet("QWidget{background: #F4F4F4; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");

//    QString btnQss = QString("QPushButton{background: #FFFFFF; border-radius: 4px;}");
//    ui->changePwdBtn->setStyleSheet(btnQss);
//    ui->changeTypeBtn->setStyleSheet(btnQss);

//    ui->addUserWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

//    QString filename = "/etc/lightdm/lightdm.conf";
//    autoSettings = new QSettings(filename, QSettings::IniFormat);

    nopwdSwitchBtn = new SwitchButton(ui->nopwdLoginFrame);
    ui->nopwdHorLayout->addWidget(nopwdSwitchBtn);

    autoLoginSwitchBtn = new SwitchButton(ui->autoLoginFrame);
    ui->autoLoginHorLayout->addWidget(autoLoginSwitchBtn);


//    ui->listWidget->setStyleSheet("QListWidget{border: none}");
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setSpacing(0);

//    ElipseMaskWidget * mainElipseMaskWidget = new ElipseMaskWidget(ui->currentUserFaceLabel);
//    mainElipseMaskWidget->setBgColor("#F4F4F4");
//    mainElipseMaskWidget->setGeometry(0, 0, ui->currentUserFaceLabel->width(), ui->currentUserFaceLabel->height());

    //设置添加用户的图标
    ui->addBtn->setIcon(QIcon("://img/plugins/userinfo/add.png"));
    ui->addBtn->setIconSize(ui->addBtn->size());
    ui->addBtn->setStyleSheet("QPushButton{background-color:transparent;}");

    ui->currentUserFaceLabel->installEventFilter(this);
    ui->addUserFrame->installEventFilter(this);

    //修改当前用户密码的回调
    connect(ui->changePwdBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        UserInfomation user = allUserInfoMap.value(g_get_user_name());

        showChangePwdDialog(user.username);
    });

    //修改当前用户类型的回调
    connect(ui->changeTypeBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        UserInfomation user = allUserInfoMap.value(g_get_user_name());

        showChangeTypeDialog(user.username);
    });

    //修改当前用户免密登录
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
    connect(autoLoginSwitchBtn, &SwitchButton::checkedChanged, [=](bool checked){
        UserInfomation user = allUserInfoMap.value(g_get_user_name());


        UserDispatcher * userdispatcher  = new UserDispatcher(user.objpath);


//        bool status = userdispatcher->get_autoLogin_status();

        bool status = this->getAutomaticLogin(user.username);
        if ((checked != status)) {
            userdispatcher->change_user_autologin(checked);
        }

//        bool lstStatus = userdispatcher->get_autoLogin_status();

        bool lstStatus = this->getAutomaticLogin(user.username);
        autoLoginSwitchBtn->setChecked(lstStatus);
    });

    //成功删除用户的回调
    connect(sysdispatcher, &SystemDbusDispatcher::deleteuserdone, this, [=](QString objPath){
        deleteUserDone(objPath);
    });

    //新建用户的回调
    connect(ui->addBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showCreateUserDialog();
    });

    //成功新建用户的回调
    connect(sysdispatcher, &SystemDbusDispatcher::createuserdone, this, [=](QString objPath){
        createUserDone(objPath);
    });
}

void UserInfo::_resetListWidgetHeigh(){
    //设置其他用户控件的总高度
    ui->listWidget->setFixedHeight((allUserInfoMap.count() - 1) * ITEMHEIGH);
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
    typeBtn->setFixedSize(88, 36);
    typeBtn->setText(tr("Change type"));
//    typeBtn->setStyleSheet(btnQss);
    connect(typeBtn, &QPushButton::clicked, this, [=](bool checked){
        Q_UNUSED(checked)
        showChangeTypeDialog(user.username);
    });
    typeBtn->hide();

    QPushButton * pwdBtn = new QPushButton(widget);
    pwdBtn->setFixedSize(88, 36);
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

    baseVerLayout->addLayout(baseHorLayout);
//    baseVerLayout->addStretch();

    baseWidget->setLayout(baseVerLayout);

    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(ui->listWidget->width(), ITEMHEIGH));
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
    connect(dialog, &CreateUserDialog::newUserWillCreate, this, [=](QString uName, QString pwd, QString pin, int aType){
        createUser(uName, pwd, pin, aType);
    });
    dialog->exec();
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
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    DelUserDialog * dialog = new DelUserDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setFace(user.iconfile);
    dialog->setUsername(user.username);
    connect(dialog, &DelUserDialog::removefile_send, this, [=](bool removeFile, QString userName){
        deleteUser(removeFile, userName);
    });
    dialog->open();
}

void UserInfo::deleteUser(bool removefile, QString username){
    qDebug() << allUserInfoMap.keys() << username;

    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    sysdispatcher->delete_user(user.uid, removefile);
}

void UserInfo::delete_user_slot(bool removefile, QString username){
    UserInfomation user = (UserInfomation)(allUserInfoMap.find(username).value());

    sysdispatcher->delete_user(user.uid, removefile);
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

    QDBusReply<QString> reply =  sysinterface->call("systemRun", QVariant(cmd));


    //重新获取全部用户QMap
    _acquireAllUsersInfo();

    //更新界面显示
    _refreshUserInfoUI();

    Q_UNUSED(reply)
}

void UserInfo::showChangePwdDialog(QString username){
    if (allUserInfoMap.keys().contains(username)){
        UserInfomation user = allUserInfoMap.value(username);

        ChangePwdDialog * dialog = new ChangePwdDialog;
        dialog->setFace(user.iconfile);
        dialog->setUsername(user.username);
        dialog->setAccountType(_accountTypeIntToString(user.accounttype));
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
    if (watched == ui->currentUserFaceLabel || watched == ui->addUserFrame){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton ){
                if(watched == ui->currentUserFaceLabel){
                    showChangeFaceDialog(ui->userNameLabel->text());
                } else if (watched == ui->addUserFrame) {
                    showCreateUserDialog();
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
