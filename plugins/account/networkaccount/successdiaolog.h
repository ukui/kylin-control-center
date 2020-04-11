#ifndef SUCCESSDIAOLOG_H
#define SUCCESSDIAOLOG_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>

class SuccessDiaolog : public QWidget
{
    Q_OBJECT
public:
    explicit SuccessDiaolog(QWidget *parent = nullptr);
    QPushButton *back_login;
    QLabel      *text_msg;
    QLabel      *bkg;
    void     set_mode_text(int mode);
private:
    QVBoxLayout *layout;
signals:

};

#endif // SUCCESSDIAOLOG_H
