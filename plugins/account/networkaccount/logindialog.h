#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListView>
#include <QComboBox>
#include <QMouseEvent>
#include <QLineEdit>
#include <QDebug>
#include <QStackedWidget>
#include <QListWidget>
#include "area_code_lineedit.h"
#include "mcode_widget.h"

class LoginDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        LoginDialog(QWidget *parent = nullptr);

    //Pages
    const int       sizeoff = 10;
    bool            login_account_thr_number();
    bool            login_account_thr_phone();
    void            startaction_1();
    void            startaction_2();
    QLineEdit*      get_user_edit();
    QPushButton*    get_forget_btn();
    QLineEdit*      get_login_pass();
    QLabel*         get_tips_pass();
    QLabel*         get_tips_code();
    QLineEdit*      get_login_code();
    QString         get_user_name();
    QString         get_user_pass();
    QPushButton*    get_user_mcode();
    QStackedWidget* get_stack_widget();
    void            set_code(QString codenum);
    mcode_widget*   get_mcode_widget();
    QLineEdit*      get_mcode_lineedit();
public slots:
    void            set_window1();
    void            set_window2();
    void            setstyleline();
private:
    //Basic Widgets
    QWidget         *widget_number;
    QWidget         *widget_phone;

    //Suppoter Widgets
    QListWidgetItem *items[5];
    QVBoxLayout     *vlayout_p;
    QVBoxLayout     *vlayout;
    QHBoxLayout     *HBox_forget;
    QHBoxLayout     *HBox_forgett;
    QVBoxLayout     *waylayout;
    QPushButton     *account_login_btn;
    QPushButton     *message_login_btn;
    QLineEdit       *account_phone;
    QLineEdit       *account_pass;
    QLineEdit       *reg_phone;
    QPushButton     *phone_number;
    QLineEdit       *reg_name;
    QLineEdit       *reg_pass;
    QLineEdit       *valid_code;
    QPushButton     *send_msg_submit;
    QPushButton     *register_submit;
    QPushButton     *back_login_btn;
    QPushButton     *forgot_pass_btn;
    QStackedWidget  *stack_widget;
    QListWidget     *list_widget;
    QListWidget     *list_content;
    area_code_lineedit *acl;
    QLabel          *error_pass;
    QLabel          *error_code;
    QString         code;
    mcode_widget    *mcode;
    QLineEdit       *mcode_lineedit;
    QHBoxLayout     *mcode_layout;
signals:
    void code_changed();
};

#endif // LOGINDIALOG_H
