/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "memoryentry.h"

#include <KFormat>
#include <QDebug>
#include <QObject>
#include <QtAlgorithms>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#elif defined(Q_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

MemoryEntry::MemoryEntry() : Entry(ki18n("Memory:"), totalMemory().at(1))
{
}

qlonglong MemoryEntry::calculateTotalRam()
{
    qlonglong ret = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        // manpage "sizes are given as multiples of mem_unit bytes"
        ret = qlonglong(info.totalram) * info.mem_unit;
#elif defined(Q_OS_FREEBSD)
    /* Stuff for sysctl */
    size_t len;

    unsigned long memory;
    len = sizeof(memory);
    sysctlbyname("hw.physmem", &memory, &len, NULL, 0);

    ret = memory;
#endif
    return ret;
}

QStringList MemoryEntry::totalMemory()
{
    QStringList res;
    const qlonglong totalRam = calculateTotalRam();

    if (totalRam > 0) {
        QString total =  KFormat().formatByteSize(totalRam, 0);
        QString available = KFormat().formatByteSize(totalRam, 1);
        if (total.toDouble() > available.toDouble()) {
            qSwap(total, available);
        }
        res << total << available;
        return res;
    }
//    return ki18n("Unknown amount of RAM", "Unknown");
    return res;
}
