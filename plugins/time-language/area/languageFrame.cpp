#include "languageFrame.h"
#include <QHBoxLayout>

LanguageFrame::LanguageFrame(QString name, QWidget *parent)
    :QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    this->setFixedHeight(50);
    this->setFrameShape(QFrame::Box);
    this->setMinimumWidth(550);
    this->setMaximumWidth(960);
    layout->setContentsMargins(16,0,16,0);

    nameLabel = new QLabel();
    nameLabel->setFixedWidth(200);
    nameLabel->setText(name);

    selectedIconLabel = new QLabel();
    selectedIconLabel->setFixedSize(16,16);
    selectedIconLabel->setScaledContents(true);
    selectedIconLabel->setPixmap(QPixmap("://img/plugins/theme/selected.svg"));
    selectedIconLabel->setVisible(false);

    layout->addWidget(nameLabel);
    layout->addStretch();
    layout->addWidget(selectedIconLabel);

}

LanguageFrame::~LanguageFrame()
{

}

void LanguageFrame::mousePressEvent(QMouseEvent *e)
{
    this->showSelectedIcon(true);
    Q_EMIT clicked();
}

void LanguageFrame::showSelectedIcon(bool flag)
{
    selectedIconLabel->setVisible(flag);
}
