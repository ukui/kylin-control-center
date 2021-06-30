#include "titlelabel.h"
#include <QFont>

#include <QGSettings>
#include <QVariant>
#include <QDebug>

TitleLabel::TitleLabel(QWidget *parent):
    QLabel(parent)
{
    /*初始化字体*/
    QFont font;
    QGSettings *m_fontSetting = new QGSettings("org.ukui.style");
    font.setFamily(m_fontSetting->get("systemFont").toString());
    font.setPixelSize(m_fontSetting->get("systemFontSize").toInt() * 18 / 11);  //设置的是pt，按照公式计算为px,标题默认字为18px
    font.setWeight(QFont::Medium);
    this->setFont(font);
    delete m_fontSetting;
    m_fontSetting = nullptr;
}

TitleLabel::~TitleLabel()
{

}
