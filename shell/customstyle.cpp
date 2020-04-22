#include "customstyle.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include <QLineEdit>
#include <QFileDialog>
#include <QAbstractItemView>

#include <QApplication>

InternalStyle::InternalStyle(const QString &styleName, QObject *parent) : QProxyStyle(styleName)
{

}

void InternalStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    //qDebug()<<element;
    switch (element) {
    case QStyle::PE_Frame: {
//        painter->fillRect(option->rect, option->palette.button().color());
        return;
//        painter->save();
//        painter->setRenderHint(QPainter::Antialiasing, true);
//        painter->setPen(QPen(option->palette.foreground(), 1.0));
//        painter->drawRect(option->rect.adjusted(+1, +1, -1, -1));
//        painter->restore();        
        QStyleOptionFrame frame = *qstyleoption_cast<const QStyleOptionFrame *>(option);
        QFrame::Shape shape = frame.frameShape;
        switch (shape) {
        case QFrame::Box: {
            //border top radius
            painter->fillRect(frame.rect, Qt::red);
            return;
        }
        case QFrame::HLine: {
            //border bottom radius
            painter->fillRect(frame.rect, Qt::green);
            return;
        }
        case QFrame::VLine: {
            //both top and bottom
            painter->fillRect(frame.rect, Qt::blue);
            return;
        }
        default:
//            qDebug()<<"this is deafault"<<endl;
            return;
        }
        return;
    }
    case PE_IndicatorViewItemCheck:
    case PE_PanelItemViewRow:
    case PE_PanelItemViewItem: {
        if (widget->parent() && widget->inherits("QComboBoxPrivateContainer"))
            break;
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
    case CE_ShapedFrame: {
        //return proxy()->drawPrimitive(PE_Frame, option, painter, widget);
        QStyleOptionFrame frame = *qstyleoption_cast<const QStyleOptionFrame *>(option);
        QFrame::Shape shape = frame.frameShape;
        switch (shape) {
        case QFrame::Box: {
            // Draw four rounded corners

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setBrush(option->palette.color(QPalette::Button));
            painter->setPen(Qt::transparent);
            painter->setOpacity(0.45);
            painter->drawRoundedRect(widget->rect(), 6, 6);
            painter->restore();
            return;
        }
        case QFrame::HLine: {
            // Draw rounded corners at the bottom left and bottom right

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);

            QPainterPath path;
            path.addRoundedRect(frame.rect, 6,6);
            path.setFillRule(Qt::WindingFill);

            path.addRect(frame.rect.width() - 6, 0, 6, 6);
            path.addRect(0, 0, 6, 6);

            painter->setPen(Qt::transparent);
            painter->setBrush(option->palette.color(QPalette::Button));

            painter->setClipPath(path);
            painter->drawRect(frame.rect);
            painter->restore();
            return;
        }
        case QFrame::VLine: {
            // Draw rounded corners in the upper left and upper right corners

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);

            QPainterPath path;
            path.addRoundedRect(frame.rect, 6,6);
            path.setFillRule(Qt::WindingFill);

            path.addRect(0, frame.rect.height() - 6, 6, 6);
            path.addRect(frame.rect.width() - 6, frame.rect.height() - 6, 6, 6);

            painter->setPen(Qt::transparent);
            painter->setBrush(option->palette.color(QPalette::Button));

            painter->setClipPath(path);
            painter->drawRect(frame.rect);
            painter->restore();
            return;
        }
        case QFrame::Panel: {
            // Do not draw corner

            painter->fillRect(frame.rect, option->palette.color(QPalette::Button));
            return;
        }
        default:
//            qDebug()<<"this is deafault"<<endl;
            return;
        }
        return;
        break;
    }
    case CE_ItemViewItem: {
        if (widget->parent() && ((widget->parent()->inherits("QComboBoxPrivateContainer") ||
                                  (qobject_cast<const QFileDialog*>(widget->topLevelWidget()) &&
                                   qobject_cast<const QAbstractItemView*>(widget)))))
        {
            break;
        }
        QStyleOptionViewItem item = *qstyleoption_cast<const QStyleOptionViewItem *>(option);
        item.palette.setColor(QPalette::Highlight, item.palette.base().color());
        return QProxyStyle::drawControl(element, &item, painter, widget);
    }
    default:
        break;
    }
    return QProxyStyle::drawControl(element, option, painter, widget);
}

void InternalStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case CC_ComboBox: {
        const QStyleOptionComboBox *tmp = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        QStyleOptionComboBox combobox = *tmp;
        combobox.palette.setColor(QPalette::Button, combobox.palette.base().color());
        return QProxyStyle::drawComplexControl(control, &combobox, painter, widget);
        break;
    }
    default:
        break;
    }

    return QProxyStyle::drawComplexControl(control, option, painter, widget);
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
