#ifndef KSC_BUSINESS_DEF_H
#define KSC_BUSINESS_DEF_H

#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtDBus/qdbusmetatype.h>
#include <QtDBus>

enum DEFENDER_MODULE_TYPE{
    DEFENDER_SCAN = 0,
    DEFENDER_ACCOUNT,
    DEFENDER_NETWORK,
    DEFENDER_VIRUS,
    DEFENDER_CONTROL,
    DEFENDER_MODULE_TYPE_MAX,  //这个类型的数量
    DEFEDNER_MODULE_TYPE_BEGIN = DEFENDER_SCAN
};

typedef struct ksc_defender_module	{
    int module_type;
    QString module_name;			//模块名称
    QString module_desc;			//模块描述
    QString module_normal_icon; 	//正常模式下图标
    QString module_hover_icon;      //悬停图标
    QString module_exec;            //命令参数
    QStringList status_list;        //模块状态描述列表

    ksc_defender_module(){}

    ksc_defender_module(const int type, const QString& str1, const QString& str2, const QString& str3, const QString& str4, const QString& str5 /*, QStringList strlist*/)
        :module_type(type)
        ,module_name(str1)
        ,module_desc(str2)
        ,module_normal_icon(str3)
        ,module_hover_icon(str4)
        ,module_exec(str5)
    {
    }

    ksc_defender_module& operator=(const ksc_defender_module& other)
    {
        this->module_type = other.module_type;
        this->module_name = other.module_name;
        this->module_desc = other.module_desc;
        this->module_normal_icon = other.module_normal_icon;
        this->module_hover_icon = other.module_hover_icon;
        this->module_exec = other.module_exec;
        this->status_list = other.status_list;

        return *this;
    }

    friend QDBusArgument &operator<<(QDBusArgument &argument, const ksc_defender_module &cfg)
    {
        argument.beginStructure();

        argument << cfg.module_type;
        argument << cfg.module_name;
        argument << cfg.module_desc;
        argument << cfg.module_normal_icon;
        argument << cfg.module_hover_icon;
        argument << cfg.module_exec;
        argument << cfg.status_list;

        argument.endStructure();

        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ksc_defender_module &cfg)
    {
        argument.beginStructure();

        argument >> cfg.module_type;
        argument >> cfg.module_name;
        argument >> cfg.module_desc;
        argument >> cfg.module_normal_icon;
        argument >> cfg.module_hover_icon;
        argument >> cfg.module_exec;
        argument >> cfg.status_list;

        argument.endStructure();

        return argument;
    }

}ksc_defender_module;
Q_DECLARE_METATYPE(ksc_defender_module)

typedef QList<ksc_defender_module> ksc_defender_module_list;

Q_DECLARE_METATYPE(ksc_defender_module_list)

#endif // KSC_BUSINESS_DEF_H
