#ifndef DELEGATE_H
#define DELEGATE_H

#include <QDebug>
#include <QObject>
#include <QPainter>
#include <QStyledItemDelegate>

class Delegate : public QStyledItemDelegate
{
public:
    Delegate(QObject *parent = nullptr);
    ~Delegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DELEGATE_H
