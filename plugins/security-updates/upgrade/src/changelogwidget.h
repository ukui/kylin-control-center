#ifndef CHANGELOGWIDGET_H
#define CHANGELOGWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

class changelogwidget :public QWidget
{
public:
    changelogwidget(QWidget *parent = nullptr);
    QLabel *logLabel;
    QTextEdit *changelogEdit;
    QVBoxLayout  *m_entryVlayout   = nullptr;

};

#endif // CHANGELOGWIDGET_H
