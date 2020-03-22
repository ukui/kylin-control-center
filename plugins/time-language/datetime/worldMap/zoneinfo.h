#ifndef ZONEINFO_H
#define ZONEINFO_H

#include <QFile>
#include <QString>
#include <QList>

struct ZoneInfo_ {
    QString country;
    QString timezone;

    double latitude;
    double longtitude;
    double distance;
};
typedef QList<ZoneInfo_> ZoneinfoList;

class ZoneInfo {
public:
    QString readRile(const QString& filepath);
    ZoneinfoList getzoneInforList();
    QString getCurrentTimzone();

    QString getLocalTimezoneName(QString timzone, QString locale);
    int getZoneInfoByZone(ZoneinfoList list, QString timezone);



    double convertoPos(const QString& pos, int num);


    //gnome-control-center
    double radians(double degrees);
    double converLatitudeToY(double latitude, double map_height);
    double converLongtitudeToX(double longitude, double map_width);

    // 在宽度：mapWidth,高度mapHeight地图上获得鼠标周围小于阈值的所有时区
    ZoneinfoList getNearestZones(ZoneinfoList allZones,double threshold,
                                 int x, int y, int mapWidth, int mapHeight);



};
#endif // ZONEINFO_H
