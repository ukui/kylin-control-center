#ifndef CHANGEPROJECTIONNAME_H
#define CHANGEPROJECTIONNAME_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>

namespace Ui {
class ChangeProjectionName;
}

class ChangeProjectionName : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeProjectionName(QWidget *parent = nullptr);
    ~ChangeProjectionName();

private:
    Ui::ChangeProjectionName *ui;

protected:
    void paintEvent(QPaintEvent *);

Q_SIGNALS:
    void sendNewProjectionName(QString name);
};

#endif // CHANGEUSERNAME_H
