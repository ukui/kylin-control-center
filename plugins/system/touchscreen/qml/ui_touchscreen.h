/********************************************************************************
** Form generated from reading UI file 'touchscreen.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOUCHSCREEN_H
#define UI_TOUCHSCREEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TouchScreen
{
public:
    QFrame *screenframe;
    QHBoxLayout *horizontalLayout;
    QLabel *monitorLabel;
    QComboBox *monitorCombo;
    QFrame *screenframe_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *touchLabel;
    QComboBox *touchscreenCombo;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QPushButton *mapButton;
    QPushButton *CalibrationButton;
    QSpacerItem *horizontalSpacer_4;
    QLabel *touchscreenLabel;
    QLabel *touchnameContent;

    void setupUi(QWidget *TouchScreen)
    {
        if (TouchScreen->objectName().isEmpty())
            TouchScreen->setObjectName(QString::fromUtf8("TouchScreen"));
        TouchScreen->resize(917, 349);
        screenframe = new QFrame(TouchScreen);
        screenframe->setObjectName(QString::fromUtf8("screenframe"));
        screenframe->setGeometry(QRect(0, 110, 741, 50));
        screenframe->setMinimumSize(QSize(550, 50));
        screenframe->setMaximumSize(QSize(960, 50));
        screenframe->setFrameShape(QFrame::Box);
        horizontalLayout = new QHBoxLayout(screenframe);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        monitorLabel = new QLabel(screenframe);
        monitorLabel->setObjectName(QString::fromUtf8("monitorLabel"));
        monitorLabel->setMinimumSize(QSize(118, 30));
        monitorLabel->setMaximumSize(QSize(118, 30));

        horizontalLayout->addWidget(monitorLabel);

        monitorCombo = new QComboBox(screenframe);
        monitorCombo->setObjectName(QString::fromUtf8("monitorCombo"));
        monitorCombo->setMinimumSize(QSize(200, 0));
        monitorCombo->setMaximumSize(QSize(16777215, 30));
        monitorCombo->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(monitorCombo);

        screenframe_2 = new QFrame(TouchScreen);
        screenframe_2->setObjectName(QString::fromUtf8("screenframe_2"));
        screenframe_2->setGeometry(QRect(0, 170, 741, 50));
        screenframe_2->setMinimumSize(QSize(550, 50));
        screenframe_2->setMaximumSize(QSize(960, 50));
        screenframe_2->setFrameShape(QFrame::Box);
        horizontalLayout_2 = new QHBoxLayout(screenframe_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        touchLabel = new QLabel(screenframe_2);
        touchLabel->setObjectName(QString::fromUtf8("touchLabel"));
        touchLabel->setMinimumSize(QSize(118, 30));
        touchLabel->setMaximumSize(QSize(118, 30));

        horizontalLayout_2->addWidget(touchLabel);

        touchscreenCombo = new QComboBox(screenframe_2);
        touchscreenCombo->setObjectName(QString::fromUtf8("touchscreenCombo"));
        touchscreenCombo->setMinimumSize(QSize(200, 0));
        touchscreenCombo->setMaximumSize(QSize(16777215, 30));
        touchscreenCombo->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_2->addWidget(touchscreenCombo);

        layoutWidget = new QWidget(TouchScreen);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 230, 741, 38));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        mapButton = new QPushButton(layoutWidget);
        mapButton->setObjectName(QString::fromUtf8("mapButton"));
        mapButton->setMinimumSize(QSize(120, 36));
        mapButton->setMaximumSize(QSize(16777215, 36));
        mapButton->setLayoutDirection(Qt::LeftToRight);


        CalibrationButton = new QPushButton(layoutWidget);
        CalibrationButton->setObjectName(QString::fromUtf8("CalibrationButton"));
        CalibrationButton->setMinimumSize(QSize(120, 36));
        CalibrationButton->setMaximumSize(QSize(16777215, 36));
        CalibrationButton->setLayoutDirection(Qt::LeftToRight);
        CalibrationButton->setGeometry(QRect(100,100,100,22));

        formLayout->setWidget(0, QFormLayout::LabelRole, mapButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        formLayout->setItem(0, QFormLayout::FieldRole, horizontalSpacer_4);

        touchscreenLabel = new QLabel(TouchScreen);
        touchscreenLabel->setObjectName(QString::fromUtf8("touchscreenLabel"));
        touchscreenLabel->setGeometry(QRect(0, 0, 913, 22));

        touchnameContent = new QLabel(TouchScreen);
        touchnameContent->setObjectName(QString::fromUtf8("touchnameContent"));
        touchnameContent->setGeometry(QRect(0, 230, 431, 22));

        QWidget::setTabOrder(monitorCombo, touchscreenCombo);
        QWidget::setTabOrder(touchscreenCombo, mapButton);

        retranslateUi(TouchScreen);

        QMetaObject::connectSlotsByName(TouchScreen);
    } // setupUi

    void retranslateUi(QWidget *TouchScreen)
    {
        TouchScreen->setWindowTitle(QApplication::translate("TouchScreen", "TouchScreen", nullptr));
        monitorLabel->setText(QApplication::translate("TouchScreen", "monitor", nullptr));
        touchLabel->setText(QApplication::translate("TouchScreen", "touch screen", nullptr));
        mapButton->setText(QApplication::translate("TouchScreen", "map", nullptr));
        touchscreenLabel->setText(QApplication::translate("TouchScreen", "TouchScreen", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TouchScreen: public Ui_TouchScreen {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOUCHSCREEN_H
