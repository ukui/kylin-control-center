#ifndef DATAFORMAT_H
#define DATAFORMAT_H

#include <QTimer>
#include <QDialog>
#include <QGSettings>
#include <QListView>
#include <QFile>
#include <QPainter>
#include <QPainterPath>

namespace Ui {
class DataFormat;
}

class DataFormat : public QDialog
{
    Q_OBJECT

public:
    explicit DataFormat(QWidget *parent = nullptr);
    ~DataFormat();

protected:
    void paintEvent(QPaintEvent *);

private:
    QTimer *m_itimer;
    QGSettings *m_gsettings = nullptr;
    QString qss;
    QString locale;

private:
    Ui::DataFormat *ui;

    void initUi();
    void initConnect();
    void initComponent();
    void writeGsettings(const QString &key, const QString &value);

Q_SIGNALS:
    void dataChangedSignal();

private slots:
    void datetime_update_slot();
    void confirm_btn_slot();
};

#endif // DATAFORMAT_H
