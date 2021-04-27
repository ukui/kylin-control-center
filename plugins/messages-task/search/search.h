#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QGSettings>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <SwitchButton/switchbutton.h>
#include <QSettings>
#include <QFileDialog>
#include <QTextCodec>
#include <QPushButton>
#include <QMessageBox>
#include "HoverWidget/hoverwidget.h"
#include "shell/interface.h"
#include "ComboxFrame/comboxframe.h"

#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define SEARCH_METHOD_KEY "indexSearch"
#define WEB_ENGINE_KEY "webEngine"
//TODO
#define CONFIG_FILE "/.config/org.ukui/ukui-search/ukui-search-block-dirs.conf"

namespace Ui {
class Search;
}

enum ReturnCode {
    Succeed,
    PathEmpty,
    NotInHomeDir,
    ParentExist
};

class Search : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    explicit Search();
    ~Search();

    QString get_plugin_name()   Q_DECL_OVERRIDE;
    int get_plugin_type()       Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui()   Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

private:
    Ui::Search *ui;

    QWidget * m_plugin_widget = nullptr;
    QString m_plugin_name = "";
    int m_plugin_type = 0;

    QGSettings * m_gsettings = nullptr;

    void initUi();
    QVBoxLayout * m_mainLyt = nullptr;
    //设置搜索模式
    QLabel * m_methodTitleLabel = nullptr;
    QLabel * m_descLabel = nullptr;
    QFrame * m_searchMethodFrame = nullptr;
    QHBoxLayout * m_searchMethodLyt = nullptr;
    QLabel * m_searchMethodLabel = nullptr;
    SwitchButton * m_searchMethodBtn = nullptr;
    //设置黑名单
    QLabel * m_blockDirTitleLabel = nullptr;
    QLabel * m_blockDirDescLabel = nullptr;
    QFrame * m_blockDirsFrame = nullptr;
    QVBoxLayout * m_blockDirsLyt = nullptr;
    HoverWidget * m_addBlockDirWidget = nullptr;
    QLabel * m_addBlockDirIcon = nullptr;
    QLabel * m_addBlockDirLabel = nullptr;
    QHBoxLayout * m_addBlockDirLyt = nullptr;

    QStringList m_blockDirs;
    QSettings * m_dirSettings = nullptr;
    void getBlockDirs();
    int setBlockDir(const QString &dirPath, const bool &is_add = true);
    void appendBlockDirToList(const QString &path);
    void removeBlockDirFromList(const QString &path);
    void initBlockDirsList();
//    void refreshBlockDirsList();
    //设置搜索引擎
    QLabel * m_webEngineLabel = nullptr;
    ComboxFrame * m_webEngineFrame = nullptr;

    void setupConnection();

private slots:
    void onBtnAddFolderClicked();
};

#endif // SEARCH_H
