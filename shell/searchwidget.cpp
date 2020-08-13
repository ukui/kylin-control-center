#include "searchwidget.h"
#include "pinyin.h"

#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QCompleter>
#include <QXmlStreamReader>
#include <QRegularExpression>

class ukCompleter : public QCompleter {
public:
    ukCompleter(QAbstractItemModel *model, QObject *parent = nullptr)
        :QCompleter(model, parent)
    {

    }

};

SearchWidget::SearchWidget(QWidget *parent)
    : QLineEdit(parent)
    , m_xmlExplain("")
    , m_bIsChinese(false)
    , m_searchValue("")
    , m_bIstextEdited(false)
{
    m_model      = new QStandardItemModel(this);
    m_completer  = new ukCompleter(m_model, this);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionRole(Qt::UserRole);

    this->setCompleter(m_completer);
    m_completer->setWrapAround(false);
    m_completer->installEventFilter(this);

    connect(this, &QLineEdit::textEdited, this, [=](QString retValue) {
        if ("" != retValue) {
            m_bIstextEdited = true;
        } else {
            m_bIstextEdited = false;
        }
    });

    connect(this, &QLineEdit::textChanged, this,[=](QString text) {

        qDebug() << " the text is " << text << endl;
        QString retValue = text;
        if (m_bIstextEdited) {
            m_bIstextEdited = false;
            return ;
        }

        if ("" == retValue || m_searchValue.contains(retValue, Qt::CaseInsensitive)) {
            m_searchValue = retValue;
            return ;
        }

        // 拼音转化为汉字
        retValue = transPinyinToChinese(text);

        if (retValue == text) {
            retValue = containTxtData(retValue);
        }

        m_searchValue = retValue;
        this->setText(retValue);
    });

    connect(this, &QLineEdit::returnPressed, this, [=]{
        QString retValue = this->text();
    });

}

SearchWidget::~SearchWidget() {

}

bool SearchWidget::jumpContentPathWidget(QString path) {

}

void SearchWidget::setLanguage(QString type) {
    m_lang = type;
    if ("zh_CN" == type || "zh_HK" == type || "zh_TW" == type) {
        m_bIsChinese = true;
        m_completer->setCompletionRole(Qt::UserRole);
    } else {
        m_completer->setCompletionRole(Qt::DisplayRole);
    }

    loadxml();
}

void SearchWidget::addModulesName(QString moduleName, QString searchName, QString translation) {

    QPair<QString, QString> data;
    data.first  = moduleName;
    data.second = searchName;
    m_moduleNameList.append(data);

    if (!translation.isEmpty()) {
        m_xmlFilePath.insert(translation);
    }
}

void SearchWidget::loadxml() {

    if (!m_EnterNewPageStruct.isEmpty()) {
        m_EnterNewPageStruct.clear();
    }

    if (!m_inputList.isEmpty()) {
        m_inputList.clear();
    }

    if (m_model->rowCount() > 0) {
        QStandardItem *item = nullptr;
        for (int i = 0; i < m_model->rowCount(); i++) {
            item = m_model->takeItem(i);
            delete item;
            item = nullptr;
        }
        m_model->clear();
    }

    // 添加一项空数据
    m_searchBoxStruct.fullPagePath = "";
    m_searchBoxStruct.actualMouldeName = "";
    m_searchBoxStruct.translateContent = "";
    m_searchBoxStruct.childPageName = "";
    m_EnterNewPageStruct.append(m_searchBoxStruct);
    m_inputList.append(SearchDataStruct());
    m_model->appendRow(new QStandardItem(""));

    // 判断是否为中文
    auto isChineseFunc = [](const QString &str)->bool {
        QRegularExpression rex_expression(R"(^[^a-zA-Z]+$)");
        return rex_expression.match(str).hasMatch();
    };

    for (const QString i : m_xmlFilePath) {
        QString xmlPath = i.arg(m_lang);
        QFile file(xmlPath);

        if (!file.exists()) {
            qWarning() << " file: SearWidget not exits";
            continue;
        }

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << " file: SearWidget  open failed";
            continue;
        }

        QXmlStreamReader xmlRead(&file);
        QStringRef dataName;
        QXmlStreamReader::TokenType type = QXmlStreamReader::Invalid;

        /**
         * 遍历XML文件
         */
        while (!xmlRead.atEnd()) {
            type = xmlRead.readNext();

            switch (type) {
            case QXmlStreamReader::StartElement:
                m_xmlExplain = xmlRead.name().toString();
                break;
            case QXmlStreamReader::Characters:
                if (!xmlRead.isWhitespace()) {

                    if (XML_Source == m_xmlExplain) {
                        m_searchBoxStruct.translateContent = xmlRead.text().toString();
                    } else if (XML_Title == m_xmlExplain) {         // 翻译内容
                        if ("" != xmlRead.text().toString()) {
                            m_searchBoxStruct.translateContent = xmlRead.text().toString();
                        }
                    } else if (XML_Numerusform == m_xmlExplain) {
                        if ("" != xmlRead.text().toString()) {
                            m_searchBoxStruct.translateContent = xmlRead.text().toString();
                        }
                    } else if (XML_Child_Path == m_xmlExplain) {
//                        m_searchBoxStruct.childPageName = tra
                    } else if (XML_Explain_Path == m_xmlExplain) {
                        m_searchBoxStruct.fullPagePath = xmlRead.text().toString();

                        m_searchBoxStruct.actualMouldeName =
                                getModulesName(m_searchBoxStruct.fullPagePath.section('/', 1, 1));

                        if (!isChineseFunc(m_searchBoxStruct.translateContent)) {
                            if (!m_TxtList.contains(m_searchBoxStruct.translateContent)) {
                                m_TxtList.append(m_searchBoxStruct.translateContent);
                            }
                        }

                        if ("" == m_searchBoxStruct.actualMouldeName || "" == m_searchBoxStruct.translateContent) {
                            clearSearchData();
                            continue;
                        }

                        m_EnterNewPageStruct.append(m_searchBoxStruct);

                        if (!m_bIsChinese) {

                            if ("" == m_searchBoxStruct.childPageName) {
                                m_model->appendRow(new QStandardItem(QString("%1 --> %2")
                                                                     .arg(m_searchBoxStruct.actualMouldeName)
                                                                     .arg(m_searchBoxStruct.translateContent)));
                            } else {
                                m_model->appendRow(new QStandardItem(QString("%1 --> %2 / %3")
                                                                     .arg(m_searchBoxStruct.actualMouldeName)
                                                                     .arg(m_searchBoxStruct.childPageName)                                                                     .arg(m_searchBoxStruct.translateContent)));
                            }
                        } else {
//                            appc
                        }
                    }
            default:
                break;
                }
            }
        }
    }
}

