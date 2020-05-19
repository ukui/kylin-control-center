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

#include <QDebug>

#define N 3
#define SMALL 1.00
#define MEDIUM 1.25
#define LARGE 1.50


#define INTERFACE_SCHEMA "org.mate.interface"
#define DOC_FONT_KEY "document-font-name" //用于阅读文档的默认字体的名称
#define GTK_FONT_KEY "font-name" //gkt+使用的默认字体
#define MONOSPACE_FONT_KEY "monospace-font-name" //用于终端等处的等宽字体

#define MARCO_SCHEMA "org.gnome.desktop.wm.preferences"
#define TITLEBAR_FONT_KEY "titlebar-font" //描述窗口标题栏字体的字符串。只有在"titlebar-uses-system-font"为false时有效


//#define PEONY_SCHEMA "org.ukui.peony.desktop"
//#define PEONY_FONT_KEY "font"  //桌面上图标描述所用的字体

#define STYLE_FONT_SCHEMA "org.ukui.style"
#define SYSTEM_FONT_EKY "system-font-size"

#define FONT_RENDER_SCHEMA           "org.ukui.font-rendering"
#define ANTIALIASING_KEY        "antialiasing" //绘制字形时使用反锯齿类型
#define HINTING_KEY             "hinting" //绘制字形时使用微调的类型
#define RGBA_ORDER_KEY          "rgba-order" //LCD屏幕上次像素的顺序；仅在反锯齿设为"rgba"时有用
#define DPI_KEY                 "dpi" //将字体尺寸转换为像素值时所用的分辨率，以每英寸点数为单位

/*
  设置字体，每套字体包括5个部件，应用程序字体、文档字体、等宽字体、桌面字体和窗口标题字体。
  字体设置为预设值，每套字体除大小不固定，其他字体类别固定。
*/

typedef struct _FontInfo FontInfo;
struct _FontInfo{
    QString type;
    QString gtkfont;
    QString docfont;
    QString monospacefont;
//    QString peonyfont;
    QString titlebarfont;
    int gtkfontsize;
    int docfontsize;
    int monospacefontsize;
//    int peonyfontsize;
    int titlebarfontsize;
};

FontInfo defaultfontinfo;

//字体效果
typedef enum{
    ANTIALIASING_NONE,
    ANTIALIASING_GRAYSCALE,
    ANTIALIASING_RGBA
}Antialiasing;

typedef enum{
    HINT_NONE,
    HINT_SLIGHT,
    HINT_MEDIUM,
    HINT_FULL
}Hinting;

struct FontEffects : QObjectUserData {
    Antialiasing antial;
    Hinting hinting;
};

QList<int> defaultsizeList = {6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};

Fonts::Fonts()
{
    ui = new Ui::Fonts;
    itemDelege = new QStyledItemDelegate();
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Fonts");
    pluginType = PERSONALIZED;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    settingsCreate = false;

    setupStylesheet();


    //    const QByteArray iddd(PEONY_SCHEMA);
    //    peonysettings = new QGSettings(iddd);

    const QByteArray styleID(STYLE_FONT_SCHEMA);
    const QByteArray id(INTERFACE_SCHEMA);
    const QByteArray idd(MARCO_SCHEMA);
    const QByteArray iid(FONT_RENDER_SCHEMA);

    if (QGSettings::isSchemaInstalled(id) && QGSettings::isSchemaInstalled(iid) && QGSettings::isSchemaInstalled(idd) &&
            QGSettings::isSchemaInstalled(styleID)){
        settingsCreate = true;
        marcosettings = new QGSettings(idd);
        ifsettings = new QGSettings(id);
        rendersettings = new QGSettings(iid);
        stylesettings = new QGSettings(styleID);


        _getDefaultFontinfo();
        setupComponent();
        setupConnect();
        initFontStatus();
    }

}

Fonts::~Fonts()
{
    delete ui;
    if (settingsCreate){
        delete ifsettings;
        delete marcosettings;
    //    delete peonysettings;
        delete rendersettings;
        delete stylesettings;
    }

}

