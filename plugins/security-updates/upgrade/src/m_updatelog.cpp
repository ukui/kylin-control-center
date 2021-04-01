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
const int LIST_BACKGROUND_WIDTH = 260 + 6;//列表背景宽度
const int SLIDER_WIDTH = 6;//滑块宽度
//const int LIST_WIDTH = LIST_BACKGROUND_WIDTH - LIST_LEFT*2 + SLIDER_WIDTH;//列表宽度
//const int LIST_HEIGHT = 524 - LIST_TOP*2;//列表高度

const int TEXT_TAB_LEFT = 16;//更新详情lable对于其背景，左侧边距
const int TEXT_TAB_RIGHT = 2;//更新详情lable对于其背景，左侧边距
const int TEXT_DSC_LEFT_SPACING = 5;//QTextEdit左侧有边距
const int TEXT_TAB_TOP = 17;//更新详情lable对于其背景，顶部边距
const int TEXT_TAB_SPACING = 18;//更新详情lable对于内容框的间距

const QString OBJECT_NAME = "OBJECT_NAME";

m_updatelog * m_updatelog::m_instance(nullptr);

m_updatelog::m_updatelog(QWidget* parent) : QDialog(parent)
{
    initUI();//初始化UI
    initGsettings();//初始化Gsettings
    dynamicLoadingInit();//动态加载
    updatesql();//更新列表
    defaultItem();//设置默认选中
    //监听更新完成信号
    UpdateDbus *uddbus = UpdateDbus::getInstance();
    connect(uddbus->interface,SIGNAL(update_sqlite_signal(QString,QString)),this,SLOT(historyUpdateNow(QString,QString)));
}

QString m_updatelog::setDefaultDescription(QString str)
{
    if(str == "")
        str = tr("No content.");  //暂无内容
    return str;
}

m_updatelog * m_updatelog::GetInstance(QWidget *parent)
{
    if(m_instance==nullptr)
    {
        m_instance = new m_updatelog(parent);
        return m_instance;
    }
    if(m_instance->isHidden())
    {
        m_instance->deleteLater();
        m_instance = new m_updatelog(parent);
    }
    return m_instance;
}

void m_updatelog::closeUpdateLog()
{
    m_instance->close();
    m_instance->deleteLater();
}

