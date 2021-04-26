#include "search.h"
#include "ui_search.h"

Search::Search()
{
    m_plugin_name = tr("Search");
    m_plugin_type = NOTICEANDTASKS;
    initUi();
}

Search::~Search()
{
    if (m_gsettings) {
        delete m_gsettings;
        m_gsettings = nullptr;
    }
    if (m_searchMethodBtn) {
        delete m_searchMethodBtn;
        m_searchMethodBtn = nullptr;
    }
    if (m_webEngineFrame) {
        delete m_webEngineFrame;
        m_webEngineFrame = nullptr;
    }
}

QString Search::get_plugin_name()
{
    return m_plugin_name;
}

int Search::get_plugin_type()
{
    return m_plugin_type;
}

QWidget *Search::get_plugin_ui()
{
    ui = new Ui::Search;
    m_plugin_widget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(m_plugin_widget);

    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if (QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id, QByteArray(), this);
        //按钮状态初始化
        if (m_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
            m_searchMethodBtn->setChecked(is_index_search_on);
        } else {
            m_searchMethodBtn->setEnabled(false);
        }
        if (m_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
            m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
        } else {
            m_webEngineFrame->mCombox->setEnabled(false);
        }
        connect(m_gsettings, &QGSettings::changed, this, [ = ](const QString &key) {
            if (key == SEARCH_METHOD_KEY) {
                bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
                m_searchMethodBtn->blockSignals(true);
                m_searchMethodBtn->setChecked(is_index_search_on);
                m_searchMethodBtn->blockSignals(false);
            } else if (key == WEB_ENGINE_KEY) {
                QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
                m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
            }
        });
        connect(m_searchMethodBtn, &SwitchButton::checkedChanged, this, [ = ](bool checked) {
            if (m_gsettings && m_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
                m_gsettings->set(SEARCH_METHOD_KEY, checked);
            }
        });
        connect(m_webEngineFrame->mCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            if (m_gsettings && m_gsettings->keys().contains(WEB_ENGINE_KEY)) {
                m_gsettings->set(WEB_ENGINE_KEY, m_webEngineFrame->mCombox->currentData().toString());
            }
        });
    } else {
        qCritical() << UKUI_SEARCH_SCHEMAS << " not installed!\n";
        m_searchMethodBtn->setEnabled(false);
        m_webEngineFrame->mCombox->setEnabled(false);
    }
    return m_plugin_widget;
}

void Search::plugin_delay_control()
{

}

const QString Search::name() const
{
    return QStringLiteral("search");
}

void Search::initUi()
{
    QFont font;
    font.setPixelSize(18);
    m_plugin_widget = new QWidget;
    m_mainLyt = new QVBoxLayout(m_plugin_widget);
    m_plugin_widget->setLayout(m_mainLyt);
    m_methodTitleLabel = new QLabel(m_plugin_widget);
    m_methodTitleLabel->setText(tr("Create Index"));
    m_methodTitleLabel->setFont(font);
    m_descLabel = new QLabel(m_plugin_widget);
    m_descLabel->setText(tr("Creating index can help you getting results quickly."));
    m_searchMethodFrame = new QFrame(m_plugin_widget);
    m_searchMethodFrame->setFrameShape(QFrame::Shape::Box);
    m_searchMethodFrame->setFixedHeight(56);
    m_searchMethodFrame->setMinimumWidth(550);
    m_searchMethodFrame->setMaximumWidth(960);
    m_searchMethodLyt = new QHBoxLayout(m_searchMethodFrame);
    m_searchMethodFrame->setLayout(m_searchMethodLyt);
    m_searchMethodLabel = new QLabel(m_searchMethodFrame);
    m_searchMethodLabel->setText(tr("Create Index"));
    m_searchMethodBtn = new SwitchButton(m_searchMethodFrame);
    m_searchMethodLyt->addWidget(m_searchMethodLabel);
    m_searchMethodLyt->addStretch();
    m_searchMethodLyt->addWidget(m_searchMethodBtn);
    m_mainLyt->addWidget(m_methodTitleLabel);
    m_mainLyt->addWidget(m_descLabel);
    m_mainLyt->addWidget(m_searchMethodFrame);

    m_webEngineLabel = new QLabel(m_plugin_widget);
    m_webEngineLabel->setFont(font);
    m_webEngineLabel->setText(tr("Web Engine"));
    m_webEngineFrame = new ComboxFrame(tr("Default web searching engine"), m_plugin_widget);
    m_webEngineFrame->setFixedHeight(56);
    m_webEngineFrame->setMinimumWidth(550);
    m_webEngineFrame->setMaximumWidth(960);
    m_webEngineFrame->mCombox->insertItem(0, QIcon(":/img/plugins/search/baidu.svg"), tr("baidu"), "baidu");
    m_webEngineFrame->mCombox->insertItem(1, QIcon(":/img/plugins/search/sougou.svg"), tr("sougou"), "sougou");
    m_webEngineFrame->mCombox->insertItem(2, QIcon(":/img/plugins/search/360.svg"), tr("360"), "360");
    m_mainLyt->addSpacing(30);
    m_mainLyt->addWidget(m_webEngineLabel);
    m_mainLyt->addWidget(m_webEngineFrame);
    m_mainLyt->addStretch();
    m_mainLyt->setContentsMargins(0, 0, 40, 0);
}
