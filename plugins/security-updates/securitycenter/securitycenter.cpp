#include "securitycenter.h"
#include "ui_securitycenter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

#include <QProcess>

#include <QDebug>


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
    textHorLayout->setSpacing(6);

    titleLable = new QLabel;
    detailLabel = new QLabel;

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
    detailLabel->setText(detail);
    _cmd = cmd;
}

void BlockWidget::enterEvent(QEvent *event){
    //style

    QWidget::enterEvent(event);
}

void BlockWidget::leaveEvent(QEvent *event){
    //style

    QWidget::leaveEvent(event);
}

void BlockWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        emit bwClicked(_cmd);
    }

    QWidget::mousePressEvent(event);
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

    pluginName = tr("SecurityCenter");
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

