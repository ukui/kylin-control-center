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
#include <libkylinssoclient/libkylinssoclient.h>

class EditPassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        EditPassDialog(QWidget *parent = nullptr);
    int             timerout_num = 60;
    void            set_code(QString codenum);
    void            set_client(libkylinssoclient *c);
public slots:
    void            on_edit_submit();
    void            on_edit_submit_finished(int req);
    void            on_edit_code_finished(int req);
    void            on_timer_start();
    void            on_send_code();
    void            on_cancel_clicked();
    void            setstyleline();
protected:
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
    void            mouseMoveEvent(QMouseEvent *event);
private:
    QLabel          *title;
    QPushButton     *del_btn;
    QLineEdit       *account;
    QLineEdit       *old_pass;
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
signals:
    void code_changed();
};

#endif // EDITPASSDIALOG_H
