#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include "timezonemap.h"
#include <QFrame>
#include <QMap>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QAbstractItemView>

class TimeZoneChooser : public QFrame
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
};

#endif // TIMEZONECHOOSER_H
