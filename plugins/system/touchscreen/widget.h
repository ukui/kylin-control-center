#ifndef WIDGET_H
#define WIDGET_H

//
#include <QWidget>
#include <QVariantMap>
#include <QGSettings>
#include <QSettings>
#include <QButtonGroup>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QStringList>
#include <QSharedPointer>

#include <KF5/KScreen/kscreen/config.h>


class QLabel;
class QMLOutput;
class QMLScreen;
class PrimaryOutputCombo;

class QPushButton;
class QComboBox;
class QStyledItemDelegate;
class XinputManager;


namespace KScreen
{
class ConfigOperation;
}

namespace Ui {
class TouchScreen;
}

class Widget : public QWidget
{
    Q_OBJECT

  public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    void setConfig(const KScreen::ConfigPtr &config);
    KScreen::ConfigPtr currentConfig() const;

    void slotFocusedOutputChangedNoParam();
    void initConnection();

    void initui();
    QString getScreenName(QString name = "");
    bool event(QEvent *event);

  protected:


  Q_SIGNALS:
    void changed();

  private Q_SLOTS:

    void slotFocusedOutputChanged(QMLOutput *output);

    void slotOutputConnectedChanged();

    void outputAdded(const KScreen::OutputPtr &output);
    void outputRemoved(int outputId);
    void touchscreenAdded();
    void touchscreenRemoved();
    void curOutoutChanged(int index);
    void curTouchScreenChanged(int index);
    void primaryOutputSelected(int index);

  public Q_SLOTS:
    void maptooutput();
    void CalibratTouch();

  private:
    void loadQml();
    void resetPrimaryCombo();
    void resettouchscreenCombo();
    void addOutputToMonitorCombo(const KScreen::OutputPtr &output);
    void addTouchScreenToTouchCombo(const QString touchscreenname );
    bool findTouchScreen();
    QString findTouchScreenName(int devicesid);
    KScreen::OutputPtr findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info);

  private:
    Ui::TouchScreen *ui;
    XinputManager *m_pXinputManager;
    QMLScreen *mScreen = nullptr;
    QString CurTouchScreenName = "";
    QString CurMonitorName = "";

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    KScreen::ConfigPtr mConfig ;
    KScreen::ConfigPtr mPrevConfig ;
    //这是outPutptr结果
    KScreen::OutputPtr res ;
#else
    KScreen::ConfigPtr mConfig = nullptr;
    KScreen::ConfigPtr mPrevConfig = nullptr;
    // outPutptr结果
    KScreen::OutputPtr res = nullptr;
#endif

    QButtonGroup *singleButton;


    bool mOriApply;
    bool mConfigChanged   = false;
    bool mOnBattery       = false;
    bool m_blockChanges = false;

};

#endif // WIDGET_H
