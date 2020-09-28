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

QMLScreen::QMLScreen(QQuickItem *parent)
    : QQuickItem(parent)
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
                addOutput(output);
                updateOutputsPlacement();
            });
    connect(m_config.data(), &KScreen::Config::outputRemoved,
            this, &QMLScreen::removeOutput);
    //qDebug()<<"所要拿取的配置为------>"<<m_config<<endl;
    for (const KScreen::OutputPtr &output : m_config->outputs()) {
//        qDebug()<<"\noutput类型----debug------>"<<output<<" "<<
//                  "\noutput原始类型------->"<<QVariant::fromValue(qobject_cast<KScreen::OutputPtr>(output))<<" "<<endl;
        addOutput(output);
    }

    updateOutputsPlacement();

    for (QMLOutput *qmlOutput : m_outputMap) {
        if (qmlOutput->output()->isConnected() && qmlOutput->output()->isEnabled()) {
            //qDebug()<<"qmlOutput---->"<<qmlOutput<<endl;
            qmlOutput->dockToNeighbours();
        }
    }
}


void QMLScreen::addOutput(const KScreen::OutputPtr &output)
{
    //qDebug()<<"qmlscreen.cpp-------> output类型------->"<<output<<endl;
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
    //在这里点击上面小屏幕
    connect(qmloutput, SIGNAL(clicked()),
            this, SLOT(setActiveOutput()));

    qmloutput->updateRootProperties();
}

void QMLScreen::removeOutput(int outputId)
{
    for (const KScreen::OutputPtr &output : m_outputMap.keys()) {
        if (output->id() == outputId) {
            QMLOutput *qmlOutput = m_outputMap.take(output);
            qmlOutput->setParentItem(nullptr);
            qmlOutput->setParent(nullptr);
            qmlOutput->deleteLater();
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

QList<QMLOutput*> QMLScreen::outputs() const
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
    //选中屏幕
    output->setFocus(true);
    Q_EMIT focusedOutputChanged(output);
}

QSize QMLScreen::maxScreenSize() const
{
    //qDebug()<<"m_config->screen()->maxSize()--->"<<m_config->screen()->maxSize()<<endl;
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
    const KScreen::OutputPtr output(qobject_cast<KScreen::Output*>(sender()), [](void *){});
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
    //qDebug()<<"qmlOutputMoved------>"<<endl;
    if (qmlOutput->isCloneMode()) {
        return;
    }
    if (!m_manuallyMovedOutputs.contains(qmlOutput))
        m_manuallyMovedOutputs.append(qmlOutput);

    updateCornerOutputs();

    if ((qmlOutput->x() + qmlOutput->width()) > 550) {
//        qDebug()<<qmlOutput->x();
        qmlOutput->setPosition(QPointF(550 - qmlOutput->width(), qmlOutput->y()));
    }
    if ((qmlOutput->y() + qmlOutput->height()) > 190) {
//        qDebug()<<qmlOutput->y();
        qmlOutput->setPosition(QPointF(qmlOutput->x(), 190 - qmlOutput->height()));
    }

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

//应该是画坐标的地方?
void QMLScreen::updateOutputsPlacement()
{
    //qDebug()<<"updateOutputsPlacement---->"<<endl;
    if (width() <= 0)
        return;

    QSizeF initialActiveScreenSize;

    Q_FOREACH (QQuickItem *item, childItems()) {
        QMLOutput *qmlOutput = qobject_cast<QMLOutput*>(item);
        if (!qmlOutput->output()->isConnected() || !qmlOutput->output()->isEnabled()) {
            continue;
        }

        if (qmlOutput->outputX() + qmlOutput->currentOutputWidth() > initialActiveScreenSize.width()) {
           // qDebug()<<qmlOutput->outputX()<<qmlOutput->currentOutputWidth()<<initialActiveScreenSize.width()<<endl;
            initialActiveScreenSize.setWidth(qmlOutput->outputX() + qmlOutput->currentOutputWidth());
        }
        if (qmlOutput->outputY() + qmlOutput->currentOutputHeight() > initialActiveScreenSize.height()) {
            initialActiveScreenSize.setHeight(qmlOutput->outputY() + qmlOutput->currentOutputHeight());
        }
    }

    auto initialScale = outputScale();

    //qDebug() << " -----debug0--->outputScale" << initialScale;
    auto scale = initialScale;
    qreal lastX = -1.0;
    do {
        auto activeScreenSize = initialActiveScreenSize * scale;

        const QPointF offset((width() - activeScreenSize.width()) / 2.0,
                             (height() - activeScreenSize.height()) / 2.0);

       // qDebug() << " ----------debug1--->offset-->" << offset;

        lastX = -1.0;
        qreal lastY = -1.0;
        Q_FOREACH (QQuickItem *item, childItems()) {
            QMLOutput *qmlOutput = qobject_cast<QMLOutput*>(item);
            if (!qmlOutput->output()->isConnected() || !qmlOutput->output()->isEnabled() ||
                 m_manuallyMovedOutputs.contains(qmlOutput)) {
                continue;
            }

            qmlOutput->blockSignals(true);
            qmlOutput->setPosition(QPointF(offset.x() + (qmlOutput->outputX() * scale),
                              offset.y() + (qmlOutput->outputY() * scale)));
            lastX = qMax(lastX, qmlOutput->position().x() + qmlOutput->width() / initialScale * scale);
            lastY = qMax(lastY, qmlOutput->position().y());
           // qDebug()<<"坐标---->"<<lastX<<" "<<lastY<<endl;
            qmlOutput->blockSignals(false);
        }

        Q_FOREACH (QQuickItem *item, childItems()) {
            QMLOutput *qmlOutput = qobject_cast<QMLOutput*>(item);
            if (qmlOutput->output()->isConnected() && !qmlOutput->output()->isEnabled() &&
                !m_manuallyMovedOutputs.contains(qmlOutput)) {
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
