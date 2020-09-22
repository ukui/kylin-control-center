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
#include "showallshortcut.h"
#include "ui_showallshortcut.h"
#include "commonComponent/ImageUtil/imageutil.h"


#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include <QDebug>

#include "realizeshortcutwheel.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gi18n.h>
}

#define TITLEWIDGETHEIGH 36

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ShowAllShortcut::ShowAllShortcut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowAllShortcut)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(tr("Show all shortcut"));

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked) {
        Q_UNUSED(checked)
        close();
    });
}

ShowAllShortcut::~ShowAllShortcut()
{
    delete ui;
}

void ShowAllShortcut::buildComponent(QMap<QString, QMap<QString, QString> > shortcutsMap){
    ui->searchLineEdit->setVisible(false);
    if (ui->scrollArea->widget()){
        ui->scrollArea->takeWidget();
        delete ui->scrollArea->widget();
    }

    QWidget * baseWidget = new QWidget;
    baseWidget->setAttribute(Qt::WA_DeleteOnClose);
//    baseWidget->setFixedWidth(ui->scrollArea->width());
    baseWidget->setFixedWidth(ui->scrollArea->width() - 4);
    baseWidget->setStyleSheet("QWidget{background: palette(base);/* border-radius: 6px;*/ }");

    QVBoxLayout * baseVerLayout = new QVBoxLayout(baseWidget);
    baseVerLayout->setSpacing(0);
    baseVerLayout->setMargin(0);

    QMap<QString, QMap<QString, QString>>::iterator it = shortcutsMap.begin();
    for (; it != shortcutsMap.end(); it++){

        ClickWidget * tWidget;
        if (it.key() == "Desktop")
            tWidget = new ClickWidget(tr("Desktop"));
        else
            tWidget = new ClickWidget(it.key());

        if (it == shortcutsMap.begin()){
//            tWidget->setStyleSheet("ClickWidget{background: #F4F4F4; border-top-left-radius: 6px; border-top-right-radius: 6px;}");
        } else{
//            tWidget->setStyleSheet("ClickWidget{background: #F4F4F4;}");
        }

        QWidget * gWidget = buildGeneralWidget(it.key(), it.value());

        if ((it+1) == shortcutsMap.end())
            connect(tWidget, &ClickWidget::widgetClicked, [=](bool checked){
                gWidget->setVisible(checked);
                if (tWidget->checked()){
//                    tWidget->setStyleSheet("ClickWidget{background: #F4F4F4;}");
                } else {
//                    tWidget->setStyleSheet("ClickWidget{background: #F4F4F4; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}");
                }
            });
        else
            connect(tWidget, &ClickWidget::widgetClicked, [=](bool checked){
                gWidget->setVisible(checked);
            });


        baseVerLayout->addWidget(tWidget);
        baseVerLayout->addWidget(gWidget);
    }
    baseVerLayout->addStretch();

    ui->scrollArea->setWidget(baseWidget);

}

QWidget * ShowAllShortcut::buildTitleWidget(QString tName){
    QWidget * titleWidget = new QWidget;
    titleWidget->setAttribute(Qt::WA_DeleteOnClose);
    titleWidget->setFixedHeight(TITLEWIDGETHEIGH);
//    titleWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

    QHBoxLayout * titleHorLayout = new QHBoxLayout(titleWidget);
    titleHorLayout->setSpacing(0);
    titleHorLayout->setContentsMargins(16, 0, 32, 0);

    QLabel * titleNameLabel = new QLabel(titleWidget);
    if (tName == "Desktop")
        titleNameLabel->setText(tr("Desktop"));
    else
        titleNameLabel->setText(tName);

    QPushButton * directionBtn = new QPushButton(titleWidget);
    directionBtn->setFixedSize(16, 16);
    directionBtn->setCheckable(true);

    titleHorLayout->addWidget(titleNameLabel);
    titleHorLayout->addStretch();
    titleHorLayout->addWidget(directionBtn);

    titleWidget->setLayout(titleHorLayout);

    return titleWidget;
}

