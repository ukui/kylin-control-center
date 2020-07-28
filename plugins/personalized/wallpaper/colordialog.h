#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QDebug>

#include "colorsquare.h"
#include "gradientslider.h"
#include "ui_colordialog.h"
#include "colorpreview.h"

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
    ~ColorDialog();

public:
    QColor color() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::ColorDialog     *ui;

    ColorSquare         *colorSquare;
    GradientSlider      *gradientSlider;

    enum CheckedColor
    {
        H,S,V,R,G,B
    }checkedColor;

    ColorPreview*       colorPreview;

    QSpinBox*           spinAlpha;
    GradientSlider*     sliderAlpha;

    QRadioButton*       radioHue;
    QSpinBox*           spinHue;
    GradientSlider*     sliderHue;

    QRadioButton*       radioSat;
    QSpinBox*           spinSat;
    GradientSlider*     sliderSat;

    QRadioButton*       radioVal;
    QSpinBox*           spinVal;
    GradientSlider*     sliderVal;

    QRadioButton*       radioRed;
    QSpinBox*           spinRed;
    GradientSlider*     sliderRed;
    QRadioButton*       radioGreen;
    QSpinBox*           spinGreen;
    GradientSlider*     sliderGreen;
    QRadioButton*       radioBlue;
    QSpinBox*           spinBlue;
    GradientSlider*     sliderBlue;

    QHBoxLayout*        horizontalLayout_3;

    void setupInit();
    void signalsBind();
    void drawSlider();
    void SetVerticalSlider();
    void SetColor(QColor color);

private slots:
    void updateWidgetsSlot();
    void setRgbSlot();
    void SetHsvSlot();

signals:
    void colorChanged(QColor);
    void checkedChanged(char);
};

#endif // COLORDIALOG_H
