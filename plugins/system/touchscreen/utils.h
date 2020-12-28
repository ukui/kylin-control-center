#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QSize>

#include <KF5/KScreen/kscreen/types.h>
#include <KF5/KScreen/kscreen/output.h>

//获取显示器名字和ID类
namespace Utils
{
    QString outputName(const KScreen::Output *output);
    QString outputName(const KScreen::OutputPtr &output);

    QString sizeToString(const QSize &size);
}


#endif // UTILS_H
