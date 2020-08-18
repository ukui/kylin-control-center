#ifndef CHANGEGROUPDIALOG_H
#define CHANGEGROUPDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QListWidget>
#include <QDebug>

#include "HoverWidget/hoverwidget.h"

//struct custom_struct
//{
//    QString groupname;
//    QString passphrase;
//    QString groupid;
//    QString usergroup;
//};

namespace Ui {
class ChangeGroupDialog;
}

class ChangeGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeGroupDialog(QWidget *parent = nullptr);
    ~ChangeGroupDialog();

public:
    void initGeneralItemsStyle();
    void initItemsStyle(QListWidget * listWidget);
    void initNewGroupBtn();
    void loadGroupInfo();
    void showCreateGroupDialog();

protected:
    void paintEvent(QPaintEvent * event);

private:
    Ui::ChangeGroupDialog *ui;

    HoverWidget *addWgt;
    //QList<custom_struct *>  *value;

    void setupInit();
    void signalsBind();


};

#endif // CHANGEGROUPDIALOG_H
