#include "regdialog.h"

RegDialog::RegDialog(QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(338);
    reg_phone = new area_code_lineedit;
    reg_user = new QLineEdit;
    reg_pass = new QLineEdit;
    valid_code = new QLineEdit;
    send_msg_btn = new QPushButton(tr("Get"));

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    pass_tip = new QLabel;
    user_tip = new QLabel;
    tips = new QLabel;

    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    reg_phone->setFixedSize(QSize(338,36));
    reg_user->setFixedSize(QSize(338,36));
    reg_pass->setFixedSize(QSize(338,36));
    valid_code->setFixedSize(QSize(192,36));
    send_msg_btn->setFixedSize(QSize(130,36));
    reg_phone->setFocusPolicy(Qt::StrongFocus);
    reg_phone->setFocus();

    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    reg_phone->setMaxLength(11);
    reg_pass->setPlaceholderText(tr("Your password here"));
    reg_pass->setEchoMode(QLineEdit::Password);
    reg_user->setPlaceholderText(tr("Your account here"));
    valid_code->setPlaceholderText(tr("Your code here"));
    valid_code->setMaxLength(4);
    user_tip->setText(tr("This operation is permanent"));
    user_tip->setContentsMargins(16,0,0,0);
    user_tip->setFixedHeight(16);
    pass_tip->setText(tr("At least 6 bit, include letters and digt"));
    pass_tip->setFixedHeight(16);
    pass_tip->setContentsMargins(16,0,0,0);
    user_tip->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85)");
    pass_tip->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85)");

    reg_user->setTextMargins(16,0,0,0);
    reg_pass->setTextMargins(16,0,0,0);
    valid_code->setTextMargins(16,0,0,0);
    valid_code->setStyleSheet(str);

    valid_code->setStyleSheet(str);
    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, valid_code );
    valid_code->setValidator( validator );

    reg_pass->setStyleSheet(str);
    reg_phone->setStyleSheet(str);
    reg_user->setStyleSheet(str);
    send_msg_btn->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
                                "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
                                "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");

    vlayout->setMargin(0);
    vlayout->setSpacing(8);
    vlayout->addWidget(reg_phone);
    vlayout->addWidget(reg_user);
    vlayout->addWidget(user_tip);
    vlayout->addWidget(reg_pass);
    vlayout->addWidget(pass_tip);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(valid_code);
    hlayout->addWidget(send_msg_btn);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(tips);
    vlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(vlayout);

    QRegExp regx_phn("^((13[0-9])|(14[5,7])|(15[0-3,5-9])|(17[0,3,5-8])|(18[0-9])|166|198|199|(147))\\d{8}$");
    QValidator *validator_phn = new QRegExpValidator(regx_phn, reg_phone);
    reg_phone->setValidator(validator_phn);

    QRegExp regx_acc("^[a-zA-Z0-9_-]{4,16}$");
    QValidator *validator_acc = new QRegExpValidator(regx_acc, reg_user );
    reg_user->setValidator(validator_acc);

    pass_tip->hide();
    pass_tip->setAttribute(Qt::WA_DontShowOnScreen);
    user_tip->hide();
    user_tip->setAttribute(Qt::WA_DontShowOnScreen);

    tips->hide();
    tips->setAttribute(Qt::WA_DontShowOnScreen);
    send_msg_btn->setFocusPolicy(Qt::NoFocus);
    connect(valid_code,SIGNAL(textChanged(QString)),this,SLOT(change_uppercase()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
    this->setFixedHeight(231);
}


void RegDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

void RegDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

QLabel* RegDialog::get_pass_tip() {

    return pass_tip;
}

QLabel* RegDialog::get_user_tip() {
    return user_tip;
}

QLineEdit* RegDialog::get_reg_user() {
    return reg_user;
}

QLineEdit* RegDialog::get_reg_pass() {
    return reg_pass;
}

QLineEdit* RegDialog::get_valid_code() {
    return valid_code;
}

QLineEdit* RegDialog::get_phone_user() {
    return reg_phone;
}

QPushButton* RegDialog::get_send_code() {
    return send_msg_btn;
}

void RegDialog::change_uppercase() {
    QString str = valid_code->text();
    valid_code->setText(str.toUpper());
}

QString RegDialog::get_user_mcode() {
    return valid_code->text();
}

QString RegDialog::get_user_phone() {
    return reg_phone->text();
}

QString RegDialog::get_user_account() {
    return reg_user->text();
}
QString RegDialog::get_user_passwd() {
    return reg_pass->text();
}

void RegDialog::set_clear() {
    if(!tips->isHidden()) {
        tips->hide();
    }
    reg_pass->setText("");
    reg_user->setText("");
    reg_phone->setText("");
    valid_code->setText("");
}

QLabel* RegDialog::get_tips() {
    return tips;
}
