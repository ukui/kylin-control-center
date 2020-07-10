#include "ql_svg_handler.h"
#include <QApplication>

ql_svg_handler::ql_svg_handler(QObject *parent) : QObject(parent)
{

}


const QPixmap ql_svg_handler::loadSvg(const QString &fileName)
{
    int size = 24;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size = 48;
    } else if (3 == ratio) {
        size = 96;
    }
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}
