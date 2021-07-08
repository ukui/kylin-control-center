#include "ksc_main_page_widget.h"
#include "ui_ksc_main_page_widget.h"
#include "ksc_module_func_widget.h"
#include <QScrollBar>

#include <locale.h>
#include <libintl.h>
#include"ksc_set_font_size.h"

#include <kysec/status.h>
#include <kysec/db.h>
#include <kysec/fdir.h>
#include <QDebug>
#include <ukui-log4qt.h>

#include"fontwatcher.h"

#define FOUND_COUINT_MAX 1000
#define MODULE_BUTTON_WIDTH 280
#define MODULE_BUTTON_HEIGHT 120

ksc_main_page_widget::ksc_main_page_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ksc_main_page_widget)
{
    ui->setupUi(this);

    qRegisterMetaType<ksc_defender_module>("ksc_defender_module");
    qDBusRegisterMetaType<ksc_defender_module>();

    qRegisterMetaType<ksc_defender_module_list>("ksc_defender_module_list");
    qDBusRegisterMetaType<ksc_defender_module_list>();

    m_pInterface = new com::ksc::defender::securitycenter("com.ksc.defender", "/securitycenter",
                                                          QDBusConnection::systemBus());
    connect(m_pInterface, SIGNAL(kylin_security_center_module_changed(ksc_defender_module)), this, SLOT(slot_recv_ksc_defender_module_change(ksc_defender_module)));

    ui->content_label->setObjectName("ksc_main_page_widget_context_label");
    ui->content_label->adjustSize();
    ui->detail_label->setObjectName("ksc_main_page_widget_detail_label");
    ui->detail_label->adjustSize();
    ui->content_label->setText(tr("Security Overview"));
    ui->detail_label->setText(tr("Anti-violence account security Real-time network intrusion detection One-click repairable system Core-file protection"));
    ui->pushButton->setText(tr("Run Security Center"));

    init_list_widget();

    QFont font;
    font.setPixelSize(20);
    ui->content_label->setStyleSheet("QLabel{color: palette(windowText);font-weight:bold;}");
    FontWatcher*m_fontWatcher = new FontWatcher(this);
    m_fontWatcher->Set_Single_Content_Special(m_fontWatcher->Font_Special(ui->content_label,50),1.3,20,font);

}

ksc_main_page_widget::~ksc_main_page_widget()
{
    delete ui;
}

void ksc_main_page_widget::init_list_widget()
{   
    m_map.clear();
    flowLayout = new FlowLayout();
    flowLayout->setContentsMargins(0, 0, 0, 0);
    ui->widget->setLayout(flowLayout);

    ksc_defender_module_list list;
    QDBusReply<int> reply = m_pInterface->get_kylin_security_center_modules(list);
    if (!reply.isValid())
        return;

    foreach (ksc_defender_module module, list) {
        ksc_module_func_widget *pWidget = new ksc_module_func_widget(this);
        pWidget->setFixedSize(QSize(MODULE_BUTTON_WIDTH, MODULE_BUTTON_HEIGHT));
        auto_set_main_icon(module);
        pWidget->set_module_data(module);

        flowLayout->addWidget(pWidget);
        m_map.insert(module.module_type, pWidget);
    }

    return ;
}

void ksc_main_page_widget::auto_set_main_icon(ksc_defender_module &module)
{
    switch (module.module_type) {
    case DEFENDER_SCAN:
        module.module_normal_icon = ":/img/plugins/securitycenter/saomiao.png";
        module.module_hover_icon = ":/img/plugins/securitycenter/saomiao-white.png";
        break;
    case DEFENDER_ACCOUNT:
        module.module_normal_icon = ":/img/plugins/securitycenter/user_sercity.png";
        module.module_hover_icon = ":/img/plugins/securitycenter/user_sercity_white.png";
        break;
    case DEFENDER_NETWORK:
        module.module_normal_icon = ":/img/plugins/securitycenter/lan.png";
        module.module_hover_icon = ":/img/plugins/securitycenter/bai.png";
        break;
    case DEFENDER_VIRUS:
        module.module_normal_icon = ":/img/plugins/securitycenter/bingdufanghu.png";
        module.module_hover_icon = ":/img/plugins/securitycenter/bingdufanghu-white.png";
        break;
    case DEFENDER_CONTROL:
        module.module_normal_icon = ":/img/plugins/securitycenter/yingyonkongzhiyubaohu.png";
        module.module_hover_icon = ":/img/plugins/securitycenter/yingyongkongzhiyubaohu-white.png";
        break;
    default:
        break;
    }
}

void ksc_main_page_widget::slot_recv_ksc_defender_module_change(ksc_defender_module module)
{
    if (m_map.contains(module.module_type))
    {
        ksc_module_func_widget *pWidget = m_map.value(module.module_type);
        if (pWidget)
        {
            auto_set_main_icon(module);
            pWidget->update_module_data(module);
        }
    }
}

void ksc_main_page_widget::on_pushButton_clicked()
{
    QProcess process(this);
    process.startDetached("/usr/sbin/ksc-defender");
}