QString Fonts::get_plugin_name(){
    return pluginName;
}

int Fonts::get_plugin_type(){
    return pluginType;
}

QWidget *Fonts::get_plugin_ui(){
    return pluginWidget;
}

void Fonts::plugin_delay_control(){

}

void Fonts::setupStylesheet(){

    ui->titleSecondLabel->setVisible(false);
    ui->sampleBtn1->setVisible(false);
    ui->sampleBtn2->setVisible(false);
    ui->sampleBtn3->setVisible(false);
    ui->sampleBtn4->setVisible(false);

//    pluginWidget->setStyleSheet("background: #ffffff;");
//    ui->fontSizeWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 4px;}");
//    ui->fontSizeLabel->setStyleSheet("QLabel{background: #F4F4F4;}");

//    ui->fontTypeWidget->setStyleSheet("QWidget#fontTypeWidget{background: #F4F4F4; border-radius: 4px;}");
//    ui->fontSelectLabel->setStyleSheet("QLabel{background: #F4F4F4;}");
//    ui->monoSelectLabel->setStyleSheet("QLabel{background: #F4F4F4;}");

//    ui->fontSizeSlider->setStyleSheet("QSlider{height: 20px;}"
//                                      "QSlider::groove:horizontal{border: none;}"
//                                      "QSlider::add-page:horizontal{background: #808080; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
//                                      "QSlider::sub-page:horizontal{background: #3D6BE5; border-radius: 2px; margin-top: 8px; margin-bottom: 9px;}"
//                                      "QSlider::handle:horizontal{width: 20px; height: 20px; border-image: url(:/img/plugins/fonts/bigRoller.png);}"
//                                      "");


//    ui->advancedBtn->setStyleSheet("QPushButton#advancedBtn{background: #F4F4F4; border: none; border-radius: 4px;}");

//    ui->advancedWidget->setStyleSheet("QWidget#advancedWidget{background: #F4F4F4; border-radius: 4px;}");
//    ui->defaultFontLabel->setStyleSheet("QLabel{background: #F4F4F4;}");
//    ui->monoFontLabel->setStyleSheet("QLabel{background: #F4F4F4;}");
//    ui->docFontLabel->setStyleSheet("QLabel{background: #F4F4F4;}");
//    ui->peonyFontLabel->setStyleSheet("QLabel{background: #F4F4F4;}");
//    ui->titleFontLabel->setStyleSheet("QLabel{background: #F4F4F4;}");


//    ui->sampleBtn1->setStyleSheet("QPushButton{background: #F0F0F0; border-radius: 4px;}"
//                                  "QPushButton:checked{background: #F0F3FD; border-radius: 4px;}");
//    ui->sampleBtn2->setStyleSheet("QPushButton{background: #F0F0F0; border-radius: 4px;}"
//                                  "QPushButton:checked{background: #F0F3FD; border-radius: 4px;}");
//    ui->sampleBtn3->setStyleSheet("QPushButton{background: #F0F0F0; border-radius: 4px;}"
//                                  "QPushButton:checked{background: #F0F3FD; border-radius: 4px;}");
//    ui->sampleBtn4->setStyleSheet("QPushButton{background: #F0F0F0; border-radius: 4px;}"
//                                  "QPushButton:checked{background: #F0F3FD; border-radius: 4px;}");
}

