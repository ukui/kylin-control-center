#ifndef QL_LINEEDIT_PASS_H
#define QL_LINEEDIT_PASS_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QRegExpValidator>

class ql_lineedit_pass : public QLineEdit
{
    Q_OBJECT
public:
    ql_lineedit_pass(QWidget *parent = nullptr);
    bool check();
    QPushButton* get_visble();
private:
    QPushButton *visble;
    QHBoxLayout *layout;
};

#endif // QL_LINEEDIT_PASS_H
