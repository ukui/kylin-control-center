#include "dialog_login_reg.h"

Dialog_login_reg::Dialog_login_reg(QWidget *parent) : QWidget(parent)
{
    login_submit = new QPushButton(tr("登录"));
    register_account = new QPushButton(tr("注册云账户"));
    box_login = new LoginDialog;
    box_reg = new RegDialog;
    box_pass = new PassDialog;
    log_reg = new QWidget;
    basewidegt = new QStackedWidget;

    title = new QLabel(status[0]);
    stack_box = new QStackedWidget;
    vboxlayout = new QVBoxLayout;
    hboxlayout = new QHBoxLayout;
    del_btn = new QPushButton(this);
    timer = new QTimer(this);
    timer_reg  = new QTimer(this);
    timer_log = new QTimer(this);
    succ = new SuccessDiaolog;
    QShortcut  *m_ALT_down_Accel= new QShortcut(QKeySequence(tr("Return")), login_submit);

    timer->stop();
    timer_reg->stop();
    timer_log->stop();

    this->setFixedSize(418,505);
    log_reg->setFixedSize(418,505);
    stack_box->addWidget(box_login);
    stack_box->addWidget(box_reg);
    stack_box->addWidget(box_pass);

    login_submit->setFocusPolicy(Qt::NoFocus);
    title->setFocusPolicy(Qt::NoFocus);
    register_account->setFocusPolicy(Qt::NoFocus);

    title->setText(status[0]);
    title->setMinimumSize(200,26);
    title->setMaximumSize(200,26);
    //setFocusPolicy(Qt::NoFocus);
    box_login->setContentsMargins(0,0,0,0);
    //title->setGeometry(31 + sizeoff,48 + sizeoff,160,24);
    title->setStyleSheet("font-size: 24px;color: rgba(0,0,0,0.85);font-weight:500;");

    login_submit->setMaximumSize(338,36);
    login_submit->setMinimumSize(338,36);
    login_submit->setFocusPolicy(Qt::NoFocus);
    register_account->setMaximumSize(120,36);
    register_account->setMinimumSize(120,36);
    stack_box->setFixedWidth(338);
    stack_box->setMinimumHeight(box_login->height());
    basewidegt->setFixedSize(418,505);
    basewidegt->setContentsMargins(0,0,0,0);
    basewidegt->addWidget(log_reg);
    basewidegt->addWidget(succ);
    succ->set_mode_text(2);
    basewidegt->setCurrentIndex(0);
    login_submit->setContentsMargins(0,0,0,0);
    register_account->setFocusPolicy(Qt::NoFocus);
    register_account->setContentsMargins(0,0,0,0);
    del_btn->setMaximumSize(30,30);
    del_btn->setMinimumSize(30,30);
    del_btn->setGeometry(this->width() - 46,10,30,30);
    del_btn->setFocusPolicy(Qt::NoFocus);

    login_submit->setStyleSheet("QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                                "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                                "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    register_account->setStyleSheet("QPushButton{font-size:14px;background: transparent;border-radius: 4px;} "
                                    "QPushButton:hover{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                                    "QPushButton:click{font-size:14px;background: transparent;border-radius: 4px;color:rgba(61,107,229,0.85);}");

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

    stack_box->setCurrentIndex(0);

    setStyleSheet("Dialog_login_reg{border-radius:6px;}");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlag(Qt::FramelessWindowHint);

    vboxlayout->setSpacing(0);
    vboxlayout->setContentsMargins(41,55,41,36);
    vboxlayout->addWidget(title);
    title->setMargin(0);
    vboxlayout->addSpacing(20);
    stack_box->setContentsMargins(0,0,0,0);
    vboxlayout->addWidget(stack_box);
    login_submit->setContentsMargins(0,0,0,0);
    vboxlayout->addSpacing(0);
    vboxlayout->addWidget(login_submit);
    hboxlayout->setSpacing(0);
    hboxlayout->setContentsMargins(0,10,0,0);
    hboxlayout->addWidget(register_account);
    vboxlayout->addSpacing(10);
    vboxlayout->addLayout(hboxlayout);
    vboxlayout->addSpacing(20);
    vboxlayout->setEnabled(true);
    log_reg->setLayout(vboxlayout);

    log_reg->setContentsMargins(0,0,0,0);
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(basewidegt);
    setLayout(hbox);



    send_btn_fgt = box_pass->get_send_msg_btn();
    send_btn_reg = box_reg->get_send_code();
    send_btn_log = box_login->get_user_mcode();

    box_login->get_user_edit()->setFocus();



    reg_pass = box_reg->get_reg_pass();
    login_pass = box_login->get_login_pass();
    login_code = box_login->get_login_code();
    codelable = box_login->get_tips_code();
    passlabel = box_login->get_tips_pass();
    reg_user = box_reg->get_reg_user();
    user_tip = box_reg->get_user_tip();
    pass_tip = box_reg->get_pass_tip();
    phone_user = box_reg->get_phone_user();
    valid_code = box_reg->get_valid_code();

    connect(del_btn,SIGNAL(clicked()),this,SLOT(close()));
    connect(box_login->get_forget_btn(),SIGNAL(clicked()),this,SLOT(linked_forget_btn()));
    connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
    connect(send_btn_fgt,SIGNAL(clicked()),this,SLOT(on_send_code()));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    connect(timer_reg,SIGNAL(timeout()),this,SLOT(on_timer_reg_out()));
    connect(timer_log,SIGNAL(timeout()),this,SLOT(on_timer_log_out()));
    connect(send_btn_reg,SIGNAL(clicked()),this,SLOT(on_send_code_reg()));
    connect(send_btn_log,SIGNAL(clicked()),this,SLOT(on_send_code_log()));

    login_submit->installEventFilter(this);

    stack_box->adjustSize();

    login_pass->installEventFilter(this);
    login_code->installEventFilter(this);
    reg_pass->installEventFilter(this);
    reg_user->installEventFilter(this);
    phone_user->installEventFilter(this);
    valid_code->installEventFilter(this);
    stack_box->installEventFilter(this);
    timerout_num = 60;
    del_btn->raise();
}

void Dialog_login_reg::set_client(libkylinssoclient *c) {
    client = c;
}

void Dialog_login_reg::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

QString Dialog_login_reg::messagebox(int code) {
    QString ret = "";
    switch(code) {
    case 101:ret = tr("服务器错误！");break;
    case 102:ret = tr("注册失败！");break;
    case 103:ret = tr("登录返回值异常！");break;
    case 104:ret = tr("登录失败！");break;
    case 105:ret = tr("手机获取验证码失败！");break;
    case 106:ret = tr("用户名获取验证码失败！");break;
    case 107:ret = tr("重置密码失败！");break;
    case 110:ret = tr("缺少必要信息！");break;
    case 401:ret = tr("账户或密码错误！");break;
    case 500:ret = tr("服务器错误！");break;
    case 501:ret = tr("缺少必要信息！");break;
    case 502:ret = tr("用户名已存在！");break;
    case 610:ret = tr("手机号码已被使用！");break;
    case 611:ret = tr("手机号码格式错误！");break;
    case 612:ret = tr("该手机当日接收短信次数达到上限！");break;
    case 613:ret = tr("手机号码其他错误！");break;
    case 614:ret = tr("手机验证码错误！");break;
    case 615:ret = tr("用户名不存在！");break;
    case 619:ret = tr("发送验证码异常！");break;
    case -1:ret = tr("请填写完整信息！");break;

    }
    return ret;
}

void Dialog_login_reg::on_login_btn() {
    int ret = -1;
    qDebug()<<QString(box_login->get_mcode_widget()->get_verificate_code()) <<box_login->get_mcode_lineedit()->text();
    if(box_login->get_stack_widget()->currentIndex() == 0 &&
        QString(box_login->get_mcode_widget()->get_verificate_code()) != box_login->get_mcode_lineedit()->text()) {
        box_login->set_code(tr("验证码错误"));
        passlabel->show();
        setshow(stack_box);
        return ;
    }
    if(box_login->get_user_name() != "" && box_login->get_user_pass() != "" && box_login->get_stack_widget()->currentIndex() == 0){
        char name[32],pass[32];
        qstrcpy(name,box_login->get_user_name().toStdString().c_str());
        qstrcpy(pass,box_login->get_user_pass().toStdString().c_str());
        ret = client->login(name,pass);
        if(ret  == 0) {
            connect(client,SIGNAL(finished_login(int)),this,SLOT(on_login_finished(int)));
        }
        emit on_login_process();

    } else if(box_login->get_user_name() != "" && box_login->get_login_code()->text() != "" && box_login->get_stack_widget()->currentIndex() == 1) {
        char phone[32],mcode[5];
        qstrcpy(phone,box_login->get_user_name().toStdString().c_str());
        qstrcpy(mcode,box_login->get_login_code()->text().toStdString().c_str());
        ret = client->user_phone_login(phone,mcode);
        if(ret == 0) {
            connect(client,SIGNAL(finished_user_phone_login(int)),this,SLOT(on_login_finished(int)));
        }
        emit on_login_process();
        qDebug()<<phone<<mcode;
    } else {
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();
            setshow(stack_box);
            return ;
        } else {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
            return ;
        }
    }
}

