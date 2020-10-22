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

namespace Utils
{
    void centerToScreen(QWidget *widget);
    void setCLIName(QCommandLineParser &parser);
}
#endif // UTILS_H
