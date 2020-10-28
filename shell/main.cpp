/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "mainwindow.h"

#include <QApplication>
#include <QtSingleApplication>
#include <QTranslator>
#include <QObject>
#include <QDebug>
#include <QGSettings>
#include <QSharedPointer>
#include <memory>
#include <QCommandLineParser>

#include <stdlib.h>

#include "framelessExtended/framelesshandle.h"
#include "customstyle.h"
#include "utils/utils.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QString id = QString(QLatin1String(getenv("DISPLAY")));
    QtSingleApplication a(id, argc, argv);

    if (a.isRunning()) {
        a.sendMessage(QApplication::arguments().length() > 1 ? QApplication::arguments().at(1) : a.applicationFilePath());
        qDebug() << QObject::tr("ukui-control-center is already running!");
        return EXIT_SUCCESS;
    } else {
        // 加载国际化文件
        QTranslator translator;
        translator.load("/usr/share/ukui-control-center/shell/res/i18n/" + QLocale::system().name());
        a.installTranslator(&translator);

        // 命令行参数
        QCoreApplication::setApplicationName("ukui-control-center");
        QCoreApplication::setApplicationVersion("2.0");

        QCommandLineParser parser;
        Utils::setCLIName(parser);
//        parser.process(a);

        MainWindow * w = new MainWindow;
        Utils::centerToScreen(w);
        w->setAttribute(Qt::WA_DeleteOnClose);

        a.setActivationWindow(w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)),w, SLOT(sltMessageReceived(const QString&)));
        w->show();

        FramelessHandle * pHandle = new FramelessHandle(w);
        pHandle->activateOn(w);

        return a.exec();
    }
}

