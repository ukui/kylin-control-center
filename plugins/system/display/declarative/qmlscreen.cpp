/*
    Copyright (C) 2012  Dan Vratil <dvratil@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "qmlscreen.h"
#include "qmloutputcomponent.h"
#include "qmloutput.h"

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/config.h>

#include <QTimer>
#include <sys/socket.h>

Q_DECLARE_METATYPE(KScreen::OutputPtr)

QMLScreen::QMLScreen(QQuickItem *parent) :
    QQuickItem(parent)
{
    connect(this, &QMLScreen::widthChanged, this, &QMLScreen::viewSizeChanged);
    connect(this, &QMLScreen::heightChanged, this, &QMLScreen::viewSizeChanged);
}

QMLScreen::~QMLScreen()
{
    qDeleteAll(m_outputMap);
    m_outputMap.clear();
}

KScreen::ConfigPtr QMLScreen::config() const
{
    return m_config;
}

void QMLScreen::setConfig(const KScreen::ConfigPtr &config)
{
    qDeleteAll(m_outputMap);
    m_outputMap.clear();
    m_manuallyMovedOutputs.clear();
    m_bottommost = m_leftmost = m_rightmost = m_topmost = nullptr;
    m_connectedOutputsCount = 0;
    m_enabledOutputsCount = 0;

    if (m_config) {
        m_config->disconnect(this);
    }

    m_config = config;
    connect(m_config.data(), &KScreen::Config::outputAdded,
            this, [this](const KScreen::OutputPtr &output) {
        QTimer::singleShot(1000, this, [=] {
            m_manuallyMovedOutputs.clear();
            addOutput(output);
            updateOutputsPlacement();
        });
    });
    connect(m_config.data(), &KScreen::Config::outputRemoved,
            this, &QMLScreen::removeOutput);

    for (const KScreen::OutputPtr &output : m_config->outputs()) {
        addOutput(output);
    }

    updateOutputsPlacement();

    for (QMLOutput *qmlOutput : m_outputMap) {
        if (qmlOutput->output()->isConnected() && qmlOutput->output()->isEnabled()) {
            qmlOutput->dockToNeighbours();
        }
    }
}

void QMLScreen::addOutput(const KScreen::OutputPtr &output)
{
    QMLOutputComponent comp(qmlEngine(this), this);

    QMLOutput *qmloutput = comp.createForOutput(output);
    if (!qmloutput) {
        qWarning() << "Failed to create QMLOutput";
        return;
    }

    m_outputMap.insert(output, qmloutput);

    qmloutput->setParentItem(this);
    qmloutput->setZ(m_outputMap.count());

    connect(output.data(), &KScreen::Output::isConnectedChanged,
            this, &QMLScreen::outputConnectedChanged);
    connect(output.data(), &KScreen::Output::isEnabledChanged,
            this, &QMLScreen::outputEnabledChanged);
    connect(output.data(), &KScreen::Output::posChanged,
            this, &QMLScreen::outputPositionChanged);
    connect(qmloutput, &QMLOutput::yChanged,
            [this, qmloutput]() {
        qmlOutputMoved(qmloutput);
    });
    connect(qmloutput, &QMLOutput::xChanged,
            [this, qmloutput]() {
        qmlOutputMoved(qmloutput);
    });
    // 在这里点击上面小屏幕
    connect(qmloutput, SIGNAL(clicked()),
            this, SLOT(setActiveOutput()));

    connect(qmloutput, SIGNAL(mouseReleased()),
            this, SLOT(setScreenPos()));

    connect(qmloutput, SIGNAL(rotationChanged()),
            this, SLOT(setScreenPos()));

    connect(qmloutput, SIGNAL(widthChanged()),
            this, SLOT(setScreenPos()));

    connect(qmloutput, SIGNAL(heightChanged()),
            this, SLOT(setScreenPos()));

    qmloutput->updateRootProperties();
}

void QMLScreen::removeOutput(int outputId)
{
    for (const KScreen::OutputPtr &output : m_outputMap.keys()) {
        if (output->id() == outputId) {
            QMLOutput *qmlOutput = m_outputMap.take(output);
            qmlOutput->setParentItem(nullptr);
            qmlOutput->setParent(nullptr);
            // TODO:bug51346
            // qmlOutput->deleteLater();
            return;
        }
    }
}

int QMLScreen::connectedOutputsCount() const
{
    return m_connectedOutputsCount;
}

int QMLScreen::enabledOutputsCount() const
{
    return m_enabledOutputsCount;
}

QMLOutput *QMLScreen::primaryOutput() const
{
    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        if (qmlOutput->output()->isPrimary()) {
            return qmlOutput;
        }
    }

    return nullptr;
}

QList<QMLOutput *> QMLScreen::outputs() const
{
    return m_outputMap.values();
}

void QMLScreen::setActiveOutput(QMLOutput *output)
{
    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        if (qmlOutput->z() > output->z()) {
            qmlOutput->setZ(qmlOutput->z() - 1);
        }
    }

    output->setZ(m_outputMap.count());
    // 中屏幕
    output->setFocus(true);
    Q_EMIT focusedOutputChanged(output);
}

void QMLScreen::setScreenCenterPos()
{
    // 组成最大矩形四个边的位置，分别对应左上(1)，右下(2)的xy坐标值
    qreal localX1 = -1, localX2 = -1, localY1 = -1, localY2 = -1;
    qreal mX1 = 0, mY1 = 0, mX2 = 0, mY2 = 0; // 矩形中点坐标
    qreal moveX = 0, moveY = 0;// 移动的值
    bool firstFlag = true;
    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        if (qmlOutput->output()->isConnected()) {
            if (firstFlag == true || localX1 > qmlOutput->x()) {
                localX1 = qmlOutput->x();
            }
            if (firstFlag == true || localX2 < qmlOutput->x() + qmlOutput->width()) {
                localX2 = qmlOutput->x() + qmlOutput->width();
            }
            if (firstFlag == true || localY1 > qmlOutput->y()) {
                localY1 = qmlOutput->y();
            }
            if (firstFlag == true || localY2 < qmlOutput->y() + qmlOutput->height()) {
                localY2 = qmlOutput->y() + qmlOutput->height();
            }
            firstFlag = false;
        }
    }

    mX1 = localX1 + (localX2-localX1)/2;
    mY1 = localY1 + (localY2-localY1)/2;

    mX2 = (width() - (localX2 - localX1))/2 + (localX2-localX1)/2;
    mY2 = (height() - (localY2 - localY1))/2 + (localY2-localY1)/2;

    moveX = mX2 - mX1;
    moveY = mY2 - mY1;

    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        qmlOutput->setX(qmlOutput->x() + moveX);
        qmlOutput->setY(qmlOutput->y() + moveY);
    }
}

void QMLScreen::setScreenPos(QMLOutput *output)
{
    // 镜像模式下跳过屏幕旋转处理
    if (this->primaryOutput() && this->primaryOutput()->isCloneMode()) {
        return;
    }

    int x1 = 0, y1 = 0;
    int width1 = 0, height1 = 0;
    int x2 = 0, y2 = 0;
    int width2 = 0, height2 = 0;

    x1 = output->x();
    y1 = output->y();
    width1 = output->width();
    height1 = output->height();

    int connectedScreen = 0;

    QMLOutput *other = NULL;
    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        if (qmlOutput->output()->isConnected()) {
            connectedScreen++;
        }
        if (qmlOutput != output && qmlOutput->output()->isConnected()) {
            other = qmlOutput;
            x2 = other->x();
            y2 = other->y();
            width2 = other->width();
            height2 = other->height();
        }
    }

    if (connectedScreen < 2) {
        setScreenCenterPos();
        return;
    }

    if (!((x1 + width1 == x2)
          || (y1 == y2 + height2)
          || (x1 == x2 + width2)
          || (y1 + height1 == y2))) {
        if (x1 + width1 < x2) {
            output->setX(x2 - width1);
            output->setY(y2);
        } else if (y1 > y2 + height2) {
            output->setX(x2);
            output->setY(y2 + height2);
        } else if (x1 > x2 + width2) {
            output->setX(x2 + width2);
            output->setY(y2);
        } else if (y1 + height1 < y2) {
            output->setX(x2);
            output->setY(y2 - height1);
        }

        // 矩形是否相交
        if (!(x1 + width1 <= x2 || x2 + width2 <= x1
              || y1 >= y2 +height2 || y2 >= y1 + height1)
            && (x1 != x2 || y1 != y2) && other != NULL
            && other->output()->isConnected()) {
            if ((x1 + width1 > x2) && (x1 < x2)) {
                output->setX(x2 - width1);
            } else if ((x1 < x2 + width2) && (x1 + width1 > x2 + width2)) {
                output->setX(x2 + width2);
            } else if ((y1 + height() > y2) && (y1 < y2 + height2)) {
                output->setY(y2 - height1);
            } else if ((y1 < y2  + height2) && (y1 + height1 > y2 + height2)) {
                output->setY(y2 + height2);
            }
        }
    }

    setScreenCenterPos();
}

void QMLScreen::setActiveOutputByCombox(int screenId)
{
    QHash<KScreen::OutputPtr, QMLOutput *>::const_iterator it = m_outputMap.constBegin();
    while (it != m_outputMap.constEnd()) {
        if (screenId == it.key()->id()) {
            setActiveOutput(it.value());
            return;
        }
        it++;
    }
}

QSize QMLScreen::maxScreenSize() const
{
    return m_config->screen()->maxSize();
}

float QMLScreen::outputScale() const
{
    return m_outputScale;
}

void QMLScreen::outputConnectedChanged()
{
    int connectedCount = 0;

    Q_FOREACH (const KScreen::OutputPtr &output, m_outputMap.keys()) {
        if (output->isConnected()) {
            ++connectedCount;
        }
    }

    if (connectedCount != m_connectedOutputsCount) {
        m_connectedOutputsCount = connectedCount;
        Q_EMIT connectedOutputsCountChanged();
        updateOutputsPlacement();
    }
}

void QMLScreen::outputEnabledChanged()
{
    const KScreen::OutputPtr output(qobject_cast<KScreen::Output *>(sender()), [](void *){
        });
    if (output->isEnabled()) {
        updateOutputsPlacement();
    }
    int enabledCount = 0;

    Q_FOREACH (const KScreen::OutputPtr &output, m_outputMap.keys()) {
        if (output->isEnabled()) {
            ++enabledCount;
        }
    }

    if (enabledCount == m_enabledOutputsCount) {
        m_enabledOutputsCount = enabledCount;
        Q_EMIT enabledOutputsCountChanged();
    }
}

void QMLScreen::outputPositionChanged()
{
    /* TODO: Reposition the QMLOutputs */
}

