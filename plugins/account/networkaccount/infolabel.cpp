#include "infolabel.h"
#include <QColor>
#include <QHBoxLayout>

InfoLabel::InfoLabel(QWidget *parent) : QLabel(parent)
{
    m_svgHandler = new SVGHandler(this);
    m_toolTips = new Tooltips(this);
    m_textLabel = new QLabel(m_toolTips);

    QHBoxLayout *worklayout = new QHBoxLayout;
    QHBoxLayout *layout = new QHBoxLayout;


    layout->addWidget(m_textLabel);


    m_toolTips->setLayout(layout);

    worklayout->addWidget(m_toolTips);

    setPixmap(m_svgHandler->loadSvgColor(":/new/image/_.svg","default",16));
    m_toolTips->adjustSize();

    setLayout(worklayout);
}

void InfoLabel::enterEvent(QEvent *e) {
    QPoint pos;
    pos.setX(this->mapToGlobal(QPoint(0, 0)).x() + 26);
    pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + 26);
    m_toolTips->move(pos);
    m_toolTips->show();
    return QLabel::enterEvent(e);
}

void InfoLabel::setTipText(QString text) {
    m_textLabel->setText(text);
    m_textLabel->adjustSize();
    m_toolTips->adjustSize();
    adjustSize();
}


void InfoLabel::leaveEvent(QEvent *e) {
    m_toolTips->hide();

    return QLabel::leaveEvent(e);
}
