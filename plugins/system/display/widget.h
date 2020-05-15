#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVariantMap>
#include <QGSettings/QGSettings>
#include <QSettings>
#include <QButtonGroup>

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

typedef struct xml{
    QString isClone;
    QString outputName;
    QString vendorName;
    QString productName;
    QString serialNum;
    QString widthValue;
    QString heightValue;
    QString rateValue;
    QString posxValue;
    QString posyValue;
    QString rotationValue;
    QString isPrimary;
    bool isEnable;
}xmlFile;

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
    //亮度调节UI
    void initBrightnessUI();
    QString getScreenName(QString name = "");
    //获得显示器名称(shell获得)
    QStringList  getscreenBrightnesName();
    //获得显示器名称(shell获得)
    QStringList  getscreenBrightnesValue();
    //获取配置文件位置
    void setBrigthnessFile();
    void initTemptSlider();
    void initConfigFile();
    //获取屏幕xml文件
    void writeScreenXml(int count);
    //初始化屏幕配置文件
    void initScreenXml(int count);
    //获取edid信息
    void getEdidInfo(QString monitorName,xmlFile *xml);

    void setIsNightMode(bool isNightMode);

    QStringList readFile(const QString& filepath);
    void writeFile(const QString& filepath, const QStringList& content);

    float converToScale(const int value);
    int scaleToSlider(const float value);

    void initUiComponent();
    void initUiQss();



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

    //按钮选择主屏确认按钮
    void primaryButtonEnable();
    //是否禁用设置主屏按钮
    void mainScreenButtonSelect(int index);
    //是否禁用屏幕
    void checkOutputScreen(bool judge);
    //设置屏幕亮度
    void setBrightnessScreen(float brightnessValue);
    //设置亮度滑块数值
    void setBrightnesSldierValue(QString screeName);
    //保存屏幕亮度配置
    void saveBrigthnessConfig();
    //设置夜间模式
    void setNightMode(const bool nightMode);
    //redshitf是否合法
    void setRedShiftIsValid(bool redshiftIsValid);
    //更新夜间模式状态
    void updateNightStatus();
    //初始化夜间模式
    void initNightStatus();

  public Q_SLOTS:
    void save();
    void scaleChangedSlot(int index);

  private:
    void loadQml();
    void resetPrimaryCombo();
    void addOutputToPrimaryCombo(const KScreen::OutputPtr &output);
    KScreen::OutputPtr findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info);

    float scaleRet();
    void writeScale(float scale);
    void initGSettings();
    bool getNightModeGSetting(const QString &key);
    void setNightModebyPanel(bool judge);
    void setSessionScale(int scale);
    void writeConfigFile();

  private:
    Ui::DisplayWindow *ui;
    QMLScreen *mScreen = nullptr;
    KScreen::ConfigPtr mConfig ;
    KScreen::ConfigPtr mPrevConfig ;

    ControlPanel *mControlPanel = nullptr;
    //这里是去设置主显示器相关控件
    OutputConfig *mOutputConfig = nullptr;


    QList<QQuickView*> mOutputIdentifiers;
    QTimer *mOutputTimer = nullptr;

    //这是outPutptr结果
    KScreen::OutputPtr res ;

    bool m_blockChanges = false;

    QString brightnessFile = nullptr;//亮度配置文件位置
    xmlFile inputXml[100];//xml文件

    SwitchButton *nightButton = nullptr;
    SwitchButton *closeScreenButton = nullptr;
    SwitchButton *m_unifybutton = nullptr;
    QLabel *nightLabel = nullptr;

    bool m_isNightMode = false;//是否为夜间模式
    bool m_redshiftIsValid = false;

    QStyledItemDelegate *itemDelege;
    QStringList proRes;        //profile文件内容

    Slider *slider;
    QGSettings *m_gsettings = nullptr;
    QGSettings *scaleGSettings = nullptr;
    QSettings *m_qsettings = nullptr;

    int screenScale = 1;

    QButtonGroup *singleButton;

};

#endif // WIDGET_H