void QMLScreen::qmlOutputMoved(QMLOutput *qmlOutput)
{
    if (qmlOutput->isCloneMode()) {
        return;
    }

    if (!m_manuallyMovedOutputs.contains(qmlOutput))
        m_manuallyMovedOutputs.append(qmlOutput);

    updateCornerOutputs();

    if (m_leftmost) {
        m_leftmost->setOutputX(0);
    }
    if (m_topmost) {
        m_topmost->setOutputY(0);
    }

    if (qmlOutput == m_leftmost) {
        Q_FOREACH (QMLOutput *other, m_outputMap) {
            if (other == m_leftmost) {
                continue;
            }

            if (!other->output()->isConnected() || !other->output()->isEnabled()) {
                continue;
            }

            other->setOutputX(float(other->x() - m_leftmost->x()) / outputScale());
        }
    } else if (m_leftmost) {
        qmlOutput->setOutputX(float(qmlOutput->x() - m_leftmost->x()) / outputScale());
    }

    if (qmlOutput == m_topmost) {
        Q_FOREACH (QMLOutput *other, m_outputMap) {
            if (other == m_topmost) {
                continue;
            }

            if (!other->output()->isConnected() || !other->output()->isEnabled()) {
                continue;
            }

            other->setOutputY(float(other->y() - m_topmost->y()) / outputScale());
        }
    } else if (m_topmost) {
        qmlOutput->setOutputY(float(qmlOutput->y() - m_topmost->y()) / outputScale());
    }
}

