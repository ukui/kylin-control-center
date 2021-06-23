#include <QDebug>
#include <QKeySequence>
#include <unistd.h>
#include <QApplication>
#include "shortcutline.h"

#define SNULL  "NULL"
#define SCTRL  "Ctrl"
#define SALT   "Alt"
#define SSHIFT "Shift"

int allowKey[] = { // 0-9  -/+ A-Z ,/.
                Qt::Key_0,     Qt::Key_1,     Qt::Key_2,     Qt::Key_3,
                Qt::Key_4,     Qt::Key_5,     Qt::Key_6,     Qt::Key_7,
                Qt::Key_8,     Qt::Key_9,     Qt::Key_Minus, Qt::Key_Equal,
                Qt::Key_A,     Qt::Key_B,     Qt::Key_C,     Qt::Key_D,
                Qt::Key_E,     Qt::Key_F,     Qt::Key_G,     Qt::Key_H,
                Qt::Key_I,     Qt::Key_J,     Qt::Key_K,     Qt::Key_L,
                Qt::Key_M,     Qt::Key_N,     Qt::Key_O,     Qt::Key_P,
                Qt::Key_Q,     Qt::Key_R,     Qt::Key_S,     Qt::Key_T,
                Qt::Key_U,     Qt::Key_V,     Qt::Key_W,     Qt::Key_X,
                Qt::Key_Y,     Qt::Key_Z,     Qt::Key_Comma, Qt::Key_Period


};

int numKey[] = {
    Qt::Key_0,     Qt::Key_1,     Qt::Key_2,     Qt::Key_3,
    Qt::Key_4,     Qt::Key_5,     Qt::Key_6,     Qt::Key_7,
    Qt::Key_8,     Qt::Key_9,     Qt::Key_Minus, Qt::Key_Equal,
    Qt::Key_Period
};


ShortcutLine::ShortcutLine(QList<KeyEntry *> generalEntries,
                           QList<KeyEntry *> customEntries, QWidget *parent) :
                           QLineEdit(parent),
                           systemEntry(generalEntries),
                           customEntry(customEntries)
{
    this->setReadOnly(true);
    initInputKeyAndText(true);
}


ShortcutLine::~ShortcutLine()
{

}

void ShortcutLine::initInputKeyAndText(const bool &clearText)
{
    firstKey  = SNULL;
    secondKey = SNULL;
    thirdKey  = SNULL;
    if (true == clearText) {
        this->setText("");
        shortCutObtainedFlag = false;
    }
}

void ShortcutLine::keyPressEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) {  //一直按着导致触发的事件，不再处理
        return;
    }
    int keyValue = e->key();
    int keyCode = e->nativeVirtualKey();
//    qDebug()<<"0x"<<QString().sprintf("%04X",keyValue);
//    qDebug()<<"keyCode = = = = =  ="<<keyCode<<"    "<<keyValue;

    if (firstKey == SNULL) {
        firstKey = keyToString(keyValue);
        if (keyValue == Qt::Key_Control || keyValue == Qt::Key_Alt || keyValue == Qt::Key_Shift) {
            this->setText(firstKey + QString(" + "));
        } else {                       //第一个键不是三个辅助键中的其中一个
            this->setText(firstKey);  //显示一下，增强用户交互性
            qApp->processEvents();
            usleep(200000);
            shortCutObtained(false);  //快捷键获取失败
            return;
        }
    } else if(secondKey == SNULL){
        /*第二个键是辅助键中的另外一个*/
        if ((keyValue == Qt::Key_Control || keyValue == Qt::Key_Alt || keyValue == Qt::Key_Shift) &&
                keyToString(keyValue) != firstKey) {
            secondKey = keyToString(keyValue);
            this->setText(firstKey + QString(" + ") + secondKey + QString(" + "));
        } else {  //第二个键是主键(最后一个键)
            if (lastKeyIsAvailable(keyValue, keyCode)) {   // 合法
                secondKey = keyToString(keyValue);
                shortCutObtained(true, 2);
            } else {                             //非法
                shortCutObtained(false);
                return;
            }
        }
    } else if(thirdKey == SNULL) {            //第三个键只能是主键
        if (lastKeyIsAvailable(keyValue, keyCode)) {   // 合法
            thirdKey = keyToString(keyValue);
            shortCutObtained(true, 3);
        } else {                             //非法
            shortCutObtained(false);
        }
    }

}

