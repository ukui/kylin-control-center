#include "m_updatelog.h"
//#include <QDir>
//#include <QCoreApplication>

const QString FIND_DES_LABLE_TYPE = "FIND_DES_LABLE_TYPE";//历史更新模块标签
const int WIDTH = 680;//窗口宽度
const int HEIGHT = 604 -40;//窗口高度   注：QFram实际高度比setFixedSize指定的高度高40像素，故减去40
const int LINE_SPACING = 2;//行间距
const int TOP_MARGIN = 18;//上边距
const int BOTTOM_MARGIN = 24;//下边距
const int LEFT_MARGIN = 32;//左（右）边距

const int LIST_LEFT = 8;//列表对于其背景，左侧边距
const int LIST_TOP = 8;//列表对于其背景，顶（底）部边距
const int LIST_BACKGROUND_WIDTH = 260;//列表背景宽度
const int SLIDER_WIDTH = 6;//滑块宽度
//const int LIST_WIDTH = LIST_BACKGROUND_WIDTH - LIST_LEFT*2 + SLIDER_WIDTH;//列表宽度
//const int LIST_HEIGHT = 524 - LIST_TOP*2;//列表高度

const int TEXT_TAB_LEFT = 16;//更新详情lable对于其背景，左侧边距
const int TEXT_TAB_RIGHT = 2;//更新详情lable对于其背景，左侧边距
const int TEXT_DSC_LEFT_SPACING = 5;//QTextEdit左侧有边距
const int TEXT_TAB_TOP = 17;//更新详情lable对于其背景，顶部边距
const int TEXT_TAB_SPACING = 18;//更新详情lable对于内容框的间距

m_updatelog::m_updatelog(QWidget *parent) : QDialog(parent)
{
    initUI();
    //dynamicLoadingInit();//动态加载暂未实现
    updatesql();//更新列表
    defaultItem();//设置
}

void m_updatelog::initUI()
{
    QFont font;
//    font.setPointSize(34);//字体大小
    font.setBold(true);

    //初始化窗口属性
    this->setWindowTitle(tr("历史更新"));
    this->setFixedSize(WIDTH,HEIGHT);
    this->setObjectName(FIND_DES_LABLE_TYPE);
    this->setAttribute(Qt::WA_DeleteOnClose);

    //实例化控件
    QFrame *listBackground = new QFrame;
    QFrame *desBackground = new QFrame;
    mainListwidget  = new QListWidget;
    QLabel *updateDesTab = new QLabel;
    des = new QTextEdit;

    //初始化控件
    updateDesTab->setFont(font);

    QPalette palette = mainListwidget->palette();
    palette.setBrush(QPalette::Base, QColor (0, 0 , 0, 0));
    mainListwidget->setPalette(palette);
    mainListwidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  //滑块平滑滚动

    QPalette palette2 = des->palette();
    palette2.setBrush(QPalette::Base, QColor (0, 0 , 0, 0));
    des->setPalette(palette2);
    des->setReadOnly(true);
    des->setObjectName(FIND_DES_LABLE_TYPE);

    listBackground->setFrameStyle(QFrame::Box);
    listBackground->setFixedWidth(LIST_BACKGROUND_WIDTH);

    desBackground->setFrameStyle(QFrame::Box);

    updateDesTab->setText(tr("更新详情"));

    //布局
    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->setSpacing(0);
    hl1->setMargin(0);
    hl1->addSpacing(LEFT_MARGIN);
    hl1->addWidget(listBackground);
    hl1->addSpacing(LINE_SPACING);
    hl1->addWidget(desBackground);
    hl1->addSpacing(LEFT_MARGIN);
    QVBoxLayout *vl1 = new QVBoxLayout;
    vl1->setSpacing(0);
    vl1->setMargin(0);
    vl1->addSpacing(TOP_MARGIN);
    vl1->addLayout(hl1);
    vl1->addSpacing(BOTTOM_MARGIN);
    this->setLayout(vl1);

    hll = new QHBoxLayout;
    hll->setSpacing(0);
    hll->setMargin(0);
    hll->addSpacing(LIST_LEFT);
    hll->addWidget(mainListwidget);
    QVBoxLayout *vll = new QVBoxLayout;
    vll->setSpacing(0);
    vll->setMargin(0);
    vll->addSpacing(LIST_TOP);
    vll->addLayout(hll);
    vll->addSpacing(LIST_TOP);
    listBackground->setLayout(vll);

    QHBoxLayout *hlrt = new QHBoxLayout;
    hlrt->setSpacing(0);
    hlrt->setMargin(0);
    hlrt->addSpacing(TEXT_DSC_LEFT_SPACING);
    hlrt->addWidget(updateDesTab);
    QVBoxLayout *vlr = new QVBoxLayout;
    vlr->setSpacing(0);
    vlr->setMargin(0);
    vlr->addSpacing(TEXT_TAB_TOP);
    vlr->addLayout(hlrt);
    vlr->addSpacing(TEXT_TAB_SPACING);
    vlr->addWidget(des);
    vlr->addSpacing(TEXT_TAB_TOP);
    QHBoxLayout *hlr = new QHBoxLayout;
    hlr->setSpacing(0);
    hlr->setMargin(0);
    hlr->addSpacing(TEXT_TAB_LEFT - TEXT_DSC_LEFT_SPACING);
    hlr->addLayout(vlr);
    hlr->addSpacing(TEXT_TAB_RIGHT);
    desBackground->setLayout(hlr);
}

void m_updatelog::updatesql()
{
    QSqlQuery query(QSqlDatabase::database("A"));
    query.exec("SELECT * FROM installed");
    //载入数据库数据
    while(query.next()){
        HistoryUpdateListWig *hulw = new HistoryUpdateListWig();
        hulw->setAttribute(query.value("appname").toString()+" "+query.value("version").toString(),
                           query.value("statue").toString(),
                           query.value("time").toString(),
                           query.value("description").toString());
        QListWidgetItem *item = new QListWidgetItem(mainListwidget);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(hulw->getTrueSize());
        mainListwidget->setItemWidget(item,hulw);
    }
}

void m_updatelog::defaultItem()
{
    //默认选中第一个
    HistoryUpdateListWig *first = mainListwidget->findChild<HistoryUpdateListWig *>();
    if(first!=nullptr)
        first->selectStyle();
}

void m_updatelog::dynamicLoadingInit()
{
    //禁用列表容器滑块
    mainListwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //实例化滑块
    QScrollBar *scrollBar = new QScrollBar;
    //设置滑块
    scrollBar->setMaximum(20);
    hll->addWidget(scrollBar);
    //绑定信号和槽
    connect(scrollBar,&QScrollBar::valueChanged, this,&m_updatelog::dynamicLoading );
}

void m_updatelog::dynamicLoading(int i)
{

}
