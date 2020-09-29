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
#include "kbdlayoutmanager.h"
#include "ui_layoutmanager.h"
#include "preview/keyboardpainter.h"

#include <QPainter>
#include <QPainterPath>

#include <QDebug>

extern "C" {
#include <libxklavier/xklavier.h>
#include <libmatekbd/matekbd-keyboard-config.h>
}

#define MAXNUM 4
#define KBD_LAYOUTS_SCHEMA "org.mate.peripherals-keyboard-xkb.kbd"
#define KBD_LAYOUTS_KEY "layouts"

XklEngine * engine;
XklConfigRegistry * config_registry;

static void kbd_set_countries(XklConfigRegistry *config_registry, XklConfigItem *config_item, QList<Layout>  *list);

static void kbd_set_languages(XklConfigRegistry *config_registry, XklConfigItem *config_item, QList<Layout>  *list);

static void kbd_set_available_countries(XklConfigRegistry *config_registry, XklConfigItem * parent_config_item, XklConfigItem *config_item, QList<Layout>  *list);

static void kbd_set_available_languages(XklConfigRegistry *config_registry, XklConfigItem * parent_config_item, XklConfigItem *config_item, QList<Layout>  *list);

QList<Layout> languages;
QList<Layout> countries;
QStringList availablelayoutsList;

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

KbdLayoutManager::KbdLayoutManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayoutManager)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Add Layout"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");


    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));
//    ui->PreBtn->setVisible(false);

    ui->variantFrame->setFrameShape(QFrame::Shape::Box);

    configRegistry();

    const QByteArray id(KBD_LAYOUTS_SCHEMA);
    if (QGSettings::isSchemaInstalled(id)){
        kbdsettings = new QGSettings(id);
        setupComponent();
        setupConnect();
    }

}

KbdLayoutManager::~KbdLayoutManager()
{
    delete ui;
    if (QGSettings::isSchemaInstalled(KBD_LAYOUTS_SCHEMA)){
        delete kbdsettings;
    }
}

void KbdLayoutManager::configRegistry(){
    engine = xkl_engine_get_instance (QX11Info::display());
    config_registry = xkl_config_registry_get_instance (engine);

    xkl_config_registry_load (config_registry, false);

    xkl_config_registry_foreach_country(config_registry,(ConfigItemProcessFunc)kbd_set_countries, NULL);

    xkl_config_registry_foreach_language(config_registry,(ConfigItemProcessFunc)kbd_set_languages, NULL);

}

void KbdLayoutManager::setupComponent(){

    ui->countryRadioButton->setChecked(true);

    //设置listwidget无点击
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    rebuildSelectListWidget();
    rebuildVariantCombo();

    rebuild_listwidget();

}

void KbdLayoutManager::setupConnect(){
    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->buttonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=]{
#else
    connect(ui->buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=]{
#endif
        rebuildSelectListWidget();

        rebuildVariantCombo();
    });

    connect(ui->selectListWidget, &QListWidget::currentItemChanged, [=]{
        rebuildVariantCombo();
    });

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->variantComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
#else
    connect(ui->variantComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
#endif
        Q_UNUSED(index)
        if (index != -1)
            installedNoSame();
    });

    connect(ui->installBtn, &QPushButton::clicked, this, [=]{
        QString layout = ui->variantComboBox->currentData().toString();

        QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
        layouts.append(layout);

        kbdsettings->set(KBD_LAYOUTS_KEY, layouts);
        rebuild_listwidget();
    });

    connect(ui->PreBtn, &QPushButton::clicked, this, &KbdLayoutManager::preview);
}

void KbdLayoutManager::installedNoSame(){

    //最多4个布局，来自GTK控制面板，原因未知
    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    if (layouts.length() < MAXNUM  && !layouts.contains(ui->variantComboBox->currentData(Qt::UserRole).toString()))
        ui->installBtn->setEnabled(true);
    else
        ui->installBtn->setEnabled(false);
}

void KbdLayoutManager::rebuildSelectListWidget(){

    ui->selectListWidget->blockSignals(true);
    ui->selectListWidget->clear();
    if (ui->countryRadioButton->isChecked()){
        for (Layout keylayout : countries){
            if (keylayout.name == "TW")
                continue;
            QListWidgetItem * item = new QListWidgetItem(ui->selectListWidget);
            item->setText(keylayout.desc);
            item->setData(Qt::UserRole, keylayout.name);
            ui->selectListWidget->addItem(item);
        }
    } else if (ui->languageRadioButton->isChecked()){
        for (Layout keylayout : languages){
            QListWidgetItem * item = new QListWidgetItem(ui->selectListWidget);
            item->setText(keylayout.desc);
            item->setData(Qt::UserRole, keylayout.name);
            ui->selectListWidget->addItem(item);
        }
    }


    ui->selectListWidget->setCurrentRow(0);
    ui->selectListWidget->blockSignals(false);
}

void KbdLayoutManager::rebuildVariantCombo(){
    QString id = ui->selectListWidget->currentItem()->data(Qt::UserRole).toString();

    availablelayoutsList.clear();
    char * iid = id.toLatin1().data();
    if (ui->countryRadioButton->isChecked())
        kbd_trigger_available_countries(iid);
    else if (ui->languageRadioButton->isChecked())
        kbd_trigger_available_languages(iid);

    ui->variantComboBox->clear();
    for (QString name : availablelayoutsList){
       QString desc = kbd_get_description_by_id(const_cast<const char *>(name.toLatin1().data()));
       ui->variantComboBox->blockSignals(true);
       ui->variantComboBox->addItem(desc, name);
       ui->variantComboBox->blockSignals(false);
    }

    installedNoSame();
}

