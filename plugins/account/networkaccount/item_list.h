#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <QObject>
#include <QListWidget>
#include <QMouseEvent>
#include <QComboBox>
#include "network_item.h"

#define CURSIZE 6

class item_list : public QListWidget
{
    Q_OBJECT
public:
    explicit        item_list(QListWidget *parent = nullptr,int itemssize = CURSIZE);
    QStringList     get_list();
    network_item*   get_item(int cur);
    void            add_item(QString item_name);
private:
    QListWidgetItem *items[30];
    //QStringList     varmap = {tr("用户头像"),tr("鼠标"),tr("输入法"),tr("登录、锁屏、桌面壁纸"),tr("主题"),tr("开始菜单"),tr("电源"),tr("SSH、GPG"),tr("软件商店"),tr("Wifi和以太网配置"),tr("快速启动栏"),tr("任务栏"),tr("文件管理器"),tr("文件默认打开方式"),tr("开机启动项"),tr("麒麟天气"),tr("麒麟影音")};
    QStringList     varmap = {tr("开始菜单"),tr("快速启动项"),tr("任务栏"),tr("用户头像"),tr("麒麟天气"),tr("麒麟影音")};
    int             itemsize = 0;
    network_item    *itempack[30];
    QPoint          m_startPoint;
signals:

};

#endif // ITEM_LIST_H
