#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <QObject>
#include <QSettings>

class Config_File
{
public:
    Config_File(QString qstrfilename = "");
    virtual ~Config_File(void);
    void Set(QString,QString,QVariant);
    QVariant Get(QString,QString);
private:
    QString m_qstrFileName;
    QSettings *m_psetting;

};

#endif // CONFIG_FILE_H
