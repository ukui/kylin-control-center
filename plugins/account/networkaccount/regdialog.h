#ifndef REGDIALOG_H
#define REGDIALOG_H

#include <QObject>
#include <QWidget>
#include "area_code_lineedit.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>


class RegDialog : public QWidget
{
    Q_OBJECT
public:
    explicit RegDialog(QWidget *parent = nullptr);
    QLabel* get_pass_tip();
    QLabel* get_user_tip();
    QLineEdit* get_reg_user();
    QLineEdit* get_reg_pass();
    QLineEdit* get_valid_code();
    QLineEdit* get_phone_user();
    QPushButton* get_send_code();
    QString     get_user_phone();
    QString     get_user_account();
    QString     get_user_passwd();
    QString     get_user_mcode();
    void        set_code(QString codenum);
public slots:
    void change_uppercase();
    void setstyleline();
protected:

signals:
    void code_changed();
private:
    area_code_lineedit *reg_phone;
    QLineEdit *reg_user;
    QLineEdit *reg_pass;
    QLineEdit *valid_code;
    QPushButton *send_msg_btn;
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QLabel      *user_tip;
    QLabel      *pass_tip;
    QLabel      *tips;
    QString     code;
};

#endif // REGDIALOG_H
