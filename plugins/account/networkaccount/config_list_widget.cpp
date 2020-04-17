#include "config_list_widget.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

config_list_widget::config_list_widget(QWidget *parent) : QWidget(parent) {
    client = new libkylinssoclient;

    thread = new QThread;
    client->moveToThread(thread);

    stacked_widget = new QStackedWidget;
    stacked_widget->resize(550,400);
    stacked_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    code = client->check_login();
    init_gui();
    if(code != "" && code !="201" && code != "203") {
        if(client->init_oss() == 0) {
            qDebug()<<"init oss is 0";
        }
        else {
            client->logout();
            stacked_widget->setCurrentWidget(null_widget);
        }
    } else {

        stacked_widget->setCurrentWidget(null_widget);
    }
    connect(client,SIGNAL(finished_init_oss(int)),this,SLOT(finished_load(int)));
    connect(client,SIGNAL(backcall_start_download_signal()),this,SLOT(download_files()));
    connect(client,SIGNAL(backcall_end_download_signal()),this,SLOT(download_over()));
    connect(client,SIGNAL(backcall_start_push_signal()),this,SLOT(push_files()));
    connect(client,SIGNAL(backcall_end_push_signal()),this,SLOT(push_over()));
    thread->start();
}

void config_list_widget::init_gui() {
    //Allocator
    home = QDir::homePath() + "/.cache/kylinssoclient/All.conf";
    vboxlayout = new QVBoxLayout;
    tab = new QWidget(this);
    container = new QWidget(this);
    namewidget = new QWidget(this);
    list = new item_list;
    //ld = new LoginDialog(this);
    auto_syn = new network_item(this);
    title = new QLabel(this);
    info = new QLabel(namewidget);
    exit_page = new QPushButton(this);
    cvlayout = new QVBoxLayout;
    login_dialog = new Dialog_login_reg;
    edit_dialog = new EditPassDialog;
    hbox = new QHBoxLayout;

    edit_dialog->hide();
    login_dialog->hide();
    edit_dialog->set_client(client);
    login_dialog->set_client(client);
    QVBoxLayout *VBox_tab = new QVBoxLayout;
    QHBoxLayout *HBox_tab_sub = new QHBoxLayout;
    QHBoxLayout *HBox_tab_btn_sub = new QHBoxLayout;

    //    gif = new QLabel(status);
    //    gif->setWindowFlags(Qt::FramelessWindowHint);//无边框
    //    gif->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    //    pm = new QMovie(":/new/image/gif.gif");
    edit = new ql_pushbutton_edit(namewidget);


    //Configuration
    tab->setFocusPolicy(Qt::NoFocus);
    title->setText(tr("Sync your settings"));
    title->setStyleSheet("font-size:18px;color:rgba(0,0,0,0.85);font-weight:500;");


    info->setText(tr("Your account:%1").arg(code));
    info->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85);");
    //    status->setText(syn[0]);
    //    status->setProperty("objectName","status");  //give object a name
    //    status->setStyleSheet(qss_btn_str);
    //    status->setProperty("is_on",false);
    //    status->style()->unpolish(status);
    //    status->style()->polish(status);
    //    status->update();
    //gif->setStyleSheet("border-radius:4px;border:none;");
    exit_page->setText(tr("Exit"));

    auto_syn->set_itemname(tr("Auto sync"));
    auto_syn->make_itemon();
    auto_syn->get_swbtn()->set_id(mapid.size());
    container->setFocusPolicy(Qt::NoFocus);
    edit->setStyleSheet("QPushButton{border-style: flat;"
                        "background-image:url(:/new/image/edit.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;}"
                        "QPushButton:hover{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;"
                        "border-radius:4px}"
                        "QPushButton:click{"
                        "background-image: url(:new/image/edit_hover.png);"
                        "background-repeat:no-repeat;background-position :center;"
                        "border-width:0px;width:34px;height:34px;border-radius:4px}");
    edit->installEventFilter(this);
    //Configuration
    stacked_widget->addWidget(container);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //可选;

    //Resize (Only for QPushButton)
    setContentsMargins(0,0,0,0);
    setMinimumWidth(550);
    setMaximumWidth(960);
    exit_page->setMinimumSize(120,36);
    exit_page->setMaximumSize(120,36);
    exit_page->resize(120,36);
    tab->resize(200,72);
    stacked_widget->adjustSize();
    list->resize(550,container->size().height());
    auto_syn->get_widget()->setFixedHeight(50);
    exit_page->setSizeIncrement(120,36);
    exit_page->setProperty("objectName","status");
    exit_page->setProperty("is_on",false);
    info->setFixedHeight(40);

    tab->setSizeIncrement(QSize(size().width(),1));
    container->setSizeIncrement(QSize(size().width(),size().height()));
    list->setSizeIncrement(QSize(size().width(),size().height()));

    namewidget->setFixedHeight(36);


