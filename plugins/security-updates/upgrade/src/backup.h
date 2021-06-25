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

    bool setProgress = false;

signals:
    void backupStartRestult(int);
    bool calCapacity();
    void bakeupFinish(int);
    void backupProgress(int);
public slots:
    int needBacdUp();
    void startBackUp(int);
    void creatInterface();

private slots:
    void sendRate(int,int);
    void receiveStartBackupResult(int result);

private:
    QString timeStamp ="";
    int bakeupState = 0;
    bool m_isActive = false;
    bool haveBackTool();
    bool readSourceManagerInfo();
    bool readBackToolInfo();
    QDBusInterface *interface = nullptr;
    QDBusServiceWatcher *watcher = nullptr;
    void onDBusNameOwnerChanged(const QString &name,const QString &oldOwner,const QString &newOwner);

};

#endif // BACKUP_H