int Dialog_login_reg::on_login_finished(int ret) {
    qDebug()<< "wb1111" <<ret;
    if(ret == 0) {
        emit on_login_success();
        close();
    } else {
        if(box_login->get_stack_widget()->currentIndex() == 0) {
            box_login->set_code(messagebox(ret));
            passlabel->show();
            setshow(stack_box);
        } else {
            box_login->set_code(messagebox(ret));
            codelable->show();
            setshow(stack_box);
        }
    }
}

int Dialog_login_reg::on_reg_finished(int ret) {
    qDebug()<<ret;
    if(ret == 0) {
        box_reg->get_reg_pass()->clear();
        box_reg->get_reg_user()->clear();
        box_reg->get_phone_user()->clear();
        box_reg->get_valid_code()->clear();
        basewidegt->setCurrentIndex(1);
        succ->set_mode_text(0);
        connect(succ->back_login,SIGNAL(clicked()),this,SLOT(back_normal()));
    }

}

int Dialog_login_reg::on_pass_finished(int ret) {
    if(ret == 0) {
        box_pass->get_reg_pass()->clear();
        box_pass->get_reg_phone()->clear();
        box_pass->get_reg_pass_confirm()->clear();
        box_pass->get_valid_code()->clear();
        basewidegt->setCurrentIndex(1);
        succ->set_mode_text(1);
        connect(succ->back_login,SIGNAL(clicked()),this,SLOT(back_normal()));
    } else {
        //do something
    }
}

