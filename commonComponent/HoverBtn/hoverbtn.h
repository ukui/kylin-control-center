#ifndef HOVERBTN_H
#define HOVERBTN_H

#include <QWidget>
#include <QEvent>
#include <QString>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class HoverBtn : public QFrame
{
    Q_OBJECT
public:
    HoverBtn(QString mname, QWidget *parent = nullptr);
    ~HoverBtn();

public:
    QString mName;
    QPushButton *mAbtBtn;

    QLabel *mPitIcon;
    QLabel *mPitLabel;

    QHBoxLayout *mHLayout;

private:
    void initUI();

protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);

Q_SIGNALS:
    void widgetClicked(QString name);
    void enterWidget(QString name);
    void leaveWidget(QString name);
};

#endif // HOVERBTN_H