//    gif->setMinimumSize(120,36);
//    gif->setMaximumSize(120,36);
//    gif->resize(120,36);

    //Layout
    HBox_tab_sub->addWidget(title,0,Qt::AlignLeft);
    HBox_tab_sub->setMargin(0);
    HBox_tab_sub->setSpacing(0);

    hbox->addWidget(info);
    hbox->setMargin(0);
    hbox->setSpacing(4);
    hbox->addWidget(edit);
    hbox->setAlignment(Qt::AlignBottom);
    namewidget->setLayout(hbox);
    namewidget->adjustSize();
    HBox_tab_btn_sub->addWidget(namewidget,0,Qt::AlignLeft);
    HBox_tab_btn_sub->setMargin(0);
    HBox_tab_btn_sub->addWidget(exit_page,0,Qt::AlignRight);

    VBox_tab->addLayout(HBox_tab_sub);  //need fixing
    VBox_tab->setSpacing(16);
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
    //tag container->setStyleSheet("QWidget{background-color:#ffffff;}");

    null_widget = new QWidget(this);
    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    title2 = new QLabel(this);
    logout = new QLabel(this);
    login  = new QPushButton(tr("Sign in/Sign up"));
    login->setFixedSize(120,36);
    title2->adjustSize();
    logout->adjustSize();
    null_widget->resize(550,892);
    logout->setText(tr("You must sign in when you attempt to sync your settings."));
    logout->setStyleSheet("font-size:14px;color:rgba(0,0,0,0.85);");

    login->setStyleSheet("QPushButton {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(0,0,0,0.85);}"
                         "QPushButton:hover {font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(61,107,229,0.85);}"
                         "QPushButton:click{font-size:14px;background-color:#E7E7E7;border:none;border-radius: 4px;color:rgba(65,95,196,0.85);}");

    gif_step = new QLabel(login);
    gif_step->setWindowFlags(Qt::FramelessWindowHint);//无边框
    gif_step->setAttribute(Qt::WA_TranslucentBackground);//背景透明

    pm = new QMovie(":/new/image/step-up.gif");

    pm_step = new QMovie(":/new/image/sign-in.gif");

    exit_page->setStyleSheet(qss_btn_str);

    login->setProperty("objectName","login");  //give object a name
    login->setStyleSheet("QPushButton#login[is_on=false]{font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:#FFFFFF;}"
                         "QPushButton#login[is_on=true] {border-radius:4px;background-color: #3D6BE5}"
                         "QPushButton#login[is_on=false]:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:#FFFFFF;}"
                         "QPushButton#login[is_on=false]:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:#FFFFFF;}");
    login->setProperty("is_on",false);

    gif_step->setStyleSheet("border-radius:4px;border:none;");
    gif_step->resize(120,36);

    title2->setText(tr("Sync your settings"));
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
    // tag null_widget->setStyleSheet("QWidget{background-color:#ffffff;}");
    vboxlayout->addWidget(stacked_widget);
    this->setLayout(vboxlayout);
    list->adjustSize();
    container->adjustSize();

    //tag this->setStyleSheet("config_list_widget{background-color:#ffffff;}");
    exit_page->setFocusPolicy(Qt::NoFocus);

    connect(auto_syn->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_auto_syn(int,int)));
    connect(login,SIGNAL(clicked()),this,SLOT(on_login()));
    //Connect
    connect(edit,SIGNAL(clicked()),this,SLOT(neweditdialog()));
    connect(exit_page,SIGNAL(clicked()),this,SLOT(on_login_out()));
    connect(this,SIGNAL(on_login_process()),this,SLOT(set_login_process()));
    connect(edit_dialog,SIGNAL(account_changed()),this,SLOT(on_login_out()));
    connect(edit_dialog,SIGNAL(close_occur()),this,SLOT(edit_lost()));
    connect(login_dialog,SIGNAL(close_occur()),this,SLOT(login_lost()));
    connect(login_dialog,SIGNAL(on_login_success()),this,SLOT(open_cloud()));
    adjustSize();
    for(int btncnt = 0;btncnt < list->get_list().size();btncnt ++) {
        connect(list->get_item(btncnt)->get_swbtn(),SIGNAL(status(int,int)),this,SLOT(on_switch_button(int,int)));
    }
}

