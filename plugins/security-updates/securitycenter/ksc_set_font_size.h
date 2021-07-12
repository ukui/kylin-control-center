#ifndef KSC_SET_FONT_SIZE_H
#define KSC_SET_FONT_SIZE_H
#include<QFont>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
class ksc_set_font_size
{
public:
    ksc_set_font_size();
    //static QFont get_font(int type);
    static int set_font(int type,QFont& font);
    static int set_font(int type,QFont& font,QLabel* lable_lineedit);
    static int set_font(int type,QFont& font,QLineEdit* l_lineedit);
    static int set_font(int type,QFont& font,QPushButton* m_Button);
};

#endif // KSC_SET_FONT_SIZE_H
