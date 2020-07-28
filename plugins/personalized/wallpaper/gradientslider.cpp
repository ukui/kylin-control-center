#include "gradientslider.h"

GradientSlider::GradientSlider(QWidget *parent) :
    QSlider(parent)
  , back( Qt::darkGray, Qt::DiagCrossPattern )

{
    col_list.push_back(Qt::black);
    col_list.push_back(Qt::white);

    setOrientation(Qt::Horizontal);
    setFixedSize(25,264);
}

void GradientSlider::setBackground(QBrush bg)
{
    back = bg;
    update();
}

void GradientSlider::setColors(QVector<QColor> bg)
{
    col_list = bg;
    update();
}

void GradientSlider::setGradient(QLinearGradient bg)
{
    col_list.clear();
    foreach(const QGradientStop& gs, bg.stops() )
    {
        col_list.push_back(gs.second);
    }
    update();
}

QLinearGradient GradientSlider::gradient() const
{
    int ior = orientation() == Qt::Horizontal ? 1 : 0;
    QLinearGradient lg(0,0,ior,1-ior); // ior : 0 垂直渐变 : 1 水平渐变
    lg.setCoordinateMode(QGradient::StretchToDeviceMode); // 设备边界模式
    for(int i = 0; i < col_list.size(); i++)
        lg.setColorAt(double(i)/(col_list.size()-1),col_list[i]); // 设置梯度颜色, 参数一处于0~1之间
    return lg;
}

void GradientSlider::setFirstColor(QColor c)
{
    col_list.clear();
    col_list.push_back(c);
    update();
}

void GradientSlider::setLastColor(QColor c)
{
    col_list.push_back(c);
    update();
}

QColor GradientSlider::firstColor() const
{
    return col_list.empty() ? QColor() : col_list.front();
}

QColor GradientSlider::lastColor() const
{
    return col_list.empty() ? QColor() : col_list.back();
}

void GradientSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const int offset = 5;
    const int offsett = 9;
    QRect rect = this->rect();
    QRect sliderRect = QRect(rect.left()+offset, rect.top(),
                             rect.right()-2*offset,rect.bottom());

    const double k = (double)(value() - minimum()) / (maximum()-minimum());
    int y = (int)((rect.height()-2*offset) * (1-k)) + offset;

    QPoint tickTop[] = {
        QPoint(sliderRect.left(), y),
        QPoint(rect.left(), y - offset),
        QPoint(rect.left(), y + offset)
    };

//    QPoint tickTop[] = {
//        QPoint(sliderRect.left()+1, y), // 19 264
//        QPoint(rect.left(), y - offset), // 24 264 -5
//        QPoint(rect.left()+10, y - offset),
//        QPoint(rect.left()+10, y + offset),
//        QPoint(rect.left(), y + offset)
//    };
    // 绘制左箭头
    painter.setPen(QPen(Qt::black, 0.5));
    painter.setBrush(QColor("#ECEAEA"));
    painter.drawPolygon(tickTop, 3);
    // 绘制彩色渐变
    painter.setPen(Qt::NoPen);
    painter.setBrush(back);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.drawRoundedRect(sliderRect,4,4);
    painter.setBrush(gradient());
    painter.drawRoundedRect(sliderRect,4,4);


    QPoint tickPts[] = {
        QPoint(sliderRect.right()+1, y),
        QPoint(rect.right(), y - offset),
        QPoint(rect.right(), y + offset)
    };
//    QPoint tickPts[] = {
//        QPoint(sliderRect.right()+1, y), // 19 264
//        QPoint(rect.right(), y - offset), // 24 264 -5
//        QPoint(rect.right()+10, y - offset),
//        QPoint(rect.right()+10, y + offset),
//        QPoint(rect.right(), y + offset)
//    };
    // 绘制右箭头
    painter.setPen(QPen(Qt::black, 0.5));
    painter.setBrush(QColor("#ECEAEA"));
    painter.drawPolygon(tickPts,3);

}
