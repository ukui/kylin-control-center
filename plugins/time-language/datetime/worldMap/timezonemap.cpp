#include "timezonemap.h"

#include <QApplication>
#include <QPainter>
#include <QIcon>
#include <QApplication>
#include <QImageReader>
#include <QDebug>

const QString timezoneMapFile =":/images/map.svg";
const QString dotFile = ":/images/indicator.png";


QPixmap TimezoneMap::loadPixmap(const QString &path)
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

TimezoneMap::TimezoneMap(QWidget* parent) : QFrame (parent),
    m_zoninfo(new ZoneInfo),m_currentZone(),m_nearestZones()
{
    m_totalZones = m_zoninfo->getzoneInforList();

    this->initUI();
}


TimezoneMap::~TimezoneMap(){
    if(m_popLists) {
        delete  m_popLists;
        m_popLists = nullptr;
    }
}


QString TimezoneMap::getTimezone() {
    return m_currentZone.timezone;
}

void TimezoneMap::initUI() {
    QLabel* backgroundLabel = new QLabel(this);
    backgroundLabel->setObjectName("background_label");

    QPixmap timezonePixmap(timezoneMapFile);
    //QPixmap timezonePixmap = loadPixmap(timezoneMapFile);
//    qDebug()<<"timezonePixmap--------->"<<timezonePixmap<<endl;
    Q_ASSERT(!timezonePixmap.isNull());
    backgroundLabel->setPixmap(timezonePixmap);

    m_dot = new QLabel(this->parentWidget());
    QPixmap dotPixmap(dotFile);
    Q_ASSERT(!dotPixmap.isNull());
    m_dot->setPixmap(dotPixmap);
    m_dot->setFixedSize(dotPixmap.size());
    m_dot->hide();


    m_singleList = new ToolPop(this->parentWidget());
    m_singleList->setFixedHeight(30);
    m_singleList->setMinimumWidth(60);
    m_singleList->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_singleList->hide();

    m_popLists = new PopList();
    m_popLists->hide();

    this->setContentsMargins(0,0,0,0);


    connect(m_popLists,&PopList::listHide,
            m_dot, &QLabel::hide);
    connect(m_popLists,&PopList::listAactive,
            this, &TimezoneMap::popListActiveSlot);

}


QPoint TimezoneMap::zoneInfoToPosition(ZoneInfo_ zone, int mapWidth, int mapHeight) {
    int x = int(m_zoninfo->converLongtitudeToX(zone.longtitude, mapWidth));
    int y = int(m_zoninfo->converLatitudeToY(zone.latitude, mapHeight));
    return QPoint(x, y);
}

void TimezoneMap::mark() {
    m_dot->hide();
    m_singleList->hide();
    m_popLists->hide();

    int mapWidth = this->width();
    int mapHeight = this->height();

    Q_ASSERT(!m_nearestZones.isEmpty());

    QString locale = QLocale::system().name();
    if (!m_nearestZones.isEmpty()) {
        m_singleList->setText(m_zoninfo->getLocalTimezoneName(m_currentZone.timezone, locale));
        m_singleList->adjustSize();

        QPoint zonePos = this->zoneInfoToPosition(m_currentZone,mapWidth,mapHeight);
        int zoneDy = zonePos.y() - m_dot->height() / 2;

        QPoint zoneSingleRlativePos(zonePos.x(), zoneDy);
        QPoint zoneSinglePos(this->mapToParent(zoneSingleRlativePos));
        m_singleList->popupSlot(zoneSinglePos);

        QPoint dotRelativePos(zonePos.x() - m_dot->width(),
                              zonePos.y() - m_dot->height());

        QPoint dotPos(this->mapToParent(dotRelativePos));
        m_dot->move(dotPos);
        m_dot->show();
    }
}

void TimezoneMap::popupZoneList(QPoint pos) {
    m_dot->hide();
    m_singleList->hide();
    m_popLists->hide();


    QString locale = QLocale::system().name();
    QStringList zoneNames;

    for (ZoneInfo_ zone : m_nearestZones) {
        zoneNames.append(m_zoninfo->getLocalTimezoneName(zone.timezone, locale));
    }

    m_popLists->setStringList(zoneNames);
    // y坐标
    int dy = pos.y() - 5;
    QPoint popupListPos = this->mapToGlobal(QPoint(pos.x(), dy));
    m_popLists->showPopLists(popupListPos);

    QPoint dotRelativePos(pos.x() - m_dot->width(),
                          pos.y() - m_dot->height());
    QPoint dot_pos(this->mapToParent(dotRelativePos));

    m_dot->move(dot_pos);
    m_dot->show();
}

void TimezoneMap::popListActiveSlot(int index) {
    m_popLists->hide();
    m_dot->hide();

    Q_ASSERT(index < m_nearestZones.length());
    if (index < m_nearestZones.length()) {
        m_currentZone = m_nearestZones.at(index);
        this->mark();
        emit this->timezoneSelected(m_currentZone.timezone);
    }
}

void TimezoneMap::setTimezone(QString timezone) {
    timezone = (timezone == "Asia/Beijing" ? "Asia/Shanghai" : timezone);
    m_nearestZones.clear();
    int index = m_zoninfo->getZoneInfoByZone(m_totalZones, timezone);
    if (index > -1) {
        m_currentZone = m_totalZones.at(index);
        m_nearestZones.append(m_currentZone);
        this->mark();
    } else {
        qWarning() <<" failed to found "<<timezone<<endl;
    }
}

void TimezoneMap::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_nearestZones = m_zoninfo->getNearestZones(m_totalZones,100.0,
                                                    event->x(), event->y(),
                                                    this->width(), this->height());
        if (m_nearestZones.length() == 1){
            m_currentZone = m_nearestZones.first();
            this->mark();
            emit this->timezoneSelected(m_currentZone.timezone);
        } else {
            this->popupZoneList(event->pos());
        }
    } else {
        QWidget::mousePressEvent(event);
    }
}

void TimezoneMap::resizeEvent(QResizeEvent *event) {
    if (m_popLists->isVisible()) {
        m_dot->hide();
        m_popLists->hide();
    }

    QLabel *background_label = findChild<QLabel*>("background_label");
    if (background_label) {
        QPixmap timezone_pixmap = loadPixmap(timezoneMapFile);
        background_label->setPixmap(timezone_pixmap.scaled(event->size() * devicePixelRatioF(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    QWidget::resizeEvent(event);
}

QDebug& operator<<(QDebug& debug, const ZoneInfo_& info) {
  debug << "ZoneInfo {"
        << "cc:" << info.country
        << "tz:" << info.timezone
        << "lat:" << info.latitude
        << "lng:" << info.longtitude
        << "}";
  return debug;
}


