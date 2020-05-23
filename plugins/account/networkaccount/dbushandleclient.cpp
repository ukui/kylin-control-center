#include "dbushandleclient.h"

DbusHandleClient::DbusHandleClient(QObject *parent) : QObject(parent)
{

}

/* DBUS接口之注册 */
int DbusHandleClient::registered(QString username, QString pwd, QString phonenumb, QString mcode,QString uuid) {
    int re = -1;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "registered");
    message<<username<<pwd<<phonenumb<<mcode<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {

    }
    emit finished_ret_reg(re);
    return re;
}

/* DBUS接口之登录 */
int DbusHandleClient::login(QString username, QString pwd,QString uuid) {
    int re = 104;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "login");
    message<<username<<pwd<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_log(re);
    return re;
}

/* DBUS接口之手机验证码 */
int DbusHandleClient::get_mcode_by_phone(QString phonenumb,QString uuid) {
    int re = 105;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "get_mcode_by_phone");
    message<<phonenumb<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_code_log(re);
    emit finished_ret_code_reg(re);
    return re;
}

/* DBUS接口之用户名验证码 */
int DbusHandleClient::get_mcode_by_username(QString username,QString uuid) {
    int re = 106;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "get_mcode_by_username");
    message<<username<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_code_pass(re);
    emit finished_ret_code_bind(re);
    return re;
}

/* DBUS接口之重置密码 */
int DbusHandleClient::user_resetpwd(QString username, QString newpwd, QString mCode,QString uuid) {
    int re = 107;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "user_resetpwd");
    message<<username<<newpwd<<mCode<<uuid;
    //qDebug()<<message;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_rest(re);
    return re;
}

/* DBUS接口之手机登录 */
int DbusHandleClient::user_phone_login(QString phone, QString mCode,QString uuid) {
    int re = 104;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "user_phone_login");
    message<<phone<<mCode<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_phonelogin(re);
    return re;
}

/* DBUS接口之登出 */
int DbusHandleClient::logout() {
    int re = 401;
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "logout");
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        //qDebug()<<"wb1010";
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_logout(re);
    return re;
}

/* DBUS接口之初始化conf文件 */
int DbusHandleClient::init_conf() {
    int re = 108;
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "init_conf");
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_conf(re);
    return re;
}

/* DBUS接口之改变conf值 */
int DbusHandleClient::change_conf_value(QString name, int flag) {
    int re = 304;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "change_conf_value");
    message<<name<<flag;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_change(re);
    return re;
}

/* DBUS接口之绑定手机号码 */
int DbusHandleClient::bindPhone(QString username, QString pwd, QString phone, QString mCode,QString uuid) {
    int re = 109;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "bindPhone");
    message<<username<<pwd<<phone<<mCode<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_ret_bind(re);
    return re;
}

/* DBUS接口之初始化OSS */
int DbusHandleClient::init_oss(QString uuid) {
    int re = 301;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "init_oss");
    message<<uuid;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_oss(re);
    return re;
}

/* DBUS接口之手动同步 */
int DbusHandleClient::manual_sync() {
    int re = 401;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "manual_sync");
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    emit finished_man(re);
    return re;
}

/* DBUS接口之检查登录，返回用户名 */
char* DbusHandleClient::check_login() {
    QString re = "";
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "check_login");
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value;
        //qDebug()<<value;
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    qDebug()<<re;
    if(!once) {
        emit finished_check(re);
    }else {
        emit finished_check_oss(re);
    }
    char*  ch = nullptr;
    QByteArray ba = re.toLatin1(); // must
    ch=ba.data();
    //qDebug()<<ch;
    return ch;
}
