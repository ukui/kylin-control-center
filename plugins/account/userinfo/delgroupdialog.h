#ifndef DELGROUPDIALOG_H
#define DELGROUPDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QDebug>

namespace Ui {
class DelGroupDialog;
}

class DelGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DelGroupDialog(QWidget *parent = nullptr);
    ~DelGroupDialog();

public:
    void setNoticeText(QString txt);

private:
    Ui::DelGroupDialog *ui;
};

#endif // DELGROUPDIALOG_H
