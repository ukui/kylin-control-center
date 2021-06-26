#include "m_updatelog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include "xatom-helper.h"
//#include <QDir>
//#include <QCoreApplication>

const QString FIND_DES_LABLE_TYPE = "FIND_DES_LABLE_TYPE";//历史更新模块标签
const int WIDTH = 880;//窗口宽度
const int HEIGHT = 610 -40;//窗口高度   注：QFram实际高度比setFixedSize指定的高度高40像素，故减去40
const int LINE_SPACING = 2;//行间距
const int TOP_MARGIN = 18;//上边距
const int BOTTOM_MARGIN = 24;//下边距
const int LEFT_MARGIN = 32;//左（右）边距

const int LIST_LEFT = 8;//列表对于其背景，左侧边距
const int LIST_TOP = 8;//列表对于其背景，顶（底）部边距
const int LIST_BACKGROUND_WIDTH = 320 + 6;//列表背景宽度
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
    /*添加窗管协议*/
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);

    initUI();//初始化UI
    initGsettings();//初始化Gsettings
    dynamicLoadingInit();//动态加载
    updatesql();//更新列表
//    defaultItem();//设置默认选中
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
    font.setBold(true);

    /* 初始化窗口属性 */
    this->setFixedSize(WIDTH,HEIGHT + 40);
    this->setObjectName(FIND_DES_LABLE_TYPE);

    /* 实例化控件 */
    /* 实例化标题栏 */
    updateTitleWidget();

    QFrame *listBackground = new QFrame;
    listBackground->setFrameStyle(QFrame::Box);
    listBackground->setFixedWidth(LIST_BACKGROUND_WIDTH);

    mainListwidget  = new QListWidget;
    QPalette palette = mainListwidget->palette();
    palette.setBrush(QPalette::Base, QColor (0, 0 , 0, 0));
    mainListwidget->setPalette(palette);
    mainListwidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  /* 滑块平滑滚动 */
    mainListwidget->verticalScrollBar()->setProperty("drawScrollBarGroove" , false);
    mainListwidget->setSpacing(2);

    QFrame *desBackground = new QFrame;
    desBackground->setFrameStyle(QFrame::Box);

    updateDesTab = new QLabel;
    updateDesTab->setFont(font);
    updateDesTab->setWordWrap(true);
    updateDesTab->setText(tr("Update Details"));  /* 更新详情 */

    /* 描述文本框 */
    des = new QTextEdit;
    QPalette palette2 = des->palette();
    palette2.setBrush(QPalette::Base, QColor (0, 0 , 0, 0));
    des->verticalScrollBar()->setProperty("drawScrollBarGroove" , false);
    des->setPalette(palette2);
    des->setReadOnly(true);
    des->setObjectName(FIND_DES_LABLE_TYPE);

    /* 布局 */
    /* 布局外边框 */
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

    vl1->addSpacing(6);
    vl1->addWidget(this->title);

    vl1->addSpacing(TOP_MARGIN);
    vl1->addLayout(hl1);
    vl1->addSpacing(BOTTOM_MARGIN);
    this->setLayout(vl1);

    /* 布局列表 */
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

    /* 布局更新详情标签 */
    QHBoxLayout *hlrt = new QHBoxLayout;
    hlrt->setSpacing(0);
    hlrt->setMargin(0);
    hlrt->addSpacing(TEXT_DSC_LEFT_SPACING);
    hlrt->addWidget(updateDesTab);

    /* 布局标签及文本框 */
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

    this->installEventFilter(this);
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
    HistoryUpdateListWig *firstWidget = NULL;
    bool flag = true;

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
        HistoryUpdateListWig *hulw = new HistoryUpdateListWig(updateDesTab);
        if (flag == true) {
            firstWidget = hulw;
            flag = false;
        }
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

    /* 选中第一条记录 */
    if (firstWidget != NULL && start == 0) {
        firstWidget->selectStyle();
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

/* 历史更新界面搜索功能 */
void m_updatelog::slotSearch(QString packageName)
{
    HistoryUpdateListWig *firstWidget = NULL;
    bool flag = true;
    /* 取消原历史界面动态加载功能 */
    cacheDynamicLoad();

    /* 转换包名 */
    QString dstPackageName = conversionPackageName(packageName);

    /* 清空列表 */
    clearList();

    /* 拼接sql */
    QString sql = "SELECT `appname` , `version` , `statue` , `time` , `description` , `id` , `keyword` FROM installed WHERE `appname` = '" + dstPackageName + "'";

    qDebug() << "Info : sql is [ " << sql << " ]";

    /* 查询数据库 , 获取数据 */
    QSqlQuery query(QSqlDatabase::database("A"));
    if (!query.exec(sql)) {
        qDebug() << "Error : search sql exec fail";
        return;
    }
    while (query.next()) {
        QString appName = query.value(0).toString();
        QString version = query.value(1).toString();
        QString statue = query.value(2).toString();
        QString time = query.value(3).toString();
        QString description = setDefaultDescription(query.value(4).toString());
        int id = query.value(5).toInt();
        QString keyword = query.value(6).toString();

        if (keyword != "" && keyword != "1") {
            continue;
        }

        /* 展示搜索内容 */
        HistoryUpdateListWig *updateItem = new HistoryUpdateListWig(updateDesTab);
        if (flag == true) {
            firstWidget = updateItem;
            flag = false;
        }
        updateItem->setAttribute(packageName + " " + version , statue , time , description , id);
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(updateItem->getTrueSize());

        mainListwidget->addItem(item);
        mainListwidget->setItemWidget(item , updateItem);

        appName.clear();
        version.clear();
        statue.clear();
        time.clear();
        description.clear();
        id = 0;
        keyword.clear();
    }

    /* 选中第一条记录 */
    if (firstWidget != NULL) {
        firstWidget->selectStyle();
    } else {
        if (this->des != NULL) {
            des->setText(QString(""));
        }
        if (this->updateDesTab != NULL) {
            updateDesTab->setText(QString(""));
        }
    }

    return;
}

void m_updatelog::cacheDynamicLoad(void)
{
     disconnect(mainListwidget->verticalScrollBar() , &QScrollBar::valueChanged , this , &m_updatelog::dynamicLoading);
}

void m_updatelog::clearList(void)
{
   int sum = mainListwidget->count();

   for (int i = sum ; i >= 0 ; i--) {
       QListWidgetItem *item = mainListwidget->takeItem(i);
       delete item;
   }

   return;
}

QString m_updatelog::conversionPackageName(QString package)
{
    if (QLocale::system().name() != "zh_CN")
        return package;
    if (package == "基本应用")
        return "kylin-update-desktop-app";
    if (package == "安全更新")
        return "kylin-update-desktop-security";
    if (package == "系统基础组件")
        return "kylin-update-desktop-support";
    if (package == "桌面环境组件")
        return "kylin-update-desktop-ukui";
    if (package == "系统内核组件")
        return "linux-generic";
    if (package == "系统内核组件")
        return "kylin-update-desktop-kernel";
    if (package == "系统内核组件")
        return "kylin-update-desktop-kernel-3a4000";
    if (package == "kydroid补丁包")
        return "kylin-update-desktop-kydroid";

    /* 从软件商店数据库根据包名获取应用英文名 */
    QString dst;
    dst.clear();

    QSqlQuery query(QSqlDatabase::database("B"));
    bool ret = query.exec(QString("SELECT `app_name` FROM application WHERE `display_name_cn` = '%1'").arg(package));    //执行
    if (ret == false) {
        qDebug() << "Error : exec select sql fail , switch pkg name fail";
        return package;
    }

    while (query.next()) {
        dst = query.value(0).toString();
        qDebug() << "Info : switch chinese pkg name is [" << dst << "]";
    }

    if (dst.isEmpty()) {
        return package;
    } else {
        return dst;
    }

    return package;
}

void m_updatelog::searchBoxWidget(void)
{
    this->searchBox = new QLineEdit(this);
    this->searchBox->setFixedSize(320 , 36);
    //this->searchBox->setPlaceholderText(tr("输入你想找的内容"));
    this->searchBox->setPlaceholderText(tr("Search content"));
    this->searchBox->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    //this->searchBox->setMaxLength(30);
    this->searchBox->installEventFilter(this);

    this->searchIcon = new QLabel(this);
    this->searchIcon->setFixedSize(this->searchBox->width() / 2 - 60 , this->searchBox->height());
    QIcon icon = QIcon::fromTheme("preferences-system-search-symbolic");
    this->searchIcon->setPixmap(icon.pixmap(icon.actualSize(QSize(16 , 16))));
    this->searchIcon->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->searchIcon->setProperty("isWindowButton" , 0x1);
    this->searchIcon->setProperty("useIconHighlightEffect" , 0x2);
    this->searchIcon->setAttribute(Qt::WA_TranslucentBackground , true);

    this->searchBox->setTextMargins(this->searchIcon->width() , 1 , 1 , 1);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->addWidget(searchIcon);
    hlayout->addStretch(0);

    this->searchBox->setLayout(hlayout);

    return;
}

bool m_updatelog::eventFilter(QObject *watch , QEvent *e)
{
    if ((e->type() == QEvent::MouseButtonPress && watch != this->searchBox)) {
        if (this->searchBox->text() == "") {
            this->searchIcon->setFixedSize(this->searchBox->width() / 2 - 60 , this->searchBox->height());
            this->searchIcon->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            this->searchBox->setAlignment(Qt::AlignLeft);
            this->searchBox->setPlaceholderText(tr("Search content"));
            this->searchBox->setTextMargins(this->searchIcon->width() , 1 , 1 , 1);
        }
    }

    if (e->type() == QEvent::MouseButtonPress && watch == this->searchBox) {
        this->searchIcon->setFixedSize(30 , 30);
        this->searchBox->setAlignment(Qt::AlignLeft);
        this->searchBox->setPlaceholderText(tr(""));
        this->searchBox->setTextMargins(this->searchIcon->width() , 1 , 1 , 1);
    }

    if (e->type() == 6 && watch == this->searchBox) {
        QKeyEvent *key = static_cast<QKeyEvent *>(e);
        if (key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return) {
            if (this->searchBox->text() != "") {
                QString appName = this->searchBox->text();
                slotSearch(appName);
            }
            if (this->searchBox->text() == "") {
                clearList();
                connect(mainListwidget->verticalScrollBar() , &QScrollBar::valueChanged , this , &m_updatelog::dynamicLoading);
                updatesql();
            }
        }
    }

    return QObject::eventFilter(watch , e);
}

void m_updatelog::updateTitleWidget(void)
{
    this->title = new QWidget(this);
    this->title->setFixedHeight(36);

    /* 标题栏图标 */
    this->titleIcon = new QLabel(this);
    this->titleIcon->setFixedSize(25 , 25);
    this->titleIcon->setPixmap(QIcon::fromTheme("ukui-control-center").pixmap(QSize(25 , 25)));

    /* 标题栏名字 */
    this->titleName = new QLabel(this);
    this->titleName->resize(56 , 20);
    QFont font;
    font.setPixelSize(14);
    this->titleName->setFont(font);
    //this->titleName->setText(tr("历史更新"));
    this->titleName->setText(tr("History Log"));

    /* 搜索框 */
    searchBoxWidget();

    /* 关闭按钮 */
    this->titleClose = new QPushButton(this);
    this->titleClose->setFixedSize(30 , 30);
    this->titleClose->setFlat(true);
    this->titleClose->setProperty("isWindowButton" , 0x2);
    this->titleClose->setProperty("useIconHighlightEffect" , 0x8);
    this->titleClose->setIconSize(QSize(16 , 16));
    this->titleClose->setIcon(QIcon::fromTheme("window-close-symbolic"));
    this->titleClose->setFocusPolicy(Qt::NoFocus);

    connect(this->titleClose , &QPushButton::clicked , this , &m_updatelog::slotClose);

    /* 布局 */
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->addSpacing(8);
    hlayout->addWidget(this->titleIcon);
    hlayout->addSpacing(2);
    hlayout->addWidget(this->titleName);

    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->setMargin(0);
    hlayout2->addLayout(hlayout);
    hlayout2->addStretch(0);
    hlayout2->addWidget(this->searchBox);
    hlayout2->addStretch(0);
    hlayout2->addWidget(this->titleClose);
    hlayout2->addSpacing(6);

    this->title->setLayout(hlayout2);
}

void m_updatelog::slotClose(void)
{
    this->close();
}
