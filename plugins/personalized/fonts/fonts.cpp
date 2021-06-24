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
#include "fonts.h"
#include "ui_fonts.h"

#include <QLabel>
#include <QStringList>
#include <QDebug>
#include <QStandardItemModel>

#define N 3
#define SMALL 1.00
#define MEDIUM 1.25
#define LARGE 1.50


#define INTERFACE_SCHEMA   "org.mate.interface"
#define DOC_FONT_KEY       "document-font-name"  // 用于阅读文档的默认字体的名称
#define GTK_FONT_KEY       "font-name"           // gkt+使用的默认字体
#define MONOSPACE_FONT_KEY "monospace-font-name" // 用于终端等处的等宽字体

#define MARCO_SCHEMA       "org.gnome.desktop.wm.preferences"
#define TITLEBAR_FONT_KEY  "titlebar-font"       // 描述窗口标题栏字体的字符串。只有在"titlebar-uses-system-font"为false时有效

#define STYLE_FONT_SCHEMA  "org.ukui.style"
#define SYSTEM_FONT_EKY    "system-font-size"
#define SYSTEM_NAME_KEY    "system-font"

#define FONT_RENDER_SCHEMA "org.ukui.font-rendering"
#define ANTIALIASING_KEY   "antialiasing" // 绘制字形时使用反锯齿类型
#define HINTING_KEY        "hinting"      // 绘制字形时使用微调的类型
#define RGBA_ORDER_KEY     "rgba-order"   // LCD屏幕上次像素的顺序；仅在反锯齿设为"rgba"时有用
#define DPI_KEY            "dpi"          // 将字体尺寸转换为像素值时所用的分辨率，以每英寸点数为单位

QList<int> defaultsizeList =    {6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
const QStringList fontList{"CESI仿宋-GB13000", "CESI仿宋-GB18030", "CESI仿宋-GB2312", "CESI宋体-GB13000",
                           "CESI宋体-GB18030", "CESI宋体-GB2312", "CESI小标宋-GB13000", "CESI小标宋-GB18030",
                           "CESI小标宋-GB2312", "CESI楷体-GB13000", "CESI楷体-GB18030", "CESI楷体-GB2312",
                           "CESI黑体-GB13000", "CESI黑体-GB18030", "CESI黑体-GB2312""仿宋", "黑体", "楷体", "宋体",
                           "华文彩云", "华文仿宋", "华文琥珀", "华文楷体", "华文隶书", " 华文宋体", "华文细黑", "华文行楷", "华文新魏",
                           "Noto Sans CJK SC", "Noto Sans CJK SC Black", "Noto Sans Mono CJK SC", "Noto Sans CJK SC DemiLight"
                           "Noto Sans CJK SC Light", "Noto Sans CJK SC Medium", "Noto Sans CJK SC", "Noto Sans CJK SC Thin"};

Fonts::Fonts() : mFirstLoad(true)
{
    pluginName = tr("Fonts");
    pluginType = PERSONALIZED;
}

Fonts::~Fonts()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
    }
}

QString Fonts::get_plugin_name() {
    return pluginName;
}

int Fonts::get_plugin_type() {
    return pluginType;
}

QWidget *Fonts::get_plugin_ui() {
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::Fonts;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        initModel();
        initSearchText();
        setupStylesheet();

        const QByteArray styleID(STYLE_FONT_SCHEMA);
        const QByteArray id(INTERFACE_SCHEMA);
        const QByteArray idd(MARCO_SCHEMA);
        const QByteArray iid(FONT_RENDER_SCHEMA);

        if (QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(iid) && QGSettings::isSchemaInstalled(idd) &&
                QGSettings::isSchemaInstalled(styleID)) {

            marcosettings = new QGSettings(idd, QByteArray(), this);
            ifsettings = new QGSettings(id, QByteArray(), this);
            rendersettings = new QGSettings(iid, QByteArray(), this);
            stylesettings = new QGSettings(styleID, QByteArray(), this);

            setupComponent();
            setupConnect();
            initFontStatus();
        }
    }
    return pluginWidget;
}

void Fonts::plugin_delay_control() {

}

const QString Fonts::name() const {
    return QStringLiteral("fonts");
}