void QMLScreen::viewSizeChanged()
{
    updateOutputsPlacement();
    setScreenCenterPos();
}

void QMLScreen::updateCornerOutputs()
{
    m_leftmost = nullptr;
    m_topmost = nullptr;
    m_rightmost = nullptr;
    m_bottommost = nullptr;

    Q_FOREACH (QMLOutput *output, m_outputMap) {
        if (!output->output()->isConnected() || !output->output()->isEnabled()) {
            continue;
        }

        QMLOutput *other = m_leftmost;
        if (!other || output->x() < other->x()) {
            m_leftmost = output;
        }

        if (!other || output->y() < other->y()) {
            m_topmost = output;
        }

        if (!other || output->x() + output->width() > other->x() + other->width()) {
            m_rightmost = output;
        }

        if (!other || output->y() + output->height() > other->y() + other->height()) {
            m_bottommost = output;
        }
    }
}

void QMLScreen::setOutputScale(float scale)
{
    if (qFuzzyCompare(scale, m_outputScale))
        return;
    m_outputScale = scale;
    emit outputScaleChanged();
}

// 画坐标
void QMLScreen::updateOutputsPlacement()
{
    if (width() <= 0)
        return;

    QSizeF initialActiveScreenSize;

    Q_FOREACH (QQuickItem *item, childItems()) {
        QMLOutput *qmlOutput = qobject_cast<QMLOutput *>(item);
        if (!qmlOutput->output()->isConnected() || !qmlOutput->output()->isEnabled()) {
            continue;
        }

        if (qmlOutput->outputX() + qmlOutput->currentOutputWidth()
            > initialActiveScreenSize.width()) {
            initialActiveScreenSize.setWidth(qmlOutput->outputX()
                                             + qmlOutput->currentOutputWidth());
        }
        if (qmlOutput->outputY() + qmlOutput->currentOutputHeight()
            > initialActiveScreenSize.height()) {
            initialActiveScreenSize.setHeight(
                qmlOutput->outputY() + qmlOutput->currentOutputHeight());
        }
    }

    auto initialScale = outputScale();

    auto scale = initialScale;
    qreal lastX = -1.0;
    do {
        auto activeScreenSize = initialActiveScreenSize * scale;

        const QPointF offset((width() - activeScreenSize.width()) / 2.0,
                             (height() - activeScreenSize.height()) / 2.0);

        lastX = -1.0;
        qreal lastY = -1.0;
        Q_FOREACH (QQuickItem *item, childItems()) {
            QMLOutput *qmlOutput = qobject_cast<QMLOutput *>(item);
            if (!qmlOutput->output()->isConnected() || !qmlOutput->output()->isEnabled()
                    || m_manuallyMovedOutputs.contains(qmlOutput)) {
                continue;
            }

            qmlOutput->blockSignals(true);
            qmlOutput->setPosition(QPointF(offset.x() + (qmlOutput->outputX() * scale),
                                           offset.y() + (qmlOutput->outputY() * scale)));
            lastX = qMax(lastX,
                         qmlOutput->position().x() + qmlOutput->width() / initialScale * scale);
            lastY = qMax(lastY, qmlOutput->position().y());
            qmlOutput->blockSignals(false);
        }

        Q_FOREACH (QQuickItem *item, childItems()) {
            QMLOutput *qmlOutput = qobject_cast<QMLOutput *>(item);
            if (qmlOutput->output()->isConnected() && !qmlOutput->output()->isEnabled()
                && !m_manuallyMovedOutputs.contains(qmlOutput)) {
                qmlOutput->blockSignals(true);
                qmlOutput->setPosition(QPointF(lastX, lastY));
                lastX += qmlOutput->width() / initialScale * scale;
                qmlOutput->blockSignals(false);
            }
        }
        // calculate the scale dynamically, so all screens fit to the dialog
        if (lastX > width()) {
            scale *= 0.8;
        }
    } while (lastX > width());

    // Use a timer to avoid binding loop on width()
    QTimer::singleShot(0, this, [scale, this] {
        setOutputScale(scale);
    });
}
