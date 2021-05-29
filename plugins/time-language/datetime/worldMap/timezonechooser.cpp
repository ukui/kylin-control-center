#include "timezonechooser.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QTimeZone>
#include <QCompleter>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>

#include "ImageUtil/imageutil.h"

const QString kcnBj = "北京";
const QString kenBj = "Asia/Beijing";

TimeZoneChooser::TimeZoneChooser(QWidget *parent) : QFrame(parent)
{
    m_map = new TimezoneMap(this);
    m_map->show();
    m_zoneinfo = new ZoneInfo;
    m_searchInput = new QLineEdit(this);
    m_title = new QLabel(this);
    m_closeBtn = new QPushButton(this);
    m_cancelBtn = new QPushButton(tr("Cancel"));
    m_confirmBtn = new QPushButton(tr("Confirm"));

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint| Qt::Tool);//无边框
    setAttribute(Qt::WA_StyledBackground,true);

    this->setObjectName("MapFrame");
    this->setStyleSheet("QFrame#MapFrame{background-color: rgb(22, 24, 26);border-radius:4px}");
    this->setWindowTitle(tr("Change time zone"));

    QIcon icon = QIcon::fromTheme("window-close-symbolic");
    m_closeBtn->setIcon(ImageUtil::drawSymbolicColoredPixmap(icon.pixmap(32, 32),"white"));
    m_closeBtn->setFlat(true);

    m_searchInput->setMinimumSize(560,40);
    m_searchInput->setMaximumSize(560,40);
    m_searchInput->setMinimumHeight(40);

    m_title->setObjectName("titleLabel");
    m_title->setStyleSheet("color: rgb(229, 240, 250 )");
    m_title->setText(tr("Change Timezone"));

    initSize();

    QHBoxLayout *wbLayout = new QHBoxLayout;
    wbLayout->setMargin(6);
    wbLayout->setSpacing(0);
    wbLayout->addStretch();
    wbLayout->addWidget(m_closeBtn);

    QHBoxLayout *btnlayout = new QHBoxLayout;
    btnlayout->addStretch();
    btnlayout->addWidget(m_cancelBtn);
    btnlayout->addSpacing(5);
    btnlayout->addWidget(m_confirmBtn);
    btnlayout->addStretch();


    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addLayout(wbLayout);
    layout->addStretch();
    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addSpacing(40);
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addSpacing(40);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addSpacing(40);

    layout->addLayout(btnlayout);
    layout->addStretch();

    setLayout(layout);

    connect(m_confirmBtn, &QPushButton::clicked,[this]{
        QString timezone = m_map->getTimezone();
        emit this->confirmed(timezone);
    });

    connect(m_cancelBtn, &QPushButton::clicked, this, [this]{
        hide();
        emit this->cancelled();
    });

    connect(m_closeBtn, &QPushButton::clicked, this, [this] {
        hide();
        emit cancelled();
    });


    connect(m_map, &TimezoneMap::timezoneSelected, this, [this]{
        m_searchInput->setText("") ;
        m_searchInput->clearFocus();
    });

    connect(m_searchInput, &QLineEdit::editingFinished, [this]{
        QString timezone = m_searchInput->text();
        timezone = m_zoneCompletion.value(timezone,timezone);
        m_map->setTimezone(timezone);
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
//        completions << kenBj;
//        completions << kcnBj;
//        m_zoneCompletion[kcnBj] = kenBj;
        for (QString timezone : QTimeZone::availableTimeZoneIds()) {
//            if ("Asia/Shanghai" == timezone) {
//                continue;
//            }
            completions << timezone;

            const QString locale = QLocale::system().name();
            QString localizedTimezone = m_zoneinfo->getLocalTimezoneName(timezone, locale);
            completions << localizedTimezone;

            m_zoneCompletion[localizedTimezone] = timezone;
        }

        QCompleter *completer = new QCompleter(completions, m_searchInput);
        completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);

        m_searchInput->setCompleter(completer);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
        connect(completer, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),
                [=](const QString &text){
#else
        //鼠标点击后直接页面跳转(https://doc.qt.io/qt-5/qcompleter.html#activated-1)
        connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
                [=](const QString &text) {
#endif
            Q_UNUSED(text);
            QString timezone = m_searchInput->text();
            timezone = m_zoneCompletion.value(timezone,timezone);
            m_map->setTimezone(timezone);
        });

        m_popup = completer->popup();
        m_popup->setAttribute(Qt::WA_TranslucentBackground);
        m_popup->installEventFilter(this);

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setSpacing(0);
        layout->setMargin(0);
        m_popup->setLayout(layout);
    });
}

void TimeZoneChooser::setTitle(QString title) {
    m_title->setText(title);
}

void TimeZoneChooser::setMarkedTimeZoneSlot(QString timezone) {
    m_map->setTimezone(timezone);
}

void TimeZoneChooser::keyRealeaseEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Cancel)) {
        hide();
        emit this->cancelled();
    }
}

bool TimeZoneChooser::eventFilter(QObject* obj, QEvent *event) {
    if (obj == m_popup && event->type() == QEvent::Move) {
        QMoveEvent* move = static_cast<QMoveEvent*>(event);
        QPoint desPos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height() + 2));

        if(move->pos() != desPos) {
            m_popup->move(desPos);
        }
    }
    return false;
}

//获取适合屏幕的地图大小
QSize TimeZoneChooser::getFitSize(){
    const QDesktopWidget *desktop = QApplication::desktop();
    const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    double width = primaryRect.width() - 360/* dcc */ - 20 * 2;
    double height = primaryRect.height() - 70/* dock */ - 20 * 2;

    return QSize(width,height);
}


void TimeZoneChooser::initSize(){

    double MapPixWidth = 978.0;
    double MapPixHeight = 500.0;
    double MapPictureWidth = 978.0;
    double MapPictureHeight = 500.0;

    QFont font = m_title->font();
    font.setPointSizeF(16.0);
    m_title->setFont(font);

    const QSize fitSize = getFitSize();
    setFixedSize(fitSize.width(), fitSize.height());

    const float mapWidth = qMin(MapPixWidth, fitSize.width() - 20 * 2.0);
    const float mapHeight = qMin(MapPixHeight, fitSize.height() - 20 * 2/*paddings*/ - 36 * 2/*buttons*/ - 10/*button spacing*/ - 40 * 3.0 /*spacings*/ - 30/*title*/ -  20 * 2/*top bottom margin*/);
    const double widthScale = MapPictureWidth / mapWidth;
    const double heightScale = MapPictureHeight / mapHeight;
    const double scale = qMax(widthScale, heightScale);

    m_map->setFixedSize(MapPictureWidth / scale, MapPictureHeight / scale);

    m_cancelBtn->setFixedHeight(36);
    m_confirmBtn->setFixedHeight(36);
    m_cancelBtn->setFixedWidth(120);
    m_confirmBtn->setFixedWidth(120);
}
