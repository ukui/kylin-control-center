#ifndef OUTPUTCONFIG_H
#define OUTPUTCONFIG_H

#include <QGroupBox>
#include <QComboBox>
#include <QWidget>

#include <KF5/KScreen/kscreen/output.h>

#include <QGSettings>

class QCheckBox;
class ResolutionSlider;
class QLabel;
class QStyledItemDelegate;


namespace Ui {
class KScreenWidget;
}

class OutputConfig : public QWidget
{
    Q_OBJECT

  public:
    explicit OutputConfig(QWidget *parent);
    explicit OutputConfig(const KScreen::OutputPtr &output, QWidget *parent = nullptr);
    ~OutputConfig() override;

    virtual void setOutput(const KScreen::OutputPtr &output);
    KScreen::OutputPtr output() const;

    void setTitle(const QString &title);
    void setShowScaleOption(bool showScaleOption);
    bool showScaleOption() const;

    //拿取配置
    void initConfig(const KScreen::ConfigPtr &config);
    QStringList readFile(const QString& filepath);
 public Q_SLOTS:
    //unidiedoutputconfig使用
    void slotRotationChanged(int index);
    void slotScaleChanged(int index);
  protected Q_SLOTS:
    void slotResolutionChanged(const QSize &size);
   // void slotRotationChanged(int index);
    void slotRefreshRateChanged(int index);
   // void slotScaleChanged(int index);

  Q_SIGNALS:
    void changed();
    void scaleChanged(int index);

  protected:
    virtual void initUi();
    int getScreenScale();

  protected:
    QLabel *mTitle = nullptr;
    KScreen::OutputPtr mOutput;
    QCheckBox *mEnabled = nullptr;
    ResolutionSlider *mResolution = nullptr;
    QComboBox *mRotation = nullptr;
    QComboBox *mScale = nullptr;
    QComboBox *mRefreshRate = nullptr;
    QComboBox *mMonitor = nullptr;
    QComboBox *tmpResolution = nullptr;
    QComboBox *scaleCombox = nullptr;
    bool mShowScaleOption  = false;

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    KScreen::ConfigPtr mConfig ;
#else
    KScreen::ConfigPtr mConfig = nullptr;
#endif

    QString qss;
    QStringList proRes;        //profile文件内容

    QGSettings *m_gsettings = nullptr;

};

#endif // OUTPUTCONFIG_H
