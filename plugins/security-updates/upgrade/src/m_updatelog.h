#ifndef M_UPDATELOG_H
#define M_UPDATELOG_H
//窗体
#include <QFrame>
#include <QDialog>
//布局
#include <QBoxLayout>
//控件
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QScrollBar>
//数据库
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
//其他
#include <QDebug>
#include <QFont>
#include "historyupdatelistwig.h"
#include "updatedbus.h"
class m_updatelog : public QDialog
{
    Q_OBJECT
public:
    static m_updatelog * GetInstance(QWidget *parent);
    static void closeUpdateLog();
    QTextEdit * des = nullptr;

protected:

public slots:
    void historyUpdateNow(QString str1, QString str2);//实时更新

private:
    m_updatelog(QWidget *parent);
    static m_updatelog * m_instance;
    QListWidget  *mainListwidget; //列表容器
    int firstCode = 0;
    int loadingCode = 0;
    QHBoxLayout *hll = nullptr;
    QString setDefaultDescription(QString str);

private slots:
    void initUI(); //初始化UI
    void dynamicLoadingInit(); //动态加载
    void dynamicLoading(int i); //动态加载
    void updatesql(const int &start=0, const int &num=20, const QString &intop="");//更新列表
    void defaultItem();//默认选中
    QString translationVirtualPackage(QString str);//翻译虚包名
};

#endif // M_UPDATELOG_H
