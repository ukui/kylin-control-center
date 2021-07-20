#include "controlpanel.h"
#include "outputconfig.h"
#include "unifiedoutputconfig.h"
#include "utils.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QDBusInterface>
#include <KF5/KScreen/kscreen/config.h>

QSize mScaleSize = QSize();

ControlPanel::ControlPanel(QWidget *parent) :
    QFrame(parent),
    mUnifiedOutputCfg(nullptr)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mLayout = new QVBoxLayout(this);
    mLayout->setContentsMargins(0, 0, 0, 0);

    isWayland();
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setConfig(const KScreen::ConfigPtr &config)
{
    qDeleteAll(mOutputConfigs);
    mOutputConfigs.clear();
    delete mUnifiedOutputCfg;
    mUnifiedOutputCfg = nullptr;

    if (mConfig) {
        mConfig->disconnect(this);
    }

    mConfig = config;
    connect(mConfig.data(), &KScreen::Config::outputAdded,
            this, [=](const KScreen::OutputPtr &output) {
        addOutput(output, false);
    });
    connect(mConfig.data(), &KScreen::Config::outputRemoved,
            this, &ControlPanel::removeOutput);

    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        if (output->isConnected()) {
            changescalemax(output);
        }
    }

    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        addOutput(output, false);
    }
}

void ControlPanel::addOutput(const KScreen::OutputPtr &output, bool connectChanged)
{
    if (!connectChanged) {
        connect(output.data(), &KScreen::Output::isConnectedChanged,
                    this, &ControlPanel::slotOutputConnectedChanged);
    }
    
    if (!output->isConnected())
        return;

    OutputConfig *outputCfg = new OutputConfig(this);
    outputCfg->setVisible(false);
    outputCfg->setShowScaleOption(mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PerOutputScaling));

    outputCfg->setOutput(output);
    connect(outputCfg, &OutputConfig::changed,
            this, &ControlPanel::changed);

    connect(outputCfg, &OutputConfig::scaleChanged,
            this, &ControlPanel::scaleChanged);

    mLayout->addWidget(outputCfg);

    mOutputConfigs << outputCfg;

    if (mIsWayland) {
        activateOutput(mCurrentOutput);
    }
}

void ControlPanel::removeOutput(int outputId)
{
    if (mUnifiedOutputCfg) {
        mUnifiedOutputCfg->setVisible(false);
    }
    for (OutputConfig *outputCfg : mOutputConfigs) {
        if (!outputCfg || !outputCfg->output()) {
            continue;
        }
        if (outputCfg->output()->id() == outputId) {
            mOutputConfigs.removeOne(outputCfg);
            outputCfg->deleteLater();
            outputCfg = nullptr;
        } else {
            if (outputCfg->output()->isConnected()) {
                outputCfg->setVisible(true);
            } else {
                outputCfg->setVisible(false);
            }
        }
    }
}

void ControlPanel::activateOutput(const KScreen::OutputPtr &output)
{
    // Ignore activateOutput when in unified mode
    if (mUnifiedOutputCfg) {
        return;
    }

    mCurrentOutput = output;

    Q_FOREACH (OutputConfig *cfg, mOutputConfigs) {
        cfg->setVisible(cfg->output()->id() == output->id());
    }
}

void ControlPanel::activateOutputNoParam()
{
    // Ignore activateOutput when in unified mode
    if (mUnifiedOutputCfg) {
        return;
    }

    Q_FOREACH (OutputConfig *cfg, mOutputConfigs) {
        cfg->setVisible(cfg->output()->id() == 66);
    }
}

void ControlPanel::changescalemax(const KScreen::OutputPtr &output)
{
    QSize sizescale = QSize();
    Q_FOREACH (const KScreen::ModePtr &mode, output->modes()) {
        if (sizescale.width() <= mode->size().width()) {
            sizescale = mode->size();
        }
    }
    if (mScaleSize == QSize() || mScaleSize.width() > sizescale.width()) {
        mScaleSize = sizescale;
    }
}

void ControlPanel::isWayland()
{
    QString sessionType = getenv("XDG_SESSION_TYPE");

    if (!sessionType.compare(kSession, Qt::CaseSensitive)) {
        mIsWayland = true;
    } else {
        mIsWayland = false;
    }
}

void ControlPanel::setUnifiedOutput(const KScreen::OutputPtr &output)
{
    Q_FOREACH (OutputConfig *config, mOutputConfigs) {
        if (!config->output()->isConnected()) {
            continue;
        }

        // 隐藏下面控制
        config->setVisible(output == nullptr);
    }

    if (output.isNull()) {
        mUnifiedOutputCfg->deleteLater();
        mUnifiedOutputCfg = nullptr;
    } else {
        mUnifiedOutputCfg = new UnifiedOutputConfig(mConfig, this);
        mUnifiedOutputCfg->setOutput(output);
        mUnifiedOutputCfg->setVisible(true);
        mLayout->insertWidget(mLayout->count() - 2, mUnifiedOutputCfg);
        connect(mUnifiedOutputCfg, &UnifiedOutputConfig::changed,
                this, &ControlPanel::changed);
    }
}

void ControlPanel::slotOutputConnectedChanged()
{
    const KScreen::OutputPtr output(qobject_cast<KScreen::Output *>(sender()), [](void *){
    });

    if (output->isConnected()) {
        changescalemax(output);
        addOutput(output, true);
        for (OutputConfig *outputCfg : mOutputConfigs) {
            outputCfg->slotScaleIndex(mScaleSize);
        }
    } else {
        removeOutput(output->id());
        mScaleSize = QSize();
        for (const KScreen::OutputPtr &output : mConfig->outputs()) {
            if (output->isConnected()) {
                changescalemax(output);
            }
        }
        for (OutputConfig *outputCfg : mOutputConfigs) {
            outputCfg->slotScaleIndex(mScaleSize);
        }
    }
}
