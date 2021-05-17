#ifndef TRAYBUSTHREAD_H
#define TRAYBUSTHREAD_H

#include <QThread>
#include <QtDBus>

class traybusthread : public QThread
{
    Q_OBJECT
signals:
    void result(QStringList updateList);

private:
    void run();
    QStringList inameList;  //重要更新列表
    QDBusInterface *m_pServiceInterface;



public slots:
    void getInameAndCnameList(QString arg);

};


#endif // TRAYBUSTHREAD_H
