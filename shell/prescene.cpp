#include "prescene.h"

#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>
#include <QApplication>
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

PreScene::PreScene(QLabel *label, QSize size, QWidget *parent) : QWidget(parent), m_size(size)
  , titleLabel(label)
{
    this->setFixedSize(m_size);
    this->setObjectName("prescene");
    this->setStyleSheet("PreScene#prescene{background:  palette(base); border-radius: 6px;}");

    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_vlayout = new QVBoxLayout;
    m_logoLayout = new QHBoxLayout;

    mTitleIcon = new QLabel(this);
    mTitleIcon->setFixedSize(24, 24);

    QIcon titleIcon = QIcon::fromTheme("ukui-control-center");
    mTitleIcon->setPixmap(titleIcon.pixmap(titleIcon.actualSize(QSize(24, 24))));

    titlebar = new QWidget(this);

    logoLabel = new QLabel(this);
    logoLabel->setFixedSize(200, 200);
    logoLabel->setPixmap(loadSvg(":/img/titlebar/ukui-control-center.svg"));
    m_logoLayout->setContentsMargins(70, 160, 0, 0);
    m_logoLayout->addWidget(logoLabel);

    m_hlayout = new QHBoxLayout;
    m_hlayout->setSpacing(0);
    m_hlayout->setContentsMargins(8, 8, 4, 0);
    m_hlayout->addWidget(mTitleIcon);
    m_hlayout->addSpacing(8);
    m_hlayout->addWidget(titleLabel);
    m_hlayout->addStretch();
    titlebar->setLayout(m_hlayout);

    m_vlayout->setSpacing(0);
    m_vlayout->setContentsMargins(0, 0, 0, 0);
    m_vlayout->addWidget(titlebar);
    m_vlayout->addLayout(m_logoLayout);
    m_vlayout->addStretch();

    this->setLayout(m_vlayout);
}

const QPixmap PreScene::loadSvg(const QString &fileName)
{
    int size = 128;
    const auto ratio = qApp->devicePixelRatio();
    size *= ratio;
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);

    QImage img = pixmap.toImage();
    return QPixmap::fromImage(img);
}

PreScene::~PreScene() {

}

void PreScene::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;

    rectPath.addRoundedRect(this->rect().adjusted(1, 1, -1, -1), 6, 6);

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
    qt_blurImage(img, 5, false, false);

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
//    p.fillPath(rectPath,QColor(0,0,0));
    p.restore();

}
