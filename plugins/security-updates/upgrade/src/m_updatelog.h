#ifndef M_UPDATELOG_H
#define M_UPDATELOG_H

#include <QFrame>
#include <QDialog>

#include <QBoxLayout>

#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QScrollBar>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include <QDebug>

#include <QFont>

#include "historyupdatelistwig.h"

class m_updatelog : public QDialog
{
    Q_OBJECT
public:
    explicit m_updatelog(QWidget *parent = nullptr);
    QTextEdit * des = nullptr;

protected:


private:
    QListWidget  *mainListwidget; //列表容器
    int maxCode = 0;
    int minCode = 0;
    QHBoxLayout *hll = nullptr;

private slots:
    void initUI(); //初始化UI
    void dynamicLoadingInit(); //动态加载
    void dynamicLoading(int i); //动态加载
    void updatesql();//更新列表
    void defaultItem();//默认选中
};

#endif // M_UPDATELOG_H