void Fonts::setupComponent(){
    ui->fontSelectComBox->setItemDelegate(itemDelege);
    ui->fontSelectComBox->setMaxVisibleItems(5);

    ui->monoSelectComBox->setItemDelegate(itemDelege);
    ui->monoSelectComBox->setMaxVisibleItems(5);

    ui->defaultFontComBox->setItemDelegate(itemDelege);
    ui->defaultFontComBox->setMaxVisibleItems(5);

    ui->docFontComBox->setItemDelegate(itemDelege);
    ui->docFontComBox->setMaxVisibleItems(5);

    ui->peonyFontComBox->setItemDelegate(itemDelege);
    ui->peonyFontComBox->setMaxVisibleItems(5);

    ui->monoFontComBox->setItemDelegate(itemDelege);
    ui->monoFontComBox->setMaxVisibleItems(5);

    ui->titleFontComBox->setItemDelegate(itemDelege);
    ui->titleFontComBox->setMaxVisibleItems(5);

    ui->defaultSizeComBox->setItemDelegate(itemDelege);
    ui->defaultSizeComBox->setMaxVisibleItems(5);

    ui->docSizeComBox->setItemDelegate(itemDelege);
    ui->docSizeComBox->setMaxVisibleItems(5);

    ui->MonoSizeComBox->setItemDelegate(itemDelege);
    ui->MonoSizeComBox->setMaxVisibleItems(5);

    ui->peonySizeComBox->setItemDelegate(itemDelege);
    ui->peonySizeComBox->setMaxVisibleItems(5);

    ui->titleSizeComBox->setItemDelegate(itemDelege);
    ui->titleSizeComBox->setMaxVisibleItems(5);

    ui->fontSizeSlider->setMinimum(100);
    ui->fontSizeSlider->setMaximum(275);
    ui->fontSizeSlider->setSingleStep(25);
    ui->fontSizeSlider->setPageStep(25);
    ui->fontSizeSlider->setTickPosition(QSlider::TicksBelow);

    //导入系统字体列表
    QStringList fontfamiles = fontdb.families();
    for (QString font : fontfamiles){
        ////通用设置
        //字体
        ui->fontSelectComBox->addItem(font);
        //等宽字体
        ui->monoSelectComBox->addItem(font);
        ////高级设置
        // gtk default
        ui->defaultFontComBox->addItem(font);
        //doc font
        ui->docFontComBox->addItem(font);
        // peony font
        ui->peonyFontComBox->addItem(font);
        //monospace font
        ui->monoFontComBox->addItem(font);
        //title font
        ui->titleFontComBox->addItem(font);
    }

    ////导入字体大小列表
    //获取当前字体
    QStringList gtkfontStrList = _splitFontNameSize(ifsettings->get(GTK_FONT_KEY).toString());
    QStringList docfontStrList = _splitFontNameSize(ifsettings->get(DOC_FONT_KEY).toString());
    QStringList monospacefontStrList = _splitFontNameSize(ifsettings->get(MONOSPACE_FONT_KEY).toString());
//    QStringList peonyfontStrList = _splitFontNameSize(peonysettings->get(PEONY_FONT_KEY).toString());
    QStringList titlebarfontStrList = _splitFontNameSize(marcosettings->get(TITLEBAR_FONT_KEY).toString());

    QList<int> gtksizeList = fontdb.pointSizes(gtkfontStrList.at(0));
    QList<int> docsizeList = fontdb.pointSizes(docfontStrList.at(0));
    QList<int> monosizeList = fontdb.pointSizes(monospacefontStrList.at(0));
//    QList<int> peonysizeList = fontdb.pointSizes(peonyfontStrList.at(0));
    QList<int> titlesizeList = fontdb.pointSizes(titlebarfontStrList.at(0));

    if (gtksizeList.length() == 0)
        gtksizeList = defaultsizeList;
    for (int size : gtksizeList)
        ui->defaultSizeComBox->addItem(QString::number(size));
    if (docsizeList.length() == 0)
        docsizeList = defaultsizeList;
    for (int size : docsizeList)
        ui->docSizeComBox->addItem(QString::number(size));
    if (monosizeList.length() == 0)
        monosizeList = defaultsizeList;
    for (int size : monosizeList)
        ui->MonoSizeComBox->addItem(QString::number(size));
//    if (peonysizeList.length() == 0)
//        peonysizeList = defaultsizeList;
//    for (int size : peonysizeList)
//        ui->peonySizeComBox->addItem(QString::number(size));
    if (titlesizeList.length() == 0)
        titlesizeList = defaultsizeList;
    for (int size : titlesizeList)
        ui->titleSizeComBox->addItem(QString::number(size));

    //设置高级配置是否显示
    ui->advancedFrame->setVisible(ui->advancedBtn->isChecked());

    //init sample button
    FontEffects * example1 = new FontEffects();
    FontEffects * example2 = new FontEffects();
    FontEffects * example3 = new FontEffects();
    FontEffects * example4 = new FontEffects();
    example1->antial = ANTIALIASING_NONE; example1->hinting = HINT_FULL;
    example2->antial = ANTIALIASING_GRAYSCALE; example2->hinting = HINT_FULL;
    example3->antial = ANTIALIASING_GRAYSCALE; example3->hinting = HINT_MEDIUM;
    example4->antial = ANTIALIASING_RGBA; example4->hinting = HINT_FULL;

    ui->sampleBtn1->setUserData(Qt::UserRole, example1);
    ui->sampleBtn2->setUserData(Qt::UserRole, example2);
    ui->sampleBtn3->setUserData(Qt::UserRole, example3);
    ui->sampleBtn4->setUserData(Qt::UserRole, example4);

//    setSampleButton(ui->sampleBtn1);

}

