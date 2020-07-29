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
#include "colordialog.h"
#include "ui_colordialog.h"
#include "colorsquare.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    setupInit();
    signalsBind();
}

ColorDialog::~ColorDialog()
{
    qDebug()<<"this is color destructor:" << endl;
    delete ui;
}

void ColorDialog::paintEvent(QPaintEvent *event)
{
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

void ColorDialog::setupInit()
{
    qDebug() << "setup init";
    // 窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);//开启窗口无边框
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Custom color"));
    // 左侧颜色框
    colorSquare = new ColorSquare(this);
    //ui->horizontalLayout_2->setContentsMargins(8,10,8,10);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    colorSquare->setSizePolicy(sizePolicy);
    ui->horizontalLayout_2->addWidget(colorSquare);

    // 垂直渐变滑动条
    gradientSlider = new GradientSlider(this);
    gradientSlider->setOrientation(Qt::Vertical); //设置滑块方向
    ui->horizontalLayout_2->addWidget(gradientSlider);

    // 颜色预览框
    colorPreview = new ColorPreview(this);
    //colorPreview = static_cast<ColorPreview*>(ui->colorPreviewWg);
    colorPreview->setFixedSize(48,48);
    colorPreview->setGeometry(45,345,48,48);

    // spinbox
    // α:
    spinAlpha = new QSpinBox(this);
    spinAlpha->setVisible(false);
    spinAlpha->setMaximum(100);
    sliderAlpha = new GradientSlider(this);
    sliderAlpha->setVisible(false);
    sliderAlpha->setMaximum(100);
    sliderAlpha->setOrientation(Qt::Horizontal);
    // H:
    spinHue = new QSpinBox(this);
    spinHue->setVisible(false);
    spinHue->setMaximum(359);
    sliderHue = new GradientSlider(this);
    sliderHue->setVisible(false);
    sliderHue->setMaximum(359);
    sliderHue->setOrientation(Qt::Horizontal);

    QVector<QColor> rainbow;
    for ( int i = 0; i < 360; i+= 360/6 )
        rainbow.push_back(QColor::fromHsv(i,255,255));
    rainbow.push_back(Qt::red);
    sliderHue->setColors(rainbow);
    // S:
    spinSat = new QSpinBox(this);
    spinSat->setVisible(false);
    spinSat->setMaximum(100);
    sliderSat = new GradientSlider(this);
    sliderSat->setVisible(false);
    sliderSat->setMaximum(100);
    sliderSat->setOrientation(Qt::Horizontal);
    // V:
    spinVal = new QSpinBox(this);
    spinVal->setVisible(false);
    spinVal->setMaximum(100);
    sliderVal = new GradientSlider(this);
    sliderVal->setVisible(false);
    sliderVal->setMaximum(100);
    sliderVal->setOrientation(Qt::Horizontal);
    // R:
    spinRed = ui->spinBox_r;
    spinRed->setMaximum(255);
    sliderRed = new GradientSlider(this);
    sliderRed->setVisible(false);
    sliderRed->setMaximum(255);
    sliderRed->setOrientation(Qt::Horizontal);
    // G:
    spinGreen = ui->spinBox_g;
    spinGreen->setMaximum(255);
    sliderGreen = new GradientSlider(this);
    sliderGreen->setVisible(false);
    sliderGreen->setMaximum(255);
    sliderGreen->setOrientation(Qt::Horizontal);
    // B:
    spinBlue = ui->spinBox_b;
    spinBlue->setMaximum(255);
    sliderBlue = new GradientSlider(this);
    sliderBlue->setVisible(false);
    sliderBlue->setMaximum(255);
    sliderBlue->setOrientation(Qt::Horizontal);

    // CloseBtn
    ui->closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    //取消按钮默认主题灰色背景
    QPalette palette = ui->closeBtn->palette();
    QColor ColorPlaceholderText(255,255,255,0);
    QBrush brush;
    brush.setColor(ColorPlaceholderText);
    palette.setBrush(QPalette::Button, brush);
    palette.setBrush(QPalette::ButtonText, brush);
    //palette.setColor(QPalette::Highlight, Qt::transparent); /* 取消按钮高亮 */
    ui->closeBtn->setPalette(palette);

    // 绘制滑动条
    drawSlider();
}

void ColorDialog::signalsBind()
{
    qDebug() << "signals bind";
    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });
