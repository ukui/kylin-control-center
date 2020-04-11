#include "MyComboBox.h"
#include <qdebug.h>
#include <qlineedit.h>

ComboboxItem::ComboboxItem(QWidget *parent): QWidget(parent)
{
    m_label = NULL;
    m_ID = "";
    m_code = new QLabel(this);
    m_wdgImg = new QWidget(this);
    m_wdgImg->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_label = new QLabel(this);
    m_layout = new QHBoxLayout(this);

    //m_label->setAttribute(Qt::WA_TransparentForMouseEvents);

    //添加图标

    m_label->setStyleSheet("QLabel{color:rgba(0,0,0,0.85);font-size:14px}"
                           "QLabel:hover{color:#FFFFFF;font-size:14px}");
    m_code->setStyleSheet("QLabel{color:rgba(0,0,0,0.65);font-size:14px}"
                          "QLabel:hover{color:#FFFFFF;font-size:14px}");
    m_layout->addWidget(m_label,0,Qt::AlignLeft);
    m_layout->addStretch();
    m_layout->addWidget(m_code,0,Qt::AlignRight);
    setLayout(m_layout);
    setFixedHeight(36);

}

void ComboboxItem::enterEvent(QEvent *e) {
    QString str = "color:#FFFFFF;font-size:14px";
    m_label->setStyleSheet(str);
    m_code->setStyleSheet(str);
}

void ComboboxItem::leaveEvent(QEvent *e) {
    m_label->setStyleSheet("color:rgba(0,0,0,0.85);font-size:14px");
    m_code->setStyleSheet("color:rgba(0,0,0,0.65);font-size:14px");
    qDebug()<<"csacsa";
}

void ComboboxItem::setText(QString str)
{
    m_label->setText(str);
}

void ComboboxItem::setItemID(QString str)
{
    m_ID = str;
}

void ComboboxItem::setCode(QString str)
{
    m_code->setText(str);
}

QLabel* ComboboxItem::get_code() {
    return m_code;
}

QLabel* ComboboxItem::get_label() {
    return m_label;
}


MyComboBox::MyComboBox(QWidget *parent):QComboBox(parent)
{

    m_listWidget = new QListWidget(this);
    m_selItem = NULL;
    m_edit = NULL;
    cnt = 0;
    //设置Edit可用
    this->setEditable(true);
    //设置下拉列表代理
    this->setModel(m_listWidget->model());
    this->setView(m_listWidget);
    m_edit = this->lineEdit();
    m_edit->setTextMargins(8,0,0,0);
    m_edit->setStyleSheet("border:none;border-radius:4px;background-color:#F4F4F4;font-size:14px");
    setQss();

    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(oncurrenIndex(int)));
    connect(this->lineEdit(),SIGNAL(textChanged (const QString & )),this,SLOT(onTextChanged(const QString & )));
}

MyComboBox::~MyComboBox()
{

}



void MyComboBox::addItem(QString str,QString code)
{
    insertItem(cnt ++,str,code);

}
void MyComboBox::insertItem(int n,QString str,QString code)
{
    ComboboxItem* item = new ComboboxItem(this);
    item->setText(str);
    item->setCode(code);
    item->setItemID(str);

    QListWidgetItem* widgetItem = new QListWidgetItem();
    m_ItemMap.insert(widgetItem,item);

    m_listWidget->insertItem(n,widgetItem);
    widgetItem->setHidden(false);
    m_listWidget->setItemWidget(widgetItem, item);

    widgetItem->setSizeHint(QSize(82 * 2,36));



}
QString MyComboBox::getSelText()
{
    return m_selStr;
}

void MyComboBox::setSelItem(int n)
{
    this->setCurrentIndex(n);
}

void MyComboBox::setEditText(QString str)
{

    QComboBox::setEditText(str);

}


void MyComboBox::setQss()
{
    QString qss ="QComboBox {border:none;border-radius: 4px;border-color:#f4f4f4;"
                 "padding: 1px 2px 1px 2px;}"
                 "QComboBox{color:rgba(0,0,0,0.85);font-family: \"Microsoft YaHei\"; font-size:14px;border:none}"
                 "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 13px;"
                 "border-left-width: 1px;border-left-color: #CFCFCF;border-left-style: none; "
                 "border-top-right-radius: 4px; border-bottom-right-radius: 4px;}"
                 "QComboBox::down-arrow {image: url(:/new/image/arrow_down.png);}"
                 "QComboBox:hover {border-color: #3D6BE5;}"
                 "QComboBox::drop-down:hover {border-left-color: #3D6BE5;}"
                 "QComboBox:selected {border-color: #ffa7a7a7;border-left-color: #3D6BE5;}"
                 "QComboBox{background: #F4F4F4;}"
                 "QListView{border:none;border-radius: 4px;border-color:#CFCFCF;}"
                 "QListView::item{background: #ffffffff;;min-height:188px;max-height:188px;}"
                 "QListView::item:selected{background: #ffffffff;}"
                 "QComboBox QAbstractItemView {outline: 1px #cfcfcf;border-radius:4px;min-width:200px;}"
                 "QListView::item:hover {background: #3D6BE5;}";

    setStyleSheet(qss);

}



void MyComboBox::onshowAccount(QString str)
{
    qDebug()<<"onshowAccount"<<str;
    setEditText(str);
    this->hidePopup();
}
void MyComboBox::oncurrenIndex(int n)
{
    qDebug()<<"oncurrenIndex"<<n;


    QListWidgetItem *Item = m_listWidget->item(n);

    ComboboxItem* itemex =  m_ItemMap[Item];

    m_selItem = itemex;
    m_selStr= itemex->m_ID;
    this->lineEdit()->setText(m_selStr);
    this->hidePopup();
}

void MyComboBox::onTextChanged(const QString &str )
{
    if(str == "")
    {
        if(m_selStr == "")
            return;
        else
        {
            setEditText(m_selStr);
        }
    }
}