void Fonts::setSampleButton(QPushButton *button){
    QVBoxLayout * base1VerLayout = new QVBoxLayout(button);
    base1VerLayout->setSpacing(12);
    base1VerLayout->setMargin(0);
//    QFont font;
//    font.setHintingPreference(QFont::PreferNoHinting);
    QLabel * enLabel = new QLabel(button);
    enLabel->setText("Thanks For Using The ukcc");
    enLabel->setAlignment(Qt::AlignCenter);
    QLabel * cnLabel = new QLabel(button);
    cnLabel->setText(tr("Thanks For Using The ukcc"));
    cnLabel->setAlignment(Qt::AlignCenter);
    base1VerLayout->addWidget(enLabel);
    base1VerLayout->addWidget(cnLabel);
    button->setLayout(base1VerLayout);
}

void Fonts::setupConnect(){
    connect(ui->fontSizeSlider, &QSlider::sliderReleased, [=]{
        int value = ui->fontSizeSlider->value();

        int setup = value / 25; int overage = value % 25;

        if (overage > 12)
            ui->fontSizeSlider->setSliderPosition(ui->fontSizeSlider->singleStep() * (setup + 1));
        else
            ui->fontSizeSlider->setSliderPosition(ui->fontSizeSlider->singleStep() * setup);
    });
    connect(ui->fontSizeSlider, &QSlider::valueChanged, [=](int value){
        if (!(value % 25)){
            float level = (float)value / 100;
//            qDebug() << "font is---->" << level<<" "<<monospacefontStrList.at(0)<<" "<<defaultfontinfo.monospacefontsize * level<<endl;
            //获取当前字体信息
            _getCurrentFontInfo();
            //设置字体大小


            ifsettings->set(GTK_FONT_KEY, QVariant(QString("%1 %2").arg(gtkfontStrList.at(0)).arg(defaultfontinfo.gtkfontsize * level)));
            ifsettings->set(DOC_FONT_KEY, QVariant(QString("%1 %2").arg(docfontStrList.at(0)).arg(defaultfontinfo.docfontsize * level)));
            ifsettings->set(MONOSPACE_FONT_KEY, QVariant(QString("%1 %2").arg(monospacefontStrList.at(0)).arg(defaultfontinfo.monospacefontsize * level)));
            stylesettings->set(SYSTEM_FONT_EKY, QVariant(QString("%1").arg(defaultfontinfo.monospacefontsize * level)));
//            peonysettings->set(PEONY_FONT_KEY, QVariant(QString("%1 %2").arg(peonyfontStrList[0]).arg(defaultfontinfo.monospacefontsize * level)));
            marcosettings->set(TITLEBAR_FONT_KEY, QVariant(QString("%1 %2").arg(titlebarfontStrList.at(0)).arg(defaultfontinfo.titlebarfontsize * level)));

        }

    });

    connect(ui->fontSelectComBox, &QComboBox::currentTextChanged, [=](QString text){
        //获取当前字体信息
        _getCurrentFontInfo();
        ifsettings->set(GTK_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(gtkfontStrList.at(1))));
        ifsettings->set(DOC_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(docfontStrList.at(1))));
//        peonysettings->set(PEONY_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(peonyfontStrList.at(1))));
        marcosettings->set(TITLEBAR_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(titlebarfontStrList.at(1))));

        //给更新高级字体配置
        initAdvancedFontStatus();

    });
    connect(ui->monoSelectComBox, &QComboBox::currentTextChanged, [=](QString text){
        //获取当前字体信息
        _getCurrentFontInfo();
        ifsettings->set(MONOSPACE_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(monospacefontStrList.at(1))));
        //更新高级字体配置
        initAdvancedFontStatus();

    });

    //高级字体按钮状态
    connect(ui->advancedBtn, &QPushButton::clicked, this, [=]{
        ui->advancedFrame->setVisible(ui->advancedBtn->isChecked());
    });

    connect(ui->defaultFontComBox, &QComboBox::currentTextChanged, [=](QString text){
        _getCurrentFontInfo();
        ifsettings->set(GTK_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(gtkfontStrList.at(1))));
    });
    connect(ui->docFontComBox, &QComboBox::currentTextChanged, [=](QString text){
        _getCurrentFontInfo();
        ifsettings->set(DOC_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(docfontStrList.at(1))));
    });
    connect(ui->monoFontComBox, &QComboBox::currentTextChanged, [=](QString text){
        _getCurrentFontInfo();
        ifsettings->set(MONOSPACE_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(monospacefontStrList.at(1))));
    });

    connect(ui->titleFontComBox, &QComboBox::currentTextChanged, [=](QString text){
        _getCurrentFontInfo();
        marcosettings->set(TITLEBAR_FONT_KEY, QVariant(QString("%1 %2").arg(text).arg(titlebarfontStrList.at(1))));
    });

    ////绑定信号
    //字体效果按钮
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    connect(ui->sampleBtnGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){
#else
    connect(ui->sampleBtnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [=](QAbstractButton * button){
#endif
        setFontEffect(button);
    });
    //重置按钮
    connect(ui->resetBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        resetDefault();
    });
}