//    connect(ui->cancelBtn,&QPushButton::clicked,this,&ColorDialog::close);
    connect(ui->okBtn,&QPushButton::clicked,this,&ColorDialog::okSlot);

    connect(sliderAlpha,&GradientSlider::valueChanged,spinAlpha,&QSpinBox::setValue);
    connect(spinAlpha,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderAlpha,&GradientSlider::setValue);
    connect(sliderHue,&GradientSlider::valueChanged,spinHue,&QSpinBox::setValue);
    connect(spinHue,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderHue,&GradientSlider::setValue);
    connect(sliderSat,&GradientSlider::valueChanged,spinSat,&QSpinBox::setValue);
    connect(spinSat,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderSat,&GradientSlider::setValue);
    connect(sliderVal,&GradientSlider::valueChanged,spinVal,&QSpinBox::setValue);
    connect(spinVal,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderVal,&GradientSlider::setValue);
    connect(sliderRed,&GradientSlider::valueChanged,spinRed,&QSpinBox::setValue);
    connect(spinRed,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderRed,&GradientSlider::setValue);
    connect(sliderGreen,&GradientSlider::valueChanged,spinGreen,&QSpinBox::setValue);
    connect(spinGreen,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderGreen,&GradientSlider::setValue);
    connect(sliderBlue,&GradientSlider::valueChanged,spinBlue,&QSpinBox::setValue);
    connect(spinBlue,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            sliderBlue,&GradientSlider::setValue);

    connect(sliderHue,&GradientSlider::valueChanged,this,&ColorDialog::SetHsvSlot);
    connect(sliderSat,&GradientSlider::valueChanged,this,&ColorDialog::SetHsvSlot);
    connect(sliderVal,&GradientSlider::valueChanged,this,&ColorDialog::SetHsvSlot);
    connect(sliderRed,&GradientSlider::valueChanged,this,&ColorDialog::setRgbSlot);
    connect(sliderGreen,&GradientSlider::valueChanged,this,&ColorDialog::setRgbSlot);
    connect(sliderBlue,&GradientSlider::valueChanged,this,&ColorDialog::setRgbSlot);

    connect(sliderAlpha,&GradientSlider::valueChanged,this,&ColorDialog::updateWidgetsSlot);
    connect(colorSquare,&ColorSquare::colorSelected,this,&ColorDialog::updateWidgetsSlot);
    connect(this,&ColorDialog::checkedChanged,colorSquare,&ColorSquare::setCheckedColorSlot);

}

void ColorDialog::drawSlider()
{
    qDebug() << "drawSlider";
    QVector<QColor> rainbow;
    for ( int i = 0; i < 360; i+= 360/6 )
        rainbow.push_front(QColor::fromHsv(i,255,255));
    rainbow.push_front(Qt::red);
    gradientSlider->setMaximum(sliderHue->maximum());
    gradientSlider->setColors(rainbow);
    gradientSlider->setValue(sliderHue->value());
    connect(gradientSlider, &GradientSlider::valueChanged, sliderHue, &GradientSlider::setValue);
    emit checkedChanged('H');
}

QColor ColorDialog::color() const
{
    QColor color = colorSquare->color();
    //color.setAlpha(qRound(sliderAlpha->value() * 2.55));
    return color;
}

void ColorDialog::SetColor(QColor color)
{
    qDebug() << "SetColor";
    //colorPreview->setPreviousColor(color);
    //colorSquare->setProperty("color",color);
    colorSquare->setColor(color);
    sliderAlpha->setValue(color.alpha() / 2.55);
    updateWidgetsSlot();
}


