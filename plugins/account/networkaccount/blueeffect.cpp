#include "blueeffect.h"

Blueeffect::Blueeffect(QWidget *parent) : QWidget(parent)
{
    m_textLabel = new QLabel(this);
    m_iconLabel = new QLabel(this);
    m_cTimer = new QTimer(this);
    m_svgHandler = new SVGHandler(this);
    m_workLayout = new QHBoxLayout;

    this->setStyleSheet("Blueeffect{background-color:#3D6BE5;border-radius:4px;}");
    m_textLabel->setStyleSheet("color:#ffffff;background:transparent;");
    m_iconLabel->setStyleSheet("background:transparent;");
    m_iconLabel->setFixedSize(24,24);
    m_workLayout->setSpacing(8);
    m_workLayout->setMargin(0);
    m_workLayout->setAlignment(Qt::AlignCenter);
    m_workLayout->addWidget(m_textLabel);
    m_workLayout->addWidget(m_iconLabel);
    setLayout(m_workLayout);
    m_cTimer->stop();
    connect(m_cTimer,&QTimer::timeout, this,[=] () {
        QPixmap pixmap = m_svgHandler->loadSvgColor(QString(":/new/image/loading1%1.svg").arg(m_cCnt),"white",16);
        m_iconLabel->setPixmap(pixmap);
        m_cCnt = (m_cCnt + 9) % 8;
    });
    hide();
}

Blueeffect::~Blueeffect(){
    m_cTimer->stop();
}

void Blueeffect::settext(const QString &t) {
    m_textLabel->setText(t);
}

void Blueeffect::startmoive() {
    m_cTimer->start(140);
    show();
    adjustSize();
}

void Blueeffect::stop() {
    m_cCnt = 1;
    m_iconLabel->setPixmap(m_svgHandler->loadSvg(":/new/image/loading11.svg"));
    if (m_cTimer->isActive())
        m_cTimer->stop();
    hide();
}
