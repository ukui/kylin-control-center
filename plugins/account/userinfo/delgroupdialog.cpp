#include "delgroupdialog.h"
#include "ui_delgroupdialog.h"

DelGroupDialog::DelGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DelGroupDialog)
{
    ui->setupUi(this);
    ui->labelPic->setPixmap(QPixmap("://img/plugins/desktop/notice.png"));
}

DelGroupDialog::~DelGroupDialog()
{
    delete ui;
}

void DelGroupDialog::setNoticeText(QString txt)
{
    qDebug() << "setNoticeText" << txt;
}

