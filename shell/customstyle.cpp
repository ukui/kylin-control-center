#include "customstyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QLineEdit>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>

InternalStyle::InternalStyle(const QString &styleName, QObject *parent) : QProxyStyle(styleName)
{

}

void InternalStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
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

        case QFrame::StyledPanel: {
            if (widget && qobject_cast<const QComboBox *>(widget->parentWidget())) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->setPen(QPen(frame.palette.color(frame.state & State_Enabled ? QPalette::Active : QPalette::Disabled, QPalette::Button), 2));
                painter->setBrush(frame.palette.base());
                painter->drawRoundedRect(frame.rect, 4, 4);
                painter->restore();
            }
            return;
        }
        default:
            return;
        }
        return;
        break;
    }
//    case CE_ItemViewItem: {
//        if (widget->parent() && ((widget->parent()->inherits("QComboBoxPrivateContainer") ||
//                                  (qobject_cast<const QFileDialog*>(widget->topLevelWidget()) &&
//                                   qobject_cast<const QAbstractItemView*>(widget)))))
//        {
//            break;
//        }
//        QStyleOptionViewItem item = *qstyleoption_cast<const QStyleOptionViewItem *>(option);
//        item.palette.setColor(QPalette::Highlight, item.palette.base().color());
//        return QProxyStyle::drawControl(element, &item, painter, widget);
//    }
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
    pal.setColor(QPalette::Inactive, QPalette::Base, pal.base().color());
}

void InternalStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);
    if (qobject_cast<QLineEdit *>(widget)) {
        auto pal = qApp->palette();
        pal.setColor(QPalette::Base, pal.alternateBase().color());
        widget->setPalette(pal);
    }

    // 跳过左侧边栏处理
    if (widget && widget->objectName() == "leftsidebarWidget") {

    } else if (widget){
        QPalette paltte = widget->palette();
        paltte.setColor(QPalette::Window, paltte.base().color());
        widget->setPalette(paltte);
    }
}
