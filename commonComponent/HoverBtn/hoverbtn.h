#ifndef HOVERBTN_H
#define HOVERBTN_H

#include <QWidget>
#include <QEvent>
#include <QString>
#include <QFrame>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPropertyAnimation>

class HoverBtn : public QWidget
{
    Q_OBJECT
public:
    HoverBtn(QString mname, bool isHide, QWidget *parent = nullptr);
    HoverBtn(QString mname, QString detailName, QWidget *parent = nullptr);
    ~HoverBtn();

public:
    QString mName;
    QString mDetailName;
    QPushButton *mAbtBtn;

    QFrame *mInfoItem;

    QLabel *mPitIcon;
    QLabel *mPitLabel;
    QLabel *mDetailLabel;

    QHBoxLayout *mHLayout;

    QTimer *mMouseTimer;

    bool mAnimationFlag = false;
    bool mIsHide;

    int  mHideWidth;

    QPropertyAnimation *mEnterAction = nullptr;
    QPropertyAnimation *mLeaveAction = nullptr;

private:
    void initUI();
    void initAnimation();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);

Q_SIGNALS:
    void widgetClicked(QString name);
    void enterWidget(QString name);
    void leaveWidget(QString name);
};

#endif // HOVERBTN_H
