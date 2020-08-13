#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QSet>
#include <QList>
#include <QWidget>
#include <QObject>
#include <QString>
#include <QLineEdit>
#include <QCompleter>
#include <QStandardItemModel>

const QString XML_Source       = "source";
const QString XML_Title        = "translation";
const QString XML_Numerusform  = "numerusform";
const QString XML_Explain_Path = "extra-contents_path";
const QString XML_Child_Path   = "extra-child_page";

class SearchWidget : public QLineEdit
{
    Q_OBJECT
public:
    struct SearchBoxStruct {
        QString translateContent;
        QString actualMouldeName;
        QString childPageName;
        QString fullPagePath;
    };

    struct SearchDataStruct {
        QString chiese;
        QString pinyin;
    };

public:
    SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget();


    bool jumpContentPathWidget(QString path);
    void setLanguage(QString type);
    void addModulesName(QString moduleName, QString searchName, QString translation = "");

private:
    void loadxml();
    QString transPinyinToChinese(QString pinyin);
    QString containTxtData(QString txt);
    QString getModulesName(QString name, bool state = true);
    QString removeDigital(QString input);
    void appendChineseData(SearchBoxStruct data);
    void clearSearchData();

signals:
    void notifyModulesSearch(QString, QString);

private:
    QStandardItemModel *m_model;
    QCompleter         *m_completer;
    QList<SearchBoxStruct> m_EnterNewPageStruct;
    SearchBoxStruct m_searchBoxStruct;
    QString m_xmlExplain;
    QSet<QString> m_xmlFilePath;    // 翻译文件位置
    QString m_lang;
    QList<QPair<QString, QString>> m_moduleNameList;
    bool m_bIsChinese;
    bool m_bIstextEdited;
    QString m_searchValue;
    QList<SearchDataStruct> m_inputList;

    QList<QString> m_TxtList; // 翻译原文？

};

#endif // SEARCHWIDGET_H
