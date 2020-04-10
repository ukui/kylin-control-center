#ifndef APPDETAIL_H
#define APPDETAIL_H

#include <QDialog>
#include <QGSettings/QGSettings>
#include <QPainter>

#include "SwitchButton/switchbutton.h"

#define MESSAGES_KEY "messages"
#define VOICE_KEY "voice"
#define MAXIMINE_KEY "maximize"
#define NAME_KEY "name"

namespace Ui {
class AppDetail;
}

class AppDetail : public QDialog
{
    Q_OBJECT

public:
    explicit AppDetail(QString Name, QString key, QGSettings *gsettings, QWidget *parent = nullptr);
    ~AppDetail();

private:
    Ui::AppDetail *ui;
    QString appName;
    QString appKey;
    SwitchButton * enablebtn;
    QGSettings * m_gsettings;

private:
    void initUiStatus();
    void initComponent();
    void initConnect();
//    void initGSettings();

protected:
    void paintEvent(QPaintEvent *);


private slots:
    void confirmbtnSlot();
};

#endif // APPDETAIL_H
