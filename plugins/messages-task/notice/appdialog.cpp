#include "appdialog.h"
#include "ui_appdialog.h"

AppDialog::AppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppDialog)
{
    ui->setupUi(this);
}

AppDialog::~AppDialog()
{
    delete ui;
    ui = nullptr;
}
