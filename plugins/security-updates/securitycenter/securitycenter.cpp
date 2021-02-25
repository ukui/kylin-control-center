#include "securitycenter.h"
#include "ui_securitycenter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QProcess>
#include <QDebug>
#include <QFontMetrics>

BlockWidget::BlockWidget()
{
    setFixedSize(QSize(260, 80));
    initComponent();
}

BlockWidget::~BlockWidget()
{

}

void BlockWidget::initComponent(){
    QHBoxLayout * mainVerLayout = new QHBoxLayout(this);
    mainVerLayout->setSpacing(12);
    mainVerLayout->setMargin(8);
    logoLabel = new QLabel(this);
    logoLabel->setFixedSize(QSize(48, 48));
    QVBoxLayout * textHorLayout = new QVBoxLayout;
    textHorLayout->setSpacing(10);
    titleLable = new QLabel(this);
    titleLable->setObjectName("Sec");
    detailLabel = new QLabel(this);
    detailLabel->setAlignment(Qt::AlignTop);
    detailLabel->setFixedHeight(32);
    QFont font;
    font.setBold(true);
    font.setPixelSize(18);
    titleLable->setFont(font);
    font.setPixelSize(14);
    font.setBold(false);
    detailLabel->setFont(font);
    textHorLayout->addStretch();
    textHorLayout->addWidget(titleLable);
    textHorLayout->addWidget(detailLabel);
    textHorLayout->addStretch();
    mainVerLayout->addWidget(logoLabel);
    mainVerLayout->addLayout(textHorLayout);
    setLayout(mainVerLayout);
}

void BlockWidget::setupComponent(QString normal_icon ,QString hover_icon, QString title, QString detail, QString cmd){
    m_normalIcon = normal_icon;
    m_hoverIcon = hover_icon;
    logoLabel->setPixmap(QPixmap(normal_icon).scaled(logoLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    titleLable->setText(title);

    QString detaildec = detail;
    QFontMetrics  fontMetrics(detailLabel->font());
    int fontSize = fontMetrics.width(detail);
    if (fontSize > detailLabel->width()) {
        detailLabel->setText(fontMetrics.elidedText(detail, Qt::ElideRight, detailLabel->width()));
        detailLabel->setToolTip(detaildec);
    } else {
        detailLabel->setText(detaildec);
        detailLabel->setToolTip("");
    }

    m_curIndex = 0;
    m_showText = detail + "    ";
    // m_charWidth = fontMetrics().width("。");
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
    logoLabel->setPixmap(QPixmap(m_hoverIcon).scaled(logoLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);
    setStyleSheet("background:palette(Highlight);"
                  "border-radius:6px;");
    QWidget::enterEvent(event);
}

void BlockWidget::leaveEvent(QEvent *event){
    timer->stop();
    m_curIndex = 0;
    this->detailLabel->update();
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setStyleSheet("background:palette(Base);"
                  "border-radius:6px;");
    logoLabel->setPixmap(QPixmap(m_normalIcon).scaled(logoLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
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

    initTitleLabel();
    initSearchText();
    initComponent();

    connect(ui->pushButton, &QPushButton::clicked, [=]{
        QString cmd = "/usr/sbin/ksc-defender";
        runExternalApp(cmd);
    });
}

SecurityCenter::~SecurityCenter()
{
    delete ui;
    ui = nullptr;
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

void SecurityCenter::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);
}

void SecurityCenter::initSearchText() {
    //~ contents_path /securitycenter/Computer Security Overview
    ui->titleLabel->setText(tr("Computer Security Overview"));
}

void SecurityCenter::initComponent(){
    //设置布局
    flowLayout = new FlowLayout;
    flowLayout->setContentsMargins(0, 0, 0, 0);
    ui->modulesWidget->setLayout(flowLayout);

    BlockWidget * account_sec_Widget = new BlockWidget();
    account_sec_Widget->setupComponent(":/img/plugins/securitycenter/user_48.png", \
                                       ":/img/plugins/securitycenter/user_48_white.png",\
                                       //~ contents_path /securitycenter/Account Security
                                       tr("Account Security"), \
                                       tr("Protect account and login security"), \
                                       "/usr/sbin/ksc-defender --account-sec");
    BlockWidget * baseline_ctrl_Widget = new BlockWidget();
    baseline_ctrl_Widget->setupComponent(":/img/plugins/securitycenter/icon_scanning_b48@1x.png", \
                                         ":/img/plugins/securitycenter/icon_scanning_w48@1x.png",\
                                         //~ contents_path /securitycenter/Safety check-up
                                         tr("Safety check-up"), \
                                         tr("Detect abnormal configuration"), \
                                         "/usr/sbin/ksc-defender --baseline-ctrl");

    BlockWidget * virus_protect_Widget = new BlockWidget();
    virus_protect_Widget->setupComponent(":/img/plugins/securitycenter/protect_48.png", \
                                         ":/img/plugins/securitycenter/protect_48_white.png",\
                                         //~ contents_path /securitycenter/Virus defense
                                         tr("Virus defense"), \
                                         tr("Real time protection from virus threat"), \
                                         "/usr/sbin/ksc-defender  --virus-protect");

    BlockWidget * exec_ctrl_Widget = new BlockWidget();
    exec_ctrl_Widget->setupComponent(":/img/plugins/securitycenter/kysec_48.png", \
                                     ":/img/plugins/securitycenter/kysec_48_white.png",\
                                     //~ contents_path /securitycenter/App protection
                                     tr("App protection"), \
                                     tr("App install"), \
                                     "/usr/sbin/ksc-defender --exec-ctrl");


    BlockWidget * net_protect_Widget = new BlockWidget();
    net_protect_Widget->setupComponent(":/img/plugins/securitycenter/net_48.png", \
                                       ":/img/plugins/securitycenter/net_48_white.png",\
                                       //~ contents_path /securitycenter/Net protection
                                       tr("Net protection"), \
                                       tr("Manage and control network"), \
                                       "/usr/sbin/ksc-defender --net-protect");

    BlockWidget * security_setting_Widget = new BlockWidget();
    security_setting_Widget->setupComponent(":/img/plugins/securitycenter/set2px.png", \
                                            ":/img/plugins/securitycenter/set2@2x_1.png",\
                                            //~ contents_path /securitycenter/Secure Config
                                            tr("Secure Config"), \
                                            tr("Simple Config"), \
                                            "/usr/sbin/ksc-defender --security-setting");

    flowLayout->addWidget(account_sec_Widget);
    flowLayout->addWidget(baseline_ctrl_Widget);
    flowLayout->addWidget(virus_protect_Widget);
    flowLayout->addWidget(net_protect_Widget);
    flowLayout->addWidget(exec_ctrl_Widget);
    flowLayout->addWidget(security_setting_Widget);

    connect(account_sec_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(baseline_ctrl_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(virus_protect_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(net_protect_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(exec_ctrl_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
    connect(security_setting_Widget, &BlockWidget::bwClicked, [=](QString cmd){
        runExternalApp(cmd);
    });
}

void SecurityCenter::runExternalApp(QString cmd) {
    QProcess process(this);
    process.startDetached(cmd);
}

