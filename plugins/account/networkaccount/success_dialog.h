#ifndef SUCCESS_DIALOG_H
#define SUCCESS_DIALOG_H

#include <QObject>
#include <QWidget>

class success_dialog : public QWidget
{
    Q_OBJECT
public:
    explicit success_dialog(QWidget *parent = nullptr);

signals:

};

#endif // SUCCESS_DIALOG_H
