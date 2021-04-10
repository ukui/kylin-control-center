#include "utils.h"

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>

QString Utils::outputName(const KScreen::OutputPtr &output)
{
    return outputName(output.data());
}

QString Utils::outputName(const KScreen::Output *output)
{
    return output->name();
}

QString Utils::sizeToString(const QSize &size)
{
    return QStringLiteral("%1x%2").arg(size.width()).arg(size.height());
}
