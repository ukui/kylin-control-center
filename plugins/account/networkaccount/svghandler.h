#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QGSettings/QGSettings>

class SVGHandler : public QObject
{
    Q_OBJECT
public:
    explicit SVGHandler(QObject *parent = nullptr,bool highLight = false);
    const QPixmap loadSvg(const QString &fileName);
    const QPixmap loadSvgColor(const QString &path, const QString &color, int size = 48);
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor);
private:
    QGSettings *themeSettings;
    QString m_color;
Q_SIGNALS:

};

#endif // QL_SVG_HANDLER_H
