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
#include "ui_kbdlayoutmanager.h"

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


KbdLayoutManager::KbdLayoutManager(QStringList ll, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KbdLayoutManager),
    layoutsList(ll)
{
    ui->setupUi(this);

    engine = xkl_engine_get_instance (QX11Info::display());
    config_registry = xkl_config_registry_get_instance (engine);

    xkl_config_registry_load (config_registry, false);

    xkl_config_registry_foreach_country(config_registry,(ConfigItemProcessFunc)kbd_set_countries, NULL);

    xkl_config_registry_foreach_language(config_registry,(ConfigItemProcessFunc)kbd_set_languages, NULL);

    const QByteArray id(KBD_LAYOUTS_SCHEMA);
    kbdsettings = new QGSettings(id);

    component_init();
    setup_component();

}

KbdLayoutManager::~KbdLayoutManager()
{
    delete ui;
    delete kbdsettings;
}

void KbdLayoutManager::component_init(){
    ui->countryRadioButton->setChecked(true);

    // init listwidget intalled
    //设置listwidget无点击
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    rebuild_listwidget();
//    for (QString layout : layoutsList){
//        create_listwidgetitem(layout);
//        QString desc = kbd_get_description_by_id(const_cast<const char *>(layout.toLatin1().data()));
//        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
//        item->setText(desc);
//        item->setData(Qt::UserRole, layout);
//        ui->listWidget->addItem(item);
//    }

    // init country comboBox
    for (Layout keylayout : countries){
        ui->countryComboBox->addItem(keylayout.desc, keylayout.name);
    }

    // init language comboBox
    for (Layout keylayout : languages){
        ui->languageComboBox->addItem(keylayout.desc, keylayout.name);
    }

    refresh_widget_status();
}

void KbdLayoutManager::setup_component(){
    connect(ui->countryComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(countries_changed_slot(int)));
    connect(ui->languageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(languages_changed_slot(int)));
    connect(this, SIGNAL(rebuild_variant_signals(bool,QString)), this, SLOT(rebuild_variant_slots(bool,QString)));

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->buttonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=]{refresh_variant_combobox();refresh_widget_status();});
#else
    connect(ui->buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=]{refresh_variant_combobox();refresh_widget_status();});
#endif


    connect(ui->addBtn, &QPushButton::clicked, [=]{emit add_new_variant_signals(ui->variantComboBox->currentData(Qt::UserRole).toString());});

    connect(this, &KbdLayoutManager::add_new_variant_signals, [=](QString id){add_layout(id);});

    connect(this, &KbdLayoutManager::del_variant_signals, [=](QString id){delete_layout(id);});

    refresh_variant_combobox();
}

void KbdLayoutManager::rebuild_listwidget(){
    //最多4个布局，来自GTK控制面板，原因未知
    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    if (layouts.length() >= MAXNUM)
        ui->addBtn->setEnabled(false);
    else
        ui->addBtn->setEnabled(true);

    ui->listWidget->clear();

    for (QString layout : layouts){
        QString desc = kbd_get_description_by_id(const_cast<const char *>(layout.toLatin1().data()));

        //自定义widget
        QWidget * layoutWidget = new QWidget();
        layoutWidget->setAttribute(Qt::WA_DeleteOnClose);
//        layoutWidget->setStyleSheet("QWidget{border-bottom: 1px solid #f5f6f7}");
        QHBoxLayout * mainHLayout = new QHBoxLayout(layoutWidget);
        QLabel * layoutLabel = new QLabel(layoutWidget);
        QPushButton * layoutdelBtn = new QPushButton(layoutWidget);
        layoutdelBtn->setIcon(QIcon("://keyboardcontrol/delete.png"));
        layoutdelBtn->setText(tr("Del"));
//        layoutdelBtn->setStyleSheet(""
//                            "QPushButton{background: #FA6056; border-radius: 2px;}"
//                            "QPushButton:hover:pressed{background: #E54A50; border-radius: 2px;}");

        connect(layoutdelBtn, &QPushButton::clicked, this, [=]{emit del_variant_signals(layout);});

        mainHLayout->addWidget(layoutLabel);
        mainHLayout->addStretch();
        mainHLayout->addWidget(layoutdelBtn);
        layoutWidget->setLayout(mainHLayout);

        QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
        item->setData(Qt::UserRole, layout);
        item->setSizeHint(QSize(328, 36));  //330 - 2

        layoutLabel->setText(desc);
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, layoutWidget);
    }

}

void KbdLayoutManager::add_layout(QString layout){
    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    layouts.append(layout);
    kbdsettings->set(KBD_LAYOUTS_KEY, layouts);
    rebuild_listwidget();
}

void KbdLayoutManager::delete_layout(QString layout){
    QStringList layouts = kbdsettings->get(KBD_LAYOUTS_KEY).toStringList();
    layouts.removeOne(layout);
    kbdsettings->set(KBD_LAYOUTS_KEY, layouts);
    rebuild_listwidget();
}

void KbdLayoutManager::refresh_variant_combobox(){

    if (ui->countryRadioButton->isChecked()){
        QString id = ui->countryComboBox->currentData(Qt::UserRole).toString();
        emit rebuild_variant_signals(true, id);
    }
    else{
        QString id = ui->languageComboBox->currentData(Qt::UserRole).toString();
        emit rebuild_variant_signals(false, id);
    }
}

void KbdLayoutManager::refresh_widget_status(){
    ui->countryWidget->setEnabled(ui->countryRadioButton->isChecked());
    ui->languageWidget->setEnabled(ui->languageRadioButton->isChecked());
}

void KbdLayoutManager::rebuild_variant_combobox(){
    ui->variantComboBox->clear();
    for (QString name : availablelayoutsList){
       QString desc = kbd_get_description_by_id(const_cast<const char *>(name.toLatin1().data()));
       ui->variantComboBox->addItem(desc, name);
    }
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

void KbdLayoutManager::countries_changed_slot(int index){
    Q_UNUSED(index);
    QString id = ui->countryComboBox->currentData().toString();
    emit rebuild_variant_signals(true, id);
}

void KbdLayoutManager::languages_changed_slot(int index){
    Q_UNUSED(index);
    QString id = ui->languageComboBox->currentData().toString();
    emit rebuild_variant_signals(false, id);
}

void KbdLayoutManager::rebuild_variant_slots(bool type, QString id){
    availablelayoutsList.clear();
    char * iid = id.toLatin1().data();
    if (type)
        kbd_trigger_available_countries(iid);
    else
        kbd_trigger_available_languages(iid);
    rebuild_variant_combobox();
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