void Dialog_login_reg::on_timer_timeout() {
    qDebug()<<timerout_num;
    if(timerout_num > 0) {
        QString str = "重新发送 (" + QString::number(timerout_num,10) + ")";
        send_btn_fgt->setText(tr(str.toStdString().c_str()));
        timerout_num --;
    }else if(timerout_num == 0) {
        timerout_num = 60;
        send_btn_fgt->setEnabled(true);
        send_btn_fgt->setText(tr("获取绑定手机验证码"));
        timer->stop();
    }
}

void Dialog_login_reg::on_timer_log_out() {
    if(timerout_num_log > 0) {
        QString str = "重新发送 (" + QString::number(timerout_num_log,10) + ")";
        send_btn_log->setText(tr(str.toStdString().c_str()));
        timerout_num_log --;
    }else if(timerout_num_log == 0) {
        timerout_num_log = 60;
        send_btn_log->setEnabled(true);
        send_btn_log->setText(tr("发送验证码"));
        timer_log->stop();
    }
}


int Dialog_login_reg::on_get_mcode_by_phone(int ret) {
    qDebug()<<ret;
}

int Dialog_login_reg::on_get_mcode_by_name(int ret) {
    qDebug()<<ret;
}

void Dialog_login_reg::on_timer_reg_out() {
    if(timerout_num_reg > 0) {
        QString str = "重新发送 (" + QString::number(timerout_num_reg,10) + ")";
        send_btn_reg->setText(tr(str.toStdString().c_str()));
        timerout_num_reg --;
    }else if(timerout_num_reg == 0) {
        timerout_num_reg = 60;
        send_btn_reg->setEnabled(true);
        send_btn_reg->setText(tr("发送验证码"));
        timer_reg->stop();
    }
}

