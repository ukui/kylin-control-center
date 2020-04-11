#include "config_list_widget.h"

config_list_widget::config_list_widget(QWidget *parent) : QWidget(parent) {
    client = new libkylinssoclient;
    stacked_widget = new QStackedWidget;
    stacked_widget->resize(550,400);
    stacked_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    code = client->check_login();
    init_gui();
    if(code != "" && code !="201" && code != "203") {
        client->init_oss();
        connect(client,SIGNAL(finished_init_oss(int)),this,SLOT(finished_load(int)));
    } else {
        stacked_widget->setCurrentWidget(null_widget);
    }
}

void config_list_widget::init_gui() {
    //Allocator
    home = QDir::homePath() + "/.cache/kylinssoclient/All.conf";
    vboxlayout = new QVBoxLayout;
    tab = new QWidget(this);
    container = new QWidget(this);
    list = new item_list;
    //ld = new LoginDialog(this);
    auto_syn = new network_item(this);
    title = new QLabel(this);
    info = new QLabel(this);
    exit_page = new QPushButton(this);
    cvlayout = new QVBoxLayout;
    QVBoxLayout *VBox_tab = new QVBoxLayout;
    QHBoxLayout *HBox_tab_sub = new QHBoxLayout;
    QHBoxLayout *HBox_tab_btn_sub = new QHBoxLayout;

    //    gif = new QLabel(status);
    //    gif->setWindowFlags(Qt::FramelessWindowHint);//无边框
    //    gif->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    //    pm = new QMovie(":/new/image/gif.gif");
    edit = new ql_pushbutton_edit;


    //Configuration
    tab->setFocusPolicy(Qt::NoFocus);
    title->setText(tr("同步你的设置"));
    title->setStyleSheet("font-size:18px;color:rgba(0,0,0,0.85);font-weight:500;");


    info->setText(tr("您的云账户：%1").arg(code));
    info->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85);");
    //    status->setText(syn[0]);
    //    status->setProperty("objectName","status");  //give object a name
    //    status->setStyleSheet(qss_btn_str);
    //    status->setProperty("is_on",false);
    //    status->style()->unpolish(status);
    //    status->style()->polish(status);
    //    status->update();
    //gif->setStyleSheet("border-radius:4px;border:none;");
    exit_page->setText(tr("退出登录"));
    exit_page->setStyleSheet("QPushButton {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(0,0,0,0.85);}"
                             "QPushButton:hover {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                             "QPushButton:click{font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(65,95,196,0.85);}");
    auto_syn->set_itemname(tr("自动同步"));
    auto_syn->make_itemon();
    auto_syn->get_swbtn()->set_id(mapid.size());
    container->setFocusPolicy(Qt::NoFocus);
    edit->setStyleSheet("QPushButton{border-style: flat;"
                        "background-image:url(:/new/image/edit.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:36px;height:36px;}"
                        "QPushButton:hover{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:36px;height:36px;"
                        "border-radius:4px}"
                        "QPushButton:click{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:36px;height:36px;border-radius:4px}");
    QPoint pos;
    pos.setX(edit->mapToGlobal(QPoint(0,0)).x());
    pos.setY(edit->mapToGlobal(QPoint(0,0)).y());
    edit->installEventFilter(this);
    //Configuration
    stacked_widget->addWidget(container);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //可选;

    //Resize (Only for QPushButton)
    setContentsMargins(0,0,0,0);
    setMinimumSize(550,850);
    setMaximumSize(960,1140);
    adjustSize();
    exit_page->setMinimumSize(120,36);
    exit_page->setMaximumSize(120,36);
    exit_page->resize(120,36);
    tab->resize(200,72);
    container->resize(550,892);
    stacked_widget->adjustSize();
    list->resize(550,container->size().height());
    auto_syn->get_widget()->setFixedHeight(50);
    exit_page->setSizeIncrement(120,36);
    info->setFixedHeight(40);

    tab->setSizeIncrement(QSize(size().width(),1));
    container->setSizeIncrement(QSize(size().width(),size().height()));
    list->setSizeIncrement(QSize(size().width(),size().height()));




//    gif->setMinimumSize(120,36);
//    gif->setMaximumSize(120,36);
//    gif->resize(120,36);

    //Layout
    HBox_tab_sub->addWidget(title,0,Qt::AlignLeft);
    HBox_tab_sub->setMargin(0);
    HBox_tab_sub->setSpacing(0);

    HBox_tab_btn_sub->addWidget(info,0,Qt::AlignLeft);
    HBox_tab_btn_sub->setMargin(0);
    HBox_tab_btn_sub->addSpacing(4);
    HBox_tab_btn_sub->addWidget(edit,0,Qt::AlignLeft);
    HBox_tab_btn_sub->addWidget(exit_page,0,Qt::AlignRight);

    VBox_tab->addLayout(HBox_tab_sub);
    VBox_tab->setSpacing(0);
    VBox_tab->addSpacing(0);
    VBox_tab->setMargin(0);
    VBox_tab->addLayout(HBox_tab_btn_sub);
    tab->setLayout(VBox_tab);
    tab->setContentsMargins(0,0,0,0);



    cvlayout->addWidget(tab);
    cvlayout->setSpacing(0);
    cvlayout->setContentsMargins(1,0,1,0);
    cvlayout->addSpacing(16);
    cvlayout->addWidget(auto_syn->get_widget());
    cvlayout->addSpacing(16);
    cvlayout->addWidget(list);
    container->setLayout(cvlayout);
    container->setStyleSheet("QWidget{background-color:#ffffff;}");

    null_widget = new QWidget(this);
    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    title2 = new QLabel(this);
    logout = new QLabel(this);
    login  = new QPushButton(tr("登录/注册"));
    login->setFixedSize(120,36);
    title2->adjustSize();
    logout->adjustSize();
    null_widget->resize(550,892);
    logout->setText(tr("如需同步设置，请先登录您的云账户。"));
    logout->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85);");

    login->setStyleSheet("QPushButton {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(0,0,0,0.85);}"
                         "QPushButton:hover {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                         "QPushButton:click{font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(65,95,196,0.85);}");

    gif_step = new QLabel(login);
    gif_step->setWindowFlags(Qt::FramelessWindowHint);//无边框
    gif_step->setAttribute(Qt::WA_TranslucentBackground);//背景透明

    pm_step = new QMovie(":/new/image/sign-in.gif");

    login->setProperty("objectName","status");  //give object a name
    login->setStyleSheet(qss_btn_str);
    login->setProperty("is_on",false);

    gif_step->setStyleSheet("border-radius:4px;border:none;");
    gif_step->setFixedSize(120,36);

    title2->setText(tr("同步你的设置"));
    title2->setStyleSheet("font-size:18px;color:rgba(0,0,0,0.85);font-weight:500;");

    vlayout->addWidget(title2,0,Qt::AlignLeft);
    vlayout->setMargin(0);
    vlayout->setSpacing(16);
    hlayout->addWidget(logout,0,Qt::AlignLeft);
    hlayout->addWidget(login,0,Qt::AlignRight);
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    vlayout->addLayout(hlayout);
    vlayout->addStretch();
    null_widget->setLayout(vlayout);
    null_widget->adjustSize();
    stacked_widget->addWidget(null_widget);
    null_widget->setStyleSheet("QWidget{background-color:#ffffff;}");
    vboxlayout->addWidget(stacked_widget);
    this->setLayout(vboxlayout);
    container->adjustSize();
    list->adjustSize();


    exit_page->setFocusPolicy(Qt::NoFocus);

    //connect(auto_syn->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_auto_syn(int,int)));
    connect(login,SIGNAL(clicked()),this,SLOT(on_login()));
    //Connect
    QWidget::connect(edit,SIGNAL(clicked()),this,SLOT(neweditdialog()));
    QWidget::connect(exit_page,SIGNAL(clicked()),this,SLOT(on_login_out()));
    for(int btncnt = 0;btncnt < list->get_list().size();btncnt ++) {
        connect(list->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
    }
}

void config_list_widget::set_client(libkylinssoclient *c) {
    client = c;
}

void config_list_widget::on_login() {
    Dialog_login_reg* login_dialog = new Dialog_login_reg;
    login_dialog->set_client(client);
    login_dialog->move(this->geometry().center() - login_dialog->geometry().center());
    login_dialog->show();
    connect(login_dialog,SIGNAL(on_login_process()),this,SLOT(set_login_process()));
    connect(login_dialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
}

void config_list_widget::set_login_process() {
    logout->hide();

    if(login->property("is_on") == false) {
        gif_step->show();
        login->setProperty("is_on",true);
        pm_step->start();
        gif_step->setMovie(pm_step);
        login->update();
    }
}

void config_list_widget::open_cloud() {
    code = client->check_login();
    client->init_oss();
    connect(client,SIGNAL(finished_init_oss(int)),this,SLOT(finished_load(int)));
}

void config_list_widget::finished_load(int ret) {
    if (ret == 0) {
        client->init_conf();
        handle_conf();
        info->setText(tr("您的云账户：%1").arg(code));
        stacked_widget->setCurrentWidget(container);
    }
}

void config_list_widget::handle_conf() {
    QString enable;
    for(int i  = 0;i < mapid.size();i ++) {
        judge_item(Config_File(home).Get(mapid[i],"enable").toString(),i);
    }

}

bool config_list_widget::judge_item(QString enable,int cur) {
    if(enable == "true") {
        list->get_item(cur)->make_itemon();
    } else {
        list->get_item(cur)->make_itemoff();
    }
    return true;
}

void config_list_widget::on_switch_button(int on,int id) {
    char name[32];
    qstrcpy(name,mapid[id].toStdString().c_str());
    qDebug()<<name;
    client->change_conf_value(name,on);
}

void config_list_widget::on_auto_syn(int on,int id) {
    char name[32];
    qstrcpy(name,"Auto-Sync");
    qDebug()<<name;
    client->change_conf_value(name,on);
}

void config_list_widget::on_login_out() {
    login->setProperty("objectName","status");  //give object a name
    login->setStyleSheet(qss_btn_str);
    login->setProperty("is_on",false);
    gif_step->hide();
    pm_step->stop();
    client->logout();
    stacked_widget->setCurrentWidget(null_widget);
    logout->show();
}

void config_list_widget::neweditdialog() {
    edit_dialog = new EditPassDialog;
    edit_dialog->set_client(client);
    edit_dialog->move(this->geometry().center() - edit_dialog->geometry().center());
    edit_dialog->show();
}


void config_list_widget::setshow(QWidget *widget) {
    widget->hide();
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_DontShowOnScreen, false);
    widget->show();
    widget->adjustSize();
}

QLabel* config_list_widget::get_info() {
    return info;
}

QLabel* config_list_widget::get_title() {
    return title;
}

config_list_widget::~config_list_widget() {

}


