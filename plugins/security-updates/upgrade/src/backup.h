#ifndef BACKUP_H
#define BACKUP_H

#include <QObject>
#include <QFile>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QFileInfo>

class BackUp : public QObject
{
    Q_OBJECT
public:
    explicit BackUp(QObject *parent = nullptr);

signals:
    void bakeupFinish(int);
    void backupProgress(int);
public slots:
    int needBacdUp();
    void startBackUp(int);

private slots:
    void sendRate(int,int);

private:
    QString timeStamp ="";
    int bakeupState = 0;
    bool haveBackTool();
    bool readSourceManagerInfo();
    bool readBackToolInfo();
    QDBusInterface *interface = nullptr;
    bool setProgress = false;
};

#endif // BACKUP_H
