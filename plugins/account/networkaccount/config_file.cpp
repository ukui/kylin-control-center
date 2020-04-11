#include "config_file.h"
#include <QDebug>

Config_File::Config_File(QString qstrfilename)
{
    if (qstrfilename.isEmpty())
    {
        m_qstrFileName = "/kylinssoclient/All.conf";
    }
    else
    {
        m_qstrFileName = qstrfilename;
    }

    m_psetting = new QSettings(m_qstrFileName, QSettings::IniFormat);
}

Config_File::~Config_File()
{
    delete m_psetting;
    m_psetting = 0;
}

void Config_File::Set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    qDebug()<<"123321";
    m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
}

QVariant Config_File::Get(QString qstrnodename,QString qstrkeyname)
{
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;
}
