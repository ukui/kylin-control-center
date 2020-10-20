#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include "timezonemap.h"
#include "CloseButton/closebutton.h"

#include <QFrame>
#include <QMap>
#include <QLabel>
#include <QPushButton>
#include <QAbstractItemView>

class TimeZoneChooser : public QFrame
{
    Q_OBJECT
public:
    explicit TimeZoneChooser();
    void setTitle();

public slots:
    void setMarkedTimeZoneSlot(QString timezone);

Q_SIGNALS:
    void confirmed(QString zone);
    void cancelled();

protected:
    void keyRealeaseEvent(QKeyEvent* event);
    bool eventFilter(QObject* obj, QEvent* event);
//    void paintEvent(QPaintEvent *);

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
    CloseButton *closeBtn;

};

#endif // TIMEZONECHOOSER_H
