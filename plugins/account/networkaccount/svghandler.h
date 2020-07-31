#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>

class SVGHandler : public QObject
{
    Q_OBJECT
public:
    explicit SVGHandler(QObject *parent = nullptr);
    const QPixmap loadSvg(const QString &fileName);
    static const QPixmap loadSvgColor(const QString &path, const QString color, int size = 48);
    static QPixmap drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor);
private:
signals:

};

#endif // QL_SVG_HANDLER_H