void config_list_widget::on_login() {
    if(!is_open) {
        login_dialog = new Dialog_login_reg;
        login_dialog->setclear();
        login_dialog->set_client(client);
        login_dialog->show();
        login_dialog->raise();
        is_open = true;
    } else {
        login_dialog->setclear();
        login_dialog->show();
        login_dialog->raise();
    }
}

void config_list_widget::set_login_process() {
    if(login->property("is_on") == false) {
        login->setText("");
        login->setProperty("is_on",true);

        gif_step->setMovie(pm_step);
        pm_step->start();
        gif_step->show();
        //login->update();
    }
    logout->hide();
}

void config_list_widget::open_cloud() {
    code = client->check_login();
    if(code != "" && code != "201" && code != "203") {
        if(client->init_oss() == 0) {
            emit on_login_process();
        }else {
            client->logout();
            stacked_widget->setCurrentWidget(null_widget);
        }
    }

}

void config_list_widget::finished_load(int ret) {
    if (ret == 0) {
        qDebug()<<"wb111 conf";
        if(client->init_conf() == 0) {
            qDebug()<<"wb222 conf";
            handle_conf();
            qDebug()<<"wb333 conf";
            info->setText(tr("Your account：%1").arg(code));
            stacked_widget->setCurrentWidget(container);
            qDebug()<<"wb444 conf";
            if(client->manual_sync() == 0) {
                qDebug()<<"0 manual sync succes";
            }
        }
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
    qDebug()<<name<<on;
    client->change_conf_value(name,on);
}

void config_list_widget::on_auto_syn(int on,int id) {
    char name[32];
    qstrcpy(name,"Auto-sync");
    qDebug()<<name;
    client->change_conf_value(name,on);
}

void config_list_widget::on_login_out() {
    if(client->logout() == 0) {
        login->setProperty("is_on",false);
        login->setText(tr("Sign in/Sign up"));
        login->style()->unpolish(login);
        login->style()->polish(login);
        login_dialog->setclear();
        gif_step->hide();
        stacked_widget->setCurrentWidget(null_widget);
        logout->show();
    }
}

void config_list_widget::neweditdialog() {
    if(!is_open_edit) {
        edit_dialog = new EditPassDialog;
        edit_dialog->set_clear();
        edit_dialog->set_client(client);
        edit_dialog->show();
        edit_dialog->raise();
        is_open_edit = true;
    } else {
        edit_dialog->set_clear();
        edit_dialog->show();
        edit_dialog->raise();
    }
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

void config_list_widget::edit_lost() {
    is_open_edit = false;
}

void config_list_widget::login_lost() {
    is_open = false;
}

void config_list_widget::download_files() {
    qDebug()<<"downloading file";
    exit_page->setText("");
    exit_page->setProperty("is_on",true);

    gif_step->setMovie(pm);
    pm->start();
    gif_step->show();
}

void config_list_widget::push_files() {
    qDebug()<<"pushing file";
    exit_page->setText("");
    exit_page->setProperty("is_on",true);

    gif_step->setMovie(pm);
    pm->start();
    gif_step->show();
}

void config_list_widget::download_over() {
    qDebug()<<"download over";
    exit_page->setProperty("is_on",false);
    exit_page->setText(tr("Exit"));
    exit_page->style()->unpolish(exit_page);
    exit_page->style()->polish(exit_page);
    gif_step->hide();
}

void config_list_widget::push_over() {
    qDebug()<<"push over";
    exit_page->setProperty("is_on",false);
    exit_page->setText(tr("Exit"));
    exit_page->style()->unpolish(exit_page);
    exit_page->style()->polish(exit_page);
    gif_step->hide();
}

config_list_widget::~config_list_widget() {

}


