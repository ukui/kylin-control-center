#ifndef COMBOXFRAME_H
#define COMBOXFRAME_H

#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>

class ComboxFrame : public QFrame
{
    Q_OBJECT
public:
    ComboxFrame(QString labelStr, QWidget *parent = nullptr);
    ~ComboxFrame();

public:
    QComboBox *mCombox;

private:
    QLabel      *mTitleLabel;
    QHBoxLayout *mHLayout;
    QString     mTitleName;

signals:
};

#endif // COMBOXFRAME_H