void Dialog_login_reg::on_pass_btn() {
    bool ok_phone = box_pass->get_user_name() == "";
    bool ok_pass = box_pass->get_user_newpass() == "";
    bool ok_confirm = box_pass->get_user_confirm() == "";
    bool ok_code = box_pass->get_user_mcode() == "";
    int ret = -1;
    if(!ok_phone && !ok_pass && !ok_code && !ok_confirm) {
        char phone[32],pass[32],confirm[32],code[5];
        qstrcpy(phone,box_pass->get_user_name().toStdString().c_str());
        qstrcpy(pass,box_pass->get_user_newpass().toStdString().c_str());
        qstrcpy(confirm,box_pass->get_user_confirm().toStdString().c_str());
        qstrcpy(code,box_pass->get_user_mcode().toStdString().c_str());
        if(qstrcmp(confirm,pass) != 0) {
            //
            return ;
        }
        ret = client->user_resetpwd(phone,pass,code);
        if(ret == 0) {
            connect(client,SIGNAL(finished_user_resetpwd(int)),this,SLOT(on_pass_finished(int)));
        }
    }
}

void Dialog_login_reg::on_send_code_reg() {
    char phone[32];
    if(box_reg->get_user_phone() != "") {
        qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        if(client->get_mcode_by_phone(phone) == 0) {
            timer_reg->start();
            timer_reg->setInterval(1000);
            send_btn_reg->setEnabled(false);
            connect(client,SIGNAL(finished_mcode_by_phone(int)),this,SLOT(on_get_mcode_by_phone(int)));
        }
    }
}

void Dialog_login_reg::on_send_code_log() {
    char phone[32];
    if(box_login->get_user_name() != "") {
        qstrcpy(phone,box_login->get_user_name().toStdString().c_str());
        if(client->get_mcode_by_phone(phone) == 0) {
            timer_log->start();
            timer_log->setInterval(1000);
            send_btn_log->setEnabled(false);
            connect(client,SIGNAL(finished_mcode_by_phone(int)),this,SLOT(on_get_mcode_by_phone(int)));
        }
    }
}

void Dialog_login_reg::on_send_code() {
    char name[32];
    int ret = -1;
    if(box_pass->get_user_name() != "") {
        qstrcpy(name,box_pass->get_user_name().toStdString().c_str());
        ret = client->get_mcode_by_username(name);
        if(ret == 0) {
            timer->start();
            timer->setInterval(1000);
            send_btn_fgt->setEnabled(false);
            connect(client,SIGNAL(finished_mcode_by_username(int)),this,SLOT(on_get_mcode_by_name(int)));
        }
    }
}

void Dialog_login_reg::on_reg_btn() {
    bool ok_mcode = box_reg->get_user_mcode() != "";
    bool ok_phone = box_reg->get_user_phone() != "";
    bool ok_account = box_reg->get_user_account() != "";
    bool ok_passwd = box_reg->get_user_passwd() != "";
    int ret = -1;
    if(ok_mcode && ok_phone && ok_account && ok_passwd) {
        char account[32],passwd[32],phone[32],mcode[32];
        qstrcpy(account,box_reg->get_user_account().toStdString().c_str());
        qstrcpy(phone,box_reg->get_user_phone().toStdString().c_str());
        qstrcpy(passwd,box_reg->get_user_passwd().toStdString().c_str());
        qstrcpy(mcode,box_reg->get_user_mcode().toStdString().c_str());
        ret = client->registered(account,passwd,phone,mcode);
        if(ret == 0) {
            connect(client,SIGNAL(finished_registered(int)),this,SLOT(on_reg_finished(int)));
        }
    }
}

void Dialog_login_reg::back_normal() {
    basewidegt->setCurrentIndex(0);

    succ->hide();
    setshow(basewidegt);

    stack_box->setCurrentIndex(0);
}

