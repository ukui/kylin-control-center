#include "controlpanel.h"
#include "outputconfig.h"
#include "unifiedoutputconfig.h"
//#include "kcm_screen_debug.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <KF5/KScreen/kscreen/config.h>

ControlPanel::ControlPanel(QWidget *parent)
    : QFrame(parent)
    , mUnifiedOutputCfg(nullptr)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mLayout = new QVBoxLayout(this);
    mLayout->setContentsMargins(0,0,0,0);
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
            this, &ControlPanel::addOutput);
    connect(mConfig.data(), &KScreen::Config::outputRemoved,
            this, &ControlPanel::removeOutput);

    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        addOutput(output);
    }
}

void ControlPanel::addOutput(const KScreen::OutputPtr &output)
{

    OutputConfig *outputCfg = new OutputConfig(this);
    outputCfg->setVisible(false);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    outputCfg->setShowScaleOption(mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PerOutputScaling));
#else

#endif

    outputCfg->setOutput(output);
    connect(outputCfg, &OutputConfig::changed,
            this, &ControlPanel::changed);

    connect(outputCfg, &OutputConfig::scaleChanged,
            this, &ControlPanel::scaleChanged);

    mLayout->addWidget(outputCfg);

    mOutputConfigs << outputCfg;

#ifdef KIRIN
    activateOutput(mCurrentOutput);
#endif

}

void ControlPanel::removeOutput(int outputId)
{
    for (OutputConfig *outputCfg : mOutputConfigs) {
        if (outputCfg->output()->id() == outputId) {
            mOutputConfigs.removeOne(outputCfg);
            delete outputCfg;
            outputCfg = nullptr;
            return;
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
    qDebug()<<"activateOutputNoParam ------>"<<endl;
   // qCDebug(KSCREEN_KCM) << "Activate output" << output->id();

    Q_FOREACH (OutputConfig *cfg, mOutputConfigs) {
        qDebug()<<cfg->output()->id()<<" id";
        cfg->setVisible(cfg->output()->id() == 66);
    }
}

void ControlPanel::setUnifiedOutput(const KScreen::OutputPtr &output)
{
    Q_FOREACH (OutputConfig *config, mOutputConfigs) {
        if (!config->output()->isConnected()) {
            continue;
        }

        //隐藏下面控制
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
