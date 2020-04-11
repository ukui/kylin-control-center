#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H


#include <QListWidget>
#include <qcombobox.h>
#include <QListWidgetItem>
#include <QLabel>
#include <QLayout>

class ComboboxItem:public QWidget
{
     Q_OBJECT


public:
    void setText(QString str);
    void setItemID(QString str);
    void setCode(QString str);
    QLabel* get_label();
    QLabel* get_code();
public:
    ComboboxItem(QWidget *parent);
    QLabel* m_code;
    QLabel* m_label;
    QHBoxLayout* m_layout;
    QWidget* m_wdgImg;
    QString m_ID;
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);

};




class MyComboBox  :public QComboBox
{
Q_OBJECT
public:

    explicit MyComboBox(QWidget *parent = Q_NULLPTR);
    ~MyComboBox();

    void addItem(QString str,QString code);      //添加行
    void insertItem(int n,QString str,QString code);
    QString getSelText();      //获取选中文字
    void setSelItem(int n);     //设置选中
    int  findText(QString str); //查找文字，返回位置，如果没有，返回-1；
    void setTab(int n);         //设置显示栏缩进
    void setEditText(QString str);      //设置显示栏文字

    void setQss();      //美化

public slots:
    void onshowAccount(QString str);
    void oncurrenIndex(int n);
    void onTextChanged(const QString & );

protected:
    QListWidget* m_listWidget;
    ComboboxItem* m_selItem;
    QString m_selStr;
    QLineEdit* m_edit;
    int cnt;
    QMap<QListWidgetItem*, ComboboxItem*> m_ItemMap;


    void SetComboChick(int n);


};

#endif // MYCOMBOBOX_H