void Fonts::initSearchText() {
    //~ contents_path /fonts/Font size
    ui->fontSizeLabel->setText(tr("Font size"));
    //~ contents_path /fonts/Fonts select
    ui->fontSizeLabel->setText(tr("Fonts select"));
    //~ contents_path /fonts/Font size
    ui->fontSizeLabel->setText(tr("Font size"));
}

void Fonts::setupStylesheet(){
}

void Fonts::setupComponent(){

    QStringList fontScale;
    fontScale<< tr("11") << tr("12") << tr("13") << tr("14") << tr("15")
              <<tr("16");

    uslider  = new Uslider(fontScale);
    uslider->setRange(1,6);
    uslider->setTickInterval(1);
    uslider->setPageStep(1);

    ui->fontLayout->addWidget(uslider);

}

void Fonts::setupConnect(){
    connectToServer();
    connect(uslider, &QSlider::valueChanged, [=](int value){
        int size = sliderConvertToSize(value);

        // 获取当前字体信息
        _getCurrentFontInfo();

        // 设置字体大小
        ifsettings->set(GTK_FONT_KEY, QVariant(QString("%1 %2").arg(gtkfontStrList.at(0)).arg(size)));
        ifsettings->set(DOC_FONT_KEY, QVariant(QString("%1 %2").arg(docfontStrList.at(0)).arg(size)));
        ifsettings->set(MONOSPACE_FONT_KEY, QVariant(QString("%1 %2").arg(monospacefontStrList.at(0)).arg(size)));
        stylesettings->set(SYSTEM_FONT_EKY, QVariant(QString("%1").arg(size)));
        marcosettings->set(TITLEBAR_FONT_KEY, QVariant(QString("%1 %2").arg(titlebarfontStrList.at(0)).arg(size)));
        fontKwinSlot();
    });

    connect(ui->fontSelectComBox, &QComboBox::currentTextChanged, [=](QString text){
        // 获取当前字体信息
        _getCurrentFontInfo();
        ifsettings->set(GTK_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(gtkfontStrList.at(1))));
        ifsettings->set(DOC_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(docfontStrList.at(1))));        
        stylesettings->set(SYSTEM_NAME_KEY, QVariant(QString("%1").arg(text)));
        marcosettings->set(TITLEBAR_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(titlebarfontStrList.at(1))));

        fontKwinSlot();
    });
    connect(ui->monoSelectComBox, &QComboBox::currentTextChanged, [=](QString text){
        // 获取当前字体信息
        _getCurrentFontInfo();
        ifsettings->set(MONOSPACE_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(monospacefontStrList.at(1))));
    });

    // 重置按钮
    connect(ui->resetBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        resetDefault();
    });
}

void Fonts::initFontStatus(){
    // 初始化字体ComBox状态
    initGeneralFontStatus();
}

void Fonts::initGeneralFontStatus(){
    // 获取当前字体信息
    _getCurrentFontInfo();

    // 初始化字体选择ComBox(选择gtk默认字体作为标准)
    QString currentfonts = gtkfontStrList[0];
    ui->fontSelectComBox->blockSignals(true);
    ui->fontSelectComBox->setCurrentText(currentfonts);
    ui->fontSelectComBox->blockSignals(false);

    // 初始化等宽字体ComBox
    QString currentmonofont = monospacefontStrList[0];
    if ("DejaVu sans Mono" == currentmonofont) {
        currentmonofont = "DejaVu Sans Mono";
    }

    ui->monoSelectComBox->blockSignals(true);
    ui->monoSelectComBox->setCurrentText(currentmonofont);
    ui->monoSelectComBox->blockSignals(false);

    int size = fontConvertToSlider(QString(docfontStrList[1]).toInt());

    uslider->blockSignals(true);
    uslider->setValue(size);
    uslider->blockSignals(false);
}

void Fonts::_getCurrentFontInfo(){
    gtkfontStrList = _splitFontNameSize(ifsettings->get(GTK_FONT_KEY).toString());
    docfontStrList = _splitFontNameSize(ifsettings->get(DOC_FONT_KEY).toString());
    monospacefontStrList = _splitFontNameSize(ifsettings->get(MONOSPACE_FONT_KEY).toString());
    titlebarfontStrList = _splitFontNameSize(marcosettings->get(TITLEBAR_FONT_KEY).toString());
}

