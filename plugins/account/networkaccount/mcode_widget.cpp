#include "mcode_widget.h"

mcode_widget::mcode_widget(QWidget *parent): QLabel(parent)
{
    colorList<<QColor(Qt::black)<<QColor(Qt::red)<<QColor(Qt::darkRed)<<QColor(Qt::darkGreen)
              <<QColor(Qt::blue)<<QColor(Qt::darkBlue)<<QColor(Qt::darkCyan)<<QColor(Qt::magenta)
              <<QColor(Qt::darkMagenta)<<QColor(Qt::darkYellow);
    setFixedSize(120,36);
    qsrand(QTime::currentTime().second() * 1000 + QTime::currentTime().msec());
    colorArray = new QColor[letter_number];
    verificationCode = new QChar[letter_number];
    noice_point_number = this->width()*4;
    QFont defaultFont;//默认字体
    defaultFont.setFamily(tr( "宋体"));
    defaultFont.setPointSize(20);
    defaultFont.setBold(true);//设置字体加粗
    this->setFont(defaultFont);
    this->setStyleSheet("background-color:transparent;");
    this->setFocusPolicy(Qt::NoFocus);
}

void mcode_widget::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton)
    {
        ok = true;
        reflushVerification();
    }
}

QChar* mcode_widget::get_verificate_code() {
    return verificationCode;
}


void mcode_widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPoint p;
    //背景设为白色
    painter.fillRect(this->rect(), Qt::white);
    //产生4个不同的字符
    if(ok) {
        produceVerificationCode();
        //产生4个不同的颜色
        produceRandomColor();
        ok = false;
    }
    //绘制噪点
    int charWidth = (this->width()- 12 ) / 4;
    for (int j = 0; j < noice_point_number; ++j) //noice_point_number噪声点数
    {
        p.setX(qrand() % this->width());
        p.setY(qrand() % this->height());
        painter.setPen(colorArray[j % 4]);
        painter.drawPoint(p);
    }
    //绘制验证码
    for (int i = 0; i < letter_number; ++i)
    {
        painter.setPen(colorArray[i]);
        painter.save();
        int charSpace = (charWidth-this->fontMetrics().width(verificationCode[i]))/2;
        charSpace += 12/2;
        painter.translate(i*charWidth+charSpace,0);
        if(qrand()%2) {
            if(qrand()%2)
            {
                painter.rotate(qrand()% 20);
            }
            //随机逆时针旋转随机角度
            else
            {
                painter.rotate(-qrand()% 20);
            }
            double xSize = (qrand()%3+9)/10.0;
            double ySize = (qrand()%3+9)/10.0;
            painter.scale(xSize,ySize);
        } else {
            double xShear = qrand()%4/10.0;
            double yShear = qrand()%4/10.0;
            if(qrand()%2)
            {
                xShear = -xShear;
            }
            if(qrand()%2)
            {
                yShear = -yShear;
            }
            painter.shear(xShear,yShear);
        }
        painter.drawText(0,this->height()-12, QString(verificationCode[i]));
        painter.restore();
    }
    QLabel::paintEvent(event);
    return;
}
//这是一个用来生成验证码的函数
void mcode_widget::produceVerificationCode() const
{
    for (int i = 0; i < letter_number; ++i)
        verificationCode[i] = produceRandomLetter();
    verificationCode[letter_number] = '\0';
    return;
}
//产生一个随机的字符
QChar mcode_widget::produceRandomLetter() const
{
    QChar c;
    c='0' + qrand() % 10;
    return c;
}
//产生随机的颜色
void mcode_widget::produceRandomColor() const
{
    for (int i = 0; i < letter_number; ++i)
        colorArray[i] = colorList.at(qrand() % 4);
    return;
}

//刷新验证码，在用户不确定的时候进行相应刷新
void mcode_widget::reflushVerification()
{
    repaint();
}