void Dialog_login_reg::back_login_btn() {
    qDebug()<<stack_box->currentIndex();
    if(stack_box->currentIndex() != 0) {
        title->setText(tr("登录云账户"));
        if(stack_box->currentIndex() == 1) {
            box_reg->get_reg_pass()->clear();
            box_reg->get_reg_user()->clear();
            box_reg->get_phone_user()->clear();
            box_reg->get_valid_code()->clear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        } else if(stack_box->currentIndex() == 2) {
            box_pass->get_reg_pass()->clear();
            box_pass->get_reg_phone()->clear();
            box_pass->get_reg_pass_confirm()->clear();
            box_pass->get_valid_code()->clear();
            disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
            connect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        }

        stack_box->setCurrentIndex(0);
        register_account->setText(tr("注册云账户"));
        login_submit->setText(tr("登录"));
        box_reg->hide();
        setshow(stack_box);
        box_pass->hide();
        setshow(stack_box);

        disconnect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
    }
}

void Dialog_login_reg::linked_forget_btn() {
    if(stack_box->currentIndex() != 2) {
        title->setText(tr("忘记密码"));
        stack_box->setCurrentIndex(2);
        login_submit->setText(tr("设置密码"));
        register_account->setText(tr("返回登录"));

        box_reg->hide();
        setshow(stack_box);
        box_login->hide();

        setshow(stack_box);

        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_pass_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}

void Dialog_login_reg::linked_register_btn() {
    if(stack_box->currentIndex() != 1) {
        title->setText(tr("注册云账户"));
        stack_box->setCurrentIndex(1);
        register_account->setText(tr("返回登录"));
        login_submit->setText(tr("立即注册"));
        box_pass->hide();
        box_login->hide();

        setshow(stack_box);
        disconnect(login_submit,SIGNAL(clicked()),this,SLOT(on_login_btn()));
        connect(login_submit,SIGNAL(clicked()),this,SLOT(on_reg_btn()));
        disconnect(register_account,SIGNAL(clicked()),this,SLOT(linked_register_btn()));
        connect(register_account,SIGNAL(clicked()),this,SLOT(back_login_btn()));
    }
}




void Dialog_login_reg::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

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

void Dialog_login_reg::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = frameGeometry().topLeft() - event->globalPos();
    }
}

void Dialog_login_reg::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() + m_startPoint);
}

bool Dialog_login_reg::eventFilter(QObject *w, QEvent *e) {
    if(w == reg_user) {
        if (e->type() == QEvent::FocusIn && user_tip->isHidden()) {

            user_tip->show();

            setshow(stack_box);
        } else if(e->type() == QEvent::FocusOut){
            user_tip->hide();
            user_tip->adjustSize();
            setshow(box_reg);
            setshow(stack_box);
        }
    }
    if(w == reg_pass) {
        if (e->type() == QEvent::FocusIn && pass_tip->isHidden()) {
            pass_tip->show();
            pass_tip->adjustSize();

            setshow(stack_box);
        } else if(e->type() == QEvent::FocusOut){
            pass_tip->hide();

            setshow(box_reg);
            setshow(stack_box);
        }
    }
    if(w == login_pass) {
        if (e->type() == QEvent::FocusIn && pass_tip->isHidden()) {
            pass_tip->show();
            pass_tip->adjustSize();


            setshow(stack_box);

        } else if(e->type() == QEvent::FocusOut){
            pass_tip->hide();

            setshow(stack_box);
        }
    }
    if(w == login_pass) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();

            setshow(stack_box);
        }
    }
    if(w == login_submit) {
        if (e->type() == QEvent::FocusIn && !passlabel->isHidden()) {
            passlabel->hide();
            setshow(stack_box);

        }
    }
    if(w == login_code) {
        if (e->type() == QEvent::FocusIn && !codelable->isHidden()) {
            codelable->hide();

            setshow(stack_box);

        }
    }
    if(w == valid_code) {
        if (e->type() == QEvent::FocusIn ) {
            //
            setshow(stack_box);
        }
    }
    if(w == phone_user) {
        if (e->type() == QEvent::FocusIn ) {
            //
            setshow(stack_box);
        }
    }
    if(w == stack_box) {
        if(e->type() == QEvent::FocusOut) {
            setshow(stack_box);
        }
    }
    return QWidget::eventFilter(w,e);
}

