#ifndef AREA_CODE_LINEEDIT_H
#define AREA_CODE_LINEEDIT_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QListView>
#include "ql_combobobx.h"

class area_code_lineedit : public QLineEdit
{
    Q_OBJECT
public:
    explicit    area_code_lineedit(QWidget *parent = nullptr);
    void        InittoCountrycode();
private:
    ql_combobobx *combobox;
    QJsonArray json_code;
    QJsonArray json_array;
    QJsonDocument *json_file;
    QFrame *vertical_line;
    QFile *loadfile;
    QMap<QString,QPair<QString,QString>>country_code;
    QStandardItemModel *model;
    QHBoxLayout *layout;
signals:

};

#endif // AREA_CODE_LINEEDIT_H
