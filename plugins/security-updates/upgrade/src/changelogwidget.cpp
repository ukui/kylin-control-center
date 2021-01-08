#include "changelogwidget.h"

changelogwidget::changelogwidget(QWidget *parent): QWidget(parent)
{

    this->setFixedSize(500,600);
    logLabel = new QLabel();
    logLabel->setText("ChangeLog");
    changelogEdit = new QTextEdit();
    changelogEdit->setReadOnly(true);

    QFont ft;
    ft.setPointSize(20);
    logLabel->setFont(ft);

    m_entryVlayout = new QVBoxLayout();
    m_entryVlayout->addWidget(logLabel);
    m_entryVlayout->addItem(new QSpacerItem(60, 60));
    m_entryVlayout->addWidget(changelogEdit);
    this->setLayout(m_entryVlayout);
}


