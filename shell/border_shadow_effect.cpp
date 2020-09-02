#include "border_shadow_effect.h"
#include <QPainter>
#include <QWidget>
#include <QDebug>
#include <QRect>
#include <QGSettings>
//qt's global function
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

BorderShadowEffect::BorderShadowEffect(QObject *parent) : QGraphicsEffect(parent)
{
}
void BorderShadowEffect::setBorderRadius(int radius)
{
    m_x_border_radius = radius;
    m_y_border_radius = radius;
}

void BorderShadowEffect::setBorderRadius(int xradius, int yradius)
{
    m_x_border_radius = xradius;
    m_y_border_radius = yradius;
}

void BorderShadowEffect::setBlurRadius(int radius)
{
    m_blur_radius = radius;
}

void BorderShadowEffect::setPadding(int padding)
{
    m_padding = padding;
}

void BorderShadowEffect::setShadowColor(const QColor &color)
{
    if (color != m_shadow_color) {
        m_force_update_cache = true;
    }
    m_shadow_color = color;
}

void BorderShadowEffect::setWindowBackground(const QColor &color)
{
    m_window_bg = color;
}

void BorderShadowEffect::setTransParentPath(const QPainterPath &path)
{
    m_transparent_path = path;
}

void BorderShadowEffect::setTransParentAreaBg(const QColor &transparentBg)
{
    m_transparent_bg = transparentBg;
}