void Fonts::initFontStatus(){
    //初始化字体ComBox状态
    initGeneralFontStatus();

    //初始化高级字体ComBox状态
    initAdvancedFontStatus();

    //初始化示例按钮状态
    initSampleFontStatus();
}

void Fonts::initGeneralFontStatus(){
    //获取当前字体信息
    _getCurrentFontInfo();

    //初始化字体选择ComBox(选择gtk默认字体作为标准)
    QString currentfonts = gtkfontStrList[0];
    ui->fontSelectComBox->blockSignals(true);
    ui->fontSelectComBox->setCurrentText(currentfonts);
    ui->fontSelectComBox->blockSignals(false);

    //初始化等宽字体ComBox
    QString currentmonofont = monospacefontStrList[0];
    if ("DejaVu sans Mono" == currentmonofont) {
        currentmonofont = "DejaVu Sans Mono";
    }
    ui->monoSelectComBox->blockSignals(true);
    ui->monoSelectComBox->setCurrentText(currentmonofont);
    ui->monoSelectComBox->blockSignals(false);

    //获取字体大小倍率,选择文档字体大小作为标准，来自gtk控制面板的逻辑
    float res = QString(docfontStrList[1]).toFloat() / (float)defaultfontinfo.docfontsize;
    int value = int(res * 100);

    ui->fontSizeSlider->blockSignals(true);

    ui->fontSizeSlider->setValue(value);

    ui->fontSizeSlider->blockSignals(false);

}

