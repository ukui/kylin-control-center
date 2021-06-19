#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include "timezonemap.h"
#include <QFrame>
#include <QMap>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QAbstractItemView>
#include <QPropertyAnimation>
#include <QDialog>

class TimeZoneChooser : public QDialog
{
    Q_OBJECT
public:
    explicit TimeZoneChooser(QWidget *parent);
    void setTitle(QString title);

public slots:
    void setMarkedTimeZoneSlot(QString timezone);

Q_SIGNALS:
    void confirmed(QString zone);
    void cancelled();

protected:
    void keyRealeaseEvent(QKeyEvent* event);
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QSize getFitSize();
    void initSize();
    void paintEvent(QPaintEvent *event);

private:
    ZoneInfo* m_zoneinfo;

    QMap<QString, QString> m_zoneCompletion;
    QAbstractItemView      *m_popup;

    TimezoneMap* m_map;
    QLineEdit* m_searchInput;
    QLabel      *m_title;
    QPushButton *m_cancelBtn;
    QPushButton *m_confirmBtn;
    QPushButton *m_closeBtn;

    QLabel            *m_queryIcon;
    QLabel            *m_queryText  = nullptr;
    QPropertyAnimation *m_animation = nullptr;
    QWidget           *m_queryWid   = nullptr;
    bool              m_isSearching = false;
    QString           m_searchKeyWords;

public slots:
    void animationFinishedSlot();
};

#endif // TIMEZONECHOOSER_H
