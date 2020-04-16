#ifndef UKMEDIAINPUTWIDGET_H
#define UKMEDIAINPUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "ukmedia_output_widget.h"
#include <QStyledItemDelegate>
#include <QFrame>

class UkmediaInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaInputWidget(QWidget *parent = nullptr);
    ~UkmediaInputWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QWidget *m_pInputWidget;
    QFrame *m_pInputDeviceWidget;
    QFrame *m_pVolumeWidget;
    QFrame *m_pInputLevelWidget;
    QLabel *m_pInputLabel;
    QLabel *m_pInputDeviceLabel;
    QLabel *m_pIpVolumeLabel;
    QLabel *m_pInputLevelLabel;
    QLabel *m_pLowLevelLabel;
    QLabel *m_pHighLevelLabel;
    QLabel *m_pIpVolumePercentLabel;
    QPushButton *m_pInputIconBtn;
    AudioSlider *m_pIpVolumeSlider;
    AudioSlider *m_pInputLevelSlider;
    QComboBox *m_pInputDeviceCombobox;

    QStyledItemDelegate *itemDelege;
    QString sliderQss;
//    QString
};

#endif // UKMEDIAINPUTWIDGET_H
