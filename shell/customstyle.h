#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QProxyStyle>

/*!
 * \brief The InternalStyle class
 * \note
 * InternalStyle is a special style naming rule of qt5-ukui-platformtheme.
 * It tells the platform do not take care of the style management of application
 * when an application set the style as default style.
 *
 * Do not modify this class name, otherwise the style will changed to default system
 * style when system style changed.
 *
 * InternalStyle是qt5-ukui-platformtheme的特殊命名规则，
 * 它用于告知平台插件不接管系统主题的管理（前提是将其设置为qApp的默认主题）。
 *
 * 不要修改这个类名，否则切换系统主题时，应用定制的应用主题会被系统主题覆盖。
 */
class InternalStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit InternalStyle(const QString &styleName, QObject *parent = nullptr);

    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    void drawControl(QStyle::ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const;

    void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;

    void polish(QPalette &pal);
    void polish(QWidget *widget);

signals:

};

#endif // CUSTOMSTYLE_H
