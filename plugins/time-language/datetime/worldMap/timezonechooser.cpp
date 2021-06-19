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
#include <QGraphicsDropShadowEffect>
#include <QtCore/qmath.h>
#include <QDialog>

#include "ImageUtil/imageutil.h"

const QString kcnBj = "北京";
const QString kenBj = "Asia/Beijing";

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);


TimeZoneChooser::TimeZoneChooser(QWidget *parent) : QDialog(parent)
{
    m_map = new TimezoneMap(this);
    m_map->show();
    m_zoneinfo = new ZoneInfo;
    m_searchInput = new QLineEdit(this);
    m_title = new QLabel(this);
    m_closeBtn = new QPushButton(this);
    m_cancelBtn = new QPushButton(tr("Cancel"));
    m_confirmBtn = new QPushButton(tr("Confirm"));



    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);//无边框
    setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_TranslucentBackground);


    this->setObjectName("MapFrame");
    this->setWindowTitle(tr("Change time zone"));


    //m_searchInput->setStyleSheet("background-color:palette(windowtext)");
    m_searchInput->setFocusPolicy(Qt::ClickFocus);
    m_searchInput->setFixedSize(400,36);
    m_searchInput->installEventFilter(this);
    m_searchInput->setFocusPolicy(Qt::ClickFocus);
    m_searchInput->setContextMenuPolicy(Qt::NoContextMenu);

    m_queryWid=new QWidget;
    m_queryWid->setParent(m_searchInput);

    QHBoxLayout* queryWidLayout = new QHBoxLayout;
    queryWidLayout->setContentsMargins(0, 0, 0, 0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(0);
    m_queryWid->setLayout(queryWidLayout);


    QIcon searchIcon = QIcon::fromTheme("edit-find-symbolic");
    m_queryIcon = new QLabel(this);
    m_queryIcon->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(16, 16))));
    m_queryIcon->setProperty("useIconHighlightEffect",0x02);

    m_queryText = new QLabel(this);
    m_queryText->setText(tr("Input what you are looking for"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");
    m_queryText->adjustSize();

    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);

    m_animation= new QPropertyAnimation(m_queryWid, "geometry", this);
    m_animation->setDuration(100);


    m_queryWid->setGeometry(QRect((m_searchInput->width() - (m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                        m_queryIcon->width()+m_queryText->width()+10,m_searchInput->height()));
    m_queryWid->show();

    connect(m_animation,&QPropertyAnimation::finished,this,&TimeZoneChooser::animationFinishedSlot);

    m_closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_closeBtn->setFlat(true);
    m_closeBtn->setFixedSize(30, 30);
    m_closeBtn->setProperty("isWindowButton", 0x2);
    m_closeBtn->setProperty("useIconHighlightEffect", 0x08);

    m_title->setObjectName("titleLabel");
    m_title->setText(tr("Change Timezone"));
    m_title->setAlignment(Qt::AlignTop);

    initSize();

    QFrame *wbFrame = new QFrame;
    wbFrame->setContentsMargins(0,0,0,0);

    QHBoxLayout *wbLayout = new QHBoxLayout(wbFrame);
    wbLayout->setContentsMargins(16,15,16,0);
    wbLayout->addWidget(m_title);
    wbLayout->addStretch();
    wbLayout->addWidget(m_closeBtn);

    QHBoxLayout *btnlayout = new QHBoxLayout;
    btnlayout->addStretch();
    btnlayout->addWidget(m_cancelBtn);
    btnlayout->addSpacing(5);
    btnlayout->addWidget(m_confirmBtn);
    btnlayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(wbFrame,0,Qt::AlignVCenter);
    layout->addSpacing(0);
    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addSpacing(32);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addSpacing(32);
    layout->addLayout(btnlayout);
    layout->addSpacing(16);

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
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
        QWidget *searchParentWid = static_cast<QWidget*>(m_searchInput->parent());
        QPoint  searchPoint      = searchParentWid->mapFromGlobal(mEvent->globalPos());
        if (!m_searchInput->geometry().contains(searchPoint)) {
                if (m_isSearching == true) {
                    m_searchInput->setFocus();
                    m_searchInput->clearFocus();
                }
        }
    }

    if (obj == m_searchInput) {
        if (event->type() == QEvent::FocusIn) {
            if (m_searchInput->text().isEmpty()) {
                m_queryWid->layout()->removeWidget(m_queryText);
                m_queryText->setParent(nullptr);
                m_animation->stop();
                m_animation->setEndValue(QRect(0, 0, m_queryIcon->width() + 5,(m_searchInput->height()+36)/2));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
                m_searchInput->setTextMargins(20, 1, 0, 1);
            }
            m_isSearching = true;
        } else if (event->type() == QEvent::FocusOut) {
            m_searchKeyWords.clear();
            if (m_searchInput->text().isEmpty()) {
                if (m_isSearching) {
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0, 0,
                                                     m_queryIcon->width()+5,(m_searchInput->height()+36)/2));
                    m_animation->setEndValue(QRect((m_searchInput->width() - (m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                   m_queryIcon->width()+m_queryText->width()+30,(m_searchInput->height()+36)/2));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching=false;
        }
    }
    return QObject::eventFilter(obj, event);
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

    double MapPixWidth = 900.0;
    double MapPixHeight = 500.0;
    double MapPictureWidth = 978.0;
    double MapPictureHeight = 500.0;

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

void TimeZoneChooser::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();

}

void TimeZoneChooser::animationFinishedSlot()
{
    if (m_isSearching) {
        m_queryWid->layout()->removeWidget(m_queryText);
        m_queryText->setParent(nullptr);
        m_searchInput->setTextMargins(20, 1, 0, 1);
        if(!m_searchKeyWords.isEmpty()) {
            m_searchInput->setText(m_searchKeyWords);
            m_searchKeyWords.clear();
        }
    } else {
        m_queryWid->layout()->addWidget(m_queryText);
    }
}
