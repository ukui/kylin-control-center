#include "fontwatcher.h"
#include <QByteArray>
#include <QVariant>
#include <QFont>
#include <QDebug>
#define THEME_QT_SCHEMA        "org.ukui.style"
#define SYS_FONT               "systemFont"
#define SYS_UKUI_NAME          "styleName"
#define SYS_FONT_SIZE          "systemFontSize"
#define MAX_FONT_POINT_SIZE                 16
#define MIN_FONT_POINT_SIZE                 11
#define FONT_K                             1.3

FontWatcher::FontWatcher(QWidget *parent) : QWidget(parent)
{

    m_SpecialFontsize=14;                                          //默认14号字体
    //配置监听字体0
    const QByteArray id(THEME_QT_SCHEMA);
    setAttribute(Qt::WA_DeleteOnClose);
    m_FontSettings  = new QGSettings(id, QByteArray(),this);
    m_init_fontsize = m_FontSettings->get(SYS_FONT_SIZE).toInt();
    init();

}

void FontWatcher::init()
{

    //绑定监听控件捕获字号
    connect(m_FontSettings,&QGSettings::changed,[=](QString key){
        if( key == SYS_FONT_SIZE || key ==SYS_UKUI_NAME || key == SYS_FONT)
        {
            m_fontsize=m_FontSettings->get(SYS_FONT_SIZE).toInt();
        }
        for(int i=0 ; i < m_Contents_list.size(); i++ )
        {
            if( m_fontsize*FONT_K >= m_Contents_list.at(i)->n_font_size)
            {

                    QFont changefont;
                    changefont.setPointSize(m_fontsize);
                    m_Contents_list.at(i)->contentwidget->setFont(changefont);
            }

        }
    });

}
void FontWatcher::Add_Item_Contents_Widget(QWidget *obj)
{
    widgetcontent *content=new widgetcontent;
    content->contentwidget = obj;
    content->n_font_size   = obj->fontInfo().pixelSize();
    content->n_font_point_size =obj->fontInfo().pointSize();
    m_Contents_list.push_back(content);
}

void FontWatcher::Set_Special_Font_Size(int size)
{
    m_SpecialFontsize=size;
}

void FontWatcher::Set_Single_Content_Special(widgetcontent *obj , float add_percent,int n_size,QFont ft)
{


    ft.setPixelSize(n_size);
    obj->contentwidget->setFont(ft);

    QFont ft_init =ft;
    float n_point=obj->contentwidget->fontInfo().pointSizeF();
    float n_point_act=n_point+(m_init_fontsize-MIN_FONT_POINT_SIZE)*add_percent;
    ft_init.setPointSizeF(n_point_act);
    obj->contentwidget->setFont(ft_init);

    //绑定监听控件捕获字号
    connect(m_FontSettings,&QGSettings::changed,[=](QString key){
        if( key == SYS_FONT_SIZE || key ==SYS_UKUI_NAME ||key == SYS_FONT )
        {
            m_fontsize=m_FontSettings->get(SYS_FONT_SIZE).toInt();
        }
        float font_size = (m_fontsize-m_init_fontsize)*add_percent+n_point_act;

        if(n_point <= font_size  && font_size <= obj->limit/FONT_K)
        {
            QFont ft_ch =ft;
            ft_ch.setPointSizeF(font_size);
            obj->contentwidget->setFont(ft_ch);
        }
        if(m_fontsize == MIN_FONT_POINT_SIZE)
        {
            QFont ft_res =ft;
            ft_res.setPixelSize(n_size);
            obj->contentwidget->setFont(ft_res);
        }

    });

}

int FontWatcher::Get_Init_Font_Size()
{
    return m_init_fontsize;
}

widgetcontent* FontWatcher::Font_Special(QWidget *obj , int limit)
{
    widgetcontent *content=new widgetcontent;
    content->contentwidget = obj;
    content->n_font_size   = obj->fontInfo().pixelSize();
    content->n_font_point_size =obj->fontInfo().pointSize();
    content->limit=limit;
    return content;
}


