#ifndef UKUICUSTOMSTYLE_H
#define UKUICUSTOMSTYLE_H

#include <QObject>
#include <QPushButton>
#include <QImage>
static QColor symbolic_color = Qt::gray;
class UkuiCustomStyle:public QObject
{
    Q_OBJECT
public:
    UkuiCustomStyle();
};

class UkuiButtonDrawSvg:public QPushButton
{
    Q_OBJECT
public:
    UkuiButtonDrawSvg(/*QImage image ,QColor color,*/QWidget *parent = nullptr);
    ~UkuiButtonDrawSvg();
    QPixmap filledSymbolicColoredPixmap(QImage &source, QColor &baseColor);
    QRect IconGeometry();
    void draw(QPaintEvent* e);
    void init(QImage image ,QColor color);
//    friend class UkmediaInputWidget;
    friend class UkmediaMainWidget;
protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *e);
private:
    QImage mImage;
    QColor mColor;
};

#endif // UKUICUSTOMSTYLE_H
