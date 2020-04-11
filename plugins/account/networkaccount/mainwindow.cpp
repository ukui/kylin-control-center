#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setStyleSheet("background-color:#FFFFFF;");
    ui->setupUi(this);
    network_account a;
    a.get_plugin_ui()->show();
   // connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(open_stack()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

