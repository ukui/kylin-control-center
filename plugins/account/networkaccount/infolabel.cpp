#include "infolabel.h"
#include <QColor>
#include <QHBoxLayout>

InfoLabel::InfoLabel(QWidget *parent) : QLabel(parent)
{
    m_svgHandler = new SVGHandler(this);
    m_toolTips = new Tooltips(this);
    m_textLabel = new QLabel(m_toolTips);

    //setFixedWidth(300);

    QHBoxLayout *worklayout = new QHBoxLayout;
    QHBoxLayout *layout = new QHBoxLayout;


    layout->addWidget(m_textLabel,0,Qt::AlignVCenter);


    m_toolTips->setLayout(layout);
    m_toolTips->setFixedWidth(225);
    m_toolTips->setMinimumHeight(44);
    m_toolTips->setMaximumHeight(500);

    m_textLabel->setMaximumHeight(480);
    setWordWrap(true);

    worklayout->addWidget(m_toolTips,0,Qt::AlignVCenter);

    setPixmap(m_svgHandler->loadSvgColor(":/new/image/_.svg","default",16));
    m_toolTips->adjustSize();

    setAlignment(Qt::AlignVCenter);

    m_textLabel->setWordWrap(true);
    m_textLabel->setAlignment(Qt::AlignTop);

    setLayout(worklayout);
}

void InfoLabel::enterEvent(QEvent *e) {
    QPoint pos;
    pos.setX(this->mapToGlobal(QPoint(0, 0)).x() + 26);
    pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + 26);
    m_toolTips->move(pos);
    m_toolTips->adjustSize();
    m_toolTips->show();
    return QLabel::enterEvent(e);
}

void InfoLabel::setTipText(const QString &text) {
    m_textLabel->setText(text);
    m_textLabel->adjustSize();
    m_toolTips->adjustSize();
    adjustSize();
}


void InfoLabel::leaveEvent(QEvent *e) {
    m_toolTips->hide();

    return QLabel::leaveEvent(e);
}
