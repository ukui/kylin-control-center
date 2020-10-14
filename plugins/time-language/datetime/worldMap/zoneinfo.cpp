#include "zoneinfo.h"

#include <cmath>
#include <QDebug>
#include <locale.h>
#include <libintl.h>

const QString zoneTabFile = "/usr/share/zoneinfo/zone.tab";

const char kTimezoneDomain[] = "installer-timezones";

const char kDefaultLocale[] = "en_US.UTF-8";

const QString kcnBj = "北京";
const QString kenBj = "Asia/Beijing";

QString ZoneInfo::readRile(const QString& filepath) {
    QFile file(filepath);
    if(file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return "";
        }
        QTextStream textStream(&file);
        QString res=  textStream.readAll();
        file.close();
        return res;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return "";
    }
}

//经度纬度坐标转换【https://en.wikipedia.org/wiki/List_of_tz_database_time_zones】
double ZoneInfo::convertoPos(const QString &pos, int num) {
    if(pos.length() < 4){
        return 0.0;
    }

    QString integer = pos.left(num + 1);
    QString decimal = pos.mid(num + 1);

    double fra_1 = integer.toDouble();
    double fra_2 = decimal.toDouble();

    if(fra_1 >= 0.0) {
        return fra_1 + fra_2 / pow(10, decimal.length());
    } else {
        return fra_1 - fra_2 / pow(10, decimal.length());
    }
}

ZoneinfoList ZoneInfo::getzoneInforList() {
    ZoneinfoList list;
    const QString content(this->readRile(zoneTabFile));
    for (const QString& line : content.split('\n')) {
        if(!line.startsWith('#')) {
            const QStringList details(line.split('\t'));
            //解析数据
            if(details.length() >= 3) {
                QString coordinate = details.at(1);
                int index = coordinate.indexOf('+',3);
                if(index == -1) {
                    index = coordinate.indexOf('-', 3);
                }
//                qDebug()<<"coordinate----->"<<coordinate<<endl;
                Q_ASSERT(index > -1);

                double latitude = convertoPos(coordinate.left(index), 2);
                double longtitude = convertoPos(coordinate.mid(index), 3);
                if ("+3114" == coordinate.left(index)) {
                    latitude = convertoPos("+3992", 2);
                }

                if ("+12128" == coordinate.mid(index)) {
                    longtitude = convertoPos("+11646", 3);
                }


                ZoneInfo_ zoneinfo_ = {details.at(0), details.at(2), latitude, longtitude,0.0};
                list.append(zoneinfo_);
            }
        }
    }
    return list;
}

QString ZoneInfo::getCurrentTimzone() {
    QString timezone = this->readRile("/etc/timezone");
    return timezone.trimmed();
}

int ZoneInfo::getZoneInfoByZone(ZoneinfoList list, QString timezone) {
    int index = -1;
    for(ZoneInfo_ info : list) {
        index++;
        if (info.timezone == timezone) {
            return index;
        }
    }
    return -1;
}


QString ZoneInfo::getLocalTimezoneName(QString timezone, QString locale) {
    (void) setlocale(LC_ALL, QString(locale + ".UTF-8").toStdString().c_str());
    const QString local_name(dgettext(kTimezoneDomain,
                                      timezone.toStdString().c_str()));
    int index = local_name.lastIndexOf('/');
    if (index == -1) {
      // Some translations of locale name contains non-standard char.
      index = local_name.lastIndexOf("∕");
    }

    // Reset locale.
    (void) setlocale(LC_ALL, kDefaultLocale);


    if ("Asia/Shanghai" == timezone) {
        if (QLocale::system().name() == "zh_CN") {
            return kcnBj;
        } else {
            return kenBj;
        }
    }

    return (index > -1) ? local_name.mid(index + 1) : local_name;
}

double ZoneInfo::radians(double degrees) {
    return (degrees / 360.0) * M_PI * 2;
}

double ZoneInfo::converLongtitudeToX(double longitude, double map_width) {

    const double xdeg_offset = -6;
    double x;
    x = (map_width * (180.0 + longitude) / 360.0)
      + (map_width * xdeg_offset / 180.0);
    return x;
}

double ZoneInfo::converLatitudeToY(double latitude, double map_height) {
    double bottom_lat = -59;
    double top_lat = 81;
    double top_per, y, full_range, top_offset, map_range;
    top_per = top_lat / 180.0;
    y = 1.25 * log (tan (M_PI_4 + 0.4 * radians (latitude)));
    full_range = 4.6068250867599998;
    top_offset = full_range * top_per;
    map_range = fabs (1.25 * log (tan (M_PI_4 + 0.4 * radians (bottom_lat))) - top_offset);
    y = fabs (y - top_offset);
    y = y / map_range;
    y = y * map_height;
    return y;
}

ZoneinfoList ZoneInfo::getNearestZones(ZoneinfoList total_zones, double threshold, int x, int y, int map_width, int map_height) {

    ZoneinfoList zones;
    double minimum_distance = map_width * map_width + map_height * map_height;
    int nearest_zone_index = -1;
    for (int index = 0; index < total_zones.length(); index++) {
      const ZoneInfo_& zone = total_zones.at(index);
      const double point_x = converLongtitudeToX(zone.longtitude, map_width);
      const double point_y = converLatitudeToY(zone.latitude, map_height);
      const double dx = point_x - x;
      const double dy = point_y - y;
      const double distance = dx * dx + dy * dy;
      if (distance < minimum_distance) {
        minimum_distance = distance;
        nearest_zone_index = index;
      }
      if (distance <= threshold) {
        zones.append(zone);
      }
    }

    // Get the nearest zone.
    if (zones.isEmpty()) {
      zones.append(total_zones.at(nearest_zone_index));
    }

    return zones;
}


