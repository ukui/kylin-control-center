#ifndef SHORTCUTLINE_H
#define SHORTCUTLINE_H
#include <QLineEdit>
#include <QKeyEvent>
#include <KActionCollection>
#include <KGlobalAccel>
#include <QObject>
#include "keyentry.h"

class ShortcutLine : public QLineEdit
{
    Q_OBJECT
public:
    ShortcutLine(QList<KeyEntry *> generalEntries, QList<KeyEntry *> customEntries,
                 QWidget *parent = nullptr);
    ~ShortcutLine();
protected:
    void keyPressEvent(QKeyEvent *event);   //键盘按下事件
    void keyReleaseEvent(QKeyEvent *event); //键盘松开事件
    void focusInEvent(QFocusEvent *e);      //焦点进入事件
    void focusOutEvent(QFocusEvent *e);     //焦点退出事件

private:
    QString firstKey, secondKey, thirdKey;
    bool shortCutObtainedFlag;
    QList<KeyEntry *> systemEntry;
    QList<KeyEntry *> customEntry;
    QKeySequence seq;


public:
    void initInputKeyAndText(const bool &clearText);
    bool lastKeyIsAvailable(const int &key, const int &keyCode);
    bool conflictWithGlobalShortcuts(const QKeySequence &keySequence);
    bool conflictWithStandardShortcuts(const QKeySequence &seq);
    bool conflictWithSystemShortcuts(const QKeySequence &seq);
    bool conflictWithCustomShortcuts(const QKeySequence &seq);
    QString keyToLib(QString key);
    void shortCutObtained(const bool &flag, const int &keyNum = 0); //true:success but may be conflicted, false: invalid
    QString keyToString(int keyValue);
    QKeySequence keySequence();
    void setKeySequence(QKeySequence setSeq);

Q_SIGNALS:
    void shortCutAvailable(const int &flag); //0:success, -1:shortcut invalid, -2:shortcut conflict

};


#endif // SHORTCUTLINE_H
