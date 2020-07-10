#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
class ql_svg_handler : public QObject
{
    Q_OBJECT
public:
    explicit ql_svg_handler(QObject *parent = nullptr);
    const QPixmap loadSvg(const QString &fileName);
private:
signals:

};

#endif // QL_SVG_HANDLER_H
