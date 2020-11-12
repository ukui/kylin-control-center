#ifndef UTILS_H
#define UTILS_H

#include <QRect>
#include <QCursor>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QVariantMap>

namespace Utils
{    
    void centerToScreen(QWidget *widget);
    void setCLIName(QCommandLineParser &parser);
    QVariantMap getModuleHideStatus();

}
#endif // UTILS_H
