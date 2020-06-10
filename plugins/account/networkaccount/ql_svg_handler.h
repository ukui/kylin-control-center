#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>

class ql_svg_handler : public QObject
{
    Q_OBJECT
public:
    explicit ql_svg_handler(QObject *parent = nullptr);
private:
    const QPixmap loadSvg(const QString &fileName);
signals:

};

#endif // QL_SVG_HANDLER_H
