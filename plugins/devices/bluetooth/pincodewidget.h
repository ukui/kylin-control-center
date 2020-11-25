#ifndef PINCODEWIDGET_H
#define PINCODEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QDialog>

class PinCodeWidget : public QDialog
{
    Q_OBJECT
public:
    explicit PinCodeWidget(QString name = "", QString pin = "");
    ~PinCodeWidget();

private slots:
    void onClick_close_btn(bool);
    void onClick_accept_btn(bool);
    void onClick_refuse_btn(bool);

signals:

private:
    QVBoxLayout *main_layout;

    QPushButton *close_btn;
    QPushButton *accept_btn;
    QPushButton *refuse_btn;

    QString dev_name;
    QString PINCode;
};

#endif // PINCODEWIDGET_H
