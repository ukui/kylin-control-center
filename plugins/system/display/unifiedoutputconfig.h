#ifndef UNIFIEDOUTPUTCONFIG_H
#define UNIFIEDOUTPUTCONFIG_H

#include "outputconfig.h"

namespace KScreen {
class Output;
class Config;
}

class UnifiedOutputConfig : public OutputConfig
{
    Q_OBJECT
public:
    explicit UnifiedOutputConfig(const KScreen::ConfigPtr &config, QWidget *parent);
    ~UnifiedOutputConfig() override;

    void setOutput(const KScreen::OutputPtr &output) override;

private Q_SLOTS:
    void slotResolutionChanged(const QSize &size);

    // 统一输出后调整屏幕方向统一代码
    void slotRotationChangedDerived(int index);

    void slotRestoreResoltion();
    void slotRestoreRatation();
    void slotRefreshRateChanged(int index);

private:
    void initUi() override;
    KScreen::OutputPtr createFakeOutput();
    QString findBestMode(const KScreen::OutputPtr &output, const QSize &size);

private:
    KScreen::ConfigPtr mConfig;
    QList<KScreen::OutputPtr> mClones;
};

#endif // UNIFIEDOUTPUTCONFIG_H
