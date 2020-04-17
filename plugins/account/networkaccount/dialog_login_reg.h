#ifndef DIALOG_LOGIN_REG_H
#define DIALOG_LOGIN_REG_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QtMath>
#include <QDebug>
#include <QStackedWidget>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "logindialog.h"
#include "regdialog.h"
#include "successdiaolog.h"
#include "passdialog.h"
#include <QTimer>
#include <QSizePolicy>
#include <libkylin-sso-client/include/libkylinssoclient.h>
#include <QShortcut>

class Dialog_login_reg : public QWidget
{
    Q_OBJECT
public:
    explicit        Dialog_login_reg(QWidget *parent = nullptr);
    QString         status = tr("Sign in Cloud");
    int             timerout_num = 60;
    int             timerout_num_reg = 60;
    int             timerout_num_log = 60;
    QString         messagebox(int code);
    void            set_client(libkylinssoclient *c);
    QPushButton     *get_login_submit();
    bool            retok = true;
    void            setclear();
    void            setshow(QWidget *w);

public slots:
    void linked_forget_btn();
    void linked_register_btn();
    void back_login_btn();
    void back_normal();
    void on_login_btn();
    void on_pass_btn();
    void on_reg_btn();
    void on_login_finished(int ret);
    void on_pass_finished(int ret);
    void on_reg_finished(int ret);
    void on_get_mcode_by_name(int ret);
    void on_get_mcode_by_phone(int ret);
    void on_timer_timeout();
    void on_send_code();
    void on_send_code_reg();
    void on_send_code_log();
    void on_timer_reg_out();
    void on_timer_log_out();
    void on_close();
    void cleanconfirm(QString str);
protected:
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
    void            mouseMoveEvent(QMouseEvent *event);
    bool            eventFilter(QObject *w,QEvent *e);

private:
    LoginDialog     *box_login;
    QLabel          *title;
    QPushButton     *login_submit;
    QPushButton     *register_account;
    QStackedWidget  *stack_box;
    QVBoxLayout     *vboxlayout;
    QHBoxLayout     *hboxlayout;
    QPoint          m_startPoint;
    QPushButton     *del_btn;
    PassDialog      *box_pass;
    RegDialog       *box_reg;
    QLabel          *user_tip;
    QLabel          *pass_tip;
    QLabel          *pass_tips;
    QLabel          *reg_tips;
    QLineEdit       *reg_pass;
    QLineEdit       *reg_user;
    QLineEdit       *login_pass;
    QLineEdit       *valid_code;
    QLineEdit       *phone_user;
    QLineEdit       *pass_user;
    QLineEdit       *pass_confirm;
    QLineEdit       *pass_pwd;
    QLineEdit       *pass_code;
    QLineEdit       *login_mcode;
    QLineEdit       *login_user;
    QLabel          *passlabel;
    QLineEdit       *login_code;
    QLabel          *codelable;
    QPushButton     *send_btn_reg;
    QPushButton     *send_btn_fgt;
    QPushButton     *send_btn_log;
    QString         *name_fgt;
    QTimer          *timer;
    QTimer          *timer_reg;
    QTimer          *timer_log;
    libkylinssoclient   *client;
    QWidget         *log_reg;
    QStackedWidget  *basewidegt;
    SuccessDiaolog  *succ;
    bool            send_is_ok = false;
    bool            send_is_ok_log = false;
    bool            send_is_ok_reg = false;

signals:
    void on_login_success();
    void on_allow_send();
    void close_occur();
};

#endif // DIALOG_LOGIN_REG_H
