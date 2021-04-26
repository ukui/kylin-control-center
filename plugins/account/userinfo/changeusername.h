#ifndef CHANGEUSERNAME_H
#define CHANGEUSERNAME_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>

namespace Ui {
class ChangeUserName;
}

class ChangeUserName : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeUserName(QWidget *parent = nullptr);
    ~ChangeUserName();

private:
    Ui::ChangeUserName *ui;

protected:
    void paintEvent(QPaintEvent *);

Q_SIGNALS:
    void sendNewName(QString name);
};

#endif // CHANGEUSERNAME_H