void Fonts::initAdvancedFontStatus(){
    //获取当前字体信息
    _getCurrentFontInfo();

    //阻塞信号
    ui->defaultFontComBox->blockSignals(true);
    ui->docFontComBox->blockSignals(true);
    ui->monoFontComBox->blockSignals(true);
//    ui->peonyFontComBox->blockSignals(true);
    ui->titleFontComBox->blockSignals(true);

    //初始化高级字体ComBox
    ui->defaultFontComBox->setCurrentText(gtkfontStrList.at(0));
    ui->docFontComBox->setCurrentText(docfontStrList.at(0));
    ui->monoFontComBox->setCurrentText(monospacefontStrList.at(0));
//    ui->peonyFontComBox->setCurrentText(peonyfontStrList.at(0));
    ui->titleFontComBox->setCurrentText(titlebarfontStrList.at(0));

    //释放信号
    ui->defaultFontComBox->blockSignals(false);
    ui->docFontComBox->blockSignals(false);
    ui->monoFontComBox->blockSignals(false);
//    ui->peonyFontComBox->blockSignals(false);
    ui->titleFontComBox->blockSignals(false);

    //阻塞字体大小ComBox信号
    ui->defaultSizeComBox->blockSignals(true);
    ui->docSizeComBox->blockSignals(true);
    ui->MonoSizeComBox->blockSignals(true);
//    ui->peonySizeComBox->blockSignals(true);
    ui->titleSizeComBox->blockSignals(true);

    ui->defaultSizeComBox->setCurrentText(gtkfontStrList.at(1));
    ui->docSizeComBox->setCurrentText(docfontStrList.at(1));
    ui->MonoSizeComBox->setCurrentText(monospacefontStrList.at(1));
//    ui->peonySizeComBox->setCurrentText(peonyfontStrList.at(1));
    ui->titleSizeComBox->setCurrentText(titlebarfontStrList.at(1));

    //阻塞字体大小ComBox信号
    ui->defaultSizeComBox->blockSignals(false);
    ui->docSizeComBox->blockSignals(false);
    ui->MonoSizeComBox->blockSignals(false);
//    ui->peonySizeComBox->blockSignals(false);
    ui->titleSizeComBox->blockSignals(false);

}

void Fonts::initSampleFontStatus(){
    //初始化字体效果按钮
    GSettings * settings = g_settings_new(FONT_RENDER_SCHEMA);
    int currentantial = g_settings_get_enum(settings, ANTIALIASING_KEY);
    int currenthinting = g_settings_get_enum(settings, HINTING_KEY);
    QList<QAbstractButton *> buttonsList = ui->sampleBtnGroup->buttons();
    for (int num = 0; num < buttonsList.size(); num++){
        FontEffects * btnFontEffects = (FontEffects *)((QPushButton *)buttonsList[num])->userData(Qt::UserRole);
        if (currentantial == btnFontEffects->antial && currenthinting == btnFontEffects->hinting){
            QPushButton * button = ((QPushButton *)buttonsList[num]);
            button->blockSignals(true);
            button->setChecked(true);
            button->blockSignals(false);
        }
    }
    g_object_unref(settings);
}

