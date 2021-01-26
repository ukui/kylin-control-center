#ifndef SECURITYCENTER_H
#define SECURITYCENTER_H

#include <QWidget>
#include <QObject>
#include <QtPlugin>

#include <QMouseEvent>
#include <QLabel>
#include <QTimer>

#include "shell/interface.h"
#include "FlowLayout/flowlayout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SecurityCenter; }
QT_END_NAMESPACE

class BlockWidget : public QWidget
{
    Q_OBJECT

public:
    BlockWidget();
    ~BlockWidget();
public:
    void initComponent();
    void setupComponent(QString normal_icon ,QString hover_icon, QString title, QString detail, QString cmd);

public:
    QString _cmd;

    QLabel * logoLabel;
    QLabel * titleLable;
    QLabel * detailLabel;

    int m_charWidth;
    int m_curIndex;
    int m_labelWidth;
    QString m_showText;
    QString m_normalIcon;
    QString m_hoverIcon;
    QTimer *timer;
    void scrollLabel();
    void updateIndex();
    void showPaint();

protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
    virtual bool eventFilter(QObject *watched, QEvent *event);
    virtual void paintEvent(QPaintEvent * event);

    virtual void mousePressEvent(QMouseEvent * event);

Q_SIGNALS:
    void bwClicked(QString _cmd);
    void indexChanged();

};

class SecurityCenter : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    SecurityCenter();
    ~SecurityCenter();

public:
    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    FlowLayout * flowLayout;

public:
    void initTitleLabel();
    void initComponent();

    void runExternalApp(QString cmd);
private:
    Ui::SecurityCenter *ui;

private:
    QString pluginName;
    int pluginType;
    QWidget * pluginWidget;

};
#endif // SECURITYCENTER_H