void BorderShadowEffect::drawWindowShadowManually(QPainter *painter, const QRect &windowRect, bool fakeShadow,bool isMax)
{
    //draw window bg;
    QRect sourceRect = windowRect;
    auto contentRect = sourceRect.adjusted(m_padding, m_padding, -m_padding, -m_padding);
    //qDebug()<<contentRect;
    QPainterPath sourcePath;
    QPainterPath contentPath;
    sourcePath.addRect(sourceRect);
    if(!isMax){
        contentPath.addRoundedRect(contentRect, m_x_border_radius, m_y_border_radius);
        auto targetPath = sourcePath - contentPath;
        auto bgPath = contentPath - m_transparent_path;
        painter->fillPath(bgPath, m_window_bg);
        painter->fillPath(m_transparent_path, m_transparent_bg);
    } else {
        contentPath.addRoundedRect(contentRect, 0, 0);
        auto targetPath = sourcePath - contentPath;
        auto bgPath = contentPath - m_transparent_path;
        painter->fillPath(bgPath, m_window_bg);
        painter->fillPath(m_transparent_path, m_transparent_bg);

    }
    //毛玻璃效果监听平板模式切换
    //    QGSettings * tablet_mode =  new QGSettings("org.ukui.SettingsDaemon.plugins.tablet-mode", QByteArray(),this);
    //    is_tablet_mode = tablet_mode->get("tablet-mode").toBool();
    //    if(is_tablet_mode){
    //        if (fakeShadow) {
    //            painter->save();
    //            auto color = m_shadow_color;
    //            color.setAlphaF(0.2);
    //            painter->setPen(color);
    //            painter->setBrush(Qt::white);
    //            painter->drawPath(contentPath);
    //            //        auto color_1 = m_shadow_color;
    //            //        color_1.setAlphaF(0);
    //            //        painter->setPen(color_1);
    //            //        painter->setBrush(QColor("#F6F6F6"));
    //            //        painter->drawPath(contentPath - m_transparent_path);
    //            painter->restore();
    //            return;
    //        }

    //        connect(tablet_mode, &QGSettings::changed, this, [=](){
    //            is_tablet_mode = tablet_mode->get("tablet-mode").toBool();
    //            if(is_tablet_mode){
    //                if (fakeShadow) {
    //                    painter->save();
    //                    auto color = m_shadow_color;
    //                    color.setAlphaF(0.2);
    //                    painter->setPen(color);
    //                    painter->setBrush(Qt::white);
    //                    painter->drawPath(contentPath);
    //                    //        auto color_1 = m_shadow_color;
    //                    //        color_1.setAlphaF(0);
    //                    //        painter->setPen(color_1);
    //                    //        painter->setBrush(QColor("#F6F6F6"));
    //                    //        painter->drawPath(contentPath - m_transparent_path);
    //                    painter->restore();
    //                    return;
    //                }
    //            } else {
    //                if (fakeShadow) {
    //                    painter->save();
    //                    auto color = m_shadow_color;
    //                    color.setAlphaF(0.2);
    //                    painter->setPen(color);
    //                    painter->setBrush(Qt::transparent);
    //                    painter->drawPath(contentPath);
    //                    //        auto color_1 = m_shadow_color;
    //                    //        color_1.setAlphaF(0);
    //                    //        painter->setPen(color_1);
    //                    //        painter->setBrush(QColor("#F6F6F6"));
    //                    //        painter->drawPath(contentPath - m_transparent_path);
    //                    painter->restore();
    //                    return;
    //                }
    //            }
    //        });
    if (fakeShadow) {
        painter->save();
        auto color = m_shadow_color;
        color.setAlphaF(0.2);
        painter->setPen(color);
        painter->setBrush(Qt::transparent);
        painter->drawPath(contentPath);
        //        右侧功能界面背景颜色
        //        auto color_1 = m_shadow_color;
        //        color_1.setAlphaF(0);
        //        painter->setPen(color_1);
        //        painter->setBrush(QColor("#F6F6F6"));
        //        painter->drawPath(contentPath - m_transparent_path);
        painter->restore();
        return;
    }

    //qDebug()<<this->boundingRect()<<offset;
    if (m_padding > 0) {
        if (m_cache_shadow.size() == windowRect.size() && !m_force_update_cache) {
            //use cache pixmap draw shadow.
            painter->save();
            painter->setClipPath(sourcePath - contentPath);
            painter->drawImage(QPoint(), m_cache_shadow);
            painter->restore();
        } else {
            //draw shadow and cache shadow pixmap
            QPixmap pixmap(sourceRect.size().width(), sourceRect.height());
            pixmap.fill(Qt::transparent);
            QPainter p(&pixmap);
            p.fillPath(contentPath, m_shadow_color);
            p.end();
            QImage img = pixmap.toImage();
            qt_blurImage(img, m_blur_radius, false, false);
            pixmap.convertFromImage(img);
            m_cache_shadow = img;
            painter->save();
            painter->setClipPath(sourcePath - contentPath);
            painter->drawImage(QPoint(), img);
            painter->restore();
            m_force_update_cache = false;
        }
    }
}
void BorderShadowEffect::draw(QPainter *painter)
{
    //draw window bg;
    auto sourceRect = boundingRect();
    auto contentRect = boundingRect().adjusted(m_padding, m_padding, -m_padding, -m_padding);
    //qDebug()<<contentRect;
    QPainterPath sourcePath;
    QPainterPath contentPath;
    sourcePath.addRect(sourceRect);
    contentPath.addRoundedRect(contentRect, m_x_border_radius, m_y_border_radius);
    auto targetPath = sourcePath - contentPath;
    //qDebug()<<contentPath;
    painter->fillPath(contentPath, m_window_bg);

    QPoint offset;
    if (sourceIsPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, QGraphicsEffect::PadToTransparentBorder);
        painter->drawPixmap(offset, pixmap);
    } else {
        // Draw pixmap in device coordinates to avoid pixmap scaling;
        const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, QGraphicsEffect::PadToTransparentBorder);
        painter->setWorldTransform(QTransform());
        painter->drawPixmap(offset, pixmap);
    }
    //qDebug()<<this->boundingRect()<<offset;
    if (m_padding > 0) {
        //draw shadow
        QPixmap pixmap(sourceRect.size().width(), sourceRect.height());
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.fillPath(contentPath, m_shadow_color);
        p.end();
        QImage img = pixmap.toImage();
        qt_blurImage(img, m_blur_radius, false, false);
        pixmap.convertFromImage(img);
        painter->save();
        painter->setClipPath(sourcePath - contentPath);
        painter->drawImage(QPoint(), img);
        painter->restore();
    }
}
