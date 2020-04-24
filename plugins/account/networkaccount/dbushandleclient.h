#ifndef DBUSHANDLECLIENT_H
#define DBUSHANDLECLIENT_H

#include <QObject>
#include <QtDBus/QtDBus>

class DbusHandleClient : public QObject
{
    Q_OBJECT
public:
    explicit DbusHandleClient(QObject *parent = nullptr);
    void connectdbus(const char *signal,QObject *receiver,const char *slot);

    int registered(char * username, char * pwd, char * phonenumb, char * mcode);  // 注册接口
    int login(char * username, char * pwd);   // 登录接口
    int get_mcode_by_phone(char * phonenumb);  // 手机获取验证码
    int get_mcode_by_username(char * username);  // 用户名获取验证码
    int user_resetpwd(char * username, char * newpwd, char * mCode);  // 重置密码
    int user_phone_login(char * phone, char * mCode);   // 手机登录
    char * check_login();  //检测登录状态
    int logout();  // 注销
    int init_conf();  // 登录后，调用init_oss后再初始化conf
    int change_conf_value(char * name, int flag);  // 应用配置开关
    int bindPhone(char * username, char * pwd, char * phone, char * mCode);


    // ----------- 云相关 -----------------
    int init_oss();  // 登录后，初始化云服务
    int manual_sync();  // 手动同步
private:

signals:

};

#endif // DBUSHANDLECLIENT_H
