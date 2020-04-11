#include "editpassdialog.h"

EditPassDialog::EditPassDialog(QWidget *parent) : QWidget(parent)
{
    //Allocate the memory
    title = new QLabel(this);
    del_btn = new QPushButton(this);
    account = new QLineEdit(this);
    old_pass = new QLineEdit(this);
    newpass = new QLineEdit(this);
    valid_code = new QLineEdit(this);
    get_code = new QPushButton(this);
    cancel = new QPushButton(this);
    confirm = new QPushButton(this);

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    btnlayout = new QHBoxLayout;
    timer = new QTimer(this);
    tips = new QLabel(this);



    //Configuration
    timer->stop();
    title->setText(tr("修改密码"));
    account->setPlaceholderText(tr("请输入用户名"));
    old_pass->setPlaceholderText(tr("当前密码"));
    newpass->setPlaceholderText(tr("新密码"));
    valid_code->setPlaceholderText(tr("输入验证码"));
    get_code->setText(tr("获取验证码"));
    cancel->setText(tr("取消"));
    confirm->setText(tr("确认"));

    old_pass->setEchoMode(QLineEdit::Password);
    newpass->setEchoMode(QLineEdit::Password);

    valid_code->setMaxLength(4);

    //Resize
    del_btn->setMaximumSize(30,30);
    del_btn->setMinimumSize(30,30);
    title->setMinimumHeight(26);
    del_btn->setFixedSize(30,30);
    account->setFixedSize(338,36);
    old_pass->setFixedSize(338,36);
    newpass->setFixedSize(338,36);
    valid_code->setFixedSize(202,34);
    get_code->setFixedSize(120,34);
    cancel->setFixedSize(120,36);
    confirm->setFixedSize(120,36);
    setFixedSize(420,446);

    del_btn->setGeometry(this->width() - 46,10,30,30);

    //Set StyleSheet
    QString liness = "QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                     "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                     "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}";
    QString labelss = "font-size: 24px;color: rgba(0,0,0,0.85)";
    QString confirmbtnss = "QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                           "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                           "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}";
    QString btnss = "QPushButton {font-size:14px;background: #F4F4F4;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                    "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                    "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";
    QString btns = "QPushButton {font-size:14px;background: #E7E7E7;color:rgba(0,0,0,0.85);border-radius: 4px;}"
                    "QPushButton:hover{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}"
                    "QPushButton:click{font-size:14px;color:rgba(61,107,229,0.85);position:relative;border-radius: 4px;}";
    del_btn->setStyleSheet("QPushButton{width:30px;height:30px;border-style: flat;"
                           "background-image:url(:/new/image/delete.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;}"
                           "QPushButton:hover{background-color:#F86457;width:30px;height:30px;"
                           "background-image: url(:new/image/delete_click.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;"
                           "border-radius:4px}"
                           "QPushButton:click{background-color:#E44C50;width:30px;height:30px;"
                           "background-image: url(:new/image/delete_click.png);"
                           "background-repeat:no-repeat;background-position :center;"
                           "border-width:0px;width:30px;height:30px;border-radius:4px}");
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    title->setStyleSheet(labelss);
    account->setStyleSheet(liness);
    old_pass->setStyleSheet(liness);
    newpass->setStyleSheet(liness);
    valid_code->setStyleSheet(liness);
    get_code->setStyleSheet(btnss);
    cancel->setStyleSheet(btns);
    confirm->setStyleSheet(confirmbtnss);
    del_btn->setFocusPolicy(Qt::NoFocus);

    //Layout
    account->setTextMargins(16,0,0,0);
    old_pass->setTextMargins(16,0,0,0);
    newpass->setTextMargins(16,0,0,0);
    valid_code->setTextMargins(16,0,0,0);
    vlayout->addWidget(title,0,Qt::AlignLeft);
    vlayout->setContentsMargins(41,55,41,36);
    vlayout->setSpacing(0);
    vlayout->addSpacing(20);
    vlayout->addWidget(account,0,Qt::AlignLeft);
    vlayout->addSpacing(8);
    vlayout->addWidget(old_pass,0,Qt::AlignLeft);
    vlayout->addSpacing(8);
    vlayout->addWidget(newpass,0,Qt::AlignLeft);
    vlayout->addSpacing(8);
    hlayout->addWidget(valid_code,0,Qt::AlignLeft);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(get_code,0,Qt::AlignRight);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(tips);
    vlayout->addStretch();
    btnlayout->addStretch();
    btnlayout->setMargin(0);
    btnlayout->setSpacing(16);
    btnlayout->addWidget(cancel,0,Qt::AlignLeft);
    btnlayout->addWidget(confirm,0,Qt::AlignRight);
    vlayout->addLayout(btnlayout,0);
    setLayout(vlayout);

    tips->hide();
    tips->setAttribute(Qt::WA_DontShowOnScreen);

    //Connect
    connect(del_btn,SIGNAL(clicked()),this,SLOT(close()));
    connect(get_code,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(confirm,SIGNAL(clicked()),this,SLOT(on_edit_submit()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_start()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));

    setStyleSheet("EditPassDialog{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlag(Qt::FramelessWindowHint);

    account->setFocus();
}

void EditPassDialog::set_client(libkylinssoclient *c) {
    client = c;
}

void EditPassDialog::on_cancel_clicked() {
    close();
}

void EditPassDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

void EditPassDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

void EditPassDialog::on_send_code() {
    char phone[32];
    if(!account->text().isNull()) {
        qstrcpy(phone,account->text().toStdString().c_str());
        if(client->get_mcode_by_username(phone) == 0) {
            timer->start();
            timer->setInterval(1000);
            get_code->setEnabled(false);
            connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_edit_code_finished(int)));
        }
    }
}

