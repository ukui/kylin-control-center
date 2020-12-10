#include "securitycenter.h"
#include "ui_securitycenter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

#include <QProcess>

#include <QDebug>

#include <QFontMetrics>


BlockWidget::BlockWidget(){
    setFixedSize(QSize(260, 80));
    initComponent();
}

BlockWidget::~BlockWidget(){

}

void BlockWidget::initComponent(){
    QHBoxLayout * mainVerLayout = new QHBoxLayout(this);
    mainVerLayout->setSpacing(12);
    mainVerLayout->setMargin(0);

    logoLabel = new QLabel;
    logoLabel->setFixedSize(QSize(48, 48));

    QVBoxLayout * textHorLayout = new QVBoxLayout;
    textHorLayout->setSpacing(10);

    titleLable = new QLabel;
    detailLabel = new QLabel;
    detailLabel->setAlignment(Qt::AlignTop);
    detailLabel->setFixedHeight(32);

    textHorLayout->addStretch();
    textHorLayout->addWidget(titleLable);
    textHorLayout->addWidget(detailLabel);
    textHorLayout->addStretch();

    mainVerLayout->addWidget(logoLabel);
    mainVerLayout->addLayout(textHorLayout);

    setLayout(mainVerLayout);
}

void BlockWidget::setupComponent(QString logo, QString title, QString detail, QString cmd){
    logoLabel->setPixmap(QPixmap(logo).scaled(logoLabel->size()));
    titleLable->setText(title);
    m_curIndex = 0;
    m_showText = detail + "    ";
    m_charWidth = fontMetrics().width("。");
    m_labelWidth = m_charWidth * (m_showText.length() - 4);
    _cmd = cmd;
}

void BlockWidget::scrollLabel(){
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BlockWidget::updateIndex);
    timer->start(200);
}

void BlockWidget::updateIndex()
{
    m_curIndex++;
    if (m_curIndex*m_charWidth > m_labelWidth){
        m_curIndex = 0;
    }
    this->detailLabel->update();
}

void BlockWidget::enterEvent(QEvent *event){
    //style
    m_charWidth = fontMetrics().width("。");
    m_labelWidth = m_charWidth * m_showText.length();
    scrollLabel();
    QWidget::enterEvent(event);
}

void BlockWidget::leaveEvent(QEvent *event){
    //style
    timer->stop();
    m_curIndex = 0;
    this->detailLabel->update();
    QWidget::leaveEvent(event);
}

void BlockWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        emit bwClicked(_cmd);
    }

    QWidget::mousePressEvent(event);
}

bool BlockWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->detailLabel && event->type() == QEvent::Paint && m_labelWidth > 280)
    {
        this->detailLabel->setText("");
        showPaint(); //响应函数
    }
    else if(m_labelWidth <= 280){
        this->detailLabel->setText(m_showText);
    }
    QWidget::eventFilter(watched,event);
    return true;
}

//实现响应函数
void BlockWidget::showPaint()
{
    QPainter painter(this->detailLabel);
    painter.drawText(0, 20, m_showText.mid(m_curIndex));
    painter.drawText(m_labelWidth - m_charWidth*m_curIndex, 20, m_showText.left(m_curIndex));
}

//子类化一个QWidget，为了能够使用样式表，则需要提供paintEvent事件。
//这是因为QWidget的paintEvent()是空的，而样式表要通过paint被绘制到窗口中。
void BlockWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

SecurityCenter::SecurityCenter()
{
    ui = new Ui::SecurityCenter;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Security Center");
    pluginType = UPDATE;

    initComponent();

    connect(ui->pushButton, &QPushButton::clicked, [=]{
        QString cmd = "/usr/sbin/ksc-defender";
        runExternalApp(cmd);
    });
}

SecurityCenter::~SecurityCenter()
{
    delete ui;
}

QString SecurityCenter::get_plugin_name(){
    return pluginName;
}

int SecurityCenter::get_plugin_type(){
    return pluginType;
}

QWidget * SecurityCenter::get_plugin_ui(){
    return pluginWidget;
}

void SecurityCenter::plugin_delay_control(){

}

const QString SecurityCenter::name() const {

    return QStringLiteral("securitycenter");
}

void SecurityCenter::initComponent(){
    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

    //设置布局
    flowLayout = new FlowLayout;
    flowLayout->setContentsMargins(0, 0, 0, 0);
    ui->modulesWidget->setLayout(flowLayout);

    BlockWidget * kysecWidget = new BlockWidget();
    kysecWidget->setupComponent(":/img/plugins/securitycenter/kysec.png", \
                                tr("Virus Protection"), \
                                tr("Protect system from threats"), \
                                "/usr/sbin/ksc-defender  --virus-protect");

    BlockWidget * netWidget = new BlockWidget();
    netWidget->setupComponent(":/img/plugins/securitycenter/net.png", \
                              tr("Network Protection"), \
                              tr("Setup app that can access web"), \
                              "/usr/sbin/ksc-defender --net-protect");

    BlockWidget * protectWidget = new BlockWidget();
    protectWidget->setupComponent(":/img/plugins/securitycenter/protect.png", \
                                  tr("App Execution Control"), \
                                  tr("App install and exe protection"), \
                                  "/usr/sbin/ksc-defender --exec-ctrl");

    BlockWidget * userWidget = new BlockWidget();
    userWidget->setupComponent(":/img/plugins/securitycenter/user.png", \
                               tr("Account Security"), \
                               tr("Protect account and login security"), \
                               "/usr/sbin/ksc-defender --account-sec");

    flowLayout->addWidget(kysecWidget);
    flowLayout->addWidget(netWidget);
    flowLayout->addWidget(protectWidget);
    flowLayout->addWidget(userWidget);

    connect(kysecWidget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(netWidget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(protectWidget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(userWidget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });


}

void SecurityCenter::runExternalApp(QString cmd){

    qDebug() << "cmd:" << cmd;

    QProcess process(this);
    process.startDetached(cmd);
}

