#include "customstyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include <QLineEdit>

#include <QApplication>

InternalStyle::InternalStyle(const QString &styleName, QObject *parent) : QProxyStyle(styleName)
{

}

void InternalStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    //qDebug()<<element;
    switch (element) {
    case QStyle::PE_Frame: {

        return;
    }
    case PE_IndicatorViewItemCheck:
    case PE_PanelItemViewRow:
    case PE_PanelItemViewItem: {
        return;
    }
    default:
        break;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void InternalStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
//    qDebug()<<element;
    switch(element)
    {
    case CE_ShapedFrame:

        break;
    case CE_ItemViewItem: {
        QStyleOptionViewItem item = *qstyleoption_cast<const QStyleOptionViewItem *>(option);
        item.palette.setColor(QPalette::Highlight, item.palette.base().color());
        return QProxyStyle::drawControl(element, &item, painter, widget);
    }
    default:
        break;
    }
    return QProxyStyle::drawControl(element, option, painter, widget);
}

void InternalStyle::polish(QPalette &pal)
{
    QProxyStyle::polish(pal);
    pal.setColor(QPalette::Window, pal.base().color());
    pal.setColor(QPalette::Inactive, QPalette::Base, pal.base().color());
//    pal.setColor(QPalette::Button, pal.alternateBase().color());
}

void InternalStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);
    if (qobject_cast<QLineEdit *>(widget)) {
        auto pal = qApp->palette();
        pal.setColor(QPalette::Base, pal.alternateBase().color());
        widget->setPalette(pal);
    }
}
