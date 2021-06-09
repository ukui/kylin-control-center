#ifndef INPUTPWDDIALOG_H
#define INPUTPWDDIALOG_H
#include <QDialog>
#include <QVBoxLayout>
#include <QGSettings>
#include <QLabel>
#include <QLineEdit>


namespace Ui {
class InputPwdDialog;
}
class InputPwdDialog : public QDialog
{
    Q_OBJECT

public:
    InputPwdDialog(QGSettings *Keygsettiings,QWidget *parent = nullptr);
    ~InputPwdDialog();


private:
    QGSettings *mgsettings;

    QPushButton *mCancelBtn;
    QPushButton *mConfirmBtn;

    QLabel *mHintLabel;

    QLineEdit *mpwd;

    QByteArray secPwd;

    bool mfirstload;

    bool mstatus;

private:
    void setupInit();

    void initConnect();

    bool eventFilter(QObject *wcg, QEvent *event);

private slots:
    void mpwdInputSlot(const QString &pwd);


};


#endif // INPUTPWDDIALOG_H