QString SearchWidget::transPinyinToChinese(QString pinyin) {

    QString value = pinyin;
    for (auto data : m_inputList) {
        if (value == data.pinyin) {
            value = data.chiese;
            break;
        }
    }

    return value;
}

QString SearchWidget::containTxtData(QString txt) {

    QString value = txt;

    // 遍历“汉字拼音列表”，将存在的“拼音”转换为“汉字”
    for (auto data : m_inputList) {
        if (data.chiese.contains(txt, Qt::CaseInsensitive) ||
                data.pinyin.contains(txt, Qt::CaseInsensitive)) {
            value = data.chiese;
            break;
        }
    }

    return value;
}

// 把路径翻译成tr("Name") ????
QString SearchWidget::getModulesName(QString name, bool state) {
        
    QString strResult = "";
    
    for (auto it : m_moduleNameList) {
        if (state) {    // 将路径翻译成块名称
            if (it.first == name) {
                strResult = it.second;
                break;
            }
        } else {
            if (it.second == name) {
                strResult = it.first;
                break;
            }
        }
    }

    qDebug() << "getModulesName is :" << strResult << endl;
    return strResult;
}

QString SearchWidget::removeDigital(QString input) {

    if ("" == input) {
        return "";
    }

    QString value = "";
    QByteArray ba = input.toLocal8Bit();
    char *data = nullptr;
    data = ba.data();
    while (*data) {
        if (!(*data >= '0' && *data <= '9')) {
            value += *data;
        }
        data++;
    }
}

void SearchWidget::appendChineseData(SearchWidget::SearchBoxStruct data) {

    if ("" == data.childPageName) {
        m_model->appendRow(new QStandardItem(QString("%1 --> %2")
                                             .arg(data.actualMouldeName)
                                             .arg(data.translateContent)));

        // 设置汉字的Qt::UserRole数据
        m_model->setData(m_model->index(m_model->rowCount() - 1, 0),
                         QString("%1 --> %2")
                         .arg(data.actualMouldeName)
                         .arg(data.translateContent),
                         Qt::UserRole);

        QString hanziTxt = QString("%1 --> %2").arg(data.actualMouldeName).arg(data.translateContent);

        for (auto datas : m_TxtList) {
            for (int i = 0; i < datas.count(); i++) {
                if (data.translateContent == datas) {
                    return;
                }
            }
        }

        QString pinyinTxt = QString("%1 --> %2")
                            .arg(removeDigital(Chinese2Pinyin(data.actualMouldeName)))
                            .arg(removeDigital(Chinese2Pinyin(data.translateContent)));

        // 添加显示的汉字?
        m_model->appendRow(new QStandardItem(hanziTxt));

        m_model->setData(m_model->index(m_model->rowCount() - 1, 0), pinyinTxt, Qt::UserRole);

        SearchDataStruct transdata;
        transdata.chiese = hanziTxt;
        transdata.pinyin = pinyinTxt;

        m_inputList.append(transdata);
    } else {

        m_model->appendRow(new QStandardItem(QString("%1 --> %2 / %3")
                                             .arg(data.actualMouldeName)
                                             .arg(data.childPageName)
                                             .arg(data.translateContent)));

        // 设置汉字的Qt::UserRole数据？
        m_model->setData(m_model->index(m_model->rowCount() - 1, 0),
                         QString("%1 --> %2 / %3")
                         .arg(data.actualMouldeName)
                         .arg(data.childPageName)
                         .arg(data.translateContent),
                         Qt::UserRole);
    }

}

void SearchWidget::clearSearchData() {
    m_searchBoxStruct.translateContent = "";
    m_searchBoxStruct.actualMouldeName = "";
    m_searchBoxStruct.childPageName    = "";
    m_searchBoxStruct.fullPagePath     = "";

}
