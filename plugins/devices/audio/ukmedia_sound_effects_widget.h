#ifndef UKMEDIASOUNDEFFECTSWIDGET_H
#define UKMEDIASOUNDEFFECTSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QStyledItemDelegate>

class UkuiMessageBox : public QMessageBox
{
public:
    explicit UkuiMessageBox();
};

class UkmediaSoundEffectsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaSoundEffectsWidget(QWidget *parent = nullptr);
    ~UkmediaSoundEffectsWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QFrame *m_pThemeWidget;
    QFrame *m_pShutDownWidget;
    QFrame *m_pLagoutWidget;
    QString qss;
    QStyledItemDelegate *itemDelegate;
    QLabel *m_pSoundEffectLabel;
    QLabel *m_pSoundThemeLabel;
    QLabel *m_pShutdownlabel;
    QLabel *m_pLagoutLabel;
    QComboBox *m_pSoundThemeCombobox;
    QComboBox *m_pShutdownCombobox;
    QComboBox *m_pLagoutCombobox;
};

#endif // UKMEDIASOUNDEFFECTSWIDGET_H
