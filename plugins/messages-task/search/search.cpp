#include "search.h"
#include "ui_search.h"
#include "ImageUtil/imageutil.h"

Search::Search()
{
    //~ contents_path /search/Search
    m_plugin_name = tr("Search");
    m_plugin_type = NOTICEANDTASKS;
    initUi();
    setupConnection();
    m_dirSettings = new QSettings(QDir::homePath() + CONFIG_FILE, QSettings::NativeFormat, this);
    m_dirSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    initBlockDirsList();
}

Search::~Search()
{
    if (m_gsettings) {
        delete m_gsettings;
        m_gsettings = nullptr;
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
            //当前是否使用索引搜索/暴力搜索
            bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
            m_searchMethodBtn->setChecked(is_index_search_on);
        } else {
            m_searchMethodBtn->setEnabled(false);
        }
        if (m_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            //当前网页搜索的搜索引擎
            QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
            m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
        } else {
            m_webEngineFrame->mCombox->setEnabled(false);
        }
        //监听gsettings值改变，更新控制面板UI
        connect(m_gsettings, &QGSettings::changed, this, [ = ](const QString &key) {
            if (key == SEARCH_METHOD_KEY) {
                bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
                m_searchMethodBtn->blockSignals(true);
                m_searchMethodBtn->setChecked(is_index_search_on);
                m_searchMethodBtn->blockSignals(false);
            } else if (key == WEB_ENGINE_KEY) {
                QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
                m_webEngineFrame->mCombox->blockSignals(true);
                m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
                m_webEngineFrame->mCombox->blockSignals(false);
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

/**
 * @brief Search::initUi 初始化此插件UI
 */
void Search::initUi()
{
    m_plugin_widget = new QWidget;
    m_mainLyt = new QVBoxLayout(m_plugin_widget);
    m_plugin_widget->setLayout(m_mainLyt);
    //设置搜索模式部分的ui
    m_methodTitleLabel = new TitleLabel(m_plugin_widget);
    m_methodTitleLabel->setText(tr("Create Index"));
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
    //设置黑名单文件夹部分的ui
    m_blockDirTitleLabel = new TitleLabel(m_plugin_widget);
    m_blockDirTitleLabel->setText(tr("Block Folders"));
    m_blockDirDescLabel = new QLabel(m_plugin_widget);
    m_blockDirDescLabel->setWordWrap(true);
    m_blockDirDescLabel->setText(tr("Following folders will not be searched. You can set it by adding and removing folders."));
    m_blockDirsFrame = new QFrame(m_plugin_widget);
    m_blockDirsFrame->setFrameShape(QFrame::Shape::NoFrame);
    m_blockDirsLyt = new QVBoxLayout(m_blockDirsFrame);
    m_blockDirsFrame->setLayout(m_blockDirsLyt);
    m_blockDirsLyt->setContentsMargins(0, 0, 0, 0);
    m_blockDirsLyt->setSpacing(2);
    m_addBlockDirWidget = new HoverWidget("", m_plugin_widget);
    m_addBlockDirWidget->setObjectName("addBlockDirWidget");
    QPalette pal;
    QBrush brush = pal.highlight();  //获取window的色值
    QColor highLightColor = brush.color();
    QString stringColor = QString("rgba(%1,%2,%3)") //叠加20%白色
           .arg(highLightColor.red()*0.8 + 255*0.2)
           .arg(highLightColor.green()*0.8 + 255*0.2)
           .arg(highLightColor.blue()*0.8 + 255*0.2);

    m_addBlockDirWidget->setStyleSheet(QString("HoverWidget#addBlockDirWidget{background: palette(button);\
                                   border-radius: 4px;}\
                                   HoverWidget:hover:!pressed#addBlockDirWidget{background: %1;  \
                                   border-radius: 4px;}").arg(stringColor));
    m_addBlockDirWidget->setFixedHeight(50);
    m_addBlockDirWidget->setMaximumWidth(960);
    m_addBlockDirIcon = new QLabel(m_addBlockDirWidget);
    m_addBlockDirIcon->setPixmap(QIcon(":/img/titlebar/add.svg").pixmap(12, 12));
    m_addBlockDirIcon->setProperty("useIconHighlightEffect", true);
    m_addBlockDirIcon->setProperty("iconHighlightEffectMode", 1);
    m_addBlockDirLabel = new QLabel(m_addBlockDirWidget);
    m_addBlockDirLabel->setText(tr("Choose folder"));
    m_addBlockDirLyt = new QHBoxLayout(m_addBlockDirWidget);
    m_addBlockDirWidget->setLayout(m_addBlockDirLyt);
    m_addBlockDirLyt->addWidget(m_addBlockDirIcon);
    m_addBlockDirLyt->addWidget(m_addBlockDirLabel);
    m_addBlockDirLyt->addStretch();
    m_mainLyt->addSpacing(30);
    m_mainLyt->addWidget(m_blockDirTitleLabel);
    m_mainLyt->addWidget(m_blockDirDescLabel);
    m_mainLyt->addWidget(m_blockDirsFrame);
    m_mainLyt->addWidget(m_addBlockDirWidget);
    //设置网页搜索引擎部分的ui
    m_webEngineLabel = new TitleLabel(m_plugin_widget);
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

    // 悬浮改变Widget状态
    connect(m_addBlockDirWidget, &HoverWidget::enterWidget, this, [=](){

        m_addBlockDirIcon->setProperty("useIconHighlightEffect", false);
        m_addBlockDirIcon->setProperty("iconHighlightEffectMode", 0);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "white", 12);
        m_addBlockDirIcon->setPixmap(pixgray);
        m_addBlockDirLabel->setStyleSheet("color: white;");
    });

    // 还原状态
    connect(m_addBlockDirWidget, &HoverWidget::leaveWidget, this, [=](){

        m_addBlockDirIcon->setProperty("useIconHighlightEffect", true);
        m_addBlockDirIcon->setProperty("iconHighlightEffectMode", 1);
        QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
        m_addBlockDirIcon->setPixmap(pixgray);
        m_addBlockDirLabel->setStyleSheet("color: palette(windowText);");
    });
}

/**
 * @brief Search::getBlockDirs 从配置文件获取黑名单并将黑名单列表传入
 */
void Search::getBlockDirs()
{
    m_blockDirs.clear();
    if (m_dirSettings)
        m_blockDirs = m_dirSettings->allKeys();
}

/**
 * @brief Search::setBlockDir 尝试写入新的黑名单文件夹
 * @param dirPath 待添加到黑名单的文件夹路径
 * @param is_add 是否是在添加黑名单
 * @return 0成功 !0添加失败的错误代码
 */
int Search::setBlockDir(const QString &dirPath, const bool &is_add)
{
    if (!is_add) {
        if (dirPath.isEmpty()) {
            return ReturnCode::PathEmpty;
        }
        //删除黑名单目录
        m_dirSettings->remove(dirPath);
        removeBlockDirFromList(dirPath);
        return ReturnCode::Succeed;
    }
    if (!dirPath.startsWith("/home")) {
        return ReturnCode::NotInHomeDir;
    }

    QString pathKey = dirPath.right(dirPath.length() - 1);

    for (QString dir : m_blockDirs) {
        if (pathKey == dir) {
            return ReturnCode::HasBeenBlocked;
        }

        if (pathKey.startsWith(dir)) {
            return ReturnCode::ParentExist;
        }

        //有它的子文件夹已被添加，删除这些子文件夹
        if (dir.startsWith(pathKey)) {
            m_dirSettings->remove(dir);
            removeBlockDirFromList(dir);
        }
    }
    m_dirSettings->setValue(pathKey, "0");
    appendBlockDirToList(dirPath);
    return ReturnCode::Succeed;
}

/**
 * @brief Search::initBlockDirsList 初始化黑名单列表
 */
void Search::initBlockDirsList()
{
    getBlockDirs();
    foreach (QString path, m_blockDirs) {
        QString wholePath = QString("/%1").arg(path);
        if (QFileInfo(wholePath).isDir() && path.startsWith("home")) {
            appendBlockDirToList(wholePath);
        }
    }
}

///**
// * @brief Search::refreshBlockDirsList
// */
//void Search::refreshBlockDirsList()
//{

//}

void Search::appendBlockDirToList(const QString &path)
{
    HoverWidget * dirWidget = new HoverWidget(path, m_blockDirsFrame);
    dirWidget->setObjectName(path);
    dirWidget->setMinimumSize(550,50);
    dirWidget->setMaximumSize(960,50);
    dirWidget->setAttribute(Qt::WA_DeleteOnClose);
    QHBoxLayout * dirWidgetLyt = new QHBoxLayout(dirWidget);
    dirWidgetLyt->setSpacing(8);
    dirWidgetLyt->setContentsMargins(0, 0, 0, 0);
    dirWidget->setLayout(dirWidgetLyt);
    QFrame * dirFrame = new QFrame(dirWidget);
    dirFrame->setFrameShape(QFrame::Shape::Box);
    dirFrame->setFixedHeight(50);
    QHBoxLayout * dirFrameLayout = new QHBoxLayout(dirFrame);
    dirFrameLayout->setSpacing(16);
    dirFrameLayout->setContentsMargins(16, 0, 16, 0);
    QLabel * iconLabel = new QLabel(dirFrame);
    QLabel * pathLabel = new QLabel(dirFrame);
    dirFrameLayout->addWidget(iconLabel);
    iconLabel->setPixmap(QIcon::fromTheme("inode-directory").pixmap(QSize(24, 24)));
    pathLabel->setText(path);
    dirFrameLayout->addWidget(pathLabel);
    dirFrameLayout->addStretch();
    QPushButton * delBtn = new QPushButton(dirWidget);
    delBtn->setText(tr("delete"));
    delBtn->hide();
    connect(delBtn, &QPushButton::clicked, this, [ = ]() {
        setBlockDir(path, false);
        getBlockDirs();
    });
    connect(dirWidget, &HoverWidget::enterWidget, this, [ = ]() {
        delBtn->show();
    });
    connect(dirWidget, &HoverWidget::leaveWidget, this, [ = ]() {
        delBtn->hide();
    });
    dirWidgetLyt->addWidget(dirFrame);
    dirWidgetLyt->addWidget(delBtn);
    m_blockDirsLyt->addWidget(dirWidget);
}

void Search::removeBlockDirFromList(const QString &path)
{
    HoverWidget * delDirWidget = m_blockDirsFrame->findChild<HoverWidget *>(path);
    if (delDirWidget) {
        qDebug() << "Delete folder succeed! path = " << path;
        delDirWidget->close();
    }
}

void Search::setupConnection()
{
    connect(m_addBlockDirWidget, &HoverWidget::widgetClicked, this, &Search::onBtnAddFolderClicked);
}

void Search::onBtnAddFolderClicked()
{
    QFileDialog * fileDialog = new QFileDialog(m_plugin_widget);
//    fileDialog->setFileMode(QFileDialog::Directory); //允许查看文件和文件夹，但只允许选择文件夹
    fileDialog->setFileMode(QFileDialog::DirectoryOnly); //只允许查看文件夹
//    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setDirectory(QDir::homePath());
    fileDialog->setNameFilter(tr("Directories"));
    fileDialog->setWindowTitle(tr("select blocked folder"));
    fileDialog->setLabelText(QFileDialog::Accept, tr("Select"));
    fileDialog->setLabelText(QFileDialog::LookIn, tr("Position: "));
    fileDialog->setLabelText(QFileDialog::FileName, tr("FileName: "));
    fileDialog->setLabelText(QFileDialog::FileType, tr("FileType: "));
    fileDialog->setLabelText(QFileDialog::Reject, tr("Cancel"));
    if(fileDialog->exec() != QDialog::Accepted) {
        fileDialog->deleteLater();
        return;
    }
    QString selectedDir = 0;
    selectedDir = fileDialog->selectedFiles().first();
    qDebug() << "Selected a folder in onBtnAddClicked(): " << selectedDir;
    int returnCode = setBlockDir(selectedDir, true);
    switch (returnCode) {
    case ReturnCode::Succeed :
        qDebug() << "Add blocked folder succeed! path = " << selectedDir;
        getBlockDirs();
        break;
    case ReturnCode::PathEmpty :
        qWarning() << "Add blocked folder failed, choosen path is empty! path = " << selectedDir;
        QMessageBox::warning(m_plugin_widget, tr("Warning"), tr("Add blocked folder failed, choosen path is empty!"));
        break;
    case ReturnCode::NotInHomeDir :
        qWarning() << "Add blocked folder failed, it is not in home path! path = " << selectedDir;
        QMessageBox::warning(m_plugin_widget, tr("Warning"), tr("Add blocked folder failed, it is not in home path!"));
        break;
    case ReturnCode::ParentExist :
        qWarning() << "Add blocked folder failed, its parent dir is exist! path = " << selectedDir;
        QMessageBox::warning(m_plugin_widget, tr("Warning"), tr("Add blocked folder failed, its parent dir is exist!"));
        break;
    case ReturnCode::HasBeenBlocked :
        qWarning() << "Add blocked folder failed, it has been already blocked! path = " << selectedDir;
        QMessageBox::warning(m_plugin_widget, tr("Warning"), tr("Add blocked folder failed, it has been already blocked!"));
        break;
    default:
        break;
    }
}