/****************Slots*********************/

void ColorDialog::okSlot()
{
    QColor color = this->color();
    emit colorSelected(color);
    this->close();
}

void ColorDialog::setRgbSlot()
{
    qDebug() << "set RGB";
    if(!signalsBlocked())
    {
        QColor color(sliderRed->value(), sliderGreen->value(), sliderBlue->value());
        if(color.saturation() == 0)
        {
            color = QColor::fromHsv(sliderHue->value(), 0, color.value());
        }
        colorSquare->setColor(color);
        updateWidgetsSlot();
    }
}

void ColorDialog::SetHsvSlot()
{
    if(!signalsBlocked())
    {
        colorSquare->setColor(QColor::fromHsv(sliderHue->value(),
                                              qRound(sliderSat->value()*2.55),
                                              qRound(sliderVal->value()*2.55)));
        updateWidgetsSlot();
    }
}

void ColorDialog::updateWidgetsSlot()
{
    blockSignals(true);
    foreach(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(true);

    QColor col = color();

    sliderRed->setValue(col.red());
    spinRed->setValue(sliderRed->value());
    sliderRed->setFirstColor(QColor(0,col.green(),col.blue()));
    sliderRed->setLastColor(QColor(255,col.green(),col.blue()));

    sliderGreen->setValue(col.green());
    spinGreen->setValue(sliderGreen->value());
    sliderGreen->setFirstColor(QColor(col.red(),0,col.blue()));
    sliderGreen->setLastColor(QColor(col.red(),255,col.blue()));

    sliderBlue->setValue(col.blue());
    spinBlue->setValue(sliderBlue->value());
    sliderBlue->setFirstColor(QColor(col.red(),col.green(),0));
    sliderBlue->setLastColor(QColor(col.red(),col.green(),255));

    sliderHue->setValue(qRound(colorSquare->hue()*360.0));
    spinHue->setValue(sliderHue->value());

    sliderSat->setValue(qRound(colorSquare->saturation()*100.0));
    spinSat->setValue(sliderSat->value());
    sliderSat->setFirstColor(QColor::fromHsvF(colorSquare->hue(),0,colorSquare->value()));
    sliderSat->setLastColor(QColor::fromHsvF(colorSquare->hue(),1,colorSquare->value()));

    sliderVal->setValue(qRound(colorSquare->value()*100.0));
    spinVal->setValue(sliderVal->value());
    sliderVal->setFirstColor(QColor::fromHsvF(colorSquare->hue(),colorSquare->saturation(),0));
    sliderVal->setLastColor(QColor::fromHsvF(colorSquare->hue(),colorSquare->saturation(),1));

    colorPreview->setColor(col);

//    QPalette label_palette;
//    label_palette.setColor(QPalette::Background, col);
//    ui->label->setAutoFillBackground(true);
//    ui->label->setPalette(label_palette);
    SetVerticalSlider();

    blockSignals(false);
    foreach(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(false);

    emit colorChanged(col);
}

void ColorDialog::SetVerticalSlider()
{
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderHue, SLOT(setValue(int)));
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderSat, SLOT(setValue(int)));
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderVal, SLOT(setValue(int)));
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderRed, SLOT(setValue(int)));
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderGreen, SLOT(setValue(int)));
    disconnect(gradientSlider, SIGNAL(valueChanged(int)), sliderBlue, SLOT(setValue(int)));


    QVector<QColor> rainbow;
    for ( int i = 0; i < 360; i+= 360/6 )
        rainbow.push_front(QColor::fromHsv(i,255,255));
    rainbow.push_front(Qt::red);
    gradientSlider->setMaximum(sliderHue->maximum());
    gradientSlider->setColors(rainbow);
    gradientSlider->setValue(sliderHue->value());

    connect(gradientSlider, SIGNAL(valueChanged(int)), sliderHue, SLOT(setValue(int)));
    emit checkedChanged('H');
}
