#ifndef DBUSHANDLECLIENT_H
#define DBUSHANDLECLIENT_H

#include <QObject>
#include <QtDBus/QtDBus>

class DbusHandleClient : public QObject
{
    Q_OBJECT
public:
    bool m_bFirstAttempt = false;
    explicit DbusHandleClient(QObject *parent = nullptr);
public slots:
    int registered(QString username, QString pwd, QString phonenumb, QString mcode,QString uuid);  // 注册接口
    int login(QString username, QString pwd,QString uuid);   // 登录接口
    int get_mcode_by_phone(QString phonenumb,QString uuid);  // 手机获取验证码
    int get_mcode_by_username(QString username,QString uuid);  // 用户名获取验证码
    int user_resetpwd(QString username, QString newpwd, QString mCode,QString uuid);  // 重置密码
    int user_phone_login(QString phone, QString mCode,QString uuid);   // 手机登录
    char * check_login();  //检测登录状态
    int logout();  // 注销
    int init_conf();  // 登录后，调用init_oss后再初始化conf
    int change_conf_value(QString name, int flag);  // 应用配置开关
    int bindPhone(QString username, QString pwd, QString phone, QString mCode,QString uuid);


    // ----------- 云相关 -----------------
    int init_oss(QString uuid);  // 登录后，初始化云服务
    int manual_sync();  // 手动同步
private:

signals:
    void login_ok();
    void reg_ok();
    void mcode_phone_ok();
    void mcode_username_ok();
    void restpwd_ok();
    void bind_ok();
    void init_ok();
    void changeret(int mode);

    void finished_ret_log(int ret);
    void finished_ret_reg(int ret);
    void finished_ret_phonelogin(int ret);
    void finished_ret_rest(int ret);
    void finished_ret_bind(int ret);
    void finished_ret_code_log(int ret);
    void finished_ret_code_reg(int ret);
    void finished_ret_code_pass(int ret);
    void finished_ret_code_bind(int ret);
    void finished_ret_check_edit(QString ret);
    void finished_ret_reset_edit(int ret);
    void finished_ret_code_edit(int ret);
    void finished_oss(int ret);
    void finished_check_oss(QString ret);
    void finished_check(QString ret);
    void finished_conf(int ret);
    void finished_man(int ret);
    void finished_change(int ret);
    void finished_logout(int ret);

};

#endif // DBUSHANDLECLIENT_H
