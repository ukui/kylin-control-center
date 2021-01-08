#include "listwig.h"

listwig::listwig(QWidget *parent) : QWidget(parent)
{
    m_entryHlayout       = new QHBoxLayout();
    m_entryVlayout       = new QVBoxLayout();

    m_Appnamelabel       = new QLabel();
    m_Versionlabel       = new QLabel();
    m_Timelabel          = new QLabel();
    m_Descriptionlabel   = new QLabel();
    m_statuelabel        = new QLabel();
    changeLogWidget      = new changelogwidget();

    namewidget           = new QWidget();

    namewidget->setFixedSize(233,60);

    m_entryVlayout->addWidget(m_Appnamelabel);
    m_entryVlayout->addWidget(m_Versionlabel);

    namewidget->setLayout(m_entryVlayout);

    m_entryHlayout->addWidget(namewidget);
    m_entryHlayout->addWidget(m_statuelabel);
    m_entryHlayout->addWidget(m_Timelabel);

    this->setLayout(m_entryHlayout);

}

void listwig::mousePressEvent(QMouseEvent *e)
{
    changeLogWidget->changelogEdit->setText(m_Descriptionlabel->text());
    changeLogWidget->show();
}

void listwig::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(QBrush(QColor(112,128,144)));
    p.setOpacity(0.2);
    p.setPen(QColor(112,128,144));
    p.drawLine(0, this->height()-8, this->width(), this->height()-8);
    QWidget::paintEvent(e);
}


headerwidget::headerwidget(QWidget *parent) : QWidget(parent)
{
    firstlabel        = new QLabel();
    secondlabel       = new QLabel();
    thirdlabel        = new QLabel();

    m_entryHlayout   = new QHBoxLayout();

    firstlabel->setText("升级包");
    secondlabel->setText("状态");
    thirdlabel->setText("更新时间");

    m_entryHlayout->addItem(new QSpacerItem(60, 20));
    m_entryHlayout->addWidget(firstlabel);
    m_entryHlayout->addItem(new QSpacerItem(70, 20));
    m_entryHlayout->addWidget(secondlabel);
    m_entryHlayout->addWidget(thirdlabel);
}

void headerwidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(QBrush(QColor(112,128,144)));
    p.setOpacity(0.2);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,0,0);
    QWidget::paintEvent(e);
}