void m_updatelog::initUI()
{
    QFont font;
//    font.setPointSize(34);//字体大小
    font.setBold(true);

    //初始化窗口属性
    this->setWindowTitle(tr("History Log"));  //历史更新
    this->setFixedSize(WIDTH,HEIGHT);
    this->setObjectName(FIND_DES_LABLE_TYPE);
    //this->setAttribute(Qt::WA_DeleteOnClose);

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
    mainListwidget->verticalScrollBar()->setProperty("drawScrollBarGroove" , false);
    mainListwidget->setSpacing(2);

    QPalette palette2 = des->palette();
    palette2.setBrush(QPalette::Base, QColor (0, 0 , 0, 0));
    des->verticalScrollBar()->setProperty("drawScrollBarGroove" , false);
    des->setPalette(palette2);
    des->setReadOnly(true);
    des->setObjectName(FIND_DES_LABLE_TYPE);

    listBackground->setFrameStyle(QFrame::Box);
    listBackground->setFixedWidth(LIST_BACKGROUND_WIDTH);

    desBackground->setFrameStyle(QFrame::Box);

    updateDesTab->setText(tr("Update Details"));  //更新详情

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
    //hll->addWidget(mainListwidget->verticalScrollBar());
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

void m_updatelog::initGsettings()
{
    timer = new QTimer;
    timer->setSingleShot(true);
    connect(timer,&QTimer::timeout,this,&m_updatelog::changeListWidgetItemHeight);
    const QByteArray iid(THEME_QT_SCHEMA);
    qtSettings = new QGSettings(iid, QByteArray(), this);

    connect(qtSettings,&QGSettings::changed,this,[=] (const QString &key) {
       if(key == "systemFontSize") {
          timer->start(100);
       }
    });
}

void m_updatelog::changeListWidgetItemHeight()
{
    if(mainListwidget->count()<1)
        return;
    int row=0;
    while(row<(mainListwidget->count()))
    {
        QListWidgetItem * item = mainListwidget->item(row);
        HistoryUpdateListWig * hulw = qobject_cast<HistoryUpdateListWig*>(mainListwidget->itemWidget(item));
        item->setSizeHint(hulw->getTrueSize());
        row++;
    }
}

void m_updatelog::updatesql( const int &start,const int &num,const QString &intop)
{
    //sql 拼接
    QString sqlCmd = "SELECT * FROM installed";
    if(intop!="")
        sqlCmd+=" where `time` = '"+intop+"'";
    else if(start>0)
        sqlCmd+=" where `id` < "+QString::number(start);
    sqlCmd+=" order by `id` desc limit ";
    sqlCmd+=QString::number(num);
    //载入数据库数据
    QSqlQuery query(QSqlDatabase::database("A"));
    query.exec(sqlCmd);
    while(query.next()){
        QString statusType = query.value("keyword").toString();
        if(statusType!=""&&statusType!="1")
            continue;
        HistoryUpdateListWig *hulw = new HistoryUpdateListWig();
        hulw->setAttribute(translationVirtualPackage(query.value("appname").toString())+" "+query.value("version").toString(),
                           query.value("statue").toString(),
                           query.value("time").toString(),
                           setDefaultDescription(query.value("description").toString()),
                           query.value("id").toInt());
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(hulw->getTrueSize());
        if(intop!="")
        {
            if(hulw->id<=firstCode)
            {
                hulw->deleteLater();
                delete item;
                item = nullptr;
                return;
            }
            firstCode=hulw->id;
            mainListwidget->insertItem(0,item);
        }
        else
        {
            loadingCode = hulw->id;//记录加载到哪个位置
            mainListwidget->addItem(item);
        }
        mainListwidget->setItemWidget(item,hulw);
        if(intop!="")
            hulw->selectStyle();//设置选中样式
    }
}

void m_updatelog::defaultItem()
{
    //默认选中第一个
    HistoryUpdateListWig *first = mainListwidget->findChild<HistoryUpdateListWig *>();
    if(first!=nullptr)
    {
        first->selectStyle();//设置选中样式
        firstCode = first->id;//记录id
    }
}

QString m_updatelog::translationVirtualPackage(QString str)
{
    if(QLocale::system().name()!="zh_CN")
        return str;
    if(str == "kylin-update-desktop-app")
        return "基本应用";
    if(str == "kylin-update-desktop-security")
        return "安全更新";
    if(str == "kylin-update-desktop-support")
        return "系统基础组件";
    if(str == "kylin-update-desktop-ukui")
        return "桌面环境组件";
    if(str == "linux-generic")
        return "系统内核组件";
    if(str == "kylin-update-desktop-kernel")
        return "系统内核组件";
    if(str == "kylin-update-desktop-kernel-3a4000")
        return "系统内核组件";
    if(str == "kylin-update-desktop-kydroid")
        return "kydroid补丁包";

    /* 从软件商店数据库根据包名获取应用中文名 */
    QString dst;
    dst.clear();

    QSqlQuery query(QSqlDatabase::database("B"));
    bool ret = query.exec(QString("SELECT display_name_cn FROM application WHERE app_name IS '%1'").arg(str));    //执行
    if (ret == false) {
        qDebug() << "Error : exec select sql fail , switch chinese pkg name fail";
        return str;
    }

    while (query.next()) {
        dst = query.value(0).toString();
        qDebug() << "Info : switch chinese pkg name is [" << dst << "]";
    }

    if (dst.isEmpty()) {
        return str;
    } else {
        return dst;
    }
}


void m_updatelog::dynamicLoadingInit()
{
    //绑定信号和槽
    connect(mainListwidget->verticalScrollBar(),&QScrollBar::valueChanged, this,&m_updatelog::dynamicLoading );
}

void m_updatelog::dynamicLoading(int i)
{
    if(mainListwidget->verticalScrollBar()->maximum()==i)
    {
        qDebug()<<"动态加载";
        updatesql(loadingCode);
    }
}

void m_updatelog::historyUpdateNow(QString str1,QString str2)
{
    qDebug()<<"动态更新:"<<str1;
    updatesql(0,1,str2);
}

