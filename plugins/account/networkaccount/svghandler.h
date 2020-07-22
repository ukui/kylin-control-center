#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
class SVGHandler : public QObject
{
    Q_OBJECT
public:
    explicit SVGHandler(QObject *parent = nullptr);
    const QPixmap loadSvg(const QString &fileName);
private:
signals:

};

#endif // QL_SVG_HANDLER_H
