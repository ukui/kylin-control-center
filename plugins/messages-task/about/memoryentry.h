/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef MEMORYENTRY_H
#define MEMORYENTRY_H

#include "entry.h"

#include <QStringList>

class MemoryEntry : public Entry
{
public:
    MemoryEntry();
    static qlonglong calculateTotalRam();
    static QStringList totalMemory();
};

#endif // MEMORYENTRY_H
