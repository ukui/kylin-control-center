#include "tips.h"

Tips::Tips(QWidget *parent) : QWidget(parent)
{
    m_textLabel = new QLabel(this);
    m_iconWidget = new QSvgWidget(":/new/image/_.svg");
    m_workLayout = new QHBoxLayout;
    m_iconWidget->setFixedSize(16,16);
    m_workLayout->addWidget(m_iconWidget);
    m_workLayout->addWidget(m_textLabel);
    m_workLayout->setContentsMargins(12,0,0,0);
    m_workLayout->setSpacing(8);
    m_textLabel->setStyleSheet("QLabel{font-size:14px;color:#F53547}");
    setLayout(m_workLayout);
    m_szContext = "";
    hide();
}

void Tips::set_text(const QString text) {
    m_szContext = text;
    this->m_textLabel->setText(m_szContext);
}

Tips::~Tips() {
    delete m_iconWidget;
}
