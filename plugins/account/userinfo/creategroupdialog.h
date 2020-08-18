#ifndef CREATEGROUPDIALOG_H
#define CREATEGROUPDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QCheckBox>
#include <QIntValidator>
#include <QRegExpValidator>

class UserInfo;
namespace Ui {
class CreateGroupDialog;
}

class CreateGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroupDialog(QWidget *parent = nullptr);
    ~CreateGroupDialog();

public:
    void limitInput();
    void getUsersList();
    void refreshCertainBtnStatus();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::CreateGroupDialog *ui;

    void setupInit();
    void signalsBind();
};

#endif // CREATEGROUPDIALOG_H
