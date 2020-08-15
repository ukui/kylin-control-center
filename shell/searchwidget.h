#pragma once

#include <QWidget>
#include <QList>
#include <QLocale>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QXmlStreamReader>

const QString XML_Source = "source";
const QString XML_Title = "translation";
const QString XML_Numerusform = "numerusform";
const QString XML_Explain_Path = "extra-contents_path";

class SearchWidget : public QLineEdit
{
    Q_OBJECT
public:
    struct SearchBoxStruct {
        QString translateContent;
        QString actualModuleName;
        QString childPageName;
        QString fullPagePath;
    };

    struct SearchDataStruct {
        QString chiese;
        QString pinyin;
    };

    struct UnexsitStruct {
        QString module;
        QString datail;
    };

public:
    SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget() override;

    bool jumpContentPathWidget(QString path);
    void setLanguage(QString type);
    void addModulesName(QString moduleName, QString searchName, QString translation = "");
    void addUnExsitData(QString module = "", QString datail = "");

private Q_SLOTS:
    void onCompleterActivated(QString value);

Q_SIGNALS:
    void notifyModuleSearch(QString);

private:
    void loadxml();
    SearchBoxStruct getModuleBtnString(QString value);
    QString getModulesName(QString name, bool state = true);
    QString removeDigital(QString input);
    QString transPinyinToChinese(QString pinyin);
    QString containTxtData(QString txt);
    void appendChineseData(SearchBoxStruct data);
    void clearSearchData();

private:
    QStandardItemModel *m_model;
    QCompleter *m_completer;
    QList<SearchBoxStruct> m_EnterNewPagelist;
    SearchBoxStruct m_searchBoxStruct;
    QString m_xmlExplain;
    QSet<QString> m_xmlFilePath;
    QString m_lang;
    QList<QPair<QString, QString>> m_moduleNameList;//用于存储如 "update"和"Update"
    QList<SearchDataStruct> m_inputList;
    bool m_bIsChinese;
    QList<UnexsitStruct> m_unexsitList;
    QString m_searchValue;
    bool m_bIstextEdited;
    QStringList m_defaultRemoveableList;//存储已知全部模块是否存在
    QList<QPair<QString, QString>> m_removedefaultWidgetList;//用于存储可以出设备名称，和该名称对应的页面
    QList<QPair<QString, QString>> m_removeableActualExistList;//存储实际模块是否存在
    bool m_bIsServerType;
    bool m_bIsIsDesktopType;
    QList<QPair<QString, bool>> m_serverTxtList;//QString表示和服务器/桌面版有关的文言,bool:true表示只有服务器版会存在,false表示只有桌面版存在
    QList<QString> m_TxtList;
    bool m_speechState;
};
