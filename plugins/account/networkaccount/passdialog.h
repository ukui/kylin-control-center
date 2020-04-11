#ifndef PASSDIALOG_H
#define PASSDIALOG_H

#include <QObject>
#include <QWidget>
#include "area_code_lineedit.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class PassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit PassDialog(QWidget *parent = nullptr);
    QPushButton* get_send_msg_btn();
    QString     get_user_name();
    QString     get_user_newpass();
    QString     get_user_confirm();
    QString     get_user_mcode();
    QLineEdit*  get_reg_phone();
    QLineEdit*  get_reg_pass();
    QLineEdit*  get_reg_pass_confirm();
    QLineEdit*  get_valid_code();
    void        set_code(QString codenum);
public slots:
    void change_uppercase();
    void setstyleline();

signals:
    void code_changed();
private:
    QLineEdit *reg_phone;
    QLineEdit *reg_pass;
    QLineEdit *reg_pass_confirm;
    QLineEdit *valid_code;
    QPushButton *send_msg_btn;
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QLabel *tips;
    QString     code;
};

#endif // PASSDIALOG_H
