#ifndef SCREENSTRUCT_H
#define SCREENSTRUCT_H

#include <QString>
#include <QDBusArgument>

struct ScreenConfig
{
    QString screenId;
    QString screenModeId;
    int screenPosX;
    int screenPosY;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const ScreenConfig &screenStruct)
    {
        argument.beginStructure();
        argument << screenStruct.screenId  << screenStruct.screenModeId << screenStruct.screenPosX << screenStruct.screenPosY;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ScreenConfig &screenStruct)
    {
        argument.beginStructure();
        argument >> screenStruct.screenId >> screenStruct.screenModeId >> screenStruct.screenPosX >> screenStruct.screenPosY;
        argument.endStructure();
        return argument;
    }
};
Q_DECLARE_METATYPE(ScreenConfig)

#endif // SCREENSTRUCT_H
