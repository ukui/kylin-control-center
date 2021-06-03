#include "historyupdatelistwig.h"

#define ORG_UKUI_STYLE "org.ukui.style"
#define GSETTING_KEY "systemFontSize"

const QString FIND_DES_LABLE_TYPE = "FIND_DES_LABLE_TYPE";//历史更新模块标签
const QString IS_SELECT = "IS_SELECT";//历史更新模块标签
const int WIDTH = 296 - 2;
const int LINE_SPACING = 2;
const int TOP_MARGIN = 5;//上(下)边距
const int RIGHT_MARGIN = 3;//右边距
const int LEFT_MARGIN = 9;//左边距文字和lable本身有边距

HistoryUpdateListWig::HistoryUpdateListWig(QLabel* destab)
{
    this->mDesTab = destab;

    initUI();
    gsettingInit();
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
    font.setBold(true);

    /* 窗口初始化 */
    this->setFrameStyle(QFrame::Box);

    /* 实例化控件 */
    debName = new QLabel;
    debName->setWordWrap(true);
    debName->setFixedWidth(WIDTH - LEFT_MARGIN - LEFT_MARGIN);
    debName->setFont(font);

    debStatue = new QLabel;
    debStatue->setWordWrap(true);
    debStatue->setFixedWidth(WIDTH - LEFT_MARGIN - LEFT_MARGIN);

    /* 布局 */
    hl1 = new QHBoxLayout;
    hl2 = new QHBoxLayout;
    vl1 = new QVBoxLayout;

    hl1->setSpacing(0);
    hl1->setMargin(0);
    hl2->setSpacing(0);
    hl2->setMargin(0);
    vl1->setSpacing(0);
    vl1->setMargin(0);

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
    //debName->setText(mname);

    this->mNameLabel = mname;

    /* 单行显示 ， 超出范围加... 和悬浮框 */
    QFontMetrics nameFontMetrics(debName->font());
    int nameFontSize = nameFontMetrics.width(mname);
    int nameLableWidth = debName->width();
    QString nameFormatBody = mname;
    if (nameFontSize > (nameLableWidth - 10)) {
        nameFormatBody = nameFontMetrics.elidedText(nameFormatBody , Qt::ElideRight , nameLableWidth - 10);
        debName->setText(nameFormatBody);
        this->nameTipStatus = true;
        debName->setToolTip(mname);
    } else {
        this->nameTipStatus = false;
        debName->setText(nameFormatBody);
    }

    QString str = "";
    if(mstatue == "Success")
        str=tr("Success");  //更新成功
    else
        str=tr("Failed");  //更新失败
    str+="  "+mtime;

    this->mStatusLabel = str;

    QFontMetrics statueFontMetrics(debStatue->font());
    int statueFontSize = statueFontMetrics.width(str);
    int statueLableWidth = debStatue->width();
    QString statueFormatBody = str;
    if (statueFontSize > (statueLableWidth - 10)) {
        statueFormatBody = statueFontMetrics.elidedText(statueFormatBody , Qt::ElideRight , statueLableWidth - 10);
        debStatue->setText(statueFormatBody);
        debStatue->setToolTip(str);
        this->statusTipStatus = true;
    } else {
        debStatue->setText(statueFormatBody);
        this->statusTipStatus = false;
    }
    //debStatue->setText(str);


    debDescription=mdescription;
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
            /* 重设tips */
            if (tmp->nameTipStatus == true) {
                tmp->debName->setToolTip(tmp->mNameLabel);
            } else {
                tmp->debName->setToolTip(QString(""));
            }

            if (tmp->statusTipStatus == true) {
                tmp->debStatue->setToolTip(tmp->mStatusLabel);
            } else {
                tmp->debStatue->setToolTip(QString(""));
            }
        }
    }

    //设置选中样式及标签
    debName->setToolTip(QString(""));
    debStatue->setToolTip(QString(""));
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
    if (this->mDesTab != nullptr) {
        this->mDesTab->setFont(this->font);
        this->mDesTab->setText(this->mNameLabel);
    }

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

void HistoryUpdateListWig::gsettingInit()
{
    const QByteArray style_id(ORG_UKUI_STYLE);
    m_pGsettingFontSize = new QGSettings(style_id);
    connect(m_pGsettingFontSize, &QGSettings::changed, this, [=] (const QString &key){
        if (key==GSETTING_KEY) {
            QFontMetrics nameFontMetrics(debName->font());
            int nameFontSize = nameFontMetrics.width(this->mNameLabel);
            int nameLableWidth = debName->width();
            QString nameFormatBody = this->mNameLabel;
            if (nameFontSize > (nameLableWidth - 10)) {
                nameFormatBody = nameFontMetrics.elidedText(nameFormatBody , Qt::ElideRight , nameLableWidth - 10);
                debName->setFont(this->font);
                debName->setText(nameFormatBody);
                debName->setToolTip(this->mNameLabel);
                this->nameTipStatus = true;
            } else {
                debName->setFont(this->font);
                debName->setToolTip(QString(""));
                this->nameTipStatus = false;
                debName->setText(nameFormatBody);
            }

            QFontMetrics statueFontMetrics(debStatue->font());
            int statueFontSize = statueFontMetrics.width(this->mStatusLabel);
            int statueLableWidth = debStatue->width();
            QString statueFormatBody = this->mStatusLabel;
            if (statueFontSize > (statueLableWidth - 10)) {
                statueFormatBody = statueFontMetrics.elidedText(statueFormatBody , Qt::ElideRight , statueLableWidth - 10);
                debStatue->setText(statueFormatBody);
                debStatue->setToolTip(this->mStatusLabel);
                this->statusTipStatus = true;
            } else {
                debStatue->setToolTip(QString(""));
                debStatue->setText(statueFormatBody);
                this->statusTipStatus = false;
            }
       }
    });
}
