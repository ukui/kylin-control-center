#include "ksc_module_func_widget.h"
#include "ui_ksc_module_func_widget.h"

#include <QMouseEvent>
#include <QEvent>
#include <QStyle>

#include <QDebug>
#define THEME_QT_SCHEMA "org.ukui.style"

#include"fontwatcher.h"

ksc_module_func_widget::ksc_module_func_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ksc_module_func_widget)
{
    ui->setupUi(this);

    setFixedSize(QSize(280, 120));
    ui->module_name->setObjectName("ksc_module_func_widget_name_label");
    ui->module_name->setProperty("mouse_state", "normal");
    ui->module_name->setText(tr("TextLabel"));
    ui->module_icon->setStyleSheet("background:transparent;");
    ui->module_name->setStyleSheet("background:transparent;");

    QFont font;
    font.setBold(true);
    FontWatcher*m_fontWatcher = new FontWatcher(this);
    m_fontWatcher->Set_Single_Content_Special(m_fontWatcher->Font_Special(ui->module_name,50),1.3,18,font);

    QFont ft;
    ft.setPixelSize(12);

    ui->interval_icon_label->setPixmap(QPixmap(":/img/plugins/securitycenter/ rectangle_bule.png").scaled(ui->interval_icon_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

ksc_module_func_widget::~ksc_module_func_widget()
{
    delete ui;
}

void ksc_module_func_widget::set_module_data(ksc_defender_module module)
{
    m_module.module_type = module.module_type;
    m_module.module_name = module.module_name;
    m_module.module_desc = module.module_desc;
    m_module.module_normal_icon = module.module_normal_icon;
    m_module.module_hover_icon = module.module_hover_icon;
    m_module.module_exec = module.module_exec;
    m_module.status_list = module.status_list;

    ui->label_1->clear();
    ui->label_2->clear();
    if (module.status_list.size() >= 1)
        ui->label_1->setText(module.status_list.at(0));
    if (module.status_list.size() == 2)
        ui->label_2->setText(module.status_list.at(1));

    ui->module_name->setText(m_module.module_name);
    ui->module_icon->setPixmap(QPixmap(m_module.module_normal_icon).scaled(ui->module_icon->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ksc_module_func_widget::update_module_data(ksc_defender_module module)
{
    m_module.module_type = module.module_type;
    m_module.module_name = module.module_name;
    m_module.module_desc = module.module_desc;
    m_module.module_normal_icon = module.module_normal_icon;
    m_module.module_hover_icon = module.module_hover_icon;
    m_module.module_exec = module.module_exec;
    m_module.status_list = module.status_list;

    ui->label_1->clear();
    ui->label_2->clear();
    if (module.status_list.size() >= 1)
        ui->label_1->setText(module.status_list.at(0));
    if (module.status_list.size() == 2)
        ui->label_2->setText(module.status_list.at(1));

    ui->module_name->setText(m_module.module_name);
}

void ksc_module_func_widget::enterEvent(QEvent *event)
{
    Q_UNUSED(event)


    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);
    this->setStyleSheet("background:palette(Highlight);"
                        "border-radius:6px;");

    ui->module_icon->setPixmap(QPixmap(m_module.module_hover_icon).scaled(ui->module_icon->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    ui->interval_icon_label->setPixmap(QPixmap(":/img/plugins/securitycenter/ rectangle_white.png").scaled(ui->interval_icon_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QWidget::enterEvent(event);
}

void ksc_module_func_widget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    this->setStyleSheet("background:transparent;"
                        "border-radius:6px;");

    ui->module_icon->setPixmap(QPixmap(m_module.module_normal_icon).scaled(ui->module_icon->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    ui->interval_icon_label->setPixmap(QPixmap(":/img/plugins/securitycenter/ rectangle_bule.png").scaled(ui->interval_icon_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QWidget::leaveEvent(event);
}

void ksc_module_func_widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ksc_module_func_widget::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button())
        runExternalApp(m_module.module_exec);

    QWidget::mousePressEvent(event);
}
