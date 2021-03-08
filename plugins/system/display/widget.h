#ifndef WIDGET_H
#define WIDGET_H

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
#include <QHash>
#include <QVariant>
#include <QScreen>
#include <QMultiMap>

#include <KF5/KScreen/kscreen/config.h>

#include "outputconfig.h"
#include "slider.h"
#include "SwitchButton/switchbutton.h"

const QString tempDayBrig  = "6500";

class QLabel;
class QMLOutput;
class QMLScreen;
class ControlPanel;
class PrimaryOutputCombo;

class QPushButton;
class QComboBox;

class QQuickView;
class QQuickWidget;
class QStyledItemDelegate;

typedef enum {
    SUN,
    CUSTOM,
}MODE;


namespace KScreen
{
class ConfigOperation;
}

namespace Ui {
class DisplayWindow;
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
    void initBrightnessUI();
    void initConnection();
    QString getScreenName(QString name = "");
    void initTemptSlider();
    void writeScreenXml();

    bool writeFile(const QString &filePath);
    void writeGlobal(const KScreen::OutputPtr &output);
    bool writeGlobalPart(const KScreen::OutputPtr &output,
                         QVariantMap &info,
                         const KScreen::OutputPtr &fallback);
    QString globalFileName(const QString &hash);
    QVariantMap getGlobalData(KScreen::OutputPtr output);

    float converToScale(const int value);
    int scaleToSlider(const float value);

    void initUiComponent();

  protected:
    bool eventFilter(QObject *object, QEvent *event) override;

  Q_SIGNALS:
    void changed();

    void nightModeChanged(const bool nightMode) const;
    void redShiftValidChanged(const bool isValid) const;

  private Q_SLOTS:
    void slotFocusedOutputChanged(QMLOutput *output);

    void slotOutputEnabledChanged();
    void slotOutputConnectedChanged();

    void slotUnifyOutputs();

    void slotIdentifyButtonClicked(bool checked = true);
    void slotIdentifyOutputs(KScreen::ConfigOperation *op);
    void clearOutputIdentifiers();

    void outputAdded(const KScreen::OutputPtr &output);
    void outputRemoved(int outputId);
    void primaryOutputSelected(int index);
    void primaryOutputChanged(const KScreen::OutputPtr &output);

    void showNightWidget(bool judge);
    void showCustomWiget(int index);

    void slotThemeChanged(bool judge);

    // 按钮选择主屏确认按钮
    void primaryButtonEnable(bool);
    // 是否禁用设置主屏按钮
    void mainScreenButtonSelect(int index);
    // 是否禁用屏幕
    void checkOutputScreen(bool judge);
    // 设置屏幕亮度
    void setBrightnessScreen(int value);
    void setDDCBrightness();

    // 设置亮度滑块数值
    void setBrightnesSldierValue();
    // 设置夜间模式
    void setNightMode(const bool nightMode);
    // 初始化夜间模式
    void initNightStatus();
    void nightChangedSlot(QHash<QString, QVariant> nightArg);

    // 设置wayland主屏幕
    void callMethod(QRect geometry, QString name);
    QString getPrimaryWaylandScreen();
    void isWayland();

    // 设置外接显示器亮度
    void setDDCBrighthessSlot(int brightnessValue);

  public Q_SLOTS:
    void save();
    void scaleChangedSlot(int index);
    void changedSlot();
    void propertiesChangedSlot(QString, QMap<QString, QVariant>, QStringList);

  private:
    void loadQml();
    void resetPrimaryCombo();
    void addOutputToPrimaryCombo(const KScreen::OutputPtr &output);
    KScreen::OutputPtr findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info);

    void setHideModuleInfo();
    void setTitleLabel();
    void writeScale(int scale);
    void initGSettings();
    void setcomBoxScale();
    void initNightUI();
    // 是否恢复应用之前的配置
    bool isRestoreConfig();
    QString getCpuInfo();

    bool isCloneMode();
    bool isBacklight();

    QString getMonitorType();
    int getDDCBrighthess();

  private:
    Ui::DisplayWindow *ui;
    QMLScreen *mScreen = nullptr;

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

    ControlPanel *mControlPanel = nullptr;
    // 设置主显示器相关控件
    OutputConfig *mOutputConfig = nullptr;

    QList<QQuickView*> mOutputIdentifiers;
    QTimer *mOutputTimer = nullptr;

    // 亮度配置文件位置
    QString brightnessFile = nullptr;

    QStringList mPowerKeys;

    SwitchButton *mNightButton       = nullptr;
    SwitchButton *mCloseScreenButton = nullptr;
    SwitchButton *mUnifyButton       = nullptr;
    SwitchButton *mThemeButton       = nullptr;

    QLabel *nightLabel               = nullptr;

    Slider *slider;

    QGSettings *mGsettings      = nullptr;
    QGSettings *scaleGSettings  = nullptr;
    QGSettings *mPowerGSettings = nullptr;
    QSettings  *mQsettings      = nullptr;

    QButtonGroup *singleButton;

    QSharedPointer<QDBusInterface> mUPowerInterface;
    QHash<QString, QVariant> mNightConfig;

    int screenScale = 1;
    int mScreenId  = 1;

    // 是否为夜间模式
    bool mIsNightMode     = false;
    bool mRedshiftIsValid = false;
    bool mIsScaleChanged  = false;
    bool mOriApply;
    bool mConfigChanged   = false;
    bool mOnBattery       = false;
    bool mBlockChanges    = false;
    bool mFirstLoad       = true;
    bool mIsWayland       = false;
    bool mIsBattery       = false;

    QString mCPU;
};

#endif // WIDGET_H
