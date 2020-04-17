#include "passdialog.h"

PassDialog::PassDialog(QWidget *parent) : QWidget(parent)
{
    reg_phone = new QLineEdit;
    reg_pass_confirm = new QLineEdit;
    reg_pass = new QLineEdit;
    valid_code = new QLineEdit;
    send_msg_btn = new QPushButton(tr("Get the phone binding code"));

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    tips = new QLabel;
    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    reg_phone->setFixedSize(QSize(338,36));
    reg_pass->setFixedSize(QSize(338,36));
    reg_pass_confirm->setFixedSize(QSize(338,36));
    valid_code->setFixedSize(QSize(120,34));
    send_msg_btn->setFixedSize(QSize(198,34));
    reg_phone->setFocusPolicy(Qt::StrongFocus);
    reg_phone->setFocus();


    reg_phone->setPlaceholderText(tr("Your account here"));
    reg_pass->setPlaceholderText(tr("Your new password here"));
    reg_pass->setEchoMode(QLineEdit::Password);
    reg_pass_confirm->setPlaceholderText(tr("Confirm your new password"));
    reg_pass_confirm->setEchoMode(QLineEdit::Password);
    valid_code->setPlaceholderText(tr("Your code here"));
    valid_code->setMaxLength(4);

    reg_phone->setTextMargins(16,0,0,0);
    reg_pass->setTextMargins(16,0,0,0);
    reg_pass_confirm->setTextMargins(16,0,0,0);
    valid_code->setTextMargins(16,0,0,0);

    valid_code->setStyleSheet(str);
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, valid_code );
    valid_code->setValidator( validator );

    QRegExp regx_acc("^[a-zA-Z0-9_-]{4,16}$");
    QValidator *validator_acc = new QRegExpValidator(regx_acc, reg_phone );
    reg_phone->setValidator(validator_acc);

//    QRegExp regx_pas("^[a-zA-Z0-9_-]{4,16}$");
//    QValidator *validator_pas = new QRegExpValidator(regx_pas, reg_pass );
//    reg_phone->setValidator(validator_acc);

    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    reg_pass->setStyleSheet(str);
    reg_phone->setStyleSheet(str);
    reg_pass_confirm->setStyleSheet(str);
    send_msg_btn->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
                                "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
                                "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");

    vlayout->setMargin(0);
    vlayout->setSpacing(8);
    vlayout->addWidget(reg_phone);
    vlayout->addWidget(reg_pass);
    vlayout->addWidget(reg_pass_confirm);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(valid_code);
    hlayout->addWidget(send_msg_btn);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(tips);
    vlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    send_msg_btn->setFocusPolicy(Qt::NoFocus);
    setLayout(vlayout);

    setFixedHeight(212);

    tips->hide();

    connect(valid_code,SIGNAL(textChanged(QString)),this,SLOT(change_uppercase()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
}

void PassDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

void PassDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

void PassDialog::change_uppercase() {
    QString str = valid_code->text();
    valid_code->setText(str.toUpper());
}

QPushButton* PassDialog::get_send_msg_btn() {
    return send_msg_btn;
}

QString PassDialog::get_user_name() {
    return reg_phone->text();
}

QString PassDialog::get_user_mcode() {
    return valid_code->text();
}

QString PassDialog::get_user_confirm() {
    return reg_pass_confirm->text();
}

QString PassDialog::get_user_newpass() {
    return reg_pass->text();
}

QLineEdit* PassDialog::get_reg_pass() {
    return reg_pass;
}

QLineEdit* PassDialog::get_reg_phone() {
    return reg_phone;
}

QLineEdit* PassDialog::get_reg_pass_confirm() {
    return reg_pass_confirm;
}

QLineEdit* PassDialog::get_valid_code() {
    return valid_code;
}

void PassDialog::set_clear() {
    if(!tips->isHidden()) {
        tips->hide();
    }
    reg_pass->setText("");
    reg_phone->setText("");
    reg_pass_confirm->setText("");
    valid_code->setText("");
}

QLabel* PassDialog::get_tips() {
    return tips;
}
