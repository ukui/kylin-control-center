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

    void initConfig(const KScreen::ConfigPtr &config);

protected Q_SLOTS:
    void slotResolutionChanged(const QSize &size);
    void slotRotationChanged(int index);
    void slotRefreshRateChanged(int index);
    void slotScaleChanged(int index);
    void slotDPIChanged(QString key);

private Q_SLOTS:
    void slotScaleIndex(const QSize &size);

Q_SIGNALS:
    void changed();
    void scaleChanged(double scale);

protected:
    virtual void initUi();
    double getScreenScale();

private:
    void initConnection();
    void initDpiConnection();
    QString scaleToString(double scale);

protected:
    KScreen::OutputPtr mOutput;
    QLabel *mTitle = nullptr;
    QCheckBox *mEnabled = nullptr;
    ResolutionSlider *mResolution = nullptr;

    QComboBox *mRotation = nullptr;
    QComboBox *mScale = nullptr;
    QComboBox *mRefreshRate = nullptr;
    QComboBox *mMonitor = nullptr;
    QComboBox *mScaleCombox = nullptr;

    bool mShowScaleOption = false;
    bool mIsWayland = false;
    bool mIsFirstLoad = true;

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    KScreen::ConfigPtr mConfig;
#else
    KScreen::ConfigPtr mConfig = nullptr;
#endif

    QGSettings *mDpiSettings = nullptr;
};

#endif // OUTPUTCONFIG_H
