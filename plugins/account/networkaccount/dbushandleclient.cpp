#include "dbushandleclient.h"

DbusHandleClient::DbusHandleClient(QObject *parent) : QObject(parent)
{

}

int DbusHandleClient::registered(QString username, QString pwd, QString phonenumb, QString mcode) {
    int re = -1;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "registered");
    message<<username<<pwd<<phonenumb<<mcode;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_reg(re);
    }
    else
    {

    }
    return re;
}

int DbusHandleClient::login(QString username, QString pwd) {
    int re = 104;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "login");
    message<<username<<pwd;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_log(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::get_mcode_by_phone(QString phonenumb) {
    int re = 105;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "get_mcode_by_phone");
    message<<phonenumb;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_code_log(re);
        emit finished_ret_code_reg(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::get_mcode_by_username(QString username) {
    int re = 106;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "get_mcode_by_username");
    message<<username;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_code_pass(re);
        emit finished_ret_code_bind(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::user_resetpwd(QString username, QString newpwd, QString mCode) {
    int re = 107;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "user_resetpwd");
    message<<username<<newpwd<<mCode;
    //qDebug()<<message;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_rest(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::user_phone_login(QString phone, QString mCode) {
    int re = 104;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "user_phone_login");
    message<<phone<<mCode;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_phonelogin(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

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
        emit finished_logout(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

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
        emit finished_conf(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

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
        emit finished_change(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::bindPhone(QString username, QString pwd, QString phone, QString mCode) {
    int re = 109;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "bindPhone");
    message<<username<<pwd<<phone<<mCode;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_ret_bind(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

int DbusHandleClient::init_oss() {
    int re = 301;
    //构造一个method_call消息，服务名称为：org.kylinssoclient.dbus，对象路径为：/org/kylinssoclient/path
    //接口名称为org.freedesktop.kylinssoclient.interface，method名称为check_login
    QDBusMessage message = QDBusMessage::createMethodCall("org.kylinssoclient.dbus",
                                                          "/org/kylinssoclient/path",
                                                          "org.freedesktop.kylinssoclient.interface",
                                                          "init_oss");
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        re = value.toInt();
        emit finished_oss(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

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
        emit finished_man(re);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    return re;
}

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
        if(!once) {
            once = true;
            emit finished_check_oss(value);
        }
        emit finished_check(value);
    }
    else
    {
        //qDebug() << "value method called failed!";
    }
    char*  ch = nullptr;
    QByteArray ba = re.toLatin1(); // must
    ch=ba.data();
    //qDebug()<<ch;
    return ch;
}
