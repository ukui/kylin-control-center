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
#include <QLineEdit>
#include <QPushButton>
//数据库
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
//GSETTINGS
#include <QGSettings>
//其他
#include <QDebug>
#include <QFont>
#include "historyupdatelistwig.h"
#include "updatedbus.h"

#define THEME_QT_SCHEMA  "org.ukui.style"

class m_updatelog : public QDialog
{
    Q_OBJECT
public:
    static m_updatelog * GetInstance(QWidget *parent);
    static void closeUpdateLog();
    QTextEdit * des = nullptr;

public:
    QString conversionPackageName(QString package);
    void clearList(void);
    void cacheDynamicLoad(void);
    void searchBoxWidget(void);
    void updateTitleWidget(void);
    bool eventFilter(QObject *watch, QEvent *e);

public slots:
    void historyUpdateNow(QString str1, QString str2);//实时更新
    void slotClose(void);

private:
    m_updatelog(QWidget *parent);
    static m_updatelog * m_instance;
    QListWidget  *mainListwidget; //列表容器
    QLabel *updateDesTab;  //右侧顶部描述标签
    int firstCode = 0;
    int loadingCode = 0;
    QHBoxLayout *hll = nullptr;
    QString setDefaultDescription(QString str);
    QGSettings *qtSettings=nullptr;
    QTimer *timer=nullptr;

    /* 搜索框 */
    QLineEdit *searchBox;
    QLabel *searchIcon;

    /* 标题栏 */
    QWidget *title;
    QLabel *titleIcon;
    QLabel *titleName;
    QPushButton *titleClose;

private slots:
    void initUI(); //初始化UI
    void initGsettings(); //初始化Gsettings
    void dynamicLoadingInit(); //动态加载
    void dynamicLoading(int i); //动态加载
    void updatesql(const int &start=0, const int &num=20, const QString &intop="");//更新列表
    void defaultItem();//默认选中
    QString translationVirtualPackage(QString str);//翻译虚包名
    void changeListWidgetItemHeight();//修改列表项高度

    void slotSearch(QString packageName);
};

#endif // M_UPDATELOG_H
