#ifndef LANGUAGEFRAME_H
#define LANGUAGEFRAME_H

//添加语言功能未完善，所以暂时只考虑中英文的情况切换。

#include <QFrame>
#include <QLabel>

class LanguageFrame : public QFrame
{
    Q_OBJECT
public:
    LanguageFrame(QString name, QWidget *parent = nullptr);
    ~LanguageFrame();
    void showSelectedIcon(bool flag);
private:
    void mousePressEvent(QMouseEvent *e);

private:
    QLabel *nameLabel         = nullptr;
    QLabel *selectedIconLabel = nullptr;

Q_SIGNALS:
    void clicked();
};


#endif // LANGUAGEFRAME_H