void EditPassDialog::on_edit_submit() {
    char cur_acc[32],new_pass[32],oldpass[32],mcode[5];
    bool ok_cur = account->text().isNull();
    bool ok_new = newpass->text().isNull();
    bool ok_old = old_pass->text().isNull();
    bool ok_code = valid_code->text().isNull();
    if(!ok_cur && !ok_new && !ok_old && !ok_old && !ok_code) {
        qstrcpy(cur_acc,account->text().toStdString().c_str());
        qstrcpy(new_pass,newpass->text().toStdString().c_str());
        qstrcpy(oldpass,old_pass->text().toStdString().c_str());
        qstrcpy(mcode,valid_code->text().toStdString().c_str());
        close();
    }
}

void EditPassDialog::on_timer_start() {
    if(timerout_num > 0) {
        QString str = "重新发送 (" + QString::number(timerout_num,10) + ")";
        get_code->setText(tr(str.toStdString().c_str()));
        timerout_num --;
    }else if(timerout_num == 0) {
        timerout_num = 60;
        get_code->setEnabled(true);
        get_code->setText(tr("发送验证码"));
        timer->stop();
    }
}

void EditPassDialog::on_edit_submit_finished(int req) {
    qDebug()<<req;
}

void EditPassDialog::on_edit_code_finished(int req) {
    qDebug()<<req;
}


void EditPassDialog::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    QColor m_defaultBackgroundColor = qRgb(0, 0, 0);
    QPainterPath path1;
    path1.setFillRule(Qt::WindingFill);
    path1.addRoundedRect(10, 10, this->width() - 20, this->height() - 20, 6, 6);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path1, QBrush(QColor(m_defaultBackgroundColor.red(),
                                          m_defaultBackgroundColor.green(),
                                          m_defaultBackgroundColor.blue())));

    QColor color(0, 0, 0, 15);
    for (int i = 0; i < 6; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(10 - i, 10 - i, this->width() - (10 - i) * 2, this->height() - (10 - i) * 2, 6, 6);
        color.setAlpha(120 - qSqrt(i) * 50);
        painter.setPen(color);
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setX(10);
    rect.setY(10);
    rect.setWidth(rect.width() - 10);
    rect.setHeight(rect.height() - 10);
    // rect: 绘制区域  15：圆角弧度
    painter.drawRoundedRect(rect, 6, 6);
}

void EditPassDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

void EditPassDialog::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}
