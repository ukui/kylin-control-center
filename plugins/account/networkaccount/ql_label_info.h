#ifndef QL_LABEL_INFO_H
#define QL_LABEL_INFO_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class ql_label_info : public QWidget
{
    Q_OBJECT
public:
    void set_text(const QString text);
    void setpixmap(const QPixmap pixmap);
    explicit ql_label_info(QWidget *parent = nullptr);
private:
    QLabel *icon;
    QLabel *text;
    QString context;
    QHBoxLayout *layout;
signals:

};

#endif // QL_LABEL_INFO_H