QWidget * ShowAllShortcut::buildGeneralWidget(QString schema, QMap<QString, QString> subShortcutsMap){

    GSettingsSchema * pSettings;
    QString domain;

    if (schema == "Desktop"){
        pSettings = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory("/usr/share/glib-2.0/schemas/", g_settings_schema_source_get_default(), FALSE, NULL),
                                                    KEYBINDINGS_DESKTOP_SCHEMA,
                                                    FALSE);
        domain = "ukui-settings-daemon";

    } else if (schema == "System"){
        pSettings = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory("/usr/share/glib-2.0/schemas/", g_settings_schema_source_get_default(), FALSE, NULL),
                                                    KEYBINDINGS_SYSTEM_SCHEMA,
                                                    FALSE);
        domain = "gsettings-desktop-schemas";
    }

    QWidget * pWidget = new QWidget;
    pWidget->setAttribute(Qt::WA_DeleteOnClose);
//    pWidget->setStyleSheet("QWidget{background: #ffffff; border: none;}");
    QVBoxLayout * pVerLayout = new QVBoxLayout(pWidget);
    pVerLayout->setSpacing(2);
    pVerLayout->setMargin(0);

    pWidget->setLayout(pVerLayout);

    QMap<QString, QString>::iterator it = subShortcutsMap.begin();

    for (; it != subShortcutsMap.end(); it++){

        QWidget * gWidget = new QWidget;
        gWidget->setFixedHeight(TITLEWIDGETHEIGH);
        gWidget->setStyleSheet("QWidget{background: palette(button); border: none;}");

        QHBoxLayout * gHorLayout = new QHBoxLayout(gWidget);
        gHorLayout->setSpacing(0);
        gHorLayout->setContentsMargins(16, 0, 32, 0);

        //
        QByteArray ba = domain.toLatin1();
        QByteArray ba1 = it.key().toLatin1();

        GSettingsSchemaKey * keyObj = g_settings_schema_get_key(pSettings, ba1.data());

        char * i18nKey;
        QLabel * nameLabel  = new QLabel(gWidget);
        i18nKey = const_cast<char *>(g_dgettext(ba.data(), g_settings_schema_key_get_summary(keyObj)));
        nameLabel->setText(QString(i18nKey));

        QLabel * bindingLabel = new QLabel(gWidget);
        bindingLabel->setText(it.value());

        gHorLayout->addWidget(nameLabel);
        gHorLayout->addStretch();
        gHorLayout->addWidget(bindingLabel);

        gWidget->setLayout(gHorLayout);


        pVerLayout->addWidget(gWidget);

        g_settings_schema_key_unref(keyObj);
    }

    g_settings_schema_unref(pSettings);

    return pWidget;

}


void ShowAllShortcut::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);

    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();

    qt_blurImage(img, 10, false, false);
    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());
    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}

ClickWidget::ClickWidget(QString name){
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedHeight(TITLEWIDGETHEIGH);

    QHBoxLayout * titleHorLayout = new QHBoxLayout(this);
    titleHorLayout->setSpacing(0);
    titleHorLayout->setContentsMargins(16, 0, 32, 0);

    QLabel * titleNameLabel = new QLabel(this);
    titleNameLabel->setText(name);
//    titleNameLabel->setStyleSheet("background: #F4F4F4;");

    directionBtn = new QPushButton(this);
    directionBtn->setFixedSize(16, 16);
    directionBtn->setCheckable(true);
    directionBtn->setChecked(true);
    directionBtn->setStyleSheet("QPushButton{background: palette(button); border: none;}");

    QPixmap pixUP = ImageUtil::loadSvg(":/img/plugins/shortcut/up.svg", "white", 24);
    QPixmap pixdwon = ImageUtil::loadSvg(":/img/plugins/shortcut/down.svg", "white", 24);

    directionBtn->setIcon(pixUP);
    connect(directionBtn, &QPushButton::toggled, [=](bool checked){
        if (checked) {
            directionBtn->setIcon(pixUP);
        } else {
            directionBtn->setIcon(pixdwon);
        }
    });

    connect(directionBtn, &QPushButton::clicked, this, &ClickWidget::widgetClicked);

    titleHorLayout->addWidget(titleNameLabel);
    titleHorLayout->addStretch();
    titleHorLayout->addWidget(directionBtn);

    setLayout(titleHorLayout);
}

bool ClickWidget::checked(){
    return directionBtn->isChecked();
}

void ClickWidget::mousePressEvent(QMouseEvent *e){
    Q_UNUSED(e)

    directionBtn->setChecked(!directionBtn->isChecked());

    emit widgetClicked(directionBtn->isChecked());
}

void ClickWidget::paintEvent(QPaintEvent *e){
    Q_UNUSED(e)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

