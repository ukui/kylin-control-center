#include "m_updatelog.h"
#include <QDir>
#include <QCoreApplication>


m_updatelog::m_updatelog(QWidget *parent) : QWidget(parent)
{
//    m_getsql();
    initUI();
}

void m_updatelog::initUI()
{

    this->setFixedSize(500,600);
    m_pmainlayout   = new QVBoxLayout();
    m_pfirstlayout  = new QHBoxLayout();
    m_psecondlayout = new QHBoxLayout();
    m_entrylayout   = new QHBoxLayout();

    mainListwidget  = new QListWidget();
    headtitle       = new headerwidget();



    firstWidget     = new QWidget();
    secondWidget    = new QWidget();

    toplabel        = new QLabel(QObject::tr("Eyeshield"));
    uplabel         = new QLabel();

    searchEdit      = new QLineEdit();



    QAction *pLeadingAction = new QAction(this);
    pLeadingAction->setIcon(QIcon(":/new/prefix1/search.png"));
    searchEdit->addAction(pLeadingAction, QLineEdit::TrailingPosition);
    searchEdit->setPlaceholderText(QString::fromLocal8Bit("搜索"));

    QFont ft;
    ft.setPointSize(20);
    uplabel->setFont(ft);

    toplabel->setText("日志");
    uplabel->setText("更新详情");

    //获取输入框的信息，更新搜索数据
    connect(searchEdit,&QLineEdit::textChanged,[=](){
        mainListwidget->clear();
        s_insearch(searchEdit->text());
    });

    //布局第一层
    m_pfirstlayout->addWidget(toplabel);
    m_pfirstlayout->addItem(new QSpacerItem(30, 20));
    m_pfirstlayout->addWidget(searchEdit);
    m_pfirstlayout->addItem(new QSpacerItem(30, 20));

    firstWidget->setLayout(m_pfirstlayout);
    //布局第二层
    m_psecondlayout->addWidget(uplabel);
    secondWidget->setLayout(m_psecondlayout);

    //总体布局
    m_pmainlayout->addWidget(firstWidget);
    m_pmainlayout->addWidget(secondWidget);
    m_pmainlayout->addWidget(headtitle);
    m_pmainlayout->addWidget(mainListwidget);
    this->setLayout(m_pmainlayout);
}

void m_updatelog::updatesql()
{

    QSqlQuery query(QSqlDatabase::database("A"));
    query.exec("SELECT * FROM installed");
    mainListwidget->clear();
    //载入数据库数据
    while(query.next()){
        listwig *mywidget = new listwig(this);
        mywidget->m_Appnamelabel->setText(query.value(0).toString());
        mywidget->m_Versionlabel->setText(query.value(1).toString());
        mywidget->m_Timelabel->setText(query.value(2).toString());
        mywidget->m_Descriptionlabel->setText(query.value(3).toString());
        if(query.value(5).toString() == "Success"){
                mywidget->m_statuelabel->setText("成功");
        } else {
                mywidget->m_statuelabel->setText("失败");
        }
        QListWidgetItem *item1 = new QListWidgetItem(mainListwidget);
        item1->setSizeHint(QSize(400,80));
        mainListwidget->setItemWidget(item1,mywidget);
    }

}

int m_updatelog::s_insearch(QString arg)
{

    QSqlQuery query(QSqlDatabase::database("A"));
    if(arg == "")
        query.exec(QString("SELECT * FROM installed"));
    else
        query.exec(QString("SELECT * FROM installed where appname LIKE '%%1%' or version LIKE '%%2%' or time LIKE '%%3%' or statue LIKE '%%4%'").arg(arg).arg(arg).arg(arg).arg(arg));
    mainListwidget->clear();
    while(query.next()){
        listwig *mywidget = new listwig(this);
        mywidget->m_Appnamelabel->setText(query.value(0).toString());
        mywidget->m_Versionlabel->setText(query.value(1).toString());
        mywidget->m_Timelabel->setText(query.value(2).toString());
        mywidget->m_Descriptionlabel->setText(query.value(3).toString());
        if(query.value(5).toString() == "Success"){
                mywidget->m_statuelabel->setText("成功");
        } else {
                mywidget->m_statuelabel->setText("失败");
        }
        QListWidgetItem *item1 = new QListWidgetItem(mainListwidget);
        item1->setSizeHint(QSize(400,80));
        mainListwidget->setItemWidget(item1,mywidget);

    }

}

