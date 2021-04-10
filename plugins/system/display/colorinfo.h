#ifndef COLORINFO_H
#define COLORINFO_H

#include <QHash>
#include <QVariant>
#include <QString>
#include <QDBusArgument>

struct ColorInfo {
    QString arg;
    QDBusVariant out;
};

QDBusArgument &operator<<(QDBusArgument &argument, const ColorInfo &mystruct)
{
    argument.beginStructure();
    argument << mystruct.arg << mystruct.out;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ColorInfo &mystruct)
{
    argument.beginStructure();
    argument >> mystruct.arg >> mystruct.out;
    argument.endStructure();
    return argument;
}

Q_DECLARE_METATYPE(ColorInfo)

#endif // COLORINFO_H
