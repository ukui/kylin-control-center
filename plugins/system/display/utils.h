#ifndef KSCREEN_KCM_UTILS_H
#define KSCREEN_KCM_UTILS_H

#include <QString>
#include <QSize>
#include <QStringList>

#include <KF5/KScreen/kscreen/types.h>
#include <KF5/KScreen/kscreen/output.h>

const QStringList kOutput {
    "Unknown",
    "VGA",
    "DVI",
    "DVII",
    "DVIA",
    "DVID",
    "HDMI",
    "eDP-1",
    "TV",
    "TVComposite",
    "TVSVideo",
    "TVComponent",
    "TVSCART",
    "TVC4",
    "DP-1"
};

namespace Utils {
QString outputName(const KScreen::Output *output);
QString outputName(const KScreen::OutputPtr &output);

QString sizeToString(const QSize &size);
}

#endif
