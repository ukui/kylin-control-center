#ifndef M_UPDATELOG_H
#define M_UPDATELOG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QString>
#include <QSqlDatabase>
#include <QApplication>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStringList>
#include <QListView>
#include <QTableWidgetItem>
#include "listwig.h"
#include <QAction>
#include <QFont>
#include <QPushButton>

class m_updatelog : public QWidget
{
    Q_OBJECT
public:
    explicit m_updatelog(QWidget *parent = nullptr);
    void initUI();

    QLabel       *toplabel;
    QLabel       *uplabel;

    QLineEdit    *searchEdit;

    QHBoxLayout  *m_pfirstlayout   = nullptr;
    QHBoxLayout  *m_psecondlayout   = nullptr;
    QHBoxLayout  *m_entrylayout   = nullptr;
    QVBoxLayout  *m_pmainlayout   = nullptr;

    QWidget      *firstWidget;
    QWidget      *secondWidget;


    QListWidget  *mainListwidget;

    headerwidget *headtitle ;

    //数据库相关
    QSqlDatabase    db;
    QSqlQueryModel  model;

    QStringList     m_Appname;
    QStringList     m_Version;
    QStringList     m_Time;
    QStringList     m_Description;
    QStringList     m_statue;

    int sqlnum;
    int s_insearch(QString arg);

signals:
    void closesignal();

public slots:
    void updatesql();


};

#endif // M_UPDATELOG_H