void Fonts::_getDefaultFontinfo(){
    GVariant * value;
    const char * font_value;
    gsize size; gint length;

    //GTK FONT
    GSettings * ifgsettings;
    ifgsettings = g_settings_new(INTERFACE_SCHEMA);
    value = g_settings_get_default_value(ifgsettings, GTK_FONT_KEY);
    size = g_variant_get_size(value);
    font_value = g_variant_get_string(value, &size);
    length = (gint)strlen(font_value);
    if (font_value[length-2] == ' '){
        defaultfontinfo.gtkfontsize = atoi(&font_value[length - 1]);
    }
    else
        defaultfontinfo.gtkfontsize = atoi(&font_value[length - 2]);
    g_variant_unref(value);

    //DOC FONT
    value = g_settings_get_default_value(ifgsettings, DOC_FONT_KEY);
    size = g_variant_get_size(value);
    font_value = g_variant_get_string(value, &size);
    length = (gint)strlen(font_value);
    if (font_value[length -2] == ' ')
        defaultfontinfo.docfontsize = atoi(&font_value[length -1]);
    else
        defaultfontinfo.docfontsize = atoi(&font_value[length -2]);
    g_variant_unref(value);

    //MON
    value = g_settings_get_default_value(ifgsettings, MONOSPACE_FONT_KEY);
    size = g_variant_get_size(value);
    font_value = g_variant_get_string(value, &size);
    length = (gint)strlen(font_value);
    if (font_value[length -2] == ' ')
        defaultfontinfo.monospacefontsize = atoi(&font_value[length -1]);
    else
        defaultfontinfo.monospacefontsize = atoi(&font_value[length -2]);
    g_variant_unref(value);

    //PEONY
//    GSettings * peonygsettings;
//    peonygsettings = g_settings_new(PEONY_SCHEMA);
//    value = g_settings_get_default_value(peonygsettings, PEONY_FONT_KEY);
//    size = g_variant_get_size(value);
//    font_value = g_variant_get_string(value, &size);
//    length = (gint)strlen(font_value);
//    if (font_value[length -2] == ' ')
//        defaultfontinfo.peonyfontsize = atoi(&font_value[length -1]);
//    else
//        defaultfontinfo.peonyfontsize = atoi(&font_value[length -2]);
//    g_variant_unref(value);

    //TITLE
    GSettings * marcogsettings;
    marcogsettings = g_settings_new(MARCO_SCHEMA);
    value = g_settings_get_default_value(marcogsettings, TITLEBAR_FONT_KEY);
    size = g_variant_get_size(value);
    font_value = g_variant_get_string(value, &size);
    length = (gint)strlen(font_value);
    if (font_value[length -2] == ' ')
        defaultfontinfo.titlebarfontsize = atoi(&font_value[length -1]);
    else
        defaultfontinfo.titlebarfontsize = atoi(&font_value[length -2]);
    g_variant_unref(value);

    g_object_unref(ifgsettings);
//    g_object_unref(peonygsettings);
    g_object_unref(marcogsettings);
}

void Fonts::_getCurrentFontInfo(){
    gtkfontStrList = _splitFontNameSize(ifsettings->get(GTK_FONT_KEY).toString());
    docfontStrList = _splitFontNameSize(ifsettings->get(DOC_FONT_KEY).toString());
    monospacefontStrList = _splitFontNameSize(ifsettings->get(MONOSPACE_FONT_KEY).toString());
//    peonyfontStrList = _splitFontNameSize(peonysettings->get(PEONY_FONT_KEY).toString());
    titlebarfontStrList = _splitFontNameSize(marcosettings->get(TITLEBAR_FONT_KEY).toString());
}

QStringList Fonts::_splitFontNameSize(QString value){
    QStringList valueStringList;
    QString str = value.right(1);
    QString ch0 = static_cast<QString>('0');
    QString ch9 = static_cast<QString>('9');
    if (str >= ch0 && str <= ch9){
        QStringList tmpStringList = value.split(' ');
        QString::SectionFlag flag = QString::SectionSkipEmpty;
        valueStringList << value.section(' ', 0, tmpStringList.length() - 2, flag);
        valueStringList << value.section(' ', tmpStringList.length() - 1, tmpStringList.length() - 1, flag);
    }
    else{
        valueStringList << value << "10";
    }
    return valueStringList;
}

void Fonts::setFontEffect(QAbstractButton *button){
    QPushButton * btnclicked = (QPushButton *)button;
    FontEffects * setFontEffects = (FontEffects *)(btnclicked->userData(Qt::UserRole));

    GSettings * settings = g_settings_new(FONT_RENDER_SCHEMA);
    g_settings_set_enum(settings, ANTIALIASING_KEY, setFontEffects->antial);
    g_settings_set_enum(settings, HINTING_KEY, setFontEffects->hinting);
    g_object_unref(settings);
}

void Fonts::resetDefault(){
    //reset font
    ifsettings->reset(GTK_FONT_KEY);
    ifsettings->reset(DOC_FONT_KEY);
    ifsettings->reset(MONOSPACE_FONT_KEY);
//    peonysettings->reset(PEONY_FONT_KEY);
    marcosettings->reset(TITLEBAR_FONT_KEY);
    stylesettings->reset(SYSTEM_FONT_EKY);

    //reset font render
    rendersettings->reset(ANTIALIASING_KEY);
    rendersettings->reset(HINTING_KEY);

    //更新全部状态
    initFontStatus();
}
