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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <QObject>
#include <QDesktopWidget>

#include "framelessExtended/framelesshandle.h"

#include <QDebug>


void centerToScreen(QWidget* widget) {
    if (!widget)
      return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtSingleApplication a(argc, argv);
    if (a.isRunning()){
        a.sendMessage(a.applicationFilePath());
        qDebug() << QObject::tr("ukui-control-center had already running!");
        return EXIT_SUCCESS;
    } else {
        //加载国际化文件
        QString locale = QLocale::system().name();
        QTranslator translator;
        if (locale == "zh_CN"){
            if (translator.load("zh_CN.qm", "://i18n/"))
                a.installTranslator(&translator);
            else
                qDebug() << "Load translations file" << locale << "failed!";
        }

        //加载qss样式文件
        QString qss;
        QFile QssFile("://global.qss");
        QssFile.open(QFile::ReadOnly);

        if (QssFile.isOpen()){
            qss = QLatin1String(QssFile.readAll());
            qApp->setStyleSheet(qss);
            QssFile.close();
        }


        MainWindow * w = new MainWindow;
        centerToScreen(w);
        w->setAttribute(Qt::WA_DeleteOnClose);

        a.setActivationWindow(w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)),w, SLOT(sltMessageReceived(const QString&)));
        w->show();

        FramelessHandle * pHandle = new FramelessHandle(w);
        pHandle->activateOn(w);
        return a.exec();
    }
}

