#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QFrame>

#include <KF5/KScreen/kscreen/output.h>

class QVBoxLayout;
class OutputConfig;
class UnifiedOutputConfig;

class QLabel;
class QCheckBox;
class QSlider;
class QComboBox;

const QString kSession = "wayland";

class ControlPanel : public QFrame
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel() override;

    void setConfig(const KScreen::ConfigPtr &config);
    void setUnifiedOutput(const KScreen::OutputPtr &output);
    void activateOutputNoParam();

private:
    void isWayland();

public Q_SLOTS:
    void activateOutput(const KScreen::OutputPtr &output);

Q_SIGNALS:
    void changed();
    void scaleChanged(int index);

private Q_SLOTS:
    void addOutput(const KScreen::OutputPtr &output);
    void removeOutput(int outputId);

public:
    QVBoxLayout *mLayout;

private:
    KScreen::ConfigPtr mConfig;
    QList<OutputConfig *> mOutputConfigs;

    UnifiedOutputConfig *mUnifiedOutputCfg;

    KScreen::OutputPtr mCurrentOutput;

    bool mIsWayland;
};

#endif // CONTROLPANEL_H
