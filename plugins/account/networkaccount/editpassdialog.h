#ifndef EDITPASSDIALOG_H
#define EDITPASSDIALOG_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QTimer>
#include <QStackedWidget>
#include "successdiaolog.h"
#include <QRegExpValidator>
#include <libkylin-sso-client/include/libkylinssoclient.h>

class EditPassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        EditPassDialog(QWidget *parent = nullptr);
    int             timerout_num = 60;
    void            set_code(QString codenum);
    void            set_client(libkylinssoclient *c);
    void            set_clear();
    QLabel*         get_tips();
    QString         messagebox(int codenum);
    void            setshow(QWidget *w);
public slots:
    void            on_edit_submit();
    void            on_edit_submit_finished(int req);
    void            on_edit_code_finished(int req);
    void            on_timer_start();
    void            on_send_code();
    void            setstyleline();
    void            on_close();
protected:
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
    void            mouseMoveEvent(QMouseEvent *event);
    bool            eventFilter(QObject *w,QEvent *e);
private:
    QLabel          *title;
    QPushButton     *del_btn;
    QLineEdit       *account;
    QLineEdit       *newpass;
    QLineEdit       *valid_code;
    QPushButton     *get_code;
    QPushButton     *cancel;
    QPushButton     *confirm;
    QVBoxLayout     *vlayout;
    QHBoxLayout     *hlayout;
    QHBoxLayout     *btnlayout;
    QPoint          m_startPoint;
    libkylinssoclient   *client;
    QTimer          *timer;
    QLabel          *tips;
    QString         code;
    QStackedWidget  *stackwidget;
    QWidget         *content;
    SuccessDiaolog  *success;
    QVBoxLayout     *vboxlayout;
signals:
    void close_occur();
    void code_changed();
    void account_changed();
};

#endif // EDITPASSDIALOG_H
