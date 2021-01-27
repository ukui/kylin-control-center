#ifndef BACKUP_H
#define BACKUP_H

#include <QObject>
#include <QFile>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
//#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>

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
    QDBusServiceWatcher *watcher = nullptr;
    void onDBusNameOwnerChanged(const QString &name,const QString &oldOwner,const QString &newOwner);
    bool setProgress = false;
    void creatInterface();
};

#endif // BACKUP_H
