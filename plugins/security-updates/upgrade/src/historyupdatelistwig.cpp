#include "historyupdatelistwig.h"

const QString FIND_DES_LABLE_TYPE = "FIND_DES_LABLE_TYPE";//历史更新模块标签
const QString IS_SELECT = "IS_SELECT";//历史更新模块标签
const int WIDTH = 236 - 2;
const int LINE_SPACING = 2;
const int TOP_MARGIN = 5;//上(下)边距
const int RIGHT_MARGIN = 3;//右边距
const int LEFT_MARGIN = 9;//左边距文字和lable本身有边距

HistoryUpdateListWig::HistoryUpdateListWig()
{
    initUI();
}
HistoryUpdateListWig::~HistoryUpdateListWig()
{
    debName->deleteLater();
    debStatue->deleteLater();
    hl1->deleteLater();
    hl2->deleteLater();
    vl1->deleteLater();
}
void HistoryUpdateListWig::initUI()
{
//    font.setPointSize(14);//字体大小
    font.setBold(true);

    //窗口初始化
    this->setFrameStyle(QFrame::Box);

    //实例化控件
    debName = new QLabel;
    debStatue = new QLabel;

    //初始化控件
    debName->setWordWrap(true);
    debName->setFixedWidth(WIDTH - LEFT_MARGIN - LEFT_MARGIN);
    debName->setFont(font);
    debStatue->setWordWrap(true);
    debStatue->setFixedWidth(WIDTH - LEFT_MARGIN - LEFT_MARGIN);

    //实例化布局
    hl1 = new QHBoxLayout;
    hl2 = new QHBoxLayout;
    vl1 = new QVBoxLayout;

    //初始化布局
    hl1->setSpacing(0);
    hl1->setMargin(0);
    hl2->setSpacing(0);
    hl2->setMargin(0);
    vl1->setSpacing(0);
    vl1->setMargin(0);

    //布局
    hl1->addSpacing(LEFT_MARGIN);
    hl1->addWidget(debName);
    hl1->addSpacing(RIGHT_MARGIN);
    hl2->addSpacing(LEFT_MARGIN);
    hl2->addWidget(debStatue);
    hl2->addSpacing(RIGHT_MARGIN);
    vl1->addSpacing(TOP_MARGIN);
    vl1->addLayout(hl1);
    vl1->addLayout(hl2);
    vl1->addSpacing(TOP_MARGIN);
    this->setLayout(vl1);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void HistoryUpdateListWig::setAttribute(const QString &mname,const QString &mstatue,const QString &mtime,const QString &mdescription,const int &myid)
{
    debName->setText(mname);
    debDescription=mdescription;
    QString str = "";
    if(mstatue == "Success")
        str=tr("Success");  //更新成功
    else
        str=tr("Failed");  //更新失败
    str+="  "+mtime;
    debStatue->setText(str);
    id = myid;
}

QSize HistoryUpdateListWig::getTrueSize()
{
    QSize lsize =this->layout()->sizeHint();
    //lsize.setHeight(lsize.rheight()+LINE_SPACING*2);
    return lsize;
}

//鼠标 点击
void HistoryUpdateListWig::mousePressEvent(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        setDescription();
        selectStyle();
    }
    if(e->button() == Qt::RightButton)
    {
        clearStyleSheet();
    }

}

void HistoryUpdateListWig::selectStyle()
{
    //如果上次选中的也是自身
    if(this->statusTip()==IS_SELECT)
        return;
    //清除其他item选中样式及标签
    QList<HistoryUpdateListWig *> list = this->parent()->findChildren<HistoryUpdateListWig *>();
    for(HistoryUpdateListWig *tmp : list)
    {
        if(tmp->statusTip()==IS_SELECT)
        {
            tmp->clearStyleSheet();
        }
    }
    //设置选中样式及标签
    debName->setStyleSheet("color:#fff;");
    debStatue->setStyleSheet("color:#fff;");
    this->setStyleSheet("QFrame{background-color:rgba(55, 144, 250, 1);border-radius:4px}");
    this->setStatusTip(IS_SELECT);
    //详细内容
    setDescription();
}

void HistoryUpdateListWig::clearStyleSheet()
{
    debName->setStyleSheet("");
    debStatue->setStyleSheet("");
    this->setStyleSheet("");
    this->setStatusTip("");
}

void HistoryUpdateListWig::setDescription()
{
    QObject *findwig = this->parent();
    while(findwig != nullptr)
    {
        if(findwig->objectName()==FIND_DES_LABLE_TYPE)
        {
            break;
        }
        findwig = findwig->parent();
    }
    QTextEdit *dsc = findwig->findChild<QTextEdit *>(FIND_DES_LABLE_TYPE,Qt::FindChildrenRecursively);
    if(dsc==nullptr)
        qDebug()<<"找不到要赋值的窗口";
    else
        dsc->setText(debDescription);
}
