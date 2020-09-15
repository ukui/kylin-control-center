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
//    setMinimumSize(553,150);
//    setMaximumSize(16777215,150);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    setFrameStyle(QFrame::NoFrame | QFrame::Sunken);
//    this->setStyleSheet("border: 1px solid #ff0000");
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
}

void ControlPanel::removeOutput(int outputId)
{
    for (OutputConfig *outputCfg : mOutputConfigs) {
        if (outputCfg->output()->id() == outputId) {
            mOutputConfigs.removeOne(outputCfg);
            delete outputCfg;
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


    //qCDebug(KSCREEN_KCM) << "Activate output" << output->id();
    //qDebug()<<"activateOutput---->"<<mOutputConfigs<<endl;
    Q_FOREACH (OutputConfig *cfg, mOutputConfigs) {

    //    qDebug()<<cfg->output()->id()<<" "<<output->id();
        cfg->setVisible(cfg->output()->id() == output->id());

        //cfg->setVisible(cfg->output()->id() == 66)
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
        //cfg->setVisible(cfg->output()->id() == output->id());
        cfg->setVisible(cfg->output()->id() == 66);
    }
}

void ControlPanel::setUnifiedOutput(const KScreen::OutputPtr &output)
{
    Q_FOREACH (OutputConfig *config, mOutputConfigs) {
        if (!config->output()->isConnected()) {
            continue;
        }
        //qDebug()<<"config is---->"<<config->output()<<"--------"<<output<<endl;

        //隐藏下面控制
        config->setVisible(output == nullptr);
    }

    if (output.isNull()) {
        mUnifiedOutputCfg->deleteLater();
        mUnifiedOutputCfg = nullptr;
    } else {
        //qDebug()<<"config is---->"<<mConfig<<endl;
        mUnifiedOutputCfg = new UnifiedOutputConfig(mConfig, this);
        mUnifiedOutputCfg->setOutput(output);
        mUnifiedOutputCfg->setVisible(true);
        mLayout->insertWidget(mLayout->count() - 2, mUnifiedOutputCfg);
        connect(mUnifiedOutputCfg, &UnifiedOutputConfig::changed,
                this, &ControlPanel::changed);
    }
}
