#include "ql_combobobx.h"

ql_combobobx::ql_combobobx(QWidget *parent) : QWidget(parent)
{
    //Allocate the memory
    lineedit = new QLineEdit(this);
    pushbutton = new QPushButton(this);
    popup = new ql_popup(this);
    listwidget = new QListWidget;
    editcontrol = new QHBoxLayout;
    popupcontrol = new QVBoxLayout;
    comboboxcontrol = new QVBoxLayout;

    //Size
    lineedit->setFixedSize(66,34);
    pushbutton->setFixedSize(14,14);
    listwidget->setMinimumSize(188,36);
    listwidget->resize(188,36);
    popup->setFixedSize(216,196);
    setMaximumSize(66,34);
    resize(66,34);
    idx = 0;
    setContentsMargins(0,0,0,0);

    //Configuration
    listwidget->setContentsMargins(0,0,0,0);
    lineedit->setTextMargins(16,0,0,0);
    lineedit->setFocusPolicy(Qt::NoFocus);
    pushbutton->setFocusPolicy(Qt::NoFocus);

    //Style
    lineedit->setStyleSheet("QLineEdit{border:none;border-radius:4px;font-size:14px;color:rgba(0,0,0,0.85);background-color:#F4F4F4;}");
    pushbutton->setStyleSheet("QPushButton{background-image: url(:/new/image/arrow_down.png);"
                              "background-repeat:no-repeat;background-position :center;font-size:14px;background-color:#F4F4F4;border:none}");
    listwidget->setStyleSheet("QListView{border:1px solid #CFCFCF;border-color:#F4F4F4;border-radius:4px;}"
                              "QListView::item{background: #FFF;border-radius:4px;}"
                              "QListView::item:selected{background: #ffffffff;}"
                              "QListView::item:hover {background: #3D6BE5;}");

    //Layout
    editcontrol->setMargin(0);
    editcontrol->setSpacing(0);
    editcontrol->addWidget(pushbutton,0,Qt::AlignRight);
    lineedit->setLayout(editcontrol);

    popupcontrol->setMargin(0);
    popupcontrol->setSpacing(0);
    popupcontrol->addWidget(listwidget);
    popup->setLayout(popupcontrol);
    popup->setContentsMargins(6,6,6,6);
    popup->hide();

    comboboxcontrol->setMargin(0);
    comboboxcontrol->setSpacing(0);
    comboboxcontrol->addWidget(lineedit);
    comboboxcontrol->addWidget(popup);
    comboboxcontrol->setAlignment(Qt::AlignLeft);
    setLayout(comboboxcontrol);
    lineedit->setText("+86");

    //Connect slots
    connect(listwidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(closepopup(QListWidgetItem *)));
    connect(pushbutton, SIGNAL(clicked()),this,SLOT(showpopup()));
}

void ql_combobobx::showpopup() {
    if (popup->isHidden()) {
        QPoint pos;
        pos.setX(this->mapToGlobal(QPoint(0, 0)).x() - 6);
        pos.setY(this->mapToGlobal(QPoint(0, 0)).y() + this->height());
        popup->move(pos);
        popup->show();
    } else {
        popup->hide();
    }
}

void ql_combobobx::closepopup(QListWidgetItem *item) {
    popup->hide();
    QWidget *wgt = listwidget->itemWidget(item);
    if(wgt != NULL) {
        QLabel *code = wgt->findChild<QLabel *>("code");
        lineedit->setText(code->text());
    }
}

void ql_combobobx::addItem(QString country, QString code) {
    QListWidgetItem* widgetItem = new QListWidgetItem;
    ql_box_item *wgt = new ql_box_item;

    wgt->set_code(country);
    wgt->set_country_code(code);
    listwidget->insertItem(idx ++,widgetItem);
    widgetItem->setSizeHint(QSize(188,36));
    widgetItem->setHidden(false);
    listwidget->setItemWidget(widgetItem, wgt);
}


