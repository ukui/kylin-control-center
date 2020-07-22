#ifndef QL_LABEL_INFO_H
#define QL_LABEL_INFO_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSvgWidget>
#include <QGraphicsSvgItem>

class Tips : public QWidget
{
    Q_OBJECT
public:
    void set_text(const QString m_textLabel);
    explicit Tips(QWidget *parent = nullptr);
    ~Tips();
private:
    QSvgWidget *m_iconWidget;
    QLabel *m_textLabel;
    QString m_szContext;
    QHBoxLayout *m_workLayout;
signals:

};

#endif // QL_LABEL_INFO_H
