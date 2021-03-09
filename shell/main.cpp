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
#include <QSharedPointer>
#include <memory>
#include <QCommandLineParser>
#include <QQuickWidget>
#include <QStandardPaths>
#include <QTime>

#include <stdlib.h>

#include "framelessExtended/framelesshandle.h"
#include "customstyle.h"
#include "utils/utils.h"
#include "utils/xatom-helper.h"

const QString KLong = "Loongson";

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    bool showDebug = true;
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ukui-control-center.log";
    if (!QFile::exists(logFilePath)) {
        showDebug = false;
    }
    FILE *log_file = nullptr;

    if (showDebug) {
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file)
        fclose(log_file);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageOutput);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (Utils::getCpuInfo().startsWith(KLong, Qt::CaseInsensitive)) {
        QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QString id = QString("ukui-control-center" + QLatin1String(getenv("DISPLAY")));
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
        QCoreApplication::setApplicationName(QObject::tr("ukui-control-center"));
        QCoreApplication::setApplicationVersion("2.0");

        QCommandLineParser parser;
        Utils::setCLIName(parser);
        parser.process(a);

        MainWindow w;
        Utils::centerToScreen(&w);

        MotifWmHints hints;
        hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_BORDER;
        XAtomHelper::getInstance()->setWindowMotifHint(w.winId(), hints);

        a.setActivationWindow(&w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)), &w, SLOT(sltMessageReceived(const QString&)));
        w.show();

        return a.exec();
    }
}