void ShortcutLine::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) {  //一直按着导致触发的事件
        return;
    }
    if (true == shortCutObtainedFlag) { //快捷键输入完毕
        initInputKeyAndText(false);
    } else {                           //快捷键输入放弃
        initInputKeyAndText(true);
    }
}

void ShortcutLine::focusInEvent(QFocusEvent *e)
{
    this->grabKeyboard();
    QLineEdit::focusInEvent(e);
    initInputKeyAndText(false);
}

void ShortcutLine::focusOutEvent(QFocusEvent *e)
{
    this->releaseKeyboard();
    QLineEdit::focusOutEvent(e);
}

void ShortcutLine::shortCutObtained(const bool &flag, const int &keyNum)
{
    if (true == flag && (2 == keyNum || 3 == keyNum)) {
        shortCutObtainedFlag = true;
        if (2 == keyNum) {
            seq = QKeySequence(firstKey + QString("+") + secondKey);
            this->setText(firstKey + QString(" + ") + secondKey);
        } else {
            seq = QKeySequence(firstKey + QString("+") + secondKey + QString("+") + thirdKey);
            this->setText(firstKey + QString(" + ") + secondKey + QString(" + ") + thirdKey);
        }

        if (conflictWithGlobalShortcuts(seq) || conflictWithStandardShortcuts(seq)
            || conflictWithSystemShortcuts(seq) || conflictWithCustomShortcuts(seq)) { //快捷键冲突
            Q_EMIT shortCutAvailable(-2);
        } else {
            Q_EMIT shortCutAvailable(0);
        }
    } else {  //快捷键无效
        shortCutObtainedFlag = false;
        initInputKeyAndText(true);
        Q_EMIT shortCutAvailable(-1);
    }
}

bool ShortcutLine::lastKeyIsAvailable(const int &keyValue, const int &keyCode)
{

    for (int i = 0; i < sizeof(numKey) / sizeof(int); ++i) {
        if (keyValue == numKey[i] && keyValue != keyCode) {  //数字键盘上的
            return false;
        }
    }

    for (u_int i = 0; i < sizeof(allowKey) / sizeof(int); ++i) {
        if (keyValue == allowKey[i]) {
            return true;
        }
    }
    return false;
}

QKeySequence ShortcutLine::keySequence()
{
    return this->seq;
}

bool ShortcutLine::conflictWithGlobalShortcuts(const QKeySequence &keySequence)
{
    QHash<QKeySequence, QList<KGlobalShortcutInfo> > clashing;
    for (int i = 0; i < keySequence.count(); ++i) {
        QKeySequence keys(keySequence[i]);

        if (!KGlobalAccel::isGlobalShortcutAvailable(keySequence)) {
            clashing.insert(keySequence, KGlobalAccel::getGlobalShortcutsByKey(keys));
        }
    }

    if (clashing.isEmpty()) {
        return false;
    } else {
        qDebug() << "conflict With Global Shortcuts";
    }

    return true;
}

bool ShortcutLine::conflictWithStandardShortcuts(const QKeySequence &seq)
{
    KStandardShortcut::StandardShortcut ssc = KStandardShortcut::find(seq);
    if (ssc != KStandardShortcut::AccelNone) {
        qDebug() << "conflict With Standard Shortcuts";
        return true;
    }
    return false;
}

bool ShortcutLine::conflictWithSystemShortcuts(const QKeySequence &seq)
{
    QString systemKeyStr = keyToLib(seq.toString());

    if (systemKeyStr.contains("Ctrl")) {
        systemKeyStr.replace("Ctrl", "Control");
    }
    for (KeyEntry *ckeyEntry : systemEntry) {
        if (systemKeyStr == ckeyEntry->valueStr) {
            qDebug() << "conflictWithSystemShortcuts" << seq;
            return true;
        }
    }
    return false;
}

bool ShortcutLine::conflictWithCustomShortcuts(const QKeySequence &seq)
{
    QString customKeyStr = keyToLib(seq.toString());

    for (KeyEntry *ckeyEntry : customEntry) {
        if (customKeyStr == ckeyEntry->bindingStr) {
            qDebug() << "conflictWithCustomShortcuts" << seq;
            return true;
        }
    }
    return false;
}

