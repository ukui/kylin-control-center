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

#include "qmloutputcomponent.h"
#include "qmloutput.h"
#include "qmlscreen.h"

#include <KF5/KScreen/kscreen/output.h>

#include <QDir>
#include <QStandardPaths>
#include <QQmlEngine>
#include <QDebug>
#include <KF5/KScreen/kscreen/types.h>

//这里注册的类型为什么不起作用
Q_DECLARE_METATYPE(KScreen::OutputPtr)
Q_DECLARE_METATYPE(QMLScreen*)

QMLOutputComponent::QMLOutputComponent(QQmlEngine *engine, QMLScreen *parent):
    QQmlComponent(engine, parent),
    m_engine(engine)
{
    //const QString qmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kcm_kscreen/qml/Output.qml"));
//    const QString qmlpath = QStringLiteral("qrc:/qml/Output.qml");
//    qDebug()<<"qmlPath is ------>"<<qmlpath<<endl;
    loadUrl(QUrl("qrc:/qml/Output.qml"));

}

QMLOutputComponent::~QMLOutputComponent()
{
}

QMLOutput* QMLOutputComponent::createForOutput(const KScreen::OutputPtr &output)
{
    QObject *instance = beginCreate(m_engine->rootContext());
    if (!instance) {
        qWarning() << errorString();
        return nullptr;
    }

    bool success = instance->setProperty("outputPtr", QVariant::fromValue(qobject_cast<KScreen::OutputPtr>(output)));
//    qDebug()<<"qmloutcomponent.cpp---->instance类型---->"<<instance<<
//              "\noutput类型---------->"<<output<<" "<<
//              "\noutput原始类型------->"<<QVariant::fromValue(qobject_cast<KScreen::OutputPtr>(output))<<" "<<endl;

    Q_ASSERT(success);
    success = instance->setProperty("screen", QVariant::fromValue(qobject_cast<QMLScreen*>(parent())));
    Q_ASSERT(success);
    Q_UNUSED(success);

    completeCreate();

    return qobject_cast<QMLOutput*>(instance);
}
