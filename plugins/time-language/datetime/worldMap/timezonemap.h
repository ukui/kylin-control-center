#ifndef TIMEZONEMAP_H
#define TIMEZONEMAP_H

#include <QFrame>
#include <QLabel>
#include <QResizeEvent>
#include <QStringListModel>


#include "zoneinfo.h"
#include "poplist.h"
#include "toolpop.h"

QDebug& operator<<(QDebug& debug, const ZoneInfo& info);

class TimezoneMap : public QFrame
{
    Q_OBJECT
public:
    explicit TimezoneMap(QWidget* parent = nullptr);
    ~TimezoneMap();

    QString getTimezone() ;//获得选择的时区

Q_SIGNALS:
    void timezoneSelected(QString timezone);
public slots:
    void setTimezone(QString timezone);

protected:
    void mousePressEvent(QMouseEvent* event);

    void resizeEvent(QResizeEvent* event);
private:
    void initUI();
    void mark(); //标记
    void popupZoneList(QPoint pos);

    QPoint zoneInfoToPosition(ZoneInfo_ zone, int mapWidth, int mapHeight);

private:
    ZoneInfo*    m_zoninfo;
    ZoneInfo_    m_currentZone;
    ZoneinfoList m_totalZones;
    ZoneinfoList m_nearestZones;

    // 圆点
    QLabel* m_dot = nullptr;

    PopList* m_popList;
    ToolPop* m_singleList;
    PopList* m_popLists;
    QPixmap loadPixmap(const QString &path);

private slots:
    void popListActiveSlot(int index);
};

#endif // TIMEZONEMAP_H
