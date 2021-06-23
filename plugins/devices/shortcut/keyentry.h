#ifndef KEYENTRY_H
#define KEYENTRY_H

#include <QDialog>

typedef struct _KeyEntry KeyEntry;

struct _KeyEntry {
    QString gsSchema;
    QString keyStr;
    QString valueStr;
    QString descStr;

    QString gsPath;
    QString nameStr;
    QString bindingStr;
    QString actionStr;
};

Q_DECLARE_METATYPE(KeyEntry *)

#endif // KEYENTRY_H