QString ShortcutLine::keyToLib(QString key)
{
    if (key.contains("+")) {
        QStringList keys = key.split("+");
        if (keys.count() == 2) {
            QString lower = keys.at(1);
            QString keyToLib = "<" + keys.at(0) + ">" + lower.toLower();

            return keyToLib;
        } else if (keys.count() == 3) {
            QString lower = keys.at(2);
            QString keyToLib = "<" + keys.at(0) + ">" + "<" + keys.at(1) + ">" + lower.toLower();

            return keyToLib;
        }
    }
    return key;
}


void ShortcutLine::setKeySequence(QKeySequence setSeq){
    this->seq = setSeq;
}

QString ShortcutLine::keyToString(int keyValue)
{
    QString keyValue_QT_KEY;//表示意义

    //键盘上大部分键值对应的都是其表示的ASCII码值
    keyValue_QT_KEY = QString(keyValue);

    //对于特殊意义的键值[无法用ASCII码展示]
    switch (keyValue)
    {
    case Qt::Key_Escape:
        keyValue_QT_KEY = QString("Esc");
        break;

    case Qt::Key_Tab:
        keyValue_QT_KEY = QString("Tab");
        break;

    case Qt::Key_CapsLock:
        keyValue_QT_KEY = QString("CapsLock");
        break;
    case Qt::Key_Shift:
        keyValue_QT_KEY = QString(SSHIFT);
        break;
    case Qt::Key_Control:
        keyValue_QT_KEY = QString(SCTRL);
        break;
    case Qt::Key_Alt:
        keyValue_QT_KEY = QString(SALT);
        break;
    case Qt::Key_Backspace:
        keyValue_QT_KEY = QString("Backspace");
        break;
    case Qt::Key_Meta:
        keyValue_QT_KEY = QString("Win");
        break;
    case Qt::Key_Return:
        keyValue_QT_KEY = QString("Enter(main)");
        break;
    case Qt::Key_Enter:
        keyValue_QT_KEY = QString("Enter(num)");
        break;
    case Qt::Key_Home:
        keyValue_QT_KEY = QString("Home");
        break;
    case Qt::Key_End:
        keyValue_QT_KEY = QString("End");
        break;
    case Qt::Key_PageUp:
        keyValue_QT_KEY = QString("PageUp");
        break;
    case Qt::Key_PageDown:
        keyValue_QT_KEY = QString("PageDown");
        break;
    case Qt::Key_Insert:
        keyValue_QT_KEY = QString("Insert");
        break;
    case Qt::Key_Up:
        keyValue_QT_KEY = QString::fromLocal8Bit("↑");
        break;
    case Qt::Key_Right:
        keyValue_QT_KEY = QString::fromLocal8Bit("→");
        break;
    case Qt::Key_Left:
        keyValue_QT_KEY = QString::fromLocal8Bit("←");
        break;
    case Qt::Key_Down:
        keyValue_QT_KEY = QString::fromLocal8Bit("↓");
        break;
    case Qt::Key_Delete:
        keyValue_QT_KEY = QString("Del");
        break;
    case Qt::Key_Space:
        keyValue_QT_KEY = QString("Space");
        break;
    case Qt::Key_F1:
        keyValue_QT_KEY = QString("F1");
        break;
    case Qt::Key_F2:
        keyValue_QT_KEY = QString("F2");
        break;
    case Qt::Key_F3:
        keyValue_QT_KEY = QString("F3");
        break;
    case Qt::Key_F4:
        keyValue_QT_KEY = QString("F4");
        break;
    case Qt::Key_F5:
        keyValue_QT_KEY = QString("F5");
        break;
    case Qt::Key_F6:
        keyValue_QT_KEY = QString("F6");
        break;
    case Qt::Key_F7:
        keyValue_QT_KEY = QString("F7");
        break;
    case Qt::Key_F8:
        keyValue_QT_KEY = QString("F8");
        break;
    case Qt::Key_F9:
        keyValue_QT_KEY = QString("F9");
        break;
    case Qt::Key_F10:
        keyValue_QT_KEY = QString("F10");
        break;
    case Qt::Key_F11:
        keyValue_QT_KEY = QString("F11");
        break;
    case Qt::Key_F12:
        keyValue_QT_KEY = QString("F12");
        break;
    case Qt::Key_NumLock:
        keyValue_QT_KEY = QString("NumLock");
        break;
    case Qt::Key_ScrollLock:
        keyValue_QT_KEY = QString("ScrollLock");
        break;
    case Qt::Key_Pause:
        keyValue_QT_KEY = QString("Pause");
        break;
    }

    return keyValue_QT_KEY;
}
