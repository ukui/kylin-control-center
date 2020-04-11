#ifndef QL_COMBOBOBX_H
#define QL_COMBOBOBX_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtMath>
#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QScrollBar>
#include "ql_box_item.h"
#include "ql_popup.h"
#include <QScrollArea>

class ql_combobobx : public QWidget
{
    Q_OBJECT
public:
    explicit ql_combobobx(QWidget *parent = nullptr);
    void addItem(QString country,QString code);
public slots:
    void showpopup();
    void closepopup(QListWidgetItem *item);
protected:
    int idx;
private:
    QLineEdit       *lineedit;
    QPushButton     *pushbutton;
    QListWidget     *listwidget;
    QWidget         *popup;
    QHBoxLayout     *editcontrol;
    QVBoxLayout     *popupcontrol;
    QVBoxLayout     *comboboxcontrol;
signals:
    void currentIndexChanged(int index);
};

#endif // QL_COMBOBOBX_H