void KbdLayoutManager::rebuild_listwidget(){
    installedNoSame();

    ui->listWidget->clear();

    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    for (QString layout : layouts){
        QString desc = kbd_get_description_by_id(const_cast<const char *>(layout.toLatin1().data()));

        //自定义widget
        QWidget * layoutWidget = new QWidget();
        layoutWidget->setAttribute(Qt::WA_DeleteOnClose);
//        layoutWidget->setStyleSheet("QWidget{border-bottom: 1px solid #f5f6f7}");
        QHBoxLayout * mainHLayout = new QHBoxLayout(layoutWidget);
        QLabel * layoutLabel = new QLabel(layoutWidget);
        QPushButton * layoutdelBtn = new QPushButton(layoutWidget);
//        layoutdelBtn->setIcon(QIcon("://keyboardcontrol/delete.png"));
        layoutdelBtn->setText(tr("Del"));
//        layoutdelBtn->setStyleSheet(""
//                            "QPushButton{background: #FA6056; border-radius: 2px;}"
//                            "QPushButton:hover:pressed{background: #E54A50; border-radius: 2px;}");

        connect(layoutdelBtn, &QPushButton::clicked, this, [=]{
            QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
            layouts.removeOne(layout);
            kbdsettings->set(KBD_LAYOUTS_KEY, layouts);
            rebuild_listwidget();
        });

        mainHLayout->addWidget(layoutLabel);
        mainHLayout->addStretch();
        mainHLayout->addWidget(layoutdelBtn);
        layoutWidget->setLayout(mainHLayout);

        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setData(Qt::UserRole, layout);
        item->setSizeHint(QSize(ui->listWidget->width(), 50));

        layoutLabel->setText(desc);
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, layoutWidget);
    }

}

void KbdLayoutManager::preview()
{
    QString variantID;
    QString layoutID = ui->variantComboBox->currentData(Qt::UserRole).toString();
    QStringList layList = layoutID.split('\t');

    for (int i = 0; i < layList.length(); i++) {
        if (0 == i) {
            layoutID = layList.at(0);
        }
        if (1 == i) {
            variantID = layList.at(1);
        }
    }

    KeyboardPainter* layoutPreview = new KeyboardPainter();

    qDebug() << " layoutID:"  << layoutID << "variantID:" << variantID <<endl;
    layoutPreview->generateKeyboardLayout(layoutID, variantID, "pc104", "");
    layoutPreview->setModal(true);
    layoutPreview->exec();
}

void KbdLayoutManager::kbd_trigger_available_countries(char *countryid){
    xkl_config_registry_foreach_country_variant (config_registry, countryid, (TwoConfigItemsProcessFunc)kbd_set_available_countries, NULL);
}

void KbdLayoutManager::kbd_trigger_available_languages(char *languageid){
    xkl_config_registry_foreach_language_variant (config_registry, languageid, (TwoConfigItemsProcessFunc)kbd_set_available_languages, NULL);
}

QString KbdLayoutManager::kbd_get_description_by_id(const char *visible){
    char *l, *sl, *v, *sv;
    if (matekbd_keyboard_config_get_descriptions(config_registry, visible, &sl, &l, &sv, &v))
        visible = matekbd_keyboard_config_format_full_layout (l, v);
    return QString(const_cast<char *>(visible));
}

static void kbd_set_countries(XklConfigRegistry *config_registry, XklConfigItem *config_item, QList<Layout> *list){
    Q_UNUSED(config_registry); Q_UNUSED(list);
    Layout item;
    item.desc = config_item->description;
    item.name = config_item->name;

//    qDebug()<<"countries" << "desc = "<<item.desc<<"name = "<<item.name ;

//    list->append(item);
    countries.append(item);
}

static void kbd_set_languages(XklConfigRegistry *config_registry, XklConfigItem *config_item, QList<Layout> *list){
    Q_UNUSED(config_registry); Q_UNUSED(list);
    Layout item;
    item.desc = config_item->description;
    item.name = config_item->name;
//     qDebug()<<"languages" << "desc = "<<item.desc<<"name = "<<item.name;
//    list->append(item);
    languages.append(item);
}

static void kbd_set_available_countries(XklConfigRegistry *config_registry, XklConfigItem * parent_config_item, XklConfigItem *config_item, QList<Layout>  *list){
    Q_UNUSED(config_registry); Q_UNUSED(list);
    const gchar *xkb_id = config_item ? matekbd_keyboard_config_merge_items (parent_config_item->name, config_item->name) : parent_config_item->name;
    availablelayoutsList.append(QString(const_cast<char *>(xkb_id)));
}

static void kbd_set_available_languages(XklConfigRegistry *config_registry, XklConfigItem *parent_config_item, XklConfigItem *config_item, QList<Layout> *list){
    Q_UNUSED(list);
    kbd_set_available_countries(config_registry, parent_config_item, config_item, NULL);
}


void KbdLayoutManager::paintEvent(QPaintEvent *event){
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
