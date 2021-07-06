#ifndef SCALESIZE_H
#define SCALESIZE_H

#include <QSize>

const QSize KRsolution(1920, 1080);

const QVector<QSize> k150Scale{QSize(1280, 1024), QSize(1440, 900), QSize(1600, 900),
                               QSize(1680, 1050), QSize(1920, 1080), QSize(1920, 1200),
                               QSize(2048, 1080), QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440),QSize(3840, 2160)};

const QVector<QSize> k175Scale{QSize(2048, 1080), QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440), QSize(3840, 2160)};

const QVector<QSize> k200Scale{QSize(2048, 1080),QSize(2048, 1280), QSize(2160, 1440),
                               QSize(2560, 1440), QSize(3840, 2160)};

const QVector<QSize> k250Scale{QSize(2560, 1440), QSize(3840, 2160)};

const QVector<QSize> k275Scale{QSize(3840, 2160)};

#define SCALE_SCHEMAS "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY     "scaling-factor"

extern QSize mScaleSize;

#endif // SCALESIZE_H