QStringList Fonts::_splitFontNameSize(QString value) {
    QStringList valueStringList;
    QString str = value.right(1);
    QString ch0 = static_cast<QString>('0');
    QString ch9 = static_cast<QString>('9');
    if (str >= ch0 && str <= ch9) {
        QStringList tmpStringList = value.split(' ');
        QString::SectionFlag flag = QString::SectionSkipEmpty;
        valueStringList << value.section(' ', 0, tmpStringList.length() - 2, flag);
        valueStringList << value.section(' ', tmpStringList.length() - 1, tmpStringList.length() - 1, flag);
    } else {
        valueStringList << value << "10";
    }
    return valueStringList;
}

int Fonts::fontConvertToSlider(const int size) const {
    switch (size) {
    case 11:
        return 1;
        break;
    case 12:
        return 2;
        break;
    case 13:
        return 3;
        break;
    case 14:
        return 4;
        break;
    case 15:
        return 5;
        break;
    case 16:
        return 6;
        break;
    case 17:
        return 7;
        break;
    case 18:
        return 8;
        break;
    default:
        return 1;
        break;
    }
}

int Fonts::sliderConvertToSize(const int value) const
{
    switch (value) {
    case 1:
        return 11;
        break;
    case 2:
        return 12;
        break;
    case 3:
        return 13;
        break;
    case 4:
        return 14;
        break;
    case 5:
        return 15;
        break;
    case 6:
        return 16;
        break;
    case 7:
        return 17;
        break;
    case 8:
        return 18;
    default:
        return 11;
        break;
    }
}

void Fonts::resetDefault(){
    // Reset font
    ifsettings->reset(GTK_FONT_KEY);
    ifsettings->reset(DOC_FONT_KEY);
    ifsettings->reset(MONOSPACE_FONT_KEY);
    marcosettings->reset(TITLEBAR_FONT_KEY);
    stylesettings->set(SYSTEM_FONT_EKY, 11);
    stylesettings->reset(SYSTEM_NAME_KEY);

    // Reset font render
    rendersettings->reset(ANTIALIASING_KEY);
    rendersettings->reset(HINTING_KEY);

    // 更新全部状态
    initFontStatus();
    fontKwinSlot();
}

void Fonts::connectToServer(){
    m_cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!m_cloudInterface->isValid()) {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "keyChanged", this, SLOT(keyChangedSlot(QString)));
    // 将以后所有DBus调用的超时设置为 milliseconds
    m_cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void Fonts::keyChangedSlot(const QString &key) {
    if(key == "font") {
        initFontStatus();
    }
}

void Fonts::fontKwinSlot() {
    const int fontSize = sliderConvertToSize(uslider->value());
    const QString fontType = ui->fontSelectComBox->currentText();
    qDebug() << fontSize << fontType;
    QDBusMessage message =QDBusMessage::createSignal("/KGlobalSettings", "org.kde.KGlobalSettings", "slotFontChange");
    QList<QVariant> args;
    args.append(fontSize);
    args.append(fontType);
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);
}

void Fonts::initModel()
{
    ui->fontSelectComBox->setModel(new QStandardItemModel());
    QStandardItemModel *fontModel = dynamic_cast<QStandardItemModel *>(ui->fontSelectComBox->model());

    ui->monoSelectComBox->setModel(new QStandardItemModel());
    QStandardItemModel *monoModel = dynamic_cast<QStandardItemModel *>(ui->monoSelectComBox->model());


    // 导入系统字体列表
    QStringList fontfamiles = fontdb.families();
    for (QString fontValue : fontfamiles) {
        if (fontList.contains(fontValue)) {
            QStandardItem *standardItem = new QStandardItem(fontValue);
            standardItem->setFont(QFont(fontValue));
            fontModel->appendRow(standardItem);
        }

        if (fontValue.contains("Mono") && !fontValue.contains("Ubuntu",Qt::CaseInsensitive)) {
            QStandardItem *monoItem = new QStandardItem(fontValue);
            monoItem->setFont(QFont(fontValue));
            monoModel->appendRow(monoItem);
        }
    }
}
