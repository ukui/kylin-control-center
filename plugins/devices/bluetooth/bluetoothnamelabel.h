#ifndef BLUETOOTHNAMELABEL_H
#define BLUETOOTHNAMELABEL_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QObject>

class BluetoothNameLabel : public QWidget
{
    Q_OBJECT
public:
    BluetoothNameLabel(QWidget *parent = nullptr, int x = 200,int y = 40);
    ~BluetoothNameLabel();
    void set_dev_name(const QString &dev_name);
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);

signals:
    void send_adapter_name(const QString &value);

public slots:
    void LineEdit_Input_Complete();
    void set_label_text(const QString &value);

private:
    QLabel *m_label = nullptr;
    QLineEdit *m_lineedit = nullptr;
    QString device_name;
};

#endif // BLUETOOTHNAMELABEL_H
